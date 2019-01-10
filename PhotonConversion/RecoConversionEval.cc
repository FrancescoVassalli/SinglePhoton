#include "RecoConversionEval.h"

#include <fun4all/Fun4AllReturnCodes.h>


#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>
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
	doNodePointers(topNode);
  int bigLoopCount=0;
  for ( SvtxTrackMap::Iter iter = _allTracks->begin(); iter != _allTracks->end(); ++bigLoopCount) {
    SvtxTrack* thisTrack = iter->second;
    //I want to now only check e tracks so check the clusters of the |charge|=1 tracks
    if (abs(thisTrack->get_charge())==1)
    {
      RawCluster* bestCluster= _mainClusterContainer->getCluster(thisTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      //what if not cluster is found?
      if(bestCluster&&bestCluster->get_prob()>_kEMProbCut){
      	//loop over the following tracks
      	for (SvtxTrackMap::Iter jter = ++iter; jter != _allTracks->end(); ++jter)
      	{
      		SvtxTrack* nextTrack = jter->second;
      		//check that the next track is an opposite charge electron
      		if (thisTrack->get_charge()*-1==nextTrack->get_charge())
      		{
      			RawCluster* nextCluster= _mainClusterContainer->getCluster(thisTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      			//what if not cluster is found?
      			if(nextCluster&&nextCluster->get_prob()>_kEMProbCut){
      				pairCuts(thisTrack,nextTrack);
      			}
      		}
      	}
      }
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

RecoConversionEval::~RecoConversionEval(){

}

int RecoConversionEval::End(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}

void RecoConversionEval::process_recoTracks(PHCompositeNode *topNode){

}

