#include "TruthConversionEval.h"
#include "Conversion.h"
#include "SVReco.h"
//#include "../PhotonConversion/RaveVertexingAux.h"

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calotrigger/CaloTriggerInfo.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Particlev1.h>
#include <g4main/PHG4Particlev2.h>
#include <g4main/PHG4VtxPoint.h>

/*#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>*/
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrCluster.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>

//#include <GenFit/GFRaveConverters.h>
#include <GenFit/FieldManager.h>
#include <GenFit/GFRaveVertex.h>
#include <GenFit/GFRaveVertexFactory.h>
#include <GenFit/MeasuredStateOnPlane.h>
#include <GenFit/RKTrackRep.h>
#include <GenFit/StateOnPlane.h>
#include <GenFit/Track.h>

#include <phgenfit/Track.h>


#include <fun4all/Fun4AllReturnCodes.h>

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>

#include <utility>
#include <iostream>
#include <math.h>

TruthConversionEval::TruthConversionEval(const std::string &name, unsigned int runnumber, 
    int particleEmbed,  int pythiaEmbed,bool makeTTree=true) : SubsysReco("TruthConversionEval"),
  _kRunNumber(runnumber),_kParticleEmbed(particleEmbed), _kPythiaEmbed(pythiaEmbed), _kMakeTTree(makeTTree)
{
  _foutname = name;
}

TruthConversionEval::~TruthConversionEval(){
  if (_f) delete _f;
  if (_vertexer) delete _vertexer;
}

