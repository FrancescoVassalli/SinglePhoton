#include "ConvertedPhotonReconstructor.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>

#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>
//#include <g4hough/SvtxClusterMap.h>
//#include <g4hough/SvtxCluster.h>
//#include <g4hough/SvtxHitMap.h>
//#include <g4hough/SvtxHit.h>

//#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <iostream>
#include <cmath> //probably should switch from TMath to cmath
#include <algorithm>

using namespace std;

ConvertedPhotonReconstructor::ConvertedPhotonReconstructor(const string &name) :
  SubsysReco("ConvertedPhotonReconstructor")
{
  this->name=name;
  _svtxevalstack=nullptr;
  verbosity = 0;
  event=0;
  _file = new TFile( name.c_str(), "UPDATE");
  _tree = new TTree("conveteredphotontree","tracks reconstructed to converted photons");
  _tree->SetAutoSave(300);

  _tree->Branch("reco_tlv",     &b_recovec);
  _tree->Branch("truth_tlv",    &b_truthvec);
  _tree->Branch("truth_vertex", &b_truthVertex);
  _tree->Branch("reco_vertex",  &b_recoVertex);
}

int ConvertedPhotonReconstructor::Init(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::InitRun(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}
  
int ConvertedPhotonReconstructor::process_event(PHCompositeNode *topNode) {
  //progress report
  if ((verbosity > 0)&&(event%100==0)) {  
    cout << "ConvertedPhotonReconstructor::process_event - Event = " << event << endl;
  }
  //let the stack get the info from the node
  if (!_svtxevalstack) {
    _svtxevalstack = new SvtxEvalStack(topNode);
    _svtxevalstack->set_strict(false); //no idea what this does 
    _svtxevalstack->set_verbosity(verbosity+1); //might be able to lower this 
  } else {
    _svtxevalstack->next_event(topNode);
  }

  reconstruct(_svtxevalstack,topNode);
  event++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::End(PHCompositeNode *topNode) {
  _tree->Write();
  delete _svtxevalstack;
  
  return Fun4AllReturnCodes::EVENT_OK;
}

void ConvertedPhotonReconstructor::reconstruct(SvtxEvalStack *stack,PHCompositeNode *topNode){
	
	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
	SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	SvtxVertexEval* vertexeval = _svtxevalstack->get_vertex_eval();
	SvtxTrackEval* trackeval = _svtxevalstack->get_track_eval();


	for (SvtxVertexMap::Iter iter = vertexmap->begin(); iter != vertexmap->end(); ++iter) {
		SvtxVertex* vertex = iter->second;
		float ntracks;
		ntracks= vertex->size_tracks();
		if(ntracks!=2)continue; //now i assume thet there are only 2 tracks in the event 
		float vx,vy,vz;
		vx = vertex->get_x();
		vy = vertex->get_y();
		vz = vertex->get_z();
		float charge1;
		SvtxTrackMap::Iter titer = trackmap->begin(); 
		SvtxTrack* track = titer->second;
		charge1 = track->get_charge();
		if(abs(charge1)!=1) continue; //only considering electron positron conversion 
		float t1x,t1y,t1z,t2x,t2y,t2z,charge2;
		t1x = track->get_px();
		t1y = track->get_py();
		t1z = track->get_pz();
		PHG4Particle* truth1 = trackeval->max_truth_particle_by_nclusters(track);	
		++titer;
    track= titer->second;
		charge2 = track->get_charge();
		if(charge1!= -1*charge2)continue; //tracks must have opposite charge 
		t2x = track->get_px();
		t2y = track->get_py();
		t2z = track->get_pz();

		PHG4Particle* truth2 = trackeval->max_truth_particle_by_nclusters(track);	
		TVector3 track1(t1x,t1y,t1z),track2(t2x,t2y,t2z);
		PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(vertex); 
    TVector3 tTrack1(truth1->get_px(),truth1->get_py(),truth1->get_pz()),
             tTrack2(truth2->get_px(),truth2->get_py(),truth2->get_pz());

		
    b_recovec= TLorentzVector(track1,pToE(track1,kEmass))
        +TLorentzVector(track2,pToE(track2,kEmass)); // make the tlv for the reco photon 
    b_recoVertex=TVector3(vx,vy,vz);
    //do i care about the truth number of particles ?
    b_truthVertex=TVector3(point->get_x(),point->get_y(),point->get_z());
    b_truthvec= TLorentzVector(tTrack1,pToE(tTrack1,kEmass))
        +TLorentzVector( tTrack2,pToE(tTrack2,kEmass));


		reconstructedConvertedPhotons.push_back(
			ReconstructedConvertedPhoton(event,b_recovec,b_recoVertex,b_truthvec,b_truthVertex)
		);
    _tree->Fill();
	}
}
