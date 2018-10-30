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
  _tree->Branch("particle_n", &_b_particle_n);
  _tree->Branch("nVtx", &_b_nVtx);
  _tree->Branch("nconvert", &_b_nconvert);
  _tree->Branch("nTpair", &_b_Tpair);
  _tree->Branch("nRpair", &_b_Rpair);
  _tree->Branch("event",&_b_event); 
  _tree->Branch("hash",&_b_hash);
  _tree->Branch("rVtx", _b_rVtx,"rVtx[particle_n]/F");
  _tree->Branch("particle_pt", _b_particle_pt,"particle_pt[particle_n]/F");
  _tree->Branch("particle_eta", _b_particle_eta,"particle_eta[particle_n]/F");
  _tree->Branch("particle_phi", _b_particle_phi,"particle_phi[particle_n]/F");
  _tree->Branch("particle_id", _b_particle_id,"particle_id[particle_n]/I");
  return 0;
}

int SinglePhotonAfter::process_event(PHCompositeNode *topNode)
{
  _b_particle_n = 0;
  _b_nVtx = 0;
  _b_nconvert=0;
  _b_Tpair=0;
  _b_Rpair=0;

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
    TLorentzVector t;
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
      }
      //isPrimary=false;
    }
    if (radius<kTPCRADIUS)
    {
      //record the particle information 
      t.SetPxPyPzE( g4particle->get_px(), g4particle->get_py(), g4particle->get_pz(), g4particle->get_e() );
      float truth_pt = t.Pt();
      float truth_eta = t.Eta();
      if (fabs(truth_eta) > 1.1) continue;
      float truth_phi = t.Phi();
      _b_rVtx[_b_particle_n] = radius; 
      _b_particle_id[ _b_particle_n ] = g4particle->get_pid();
      _b_particle_pt[ _b_particle_n ] = truth_pt;
      _b_particle_eta[ _b_particle_n ] = truth_eta;
      _b_particle_phi[ _b_particle_n ] = truth_phi;
      _b_particle_n++;
    }
  }
  //record event information 
  _b_nVtx=numUnique(&vtxList,&mapConversions,trackeval);
  //make a hash of the event number and file number 
  std::stringstream ss;
  ss<<"-"<<_b_event;             //this is where the file number is 
  _b_hash=_foutname.c_str()[_foutname.length()-6]+ss.str();

  //currently my reco can only handle single conversion events

  _tree->Fill();
  std::cout<<"Filled "<<_b_particle_n<<" particles"<<std::endl;
  _b_event++;
  return 0;
}

int SinglePhotonAfter::numUnique(std::list<int> *l,std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL){
  l->sort();
  int last=-1;
  int r=0;
  for (std::list<int>::iterator i = l->begin(); i != l->end(); ++i) {
    if(*i!=last){
      r++;
      TLorentzVector t;
      PHG4VtxPoint *vtx =(mymap->at(*i)).getVtx();
      t.SetXYZM(vtx->get_x(),vtx->get_y(),vtx->get_z(),0);
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
    }
  }
  return r;
}

int SinglePhotonAfter::End(PHCompositeNode *topNode)
{
  std::cout<<"Writing truth tree"<<std::endl;
  _f->Write();
  _f->Close();
  return 0;
}