int TruthConversionEval::InitRun(PHCompositeNode *topNode)
{
  _vertexer = new SVReco();
  _vertexer->InitRun(topNode);
  if(_kMakeTTree){
    _runNumber=_kRunNumber;
    _f = new TFile( _foutname.c_str(), "RECREATE");

    _vtxingTree = new TTree("vtxingTree","data predicting vtx from track pair");
    _vtxingTree->SetAutoSave(300);
    _vtxingTree->Branch("vtx_radius", &_b_vtx_radius);
    _vtxingTree->Branch("tvtx_radius", &_b_tvtx_radius);
    _vtxingTree->Branch("vtx_phi", &_b_vtx_phi);
    _vtxingTree->Branch("vtx_eta", &_b_vtx_eta);
    _vtxingTree->Branch("vtx_x", &_b_vtx_x);
    _vtxingTree->Branch("vtx_y", &_b_vtx_y);
    _vtxingTree->Branch("vtx_z", &_b_vtx_z);
    _vtxingTree->Branch("tvtx_eta", &_b_tvtx_eta);
    _vtxingTree->Branch("tvtx_x", &_b_tvtx_x);
    _vtxingTree->Branch("tvtx_y", &_b_tvtx_y);
    _vtxingTree->Branch("tvtx_z", &_b_tvtx_z);
    _vtxingTree->Branch("tvtx_phi", &_b_tvtx_phi);
    _vtxingTree->Branch("vtx_chi2", &_b_vtx_chi2);
    _vtxingTree->Branch("track1_pt", &_b_track1_pt,"track1_pt");
    _vtxingTree->Branch("track1_eta",& _b_track1_eta,"track1_eta");
    _vtxingTree->Branch("track1_phi",& _b_track1_phi,"track1_phi");
    _vtxingTree->Branch("track2_pt", &_b_track2_pt,"track2_pt");
    _vtxingTree->Branch("track2_eta",& _b_track2_eta,"track2_eta");
    _vtxingTree->Branch("track2_phi",& _b_track2_phi,"track2_phi");

    _trackBackTree = new TTree("trackBackTree","track background all single tracks");
    _trackBackTree->SetAutoSave(300);
    _trackBackTree->Branch("track_dca", &_bb_track_dca);
    _trackBackTree->Branch("track_pT",  &_bb_track_pT);
    _trackBackTree->Branch("track_layer", &_bb_track_layer);
    _trackBackTree->Branch("cluster_prob", &_bb_cluster_prob);

    _pairBackTree = new TTree("pairBackTree","pair background all possible combinations");
    _pairBackTree->SetAutoSave(300);
    _pairBackTree->Branch("track_deta", &_bb_track_deta);
    _pairBackTree->Branch("track2_pid", &_bb_track2_pid);
    _pairBackTree->Branch("track1_pid", &_bb_track1_pid);
    _pairBackTree->Branch("track_dphi", &_bb_track_dphi);
    _pairBackTree->Branch("track_dlayer",&_bb_track_dlayer);
    _pairBackTree->Branch("approach_dist", &_bb_approach);
    _pairBackTree->Branch("track_dca", &_bb_track_dca);
    _pairBackTree->Branch("track_pT",  &_bb_track_pT);
    _pairBackTree->Branch("track_layer", &_bb_track_layer);
    _pairBackTree->Branch("cluster_prob", &_bb_cluster_prob);
    _pairBackTree->Branch("nCluster", &_bb_nCluster);
    _pairBackTree->Branch("cluster_dphi", &_bb_cluster_dphi);
    _pairBackTree->Branch("cluster_deta", &_bb_cluster_deta);

    _vtxBackTree = new TTree("vtxBackTree","events that pass track pair cuts");
    _vtxBackTree->SetAutoSave(300);
    _vtxBackTree->Branch("track_deta", &_bb_track_deta);
    _vtxBackTree->Branch("track1_pid", &_bb_track1_pid);
    _vtxBackTree->Branch("track2_pid", &_bb_track2_pid);
    _vtxBackTree->Branch("track_dphi", &_bb_track_dphi);
    _vtxBackTree->Branch("track_dlayer",&_bb_track_dlayer);
    _vtxBackTree->Branch("approach_dist", &_bb_approach);
    _vtxBackTree->Branch("track_dca", &_bb_track_dca);
    _vtxBackTree->Branch("track_pT",  &_bb_track_pT);
    _vtxBackTree->Branch("track_layer", &_bb_track_layer);
    _vtxBackTree->Branch("cluster_prob", &_bb_cluster_prob);
    _vtxBackTree->Branch("vtx_radius",&_bb_vtx_radius);
    _vtxBackTree->Branch("photon_m",&_bb_photon_m);
    _vtxBackTree->Branch("photon_pT",&_bb_photon_pT);
    _vtxBackTree->Branch("nCluster", &_bb_nCluster);
    _vtxBackTree->Branch("cluster_dphi", &_bb_cluster_dphi);
    _vtxBackTree->Branch("cluster_deta", &_bb_cluster_deta);

    _signalCutTree = new TTree("cutTreeSignal","signal data for making track pair cuts");
    _signalCutTree->SetAutoSave(100);
    _signalCutTree->Branch("track_deta", &_b_track_deta);
    _signalCutTree->Branch("track_dca", &_b_track_dca);
    _signalCutTree->Branch("track_dphi", &_b_track_dphi);
    _signalCutTree->Branch("track_dlayer",&_b_track_dlayer);
    _signalCutTree->Branch("track_layer", &_b_track_layer);
    _signalCutTree->Branch("track_pT", &_b_track_pT);
    //_signalCutTree->Branch("ttrack_pT", &_b_ttrack_pT);
    _signalCutTree->Branch("approach_dist", &_b_approach);
    _signalCutTree->Branch("vtx_radius", &_b_vtx_radius);
    _signalCutTree->Branch("vtx_chi2", &_b_vtx_chi2);
    //_signalCutTree->Branch("vtxTrackRZ_dist", &_b_vtxTrackRZ_dist);
    //_signalCutTree->Branch("vtxTrackRPhi_dist", &_b_vtxTrackRPhi_dist);
    _signalCutTree->Branch("photon_m", &_b_photon_m);
    _signalCutTree->Branch("rephoton_m", &_b_rephoton_m);
    //_signalCutTree->Branch("tphoton_m", &_b_tphoton_m);
    _signalCutTree->Branch("photon_pT", &_b_photon_pT);
    _signalCutTree->Branch("cluster_prob", &_b_cluster_prob);
    //_signalCutTree->Branch("nCluster", &_b_nCluster);
    _signalCutTree->Branch("cluster_dphi", &_b_cluster_dphi);
    _signalCutTree->Branch("cluster_deta", &_b_cluster_deta);
    _signalCutTree->Branch("refitdiffx",&_b_refitdiffx);
    _signalCutTree->Branch("refitdiffy",&_b_refitdiffy);
    _signalCutTree->Branch("refitdiffz",&_b_refitdiffz);
  }
  return 0;
}

