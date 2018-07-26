#include "SinglePhotonAfter.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include "TLorentzVector.h"
#include <iostream>

#include <calotrigger/CaloTriggerInfo.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

SinglePhotonAfter::SinglePhotonAfter(const std::string &name) : SubsysReco("SinglePhoton")
{

  _foutname = name;

}

int SinglePhotonAfter::Init(PHCompositeNode *topNode)
{
  _f = new TFile( _foutname.c_str(), "RECREATE");
  _tree = new TTree("ttree","a succulent orange tree");
  _tree->SetAutoSave(300);
  _tree->Branch("particle_n", &_b_particle_n,"particle_n/I");
  _tree->Branch("particle_pt", _b_particle_pt,"particle_pt[particle_n]/F");
  _tree->Branch("particle_eta", _b_particle_eta,"particle_eta[particle_n]/F");
  _tree->Branch("particle_phi", _b_particle_phi,"particle_phi[particle_n]/F");
  return 0;

}

int SinglePhotonAfter::process_event(PHCompositeNode *topNode)
{

  _b_particle_n = 0;
  
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
    TLorentzVector t; t.SetPxPyPzE( g4particle->get_px(), g4particle->get_py(), g4particle->get_pz(), g4particle->get_e() );
    float truth_pt = t.Pt();
    float truth_eta = t.Eta();
    if (fabs(truth_eta) > 1.1) continue;
    float truth_phi = t.Phi();

    _b_particle_pt[ _b_particle_n ] = truth_pt;
    _b_particle_eta[ _b_particle_n ] = truth_eta;
    _b_particle_phi[ _b_particle_n ] = truth_phi;
    _b_particle_n++;
  }

  _tree->Fill();
  return 0;
}



int SinglePhotonAfter::End(PHCompositeNode *topNode)
{
  _tree->Write();
  _f->Write();
  _f->Close();

  return 0;
}

