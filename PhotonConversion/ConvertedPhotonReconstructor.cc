#include "ConvertedPhotonReconstructor.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <phool/PHDataNode.h>

#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4eval/SvtxEvalStack.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <iostream>
#include <cmath> 
#include <algorithm>
#include <sstream>

using namespace std;

//will decide how to do removal later 
ConvertedPhotonReconstructor::ConvertedPhotonReconstructor(const std::string &name) :
	SubsysReco("ConvertedPhotonReconstructor")
{
	this->name=name+"recovered.root";
	verbosity = 0;
	event=0;
	_file = new TFile( this->name.c_str(), "RECREATE");
	_tree = new TTree("convertedphotontree","tracks reconstructed to converted photons");
	//_tree->SetAutoSave(300);
	b_recovec1 = new TLorentzVector();
	b_recovec1 = new TLorentzVector();
	b_truthvec2 = new TLorentzVector();
	b_truthvec2 = new TLorentzVector();
	b_truthVertex = new TVector3();
	b_recoVertex = new  TVector3();
	_tree->Branch("status",&b_failed);
	_tree->Branch("hash",&hash);
	_tree->Branch("reco_tlv1",    "TLorentzVector",  &b_recovec1);
	_tree->Branch("reco_tlv2",    "TLorentzVector",  &b_recovec2);
	_tree->Branch("truth_tlv1",   "TLorentzVector", &b_truthvec1);
	_tree->Branch("truth_tlv2",   "TLorentzVector", &b_truthvec2);
	_tree->Branch("truth_vertex","TVector3",        &b_truthVertex);
	_tree->Branch("reco_vertex", "TVector3",        &b_recoVertex);
}