bool TruthConversionEval::doNodePointers(PHCompositeNode* topNode){
  bool goodPointers=true;
  _mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  _truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  _clusterMap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  //  _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");
  //if(!_mainClusterContainer||!_truthinfo||!_clusterMap||!_hitMap){
  if(!_mainClusterContainer||!_truthinfo||!_clusterMap){
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
    /*if(!_hitMap){
      cerr<<"\t SvtxHitMap is bad";
      }*/
    cerr<<endl;
    goodPointers=false;
  }
  return goodPointers;
}

SvtxVertex* TruthConversionEval::get_primary_vertex(PHCompositeNode *topNode)const{
  SvtxVertexMap *vertexMap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  return vertexMap->get(0);
}

int TruthConversionEval::process_event(PHCompositeNode *topNode)
{
  if(!doNodePointers(topNode)) return Fun4AllReturnCodes::ABORTEVENT;
  _vertexer->InitEvent(topNode);
  _conversionClusters.Reset(); //clear the list of conversion clusters
  PHG4TruthInfoContainer::Range range = _truthinfo->GetParticleRange(); //look at all truth particles
  SvtxEvalStack *stack = new SvtxEvalStack(topNode); //truth tracking info
  SvtxTrackEval* trackeval = stack->get_track_eval();
  if (!trackeval)
  {
    cerr<<"NULL track eval in " <<Name()<<" fatal error"<<endl;
    return 1;
  }
  //make a map of the conversions
  std::map<int,Conversion> mapConversions;
  //h is for hadronic e is for EM
  std::map<int,Conversion> hbackgroundMap;
  std::map<int,Conversion> ebackgroundMap;
  std::vector<SvtxTrack*> backgroundTracks;
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second;
    PHG4Particle* parent =_truthinfo->GetParticle(g4particle->get_parent_id());
    //cout<<"partent id:"<<g4particle->get_parent_id()<<endl;
    //cout<<g4particle->get_track_id()<<endl;
    PHG4VtxPoint* vtx=_truthinfo->GetVtx(g4particle->get_vtx_id()); //get the vertex
    if(!vtx){
      cout<<"null vtx primaryid="<<g4particle->get_primary_id()<<'\n';
      g4particle->identify();
      cout<<endl;
      continue;
    }
    float radius=sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
    //if outside the tracker skip this
    if(radius>s_kTPCRADIUS) continue;
    //cout<<"got vtx with r="<<radius<<endl;
    int embedID;
    if (parent)//if the particle is not primary
    {
      embedID=get_embed(parent,_truthinfo);
      if(parent->get_pid()==22&&TMath::Abs(g4particle->get_pid())==11){ //conversion check
        if (Verbosity()==10)
        {
          std::cout<<"Conversion with radius [cm]:"<<radius<<'\n';
        }
        //initialize the conversion object -don't use constructor b/c setters have error handling
        (mapConversions[vtx->get_id()]).setElectron(g4particle);
        (mapConversions[vtx->get_id()]).setVtx(vtx);
        (mapConversions[vtx->get_id()]).setParent(parent);
        (mapConversions[vtx->get_id()]).setEmbed(embedID);
        PHG4Particle* grand =_truthinfo->GetParticle(parent->get_parent_id()); //grandparent
        if (grand) (mapConversions[vtx->get_id()]).setSourceId(grand->get_pid());//record pid of the photon's source
        else (mapConversions[vtx->get_id()]).setSourceId(0);//or it is from the G4 generator?
      }
      else if(_kMakeTTree){//not a conversion
        SvtxTrack *testTrack = trackeval->best_track_from(g4particle);
        backgroundTracks.push_back(testTrack);
      }//make tree
    }// not primary 
    else if(_kMakeTTree){ //is primary therefore not a conversion 
      embedID=get_embed(g4particle,_truthinfo);
      //cout<<"primary particle"<<endl;
      SvtxTrack *testTrack = trackeval->best_track_from(g4particle);
      backgroundTracks.push_back(testTrack);
    }//make tree
  }//truth particle loop
  //pass the map to this helper method which fills the fields for the TTree 
  numUnique(&mapConversions,trackeval,_mainClusterContainer,topNode);
  if (Verbosity()==10)
  {
    cout<<Name()<<"# conversion clusters="<<_conversionClusters.size()<<'\n';
  }
  if (_kMakeTTree)
  {
    processTrackBackground(&backgroundTracks,_clusterMap);
  }
  delete stack;
  return 0;
}

