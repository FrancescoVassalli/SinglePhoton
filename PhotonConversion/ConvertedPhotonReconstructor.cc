
#include "ConvertedPhotonReconstructor.h"

#include "SvtxEvalStack.h"

#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>

#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxCluster.h>
#include <g4hough/SvtxHitMap.h>
#include <g4hough/SvtxHit.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CylinderCellGeom.h>
#include <g4detectors/PHG4CylinderCellGeomContainer.h>

#include <iostream>
#include <set>
#include <cmath>
#include <cassert>
#include <algorithm>

using namespace std;

ConvertedPhotonReconstructor::ConvertedPhotonReconstructor(const string &name) :
  SubsysReco("ConvertedPhotonReconstructor"),
  _svtxevalstack(nullptr),
{
  verbosity = 0;
  event=0;
}

int ConvertedPhotonReconstructor::Init(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::InitRun(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}
  
int ConvertedPhotonReconstructor::process_event(PHCompositeNode *topNode) {
  //progress report
  if ((verbosity > 0)&&(_ievent%100==0)) {  
    cout << "ConvertedPhotonReconstructor::process_event - Event = " << _ievent << endl;
  }
  //let the stack get the info from the node
  if (!_svtxevalstack) {
    _svtxevalstack = new SvtxEvalStack(topNode);
    _svtxevalstack->set_strict(false); //no idea what this does 
    _svtxevalstack->set_verbosity(verbosity+1); //might be able to lower this 
  } else {
    _svtxevalstack->next_event(topNode);
  }

  reconstruct(_svtxevalstack);
  event++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::End(PHCompositeNode *topNode) {

  delete _svtxevalstack;
  
  return Fun4AllReturnCodes::EVENT_OK;
}

void ConvertedPhotonReconstructor::reconstruct(SvtxEvalStack *stack){
	
	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
	SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,_trackmapname.c_str());
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
		SvtxTrackMap::Iter iter = trackmap->begin(); 
		SvtxTrack* track = iter->second;
		charge1 = track->get_charge();
		if(abs(charge1)!=1) continue; //only considering electron positron conversion 
		float t1x,t1y,t1z,t2x,t2y,t2z,charge2;
		t1x = track->get_px();
		t1y = track->get_py();
		t1z = track->get_pz();
		PHG4Particle* truth1 = trackeval->max_truth_particle_by_nclusters(track);	
		++iter;
		charge2 = track->get_charge();
		if(charge1!= -1*charge2)continue; //tracks must have opposite charge 
		t2x = track->get_px();
		t2y = track->get_py();
		t2z = track->get_pz();
		PHG4Particle* truth2 = trackeval->max_truth_particle_by_nclusters(track);	
		TVector3 track1(t1x,t1y,t1z),track2(t1x,t1y,t1z);
		TLorentzVector recotlv(
			TLorentzVector(track1,pToE(track1,kEmass))
			+TLorentzVector(track2,pToE(track2,kEmass))
		); // make the tlv for the reco photon 

		PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(vertex); 
		//do i care about the truth number of particles ?
		TVector3 truthConversionVertex(point->get_x(),point->get_y(),point->get_z());
		TVector3 truthtlv(TLorentzVector(truth1,pToE(truth1,kEmass))
			+TLorentzVector(truth2,pToE(truth2,kEmass))
		);


		reconstructedConvertedPhotons.push_back(
			ReconstructedConvertedPhoton(event,recotlv,truthtlv,truthConversionVertex)
		);
	}
}
