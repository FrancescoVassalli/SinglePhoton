#include "SinglePhotonAfter.h"

#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calotrigger/CaloTriggerInfo.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>


#include <iostream>
#include <sstream>
#include <math.h>

SinglePhotonAfter::SinglePhotonAfter(const std::string &name) : SubsysReco("SinglePhoton")
{
  _foutname = name;
}

SinglePhotonAfter::~SinglePhotonAfter(){
  delete _f;
}

int SinglePhotonAfter::InitRun(PHCompositeNode *topNode)
{
  _b_event=0;
  _f = new TFile( _foutname.c_str(), "RECREATE");
  _tree = new TTree("ttree","a succulent orange tree");
  //_tree->SetAutoSave(300);
  _tree->Branch("event",&_b_event); 
  _tree->Branch("hash",&_b_hash);
  _tree->Branch("nVtx", &_b_nVtx);
  _tree->Branch("nconvert", &_b_nconvert);
  _tree->Branch("nTpair", &_b_Tpair);
  _tree->Branch("nRpair", &_b_Rpair);
  _tree->Branch("rVtx", _b_rVtx,"rVtx[nVtx]/F");
  _tree->Branch("electron_pt", _b_electron_pt,"electron_pt[nVtx]/F");
  _tree->Branch("positron_pt", _b_positron_pt,"positron_pt[nVtx]/F");
  _tree->Branch("photon_pt",   _b_parent_pt    ,"photon_pt[nVtx]/F");
  _tree->Branch("photon_eta",  _b_parent_eta  ,"photon_eta[nVtx]/F");
  _tree->Branch("photon_phi",  _b_parent_phi  ,"photon_phi[nVtx]/F");
  return 0;
}

int SinglePhotonAfter::process_event(PHCompositeNode *topNode)
{
  

  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetParticleRange();
  SvtxEvalStack *stack = new SvtxEvalStack(topNode);
  SvtxTrackEval* trackeval =   stack->get_track_eval();

  //make a list of the conversions
  std::list<int> vtxList;
  std::map<int,Conversion> mapConversions;
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    //bool isPrimary;
    PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
    PHG4Particle* parent =truthinfo->GetParticle(g4particle->get_parent_id());
    float radius=0;
    if(!parent){ //if the parent point is null then the particle is primary 
      //checking the embed ID to make sure it is a particle I made need to change the magic literal
      if(get_embed(g4particle,truthinfo)!=2) continue;
      //isPrimary=true;
    }
    else{ //if the particle is not primary find its vertex 
      if(get_embed(parent,truthinfo)!=2) continue;
      PHG4VtxPoint* vtx=truthinfo->GetVtx(g4particle->get_vtx_id());
      radius=sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
      if (radius<kTPCRADIUS) //limits to truth conversions within the tpc radius
      {
        std::cout<<radius<<'\n';
        vtxList.push_back(vtx->get_id());
        (mapConversions[vtx->get_id()]).setElectron(g4particle);
        (mapConversions[vtx->get_id()]).setVtx(vtx);
        (mapConversions[vtx->get_id()]).setParent(parent);
      }
      //isPrimary=false;
    }
  }
  //record event information 
  numUnique(&vtxList,&mapConversions,trackeval);
  //make a hash of the event number and file number 
  std::stringstream ss;
  ss<<"-"<<_b_event;             //this is where the file number is 
  _b_hash=_foutname.substr(_foutname.length()-10,5)+ss.str();

  //currently my reco can only handle single conversion events

  _tree->Fill();
  std::cout<<"Filled "<<_b_nVtx<<" vertices"<<std::endl;
  _b_event++;
  delete stack;
  return 0;
}

void SinglePhotonAfter::numUnique(std::list<int> *l,std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL){
  l->sort();
  int last=-1;
  _b_nVtx = 0;
  _b_nconvert=0;
  _b_Tpair=0;
  _b_Rpair=0;
  for (std::list<int>::iterator i = l->begin(); i != l->end(); ++i) {
    //make sure the conversions are not double counted 
    if(*i!=last){
      //fill the tree
      PHG4VtxPoint *vtx =(mymap->at(*i)).getVtx();
      _b_rVtx[_b_nVtx] = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
      PHG4Particle *temp = (mymap->at(*i)).getPhoton();
      TLorentzVector t;
      t.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
      _b_parent_pt[_b_nVtx]=t.Pt();
      _b_parent_phi[_b_nVtx]=t.Phi();
      _b_parent_eta[_b_nVtx]=t.Eta();
      temp=(mymap->at(*i)).getElectron();
      t.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
      _b_electron_pt[_b_nVtx]=t.Pt();
      temp=(mymap->at(*i)).getPositron();
      t.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
      _b_positron_pt[_b_nVtx]=t.Pt();
      if (t.Rapidity()<kRAPIDITYACCEPT)
      {
        _b_nconvert++;
        if (mymap&&mymap->at(*i).hasPair())
        {
          _b_Tpair++;
          mymap->at(*i).setRecoTracks(trackeval);
          if (mymap->at(*i).recoCount()==2)
          {
            _b_Rpair++;
          }
        }
      }
      last=*i;
      _b_nVtx++; //if conversion is unique record it 
    }
  }
}

int SinglePhotonAfter::End(PHCompositeNode *topNode)
{
  std::cout<<"Writing truth tree"<<std::endl;
  _f->Write();
  _f->Close();
  return 0;
}



