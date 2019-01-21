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
	cout<<"Did pointers: \n \n";
  int bigLoopCount=0;
/*the is not optimized but is just a nlogn process*/
  for ( SvtxTrackMap::Iter iter = _allTracks->begin(); iter != _allTracks->end(); ++bigLoopCount) {
    //I want to now only check e tracks so check the clusters of the |charge|=1 tracks
    if (abs(iter->second->get_charge())==1&&iter->second->get_pt()>_kTrackPtCut)
    {
    	SvtxTrack* thisTrack = iter->second;
      RawCluster* bestCluster= _mainClusterContainer->getCluster(thisTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      //what if not cluster is found?
      if(bestCluster&&bestCluster->get_prob()>_kEMProbCut){
      	//loop over the following tracks
      	for (SvtxTrackMap::Iter jter = iter; jter != _allTracks->end(); ++jter)
      	{
      		//check that the next track is an opposite charge electron
      		if (thisTrack->get_charge()*-1==jter->second->get_charge()&&jter->second->get_pt()>_kTrackPtCut)
      		{
      			RawCluster* nextCluster= _mainClusterContainer->getCluster(jter->second->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      			//what if not cluster is found?
      			if(nextCluster&&nextCluster->get_prob()>_kEMProbCut&&pairCuts(thisTrack,jter->second)){
      				SvtxVertex* vtxCan= _auxVertexer->makeVtx(thisTrack,jter->second);
              if (vtxCan&&vtxCuts(vtxCan))
              {
                /* code */
              }
      			}
      		}
      	}
      }
    }
    ++iter;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

bool RecoConversionEval::vtxCuts(SvtxVertex *vtx){
  return true;
}

int RecoConversionEval::End(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}

void RecoConversionEval::process_recoTracks(PHCompositeNode *topNode){

}

