#include "RecoConversionEval.h"
#include "SVReco.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterv1.h>
#include <g4eval/SvtxEvalStack.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>


#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>

#include <iostream>
#include <cmath> 
#include <algorithm>
#include <sstream>

using namespace std;

RecoConversionEval::RecoConversionEval(const std::string &name,std::string tmvamethod,std::string tmvapath) :
	SubsysReco("RecoConversionEval"), _fname(name) 
{
	_regressor = new VtxRegressor(tmvamethod,tmvapath);
}

RecoConversionEval::~RecoConversionEval(){
	if(_vertexer) delete _vertexer;
	if(_regressor) delete _regressor;
}

int RecoConversionEval::Init(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int RecoConversionEval::InitRun(PHCompositeNode *topNode) {
	_vertexer = new SVReco();
	//TODO turn this back into a subsystem and put it on the node tree
	_vertexer->InitRun(topNode);
	_file = new TFile( _fname.c_str(), "RECREATE");
	_tree = new TTree("recoSignal","strong saharah bush");
	_tree = new TTree("cutTreeSignal","signal data for making track pair cuts");
    _tree->SetAutoSave(100);
    _tree->Branch("photon_m",   &_b_photon_m);
    _tree->Branch("photon_pT",  &_b_photon_pT);
    _tree->Branch("photon_eta", &_b_photon_eta);
    _tree->Branch("photon_phi", &_b_photon_phi);
    _tree->Branch("tphoton_pT",  &_b_tphoton_pT);
    _tree->Branch("tphoton_eta", &_b_tphoton_eta);
    _tree->Branch("tphoton_phi", &_b_tphoton_phi);
    _tree->Branch("fake", &_b_fake);
    
	return Fun4AllReturnCodes::EVENT_OK;
}

void RecoConversionEval::doNodePointers(PHCompositeNode *topNode){
	_allTracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	_mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
	/*These are deprecated
	 * _svtxClusterMap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap");
	 _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");*/
	//new version
	_clusterMap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
	_vertexer->InitEvent(topNode);
	//to check if the id is correct
 	_truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");

}

bool RecoConversionEval::hasNodePointers()const{
	return _allTracks &&_mainClusterContainer && _vertexer;
}

int RecoConversionEval::process_event(PHCompositeNode *topNode) {
	doNodePointers(topNode);
	cout<<"Did pointers: \n \n";
	/*the is not optimized but is just a nlogn process*/
	for ( SvtxTrackMap::Iter iter = _allTracks->begin(); iter != _allTracks->end(); ++iter) {
		//I want to now only check e tracks so check the clusters of the |charge|=1 tracks
		if (abs(iter->second->get_charge())==1&&iter->second->get_pt()>_kTrackPtCut) //TODO dca cut
		{
			SvtxTrack* thisTrack = iter->second;
			RawCluster* bestCluster= _mainClusterContainer->getCluster(thisTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
			//TODO what if no cluster is found?
			if(bestCluster&&bestCluster->get_prob()>_kEMProbCut){
				//loop over the following tracks
				for (SvtxTrackMap::Iter jter = iter; jter != _allTracks->end(); ++jter)
				{
					//check that the next track is an opposite charge electron
					if (thisTrack->get_charge()*-1==jter->second->get_charge()&&jter->second->get_pt()>_kTrackPtCut)
					{
						RawCluster* nextCluster= _mainClusterContainer->getCluster(jter->second->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
						//what if no cluster is found?
						if(nextCluster&&nextCluster->get_prob()>_kEMProbCut&&pairCuts(thisTrack,jter->second)){
							genfit::GFRaveVertex* vtxCan = _vertexer->findSecondaryVertex(thisTrack,jter->second);
							vtxCan=correctSecondaryVertex(vtxCan,thisTrack,jter->second);
							if (vtxCan&&vtxCuts(vtxCan))
							{
								_b_fake=false;
								std::pair<PHGenFit::Track*,PHGenFit::Track*> refit_tracks = refitTracks(vtxCan,thisTrack,jter->second);
								TLorentzVector* photon;
								if (refit_tracks.first&&refit_tracks.second)
								{
									photon = reconstructPhoton(refit_tracks);
								}
								else{
									photon = reconstructPhoton(thisTrack,jter->second);
								}
								_b_photon_m = photon->Dot(*photon);
								_b_photon_pT = photon->Pt();
								_b_photon_eta = photon->Eta();
								_b_photon_phi = photon->Phi();
								delete photon;
								PHG4Particle* truthparticle = _truthinfo->GetParticle(thisTrack->get_truth_track_id());
								PHG4Particle* parent = _truthinfo->GetParticle(truthparticle->get_parent_id());
								if(TMath::Abs(truthparticle->get_pid())!=11||!parent||parent->get_pid()!=22){
									_b_fake=true;
								}
								else if(parent&&parent->get_pid()==22){
									_b_tphoton_phi = parent->get_phi();
									_b_tphoton_eta = parent->get_eta();
									_b_tphoton_pT = parent->get_pt();
								}
								_tree->Fill();
							}//vtx cuts
						}
					}
				}
			}
		}
	}
	return Fun4AllReturnCodes::EVENT_OK;
}

genfit::GFRaveVertex* RecoConversionEval::correctSecondaryVertex(genfit::GFRaveVertex* vtx,SvtxTrack* reco1,SvtxTrack* reco2){
  if(!(recoVertex&&reco1&&reco2)) {
    return vtx;
  }

  TVector3 nextPos = recoVertex->getPos();
  nextPos.SetMagThetaPhi(_regressor->regress(reco1,reco2,recoVertex),nextPos.Theta(),nextPos.Phi());

  using namespace genfit;
 // GFRaveVertex* temp = recoVertex;
  std::vector<GFRaveTrackParameters*> tracks;
  for(unsigned i =0; i<recoVertex->getNTracks();i++){
    tracks.push_back(recoVertex->getParameters(i));
  }
  recoVertex = new GFRaveVertex(nextPos,recoVertex->getCov(),tracks,recoVertex->getNdf(),recoVertex->getChi2(),recoVertex->getId());
//  delete temp; //this caused outside references to seg fault //TODO shared_ptr is better 
  return recoVertex;
}

TLorentzVector RecoConversionEval::reconstructPhoton(std::pair<PHGenFit::Track*,PHGenFit::Track*> recos){
if (reco1&&reco2)
  {
    TLorentzVector tlv1();
    tlv1.SetVectM(recos.first->getMom(),_kElectronRestM);
    TLorentzVector tlv2();
    tlv2.SetVectM(recos.second->getMom(),_kElectronRestM);
    return new TLorentzVector(tlv1+tlv2);
  }
  else return NULL;
}

TLorentzVector* RecoConversionEval::reconstructPhoton(SvtxTrack* reco1,SvtxTrack* reco2){
if (reco1&&reco2)
  {
    TLorentzVector tlv1(reco1->get_px(),reco1->get_py(),reco1->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco1->get_p()*reco1->get_p()));
    TLorentzVector tlv2(reco2->get_px(),reco2->get_py(),reco2->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco2->get_p()*reco2->get_p()));
    if (recoPhoton) delete recoPhoton;
    return new TLorentzVector(tlv1+tlv2);
  }
  else return NULL;
}

std::pair<PHGenFit::Track*,PHGenFit::Track*> RecoConversionEval::refitTracks(genfit::GFRaveVertex* vtx,SvtxTrack* reco1,SvtxTrack* reco2){
	std::pair<PHGenFit::Track*,PHGenFit::Track*> r;
  if(!vtx)
  {
    cerr<<"WARNING: No vertex to refit tracks"<<endl;
    r.first=NULL;
    r.second=NULL;

  }
  else{
    r.first=_vertexer->refitTrack(vtx,reco1);
    r.second=_vertexer->refitTrack(vtx,reco2);
  }
  return r;
}

bool RecoConversionEval::pairCuts(SvtxTrack* t1, SvtxTrack* t2)const{
	return detaCut(t1->get_eta(),t2->get_eta()) && hitCuts(t1,t2); //TODO add approach distance ?
}

bool RecoConversionEval::hitCuts(SvtxTrack* reco1, SvtxTrack* reco2)const {
	TrkrCluster *c1 = _clusterMap->findCluster(*(reco1->begin_cluster_keys()));
	TrkrCluster *c2 = _clusterMap->findCluster(*(reco2->begin_cluster_keys()));
	unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
	unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
	//check that the first hits are close enough
	if (l1>_kNSiliconLayer&&l1>_kNSiliconLayer)
	{
		if (abs(l1-l2)>_kFirstHitStrict)
		{
			return false;
		}
	}
	else{
		if (abs(l1-l2)>_kFirstHit)
		{
			return false;
		}
	}
	return true;
}

bool RecoConversionEval::vtxCuts(genfit::GFRaveVertex* vtxCan, SvtxTrack* t1, SvtxTrack *t2){
	//TODO program the cuts invariant mass, pT
	return vtxRadiusCut(vtxCan->getPos());
	// && vtxTrackRPhiCut(vtxCan->getPos(),t1)&&vtxTrackRPhiCut(vtxCan->getPos(),t2)&& 
		//vtxTrackRZCut(vtxCan->getPos(),t1)&&vtxTrackRZCut(vtxCan->getPos(),t2)&&vtxCan->getChi2()>_kVtxChi2Cut;
}

bool RecoConversionEval::vtxTrackRZCut(TVector3 recoVertPos, SvtxTrack* track){
	float dR = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y())-sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
	float dZ = recoVertPos.z()-track->get_z();
	return sqrt(dR*dR+dZ*dZ)<_kVtxRZCut;
}