int ConvertedPhotonReconstructor::Init(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::InitRun(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::process_event(PHCompositeNode *topNode) {
	if (((verbosity > 0)&&(event%100==0))) {  
		cout << "ConvertedPhotonReconstructor::process_event - Event = " << event << endl;
	}
	//  ReconstructedConvertedPhoton* recovered=reconstruct(topNode);
	/*if(recovered){
		cout<<"recovered"<<endl;
	//recoveredPhotonVec.push_back(recovered);
	cout<<"pushed"<<endl;
	}
	else{
	cout<<"no recovery"<<endl;
	}*/
	b_failed=true;

  std::stringstream ss;
  ss<<event;             //this is where the file number is 
  hash=name.c_str()[name.length()-21]+ss.str();
	
	reconstruct(topNode);
	event++;
	cout<<"return event::ok"<<endl;
	return Fun4AllReturnCodes::EVENT_OK;
}

ConvertedPhotonReconstructor::~ConvertedPhotonReconstructor(){

	_file->Write();
	_file->Close();
	delete _file;
}

int ConvertedPhotonReconstructor::End(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

ReconstructedConvertedPhoton* ConvertedPhotonReconstructor::reconstruct(PHCompositeNode *topNode){
	//let the stack get the info from the node
	bool doubletrack=true;
	bool goodR=true;
	SvtxEvalStack *stack = new SvtxEvalStack(topNode);
	if(!stack){
		cout<<"Evaluator is null quiting photon recovery\n";
		return nullptr;
	}
	stack->set_strict(false); //no idea what this does 
	stack->set_verbosity(verbosity+1); //might be able to lower this 
	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
	SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	//SvtxClusterMap* clustermap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap"); 
	SvtxVertexEval* vertexeval = stack->get_vertex_eval();
	SvtxTrackEval* trackeval =   stack->get_track_eval();
	if(!vertexeval||!trackeval){
		cout<<"Evaluator is null quiting photon recovery\n";
		return nullptr;
	}
	cout<<"In reconstruct num vertex="<<vertexmap->size()<<endl;
	for (SvtxVertexMap::Iter iter = vertexmap->begin(); iter != vertexmap->end(); ++iter) {
		//cout<<"Enter loop"<<endl;
		SvtxVertex* vertex = iter->second;
		if(!vertex){
			cout<<"Vertex is null \n";
			continue;
		}
		float ntracks;
		ntracks= vertex->size_tracks();
		if(ntracks!=2){
			cout<<"Quiting photon recovery due to "<<ntracks<<" tracks\n";
			doubletrack=false;
		}
		float vx,vy,vz;
		vx = vertex->get_x();
		vy = vertex->get_y();
		vz = vertex->get_z();
		cout<<"Vertex:"<<vx<<", "<<vy<<", "<<vz<<'\n';
		if(sqrt(vx*vx+vy*vy+vz*vz)<1){
			goodR=false;
		}
		if (goodR&&doubletrack)
		{
			bool goodCharge=true;
			float charge1;
			SvtxVertex::TrackIter titer = vertex->begin_tracks(); 
			SvtxTrack* track = trackmap->get(*titer);
			if(!track){
				cout<<"null track quting photon recovery\n";
				continue;
			}
			charge1 = track->get_charge();
			if(abs(charge1)!=1){
				cout<<"Quiting photon recovery due to charge="<<charge1<<'\n';
				continue; //only considering electron positron conversion 
			}
			float t1x,t1y,t1z,t2x,t2y,t2z,charge2;
			t1x = track->get_px();
			t1y = track->get_py();
			t1z = track->get_pz();
			PHG4Particle* truth1 = trackeval->max_truth_particle_by_nclusters(track); 
			if(!truth1){
				cout<<"truth1 is null quting photon recovery \n";
				continue;
			}
			++titer;
			SvtxTrack* ftrack=track;
			track= trackmap->get(*titer);
			if(!track){
				cout<<"null track quting photon recovery\n";
				continue;
			}
			charge2 = track->get_charge();
			if(charge1!= -1*charge2){
				goodCharge=false;
			}
			t2x = track->get_px();
			t2y = track->get_py();
			t2z = track->get_pz();

			PHG4Particle* truth2 = trackeval->max_truth_particle_by_nclusters(track); 
			if(!truth2){
				cout<<"truth2 is null quting photon recovery \n";
				continue;
			}
			if (goodCharge)
			{
        b_failed=false;
				TVector3 track1(t1x,t1y,t1z),track2(t2x,t2y,t2z);
				PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(vertex); //not entirely sure what this does
				TVector3 tTrack1(truth1->get_px(),truth1->get_py(),truth1->get_pz()),
								 tTrack2(truth2->get_px(),truth2->get_py(),truth2->get_pz());

				b_recovec1= new TLorentzVector(track1,pToE(track1,kEmass));
				b_recovec2= new TLorentzVector(track2,pToE(track2,kEmass)); // make the tlv for the reco photon 
				b_recoVertex= new TVector3(vx,vy,vz);
				//do i care about the truth number of particles ?
				b_truthVertex= new TVector3(point->get_x(),point->get_y(),point->get_z());
				b_truthvec1= new TLorentzVector(tTrack1,pToE(tTrack1,kEmass));
				b_truthvec2= new TLorentzVector( tTrack2,pToE(tTrack2,kEmass));
				if(!ftrack->get_positive_charge()){ // will want to match these tracks to a truth particle 
					SvtxTrack* temp=ftrack;
					ftrack=track;
					track=temp;
				}
			}
		}
    _tree->Fill();
		delete stack;
		//return new ReconstructedConvertedPhoton(event,*b_recovec,*b_recoVertex,*b_truthvec,*b_truthVertex,ftrack,track,clustermap);

		// add the vector to the node tree 
		/* PHDataNode<std::vector<ReconstructedConvertedPhoton>>* vecNode = 
			 new PHDataNode<std::vector<ReconstructedConvertedPhoton>>(
			 reconstructedConvertedPhotons,"ReconstructedConvertedPhotons");
			 topNode->addNode(vecNode);*/
	}
	return nullptr;
}
/*void reconstructTracks(SvtxEvalStack *stack,PHCompositeNode *topNode){

	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
	SvtxVertexEval* vertexeval = stack->get_vertex_eval();
	SvtxTrackEval* trackeval =   stack->get_track_eval();
	std::vector<ReconstructedConvertedPhoton>* reconstructedConvertedPhotons=new std::vector<ReconstructedConvertedPhoton>();

	for (SvtxVertexMap::Iter iter = vertexmap->begin(); iter != vertexmap->end(); ++iter) {
	SvtxVertex* vertex = iter->second;
	float ntracks;
	ntracks= vertex->size_tracks();
	std::pair<SvtxTrack*,SvtxTrack*> trackPair = getTrackPair(vertex);
	float vx,vy,vz;
	vx = vertex->get_x();
	vy = vertex->get_y();
	vz = vertex->get_z();
	float charge1;
	SvtxTrack* track = titer->second;
	charge1 = track->get_charge();
	if(abs(charge1)!=1) continue; //only considering electron positron conversion 
	float t1x,t1y,t1z,t2x,t2y,t2z,charge2;
	t1x = track->get_px();
	t1y = track->get_py();
	t1z = track->get_pz();
	PHG4Particle* truth1 = trackeval->max_truth_particle_by_nclusters(track);	
	++titer;
	SvtxTrack* ftrack=track;
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

if(!ftrack->get_positive_charge()){ // will want ot match these tracks to a truth particle 
SvtxTrack* temp=ftrack;
ftrack=track;
track=temp;
}
reconstructedConvertedPhotons->push_back(
ReconstructedConvertedPhoton(event,b_recovec,b_recoVertex,b_truthvec,b_truthVertex,ftrack,track)
);
_tree->Fill();
// add the vector to the node tree 
PHDataNode<std::vector<ReconstructedConvertedPhoton>>* vecNode = 
new PHDataNode<std::vector<ReconstructedConvertedPhoton>>(
reconstructedConvertedPhotons,"ReconstructedConvertedPhotons");
topNode->addNode(vecNode);
}
}*/

/*std::pair<SvtxTrack*,SvtxTrack*> getTrackPair(SvtxVertex* vertex)const{
	std::pair<SvtxTrack*,SvtxTrack*> r;
	r.first=nullptr;
	r.second=nullptr;
	if (vertex->size_tracks()<2)
	{
	cout<<"Warning not enough tracks to pair\n";
	}
	else if(vertex->size_tracks()=2){
	TrackIter titer=vertex->begin_tracks();
	r.first = *(titer);
	titer++;
	r.second= *(titer);
	}
	else{
	TrackIter min=vertex->begin_tracks();
	for (TrackIter i = min; i != vertex->end_tracks(); ++i)
	{

	}
	}

	}*/