void TruthConversionEval::numUnique(std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL,RawClusterContainer *mainClusterContainer=NULL,PHCompositeNode *topNode=NULL){
  for (std::map<int,Conversion>::iterator i = mymap->begin(); i != mymap->end(); ++i) {
    PHG4VtxPoint *vtx =i->second.getVtx(); //get the vtx
    PHG4Particle *temp = i->second.getPhoton(); //set the photon
    TLorentzVector tlv_photon,tlv_electron,tlv_positron; //make tlv for each particle 
    tlv_photon.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //intialize
    temp=i->second.getElectron(); //set the first child 
    tlv_electron.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
    temp=i->second.getPositron();
    if(temp){ //this will be false for conversions with 1 truth track
      tlv_positron.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //init the tlv
      if (TMath::Abs(tlv_electron.Eta())<_kRAPIDITYACCEPT&&TMath::Abs(tlv_positron.Eta())<_kRAPIDITYACCEPT)
      {
        unsigned int nRecoTracks = i->second.setRecoTracks(trackeval); //find the reco tracks for this conversion
        pair<float,float> pTstemp = i->second.getTrackpTs();
        switch(nRecoTracks)
        {
          case 2: //there are 2 reco tracks
            {
              if(_kMakeTTree){
                _b_track_deta = i->second.trackDEta();
                _b_track_dphi = i->second.trackDPhi();
                _b_track_dlayer = i->second.trackDLayer(_clusterMap);
                _b_track_layer = i->second.firstLayer(_clusterMap);
                _b_track_pT = i->second.minTrackpT();
                if(tlv_electron.Pt()>tlv_positron.Pt()) _b_ttrack_pT = tlv_positron.Pt();
                else _b_ttrack_pT = tlv_electron.Pt();
                _b_approach = i->second.approachDistance();
                _b_track_dca = i->second.minDca();
                TLorentzVector* recoPhoton = i->second.getRecoPhoton();
                PHG4Particle* truthphoton = i->second.getTruthPhoton(_truthinfo);
                TLorentzVector tlv_tphoton;
                if(truthphoton){
                  tlv_tphoton.SetPxPyPzE(truthphoton->get_px(),truthphoton->get_py(),truthphoton->get_pz(),truthphoton->get_e());
                  if (recoPhoton)
                  {
                    _b_photon_m=recoPhoton->Dot(*recoPhoton);
                    _b_tphoton_m=tlv_tphoton.Dot(tlv_tphoton);
                    _b_photon_pT=recoPhoton->Pt();
                  }
                }
                else{ //photon was not reconstructed
                  _b_photon_m =-1;
                  _b_tphoton_m =-1;
                  _b_photon_pT=-1;
                }
                //TODO check Conversion operations for ownership transfer->memleak due to lack of delete
                pair<TLorentzVector*, TLorentzVector*> reco_tlvs = i->second.getRecoTlvs();
                genfit::GFRaveVertex* recoVert = i->second.getSecondaryVertex(_vertexer);
                std::pair<PHGenFit::Track*,PHGenFit::Track*> ph_gf_tracks = i->second.getPHGFTracks(_vertexer);
                if (recoVert)
                {
                  std::pair<PHGenFit::Track*,PHGenFit::Track*> refit_phgf_tracks=i->second.refitTracks(_vertexer,get_primary_vertex(topNode));
                  cout<<"here"<<endl;
                  pair<TLorentzVector*, TLorentzVector*> refit_reco_tlvs = i->second.getRefitRecoTlvs();
                  if(refit_reco_tlvs.first&&refit_reco_tlvs.second){
                    _b_refitdiffx = reco_tlvs.first->X()-refit_reco_tlvs.first->X();
                    _b_refitdiffy = reco_tlvs.first->Y()-refit_reco_tlvs.first->Y();
                    _b_refitdiffz = reco_tlvs.first->Z()-refit_reco_tlvs.first->Z();
                  }
                  else{
                    _b_refitdiffx = -99.;
                    _b_refitdiffy = -99.;
                    _b_refitdiffz = -99.;
                  }
                  if (ph_gf_tracks.first&&refit_phgf_tracks.first)
                  {
                    cout<<"Good Track refit with original:\n";ph_gf_tracks.first->get_mom().Print();cout<<"\n\t and refit:\n";
                    refit_phgf_tracks.first->get_mom().Print();
                  }
                  if (ph_gf_tracks.second&&refit_phgf_tracks.second)
                  {
                    cout<<"Good Track refit with original:\n"; 
                    ph_gf_tracks.second->get_mom().Print(); 
                    cout<<"\n\t and refit:\n";
                    refit_phgf_tracks.second->get_mom().Print();
                  }
                  recoPhoton = i->second.getRefitRecoPhoton();
                  if(recoPhoton) _b_rephoton_m=recoPhoton->Dot(*recoPhoton);
                  TVector3 recoVertPos = recoVert->getPos();
                  _b_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
                  _b_tvtx_radius = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
                  _b_vtx_phi = recoVertPos.Phi();
                  _b_vtx_eta = recoVertPos.Eta();
                  _b_vtx_z = recoVertPos.Z();
                  _b_vtx_x = recoVertPos.X();
                  _b_vtx_y = recoVertPos.Y();
                  TVector3 tVertPos(vtx->get_x(),vtx->get_y(),vtx->get_z());
                  _b_tvtx_phi = tVertPos.Phi();
                  _b_tvtx_eta = tVertPos.Eta();
                  _b_tvtx_z = tVertPos.Z();
                  _b_tvtx_x = tVertPos.X();
                  _b_tvtx_y = tVertPos.Y();
                  _b_vtx_chi2 = recoVert->getChi2();
                  _b_vtxTrackRZ_dist = i->second.vtxTrackRZ(recoVertPos);
                  _b_vtxTrackRPhi_dist = i->second.vtxTrackRPhi(recoVertPos);

                  _b_track1_pt = pTstemp.first;
                  _b_track2_pt = pTstemp.second;
                  pair<float,float> etasTemp = i->second.getTrackEtas();
                  _b_track1_eta = etasTemp.first;
                  _b_track2_eta = etasTemp.second;
                  pair<float,float> phisTemp = i->second.getTrackPhis();
                  _b_track1_phi = phisTemp.first;
                  _b_track2_phi = phisTemp.second;
                  _vtxingTree->Fill();
                }


                //reset the values
                _b_cluster_prob=-1;
                _b_cluster_deta=-1;
                _b_cluster_dphi=-1;
                _b_nCluster=-1;
              }
              pair<int,int> clusterIds = i->second.get_cluster_ids();
              RawCluster *clustemp;
              if(mainClusterContainer->getCluster(clusterIds.first)){//if there is matching cluster 
                clustemp =   dynamic_cast<RawCluster*>(mainClusterContainer->getCluster(clusterIds.first)->Clone());
                //this is for cluster subtraction which will not be implented soon
                // _conversionClusters.AddCluster(clustemp); //add the calo cluster to the container
                if (_kMakeTTree)
                {
                  _b_cluster_prob=clustemp->get_prob();
                  RawCluster *clus2 = mainClusterContainer->getCluster(clusterIds.second);
                  if (clus2)
                  {
                    _b_cluster_dphi=fabs(clustemp->get_phi()-clus2->get_phi());
                    TVector3 etaCalc(clustemp->get_x(),clustemp->get_y(),clustemp->get_z());
                    //TODO check cluster_prob distribution for signal
                    if (clus2->get_prob()>_b_cluster_prob)
                    {
                      _b_cluster_prob=clus2->get_prob();
                    }
                    //calculate deta
                    float eta1 = etaCalc.PseudoRapidity();
                    etaCalc.SetXYZ(clus2->get_x(),clus2->get_y(),clus2->get_z());
                    _b_cluster_deta=fabs(eta1-etaCalc.PseudoRapidity());
                    if (clusterIds.first!=clusterIds.second) //if there are two district clusters
                    {
                      _b_nCluster=2;
                    }
                    else{
                      _b_nCluster=1;
                    }
                  }
                }
              }
              _signalCutTree->Fill();  
              break;
            }
          case 1: //there's one reco track I am not atempting to recover these at this time
            {
              break;
            }
          case 0: //no reco tracks
            break;
          default:
            if (Verbosity()>1)
            {
              cerr<<Name()<<" error setting reco tracks"<<endl;
            }
            break;
        }//switch
      }//rapidity cut
    }// has 2 truth tracks
  }//map loop
}

