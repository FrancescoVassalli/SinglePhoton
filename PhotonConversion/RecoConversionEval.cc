#include "RecoConversionEval.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <phool/PHDataNode.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

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
  RawClusterContainer* mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  for ( SvtxTrackMap::Iter iter = allTracks->begin(); iter != allTracks->end(); ++iter ) {
    SvtxTrack* thisTrack = iter->second;
    //I want to now only check e tracks so check the clusters of the |charge|=1 tracks
    if (abs(thisTrack->get_charge())==1)
    {
      RawCluster* bestcluster= mainClusterContainer->getCluster(thisTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      if(bestcluster){
        cout<<"cluster prob="<<bestcluster->get_prob()<<'\n';
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

