#include "SinglePhotonAfter.h"

#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calotrigger/CaloTriggerInfo.h>

#include "TLorentzVector.h"
#include <iostream>
#include <sstream>

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
  _tree->Branch("npair", &_b_pair);
  _tree->Branch("event",&_b_event);
  _tree->Branch("hash",&_b_hash);
  _tree->Branch("rVtx", _b_rVtx,"rVtx[nVtx]/F");
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
  _b_pair=0;

  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetParticleRange();
  //make a list of the conversions
  std::list<int> vtxList;
  std::map<int,Conversion> mapConversions;
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
    PHG4Particle* parent =truthinfo->GetParticle(g4particle->get_parent_id());
    float radius=0;
    TLorentzVector t;
    if(!parent){ //if the parent point is null then the particle is primary 
      if(get_embed(g4particle,truthinfo)!=2) continue;
    }
    else{ //if the particle is not primary find its vertex 
      if(get_embed(parent,truthinfo)!=2) continue;
      PHG4VtxPoint* vtx=truthinfo->GetVtx(g4particle->get_vtx_id())
      std::cout<<radius<<'\n';
      vtxList.push_back(vtx->get_id());
      mapConversions[vtx->get_id()]->setElectron(g4particle);
    }
    //record the particle information 
    t.SetPxPyPzE( g4particle->get_px(), g4particle->get_py(), g4particle->get_pz(), g4particle->get_e() );
    float truth_pt = t.Pt();
    float truth_eta = t.Eta();
    if (fabs(truth_eta) > 1.1) continue;
    float truth_phi = t.Phi();
    _b_rVtx[0] = radius; 
    _b_particle_id[ _b_particle_n ] = g4particle->get_pid();
    _b_particle_pt[ _b_particle_n ] = truth_pt;
    _b_particle_eta[ _b_particle_n ] = truth_eta;
    _b_particle_phi[ _b_particle_n ] = truth_phi;
    _b_particle_n++;
  }
  //record event information 
  _b_nVtx=numUnique(vtxList,mapConversions);
  //make a hash of the event number and file number 
  std::stringstream ss;
  ss<<_b_event;             //this is where the file number is 
  _b_hash=_foutname.c_str()[_foutname.length()-7]+ss.str();

  _tree->Fill();
  std::cout<<"Filled "<<_b_particle_n<<" particles"<<std::endl;
  _b_event++;
  return 0;
}

int SinglePhotonAfter::End(PHCompositeNode *topNode)
{
  std::cout<<"Writing truth tree"<<std::endl;
  _f->Write();
  _f->Close();
  return 0;
}

