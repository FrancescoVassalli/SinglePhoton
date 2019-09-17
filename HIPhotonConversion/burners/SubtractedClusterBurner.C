#include "SubtractedClusterBurner.h"

SubtractedClusterBurner::SubtractedClusterBurner(const std::string &name) : SubsysReco("SubtractedClusterBurner"),
	_kRunNumber(runnumber)
{
	_foutname = name;
}

SubtractedClusterBurner::~SubtractedClusterBurner(){
	if (_f) delete _f;
	if (_vertexer) delete _vertexer;
	if(_regressor) delete _regressor;
}

int SubtractedClusterBurner::InitRun(PHCompositeNode *topNode)
{

	//_f = new TFile( _foutname.c_str(), "RECREATE");
	//_observTree = new TTree("observTree","per event observables");

	return 0;
}

bool SubtractedClusterBurner::doNodePointers(PHCompositeNode* topNode){
	/*bool goodPointers=true;
		_mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
		_truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
		_clusterMap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
		_allTracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	//  _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");
	//if(!_mainClusterContainer||!_truthinfo||!_clusterMap||!_hitMap){
	if(!_mainClusterContainer||!_truthinfo||!_clusterMap||!_allTracks){
	cerr<<Name()<<": critical error-bad nodes \n";
	if(!_mainClusterContainer){
	cerr<<"\t RawClusterContainer is bad";
	}
	if(!_truthinfo){
	cerr<<"\t TruthInfoContainer is bad";
	}
	if(!_clusterMap){
	cerr<<"\t TrkrClusterMap is bad";
	}
	if(!_allTracks){
	cerr<<"\t SvtxTrackMap is bad";
	}
	cerr<<endl;
	goodPointers=false;
	}*/
	return goodPointers;
}

int SubtractedClusterBurner::process_event(PHCompositeNode *topNode)
{

	return 0;
}


int SubtractedClusterBurner::End(PHCompositeNode *topNode)
{
	cout<<"closing"<<endl;
	//_signalCutTree->Write();
	//_f->Write();
	//_f->Close();
	return 0;
}
