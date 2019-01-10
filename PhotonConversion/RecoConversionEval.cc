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
		SvtxTrack* thisTrack = iter->second;
		/*there's a lot of track functions that I don't really know what they do
		chisq()
		error()
		id()
		p()
		pos()
		*/ 
		if (abs(thisTrack->get_charge())==1)//I want to now only check e tracks
		{
			cout<<"Charged Track:\n \tchi:"<<thisTrack->get_chisq()<<"\n \tp:"<<thisTrack->get_p()<<"\n \tid:"<<thisTrack->get_id()<<"\n \tpos:"<<thisTrack->get_pos(11)<<'\n';
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