//bool RecoConversionEval::invariantMassCut()

bool RecoConversionEval::vtxTrackRPhiCut(TVector3 recoVertPos, SvtxTrack* track){
	float vtxR=sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
	float trackR=sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
	return sqrt(vtxR*vtxR+trackR*trackR-2*vtxR*trackR*cos(recoVertPos.Phi()-track->get_phi()))<_kVtxRPhiCut;
}

bool RecoConversionEval::vtxRadiusCut(TVector3 recoVertPos){
	return sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y()) > _kVtxRCut;
}

int RecoConversionEval::End(PHCompositeNode *topNode) {
	if(_file){
		_file->Write();
		_file->Close();
	}
	return Fun4AllReturnCodes::EVENT_OK;
}

bool RecoConversionEval::approachDistance(SvtxTrack *t1,SvtxTrack* t2)const{
	static const double eps = 0.000001;
	TVector3 u(t1->get_px(),t1->get_py(),t1->get_pz());
	TVector3 v(t2->get_px(),t2->get_py(),t2->get_pz());
	TVector3 w(t1->get_x()-t2->get_x(),t1->get_x()-t2->get_y(),t1->get_x()-t2->get_z());

	double a = u.Dot(u);
	double b = u.Dot(v);
	double c = v.Dot(v);
	double d = u.Dot(w);
	double e = v.Dot(w);

	double D = a*c - b*b;
	double sc, tc;
	// compute the line parameters of the two closest points
	if (D < eps) {         // the lines are almost parallel
		sc = 0.0;
		tc = (b>c ? d/b : e/c);   // use the largest denominator
	}
	else {
		sc = (b*e - c*d) / D;
		tc = (a*e - b*d) / D;
	}
	// get the difference of the two closest points
	u*=sc;
	v*=tc;
	w+=u;
	w-=v;
	return w.Mag()<=_kApprochCut;   // return the closest distance 
}