//only call if _kMakeTTree is true
void TruthConversionEval::processTrackBackground(std::vector<SvtxTrack*> *v_tracks,TrkrClusterContainer* clusterMap){
  Conversion pairMath;
  float lastpT=-1.;
  unsigned nNullTrack=0;
  cout<<"The total possible background track count is "<<v_tracks->size()<<'\n';
  for (std::vector<SvtxTrack*>::iterator iTrack = v_tracks->begin(); iTrack != v_tracks->end(); ++iTrack) {
    //if(!*iTrack||TMath::Abs((*iTrack)->get_eta())>1.1||(*iTrack)->get_pt()!=lastpT)continue;
    if(!*iTrack){
      nNullTrack++;
      continue;
    }
    if(TMath::Abs((*iTrack)->get_eta())>1.1||(*iTrack)->get_pt()==lastpT)continue;
    lastpT=(*iTrack)->get_pt();
    cout<<"\t pT="<<lastpT<<'\n';
    auto temp_key_it=(*iTrack)->begin_cluster_keys();//key iterator to first cluster
    if(temp_key_it!=(*iTrack)->end_cluster_keys()){//if the track has clusters
      TrkrCluster* temp_cluster = clusterMap->findCluster(*temp_key_it);//get the cluster 
      if(temp_cluster) _bb_track_layer = TrkrDefs::getLayer(temp_cluster->getClusKey());//if there is a cluster record its layer
      else _bb_track_layer=-1;
    }
    _bb_track_dca = (*iTrack)->get_dca();
    _bb_track_pT = (*iTrack)->get_pt();
    auto cluster1 = _mainClusterContainer->getCluster((*iTrack)->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
    if(cluster1) _bb_cluster_prob= cluster1->get_prob();
    else _bb_cluster_prob=-1;
    for(std::vector<SvtxTrack*>::iterator jTrack =std::next(iTrack,1);jTrack!=v_tracks->end(); ++jTrack){//posible bias by filling the track level variables with iTrack instead of min(iTrack,jTrack)
      if(!*jTrack||TMath::Abs((*jTrack)->get_eta())>1.1)continue;
      _bb_track_deta = pairMath.trackDEta((*iTrack),(*jTrack));
      _bb_track_dphi = pairMath.trackDPhi((*iTrack),(*jTrack));
      _bb_track_dlayer = pairMath.trackDLayer(_clusterMap,(*iTrack),(*jTrack));
      _bb_approach = pairMath.approachDistance((*iTrack),(*jTrack));
      auto cluster2 = _mainClusterContainer->getCluster((*iTrack)->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      if (cluster2&&cluster1)
      {
        if(cluster2->get_id()!=cluster1->get_id())_bb_nCluster = 2;
        else _bb_nCluster = 1;
        _bb_cluster_dphi=fabs(cluster1->get_phi()-cluster2->get_phi());
        TVector3 etaCalc(cluster1->get_x(),cluster1->get_y(),cluster1->get_z());
        float eta1 = etaCalc.PseudoRapidity();
        etaCalc.SetXYZ(cluster2->get_x(),cluster2->get_y(),cluster2->get_z());
        _bb_cluster_deta=fabs(eta1-etaCalc.PseudoRapidity());
        _bb_track1_pid = get_track_pid(*iTrack);
        _bb_track2_pid = get_track_pid(*jTrack);
      }
      else {
        if(cluster1||cluster2) _bb_nCluster = 1;
        else _bb_nCluster=0;
        _bb_cluster_deta=-1;
        _bb_cluster_dphi=-1;
      }
      if (_bb_track_layer>0&&_bb_track_pT>.6&&_bb_track_deta<.0082&&TMath::Abs(_bb_track_dlayer)<=2)
      {
        (*iTrack)->identify();
        (*jTrack)->identify();
        genfit::GFRaveVertex* recoVert = _vertexer->findSecondaryVertex(*iTrack,*jTrack);
        _bb_vtx_radius = -1;
        _bb_vtx_chi2 = -1;
        _bb_vtxTrackRZ_dist = -1;
        _bb_vtxTrackRPhi_dist = -1;
        if (recoVert)
        {
          TVector3 recoVertPos = recoVert->getPos();
          _bb_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
          _bb_vtx_chi2 = recoVert->getChi2();
          _bb_vtxTrackRZ_dist = pairMath.vtxTrackRZ(recoVertPos,*iTrack,*jTrack);
          _bb_vtxTrackRPhi_dist = pairMath.vtxTrackRPhi(recoVertPos,*iTrack,*jTrack);
        }
        _vtxBackTree->Fill();
      }//pair cuts
      _pairBackTree->Fill();
    }//jTrack loop
    _trackBackTree->Fill();
  }//iTrack loop
  cout<<"Null track count ="<<nNullTrack<<'\n';
}

const RawClusterContainer* TruthConversionEval::getClusters()const {return &_conversionClusters;} 

int TruthConversionEval::get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo)const{
  return truthinfo->isEmbeded(particle->get_track_id());
}

float TruthConversionEval::vtoR(PHG4VtxPoint* vtx)const{
  return (float) sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
}

int TruthConversionEval::get_track_pid(SvtxTrack* track) const{
  auto truth =  _truthinfo->GetParticle(track->get_truth_track_id());
  if(truth) return truth->get_pid();
  else return 0;
}


int TruthConversionEval::End(PHCompositeNode *topNode)
{
  if(_kMakeTTree){
    cout<<"closing"<<endl;
    _signalCutTree->Write();
    _f->Write();
    _f->Close();
  }
  return 0;
}
