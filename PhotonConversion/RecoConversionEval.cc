#include "RecoConversionEval.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <phool/PHDataNode.h>

#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <g4eval/SvtxEvalStack.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <iostream>
#include <cmath> 
#include <algorithm>
#include <sstream>

using namespace std;

RecoConversionEval::RecoConversionEval(const std::string &name) :
	SubsysReco("RecoConversionEval"), _fname(name) 
	{}

int RecoConversionEval::Init(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int RecoConversionEval::InitRun(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int RecoConversionEval::process_event(PHCompositeNode *topNode) {
	SvtxTrackMap* allTracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	for ( SvtxTrackMap::Iter iter = allTracks->begin(); iter != allTracks->end(); ++iter ) {
		SvtxTrack* thisTrack = iter.second;
		/*there's a lot of track functions that I don't really know what they do
		chisq()
		error()
		id()
		p()
		pos()
		*/ 
		if (abs(thisTrack->get_charge())==1)//I want to now only check e tracks
		{
			cout<<"Charged Track:\n \tchi:"<<thisTrack->chisq()<<"\n \tp:"<<thisTrack->p()<<"\n \tpos:"<<thisTrack->pos()<<"\n \tid:"<<thisTrack->id()<<"\n \terror:"<<thisTrack->error()<<'\n';
		}
	}
	return Fun4AllReturnCodes::EVENT_OK;
}

RecoConversionEval::~RecoConversionEval(){

	_file->Write();
	_file->Close();
	delete _file;
}

int RecoConversionEval::End(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

void RecoConversionEval::process_recoTracks(PHCompositeNode *topNode){

}

ReconstructedConvertedPhoton* RecoConversionEval::reconstruct(PHCompositeNode *topNode){
	//let the stack get the info from the node
	SvtxEvalStack *stack = new SvtxEvalStack(topNode);
	if(!stack){
		cout<<"Evaluator is null quiting photon recovery\n";
		return nullptr;
	}
	stack->set_strict(false); //no idea what this does 
	//stack->set_verbosity(verbosity+1); //might be able to lower this 
	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
	SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	SvtxClusterMap* clustermap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap"); 
	SvtxVertexEval* vertexeval = stack->get_vertex_eval();
	SvtxTrackEval* trackeval =   stack->get_track_eval();
	//use the bools to check the event is good
	if(!vertexeval||!trackeval||!clustermap){
		cout<<"Evaluator is null quiting photon recovery\n";
		return nullptr;
	}
  if(vertexmap->size()!=1){
    cout<<"Vertex count != 1 not reconstructing event\n";
    return nullptr;
  }
	for (SvtxVertexMap::Iter iter = vertexmap->begin(); iter != vertexmap->end(); ++iter) {
		SvtxVertex* vertex = iter->second;
		//only take 2 track events
		if(vertex&&vertex->size_tracks()==2){
			SvtxVertex::TrackIter titer = vertex->begin_tracks(); 
			SvtxTrack* track1 = trackmap->get(*titer);
			PHG4Particle* truth1 = trackeval->max_truth_particle_by_nclusters(track1); 
			++titer;
			SvtxTrack* track2= trackmap->get(*titer);
			PHG4Particle* truth2 = trackeval->max_truth_particle_by_nclusters(track2);
      cout<<"Layers: maps:"<<n_intt_layer<<" intt:"<<n_maps_layer<<'\n';
			if (!b_hasSilicone&&clustermap->get(*track1->begin_clusters())->get_layer()<n_intt_layer+n_maps_layer){
				b_hasSilicone=true;
			}
			//both the truth particles must come from the same vertex 
			if (!truth1||!truth2||truth1->get_vtx_id()!=truth2->get_vtx_id())
			{
				cout<<"Skipping photon recovery tracks do not match \n";
				continue;
			}
			//record the vertex position
			float vx,vy,vz;
			vx = vertex->get_x();
			vy = vertex->get_y();
			vz = vertex->get_z();
			cout<<"Vertex:"<<vx<<", "<<vy<<", "<<vz<<'\n';
			//not sure what I want to do with these charges
			float charge1 = track1->get_charge();
			if(abs(charge1)!=1){
				cout<<"Quiting photon recovery due to charge="<<charge1<<'\n';
				continue; //only considering electron positron conversion 
			}
			//record the track momentum and charge
			float t1x,t1y,t1z,t2x,t2y,t2z,charge2;
			t1x = track1->get_px();
			t1y = track1->get_py();
			t1z = track1->get_pz();
			charge2 = track2->get_charge();
			if(charge1!= -1*charge2){
				b_goodCharge=false;
			}
			t2x = track2->get_px();
			t2y = track2->get_py();
			t2z = track2->get_pz();


			//convert to TObjects
			TVector3 Ttrack1(t1x,t1y,t1z), Ttrack2(t2x,t2y,t2z);
			PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(vertex); //not entirely sure what this does
			//double check these give the right values 
			b_recovec1= new TLorentzVector(Ttrack1,pToE(Ttrack1,kEmass));
			b_recovec2= new TLorentzVector(Ttrack2,pToE(Ttrack2,kEmass)); 
			b_recoVertex= new TVector3(vx,vy,vz);
			TVector3 tTrack1(truth1->get_px(),truth1->get_py(),truth1->get_pz()),
							 tTrack2(truth2->get_px(),truth2->get_py(),truth2->get_pz());
			b_truthVertex= new TVector3(point->get_x(),point->get_y(),point->get_z());
			b_truthvec1= new TLorentzVector(tTrack1,pToE(tTrack1,kEmass));
			b_truthvec2= new TLorentzVector( tTrack2,pToE(tTrack2,kEmass));
			_tree->Fill();
		}
	}
	delete stack;
	return nullptr;
}
