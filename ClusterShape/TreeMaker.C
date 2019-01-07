#include "TreeMaker.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include "TLorentzVector.h"
#include <iostream>
#include <vector>

#ifndef __CINT__
#include <list>
#endif //CINT

#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <jetbackground/TowerBackground.h>


////////////////////////////////////////////////////////////////////////////////////
//
//
//  These functions are for finding Sasha Bazilevsky's chi values for myself
//
//
///////////////////////////////////////////////////////////////////////////////////

float deltaR( float eta1, float eta2, float phi1, float phi2 ) 
{
  float deta = eta1 - eta2;
  float dphi = phi1 - phi2;
  if(dphi > TMath::Pi()){dphi -= 2*TMath::Pi();} //make sure dif_phi is between -pi and pi
  else if(dphi < -1*TMath::Pi()){dphi += 2*TMath::Pi();}
  return sqrt( deta*deta + dphi*dphi);
}

ChaseTower findMaxTower(std::vector<ChaseTower> towers)
{
  ChaseTower MaxTower;
  MaxTower.setKey(0);
  MaxTower.setEnergy(0);
  MaxTower.setPhi(0);
  MaxTower.setEta(0);

  for(unsigned int i = 0; i < towers.size(); i++)
  {
    if(towers.at(i).getEnergy() > MaxTower.getEnergy())
    {
      MaxTower.setKey(towers.at(i).getKey());
      MaxTower.setEnergy(towers.at(i).getEnergy());
      MaxTower.setPhi(towers.at(i).getPhi());
      MaxTower.setEta(towers.at(i).getEta());
    }
  }
  return MaxTower;
}

/*
EtaPhiPoint CenterOfEnergy_BazilevskyStyle(std::vector<ChaseTower> towers)
{
  double avgeta = 0;
  double avgphi = 0;
  double etot = 0;
  for(unsigned int i = 0; i < towers.size(); i++)
  {
    etot += towers.at(i).getEnergy();
  }
  for(unsigned int i = 0; i < towers.size(); i++)
  {
    avgeta += towers.at(i).getEta() * (towers.at(i).getEnergy() / etot);
    avgphi += towers.at(i).getPhi() * (towers.at(i).getEnergy() / etot);
  }
  return EtaPhiPoint(avgeta , avgphi);
}


bool my_compare(ChaseTower a, ChaseTower b, EtaPhiPoint CoE)
{
  return deltaR(CoE.eta, a.getEta(), CoE.phi, a.getPhi()) < deltaR(CoE.eta, b.getEta(), CoE.phi, b.getPhi());
}

cutValues CutValues_BazilevskyStyle(std::vector<ChaseTower> towers, EtaPhiPoint CoE)
{
  std::list<int> central4;
  central4.push_front(0);
  for(unsigned int i = 1; i < towers.size(); i++) //loops through the vector of chaseTowers, sorted towers by distance
  {
    for (std::list<int>::iterator it = central4.begin(); it != central4.end(); ++it) //iterate through list
    {
      if(my_compare(towers.at(i), towers.at(*it), CoE)) //if tower is shorter distance to CoE than current tower, insert
      {
        central4.insert(it,i); //yay insert sort, break when spot is found
        break;
      }
    }
  }

  double etot = 0;
  for(unsigned int i = 0; i < towers.size(); i++) //loops through the vector of chaseTowers
  {
    etot += towers.at(i).getEnergy();
  }

  double e1 = 0;
  double e2 = 0;
  double e3 = 0;
  double e4 = 0;

  std::list<int>::iterator it = central4.begin();
  e1 = towers.at(*it).getEnergy(); //closest tower
  if(central4.size() >= 3)
  {
    ++it;
    std::list<int>::iterator it2 = it;
    ++it2;
    if(fabs(towers.at(*it).getEta() - CoE.eta) < fabs(towers.at(*it2).getEta() - CoE.eta)) //if tower is closer in eta, it is e2
    {
      e2 = towers.at(*it).getEnergy(); 
      e4 = towers.at(*it2).getEnergy(); 
    }
    else
    {
      e2 = towers.at(*it2).getEnergy(); 
      e4 = towers.at(*it).getEnergy(); 
    }
  }
  if(central4.size() >= 4)
  {
    ++it;
    ++it;
    e3 = towers.at(*it).getEnergy(); //off diagonal tower
  }

  double e1t = (e1 + e2 + e3 + e4)/etot; //energy in central 4
  double e2t = (e1 - e2 - e3 + e4)/etot; //vertical symmetry
  double e3t = (e1 + e2 - e3 - e4)/etot; //horizontal symetry
  double e4t = (e3)/etot; //off diagonal
  return cutValues(e1t, e2t, e3t, e4t);
}

cutValues CutValues_ChaseStyle(std::vector<ChaseTower> towers, EtaPhiPoint CoE)
{
  std::list<int> central4;
  central4.push_front(0);
  for(unsigned int i = 1; i < towers.size(); i++) //loops through the vector of chaseTowers, sorted towers by distance
  {
    for (std::list<int>::iterator it = central4.begin(); it != central4.end(); ++it) //iterate through list
    {
      if(my_compare(towers.at(i), towers.at(*it), CoE)) //if tower is shorter distance to CoE than current tower, insert
      {
        central4.insert(it,i); //yay insert sort, break when spot is found
        break;
      }
    }
  }

  double etot = 0;
  for(unsigned int i = 0; i < towers.size(); i++) //loops through the vector of chaseTowers
  {
    etot += towers.at(i).getEnergy();
  }

  double e1 = 0;
  double e2 = 0;
  double e3 = 0;
  double e4 = 0;

  std::list<int>::iterator it = central4.begin();
  e1 = towers.at(*it).getEnergy(); //closest tower
  if(central4.size() >= 3)
  {
    ++it;
    std::list<int>::iterator it2 = it;
    ++it2;
    if(fabs(towers.at(*it).getEta() - CoE.eta) < fabs(towers.at(*it2).getEta() - CoE.eta)) //if tower is closer in eta, it is e2
    {
      e2 = towers.at(*it).getEnergy(); 
      e4 = towers.at(*it2).getEnergy(); 
    }
    else
    {
      e2 = towers.at(*it2).getEnergy(); 
      e4 = towers.at(*it).getEnergy(); 
    }
  }
  if(central4.size() >= 4)
  {
    ++it;
    ++it;
    e3 = towers.at(*it).getEnergy(); //off diagonal tower
  }

  double e1t = (e1 + e2 + e3 + e4); //energy in central 4
  double e2t = (e1 - e2 - e3 + e4); //vertical symmetry
  double e3t = (e1 + e2 - e3 - e4); //horizontal symetry
  double e4t = (e3); //off diagonal
  return cutValues(e1t, e2t, e3t, e4t);
}
*/

////////////////////////////////////////////////////////////////////////////////////
//
//
//  This is the actual meat of the afterburner module
//
//
///////////////////////////////////////////////////////////////////////////////////



TreeMaker::TreeMaker(const std::string &name, int embed_id) : SubsysReco("IsoTree")
{
  _foutname = name;
  _embed_id = embed_id;

}

TreeMaker::TreeMaker(const std::string &name, int embed_id,SinglePhotonAfter* conversionModule) : SubsysReco("IsoTree"),conversionModule(conversionModule)
{
  _foutname = name;
  _embed_id = embed_id;
}
int TreeMaker::Init(PHCompositeNode *topNode)
{
  _f = new TFile( _foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree","a gentle baobab tree");
  //truth particle information
  //_tree->Branch("particle_n", &_b_particle_n);
  //_tree->Branch("particle_pt", _b_particle_pt,"particle_pt[particle_n]/F");
  //_tree->Branch("particle_eta", _b_particle_eta,"particle_eta[particle_n]/F");
  //_tree->Branch("particle_phi", _b_particle_phi,"particle_phi[particle_n]/F");
  //_tree->Branch("particle_pid", _b_particle_pid,"particle_pid[particle_n]/I");
  //_tree->Branch("particle_et", _b_particle_et,"particle_et[particle_n]/F");

  //reco cluster information
  _tree->Branch("cluster_n", &_b_cluster_n);
  _tree->Branch("cluster_et", _b_cluster_et,"cluster_et[cluster_n]/D");
  _tree->Branch("cluster_eta",_b_cluster_eta,"cluster_eta[cluster_n]/D");
  _tree->Branch("cluster_phi",_b_cluster_phi,"cluster_phi[cluster_n]/D");
  _tree->Branch("cluster_prob",_b_cluster_prob,"cluster_prob[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_sub_R01",_b_et_iso_calotower_sub_R01, "et_iso_calotower_sub_R01[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R01",_b_et_iso_calotower_R01, "et_iso_calotower_R01[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_sub_R02",_b_et_iso_calotower_sub_R02, "et_iso_calotower_sub_R02[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R02",_b_et_iso_calotower_R02, "et_iso_calotower_R02[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_sub_R03",_b_et_iso_calotower_sub_R03, "et_iso_calotower_sub_R03[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R03",_b_et_iso_calotower_R03, "et_iso_calotower_R03[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_sub_R04",_b_et_iso_calotower_sub_R04, "et_iso_calotower_sub_R04[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R04",_b_et_iso_calotower_R04, "et_iso_calotower_R04[cluster_n]/D");

  _tree->Branch("NTowers",_b_NTowers,"NTowers[cluster_n]/I");
  //_tree->Branch("etot",_b_etot,"etot[cluster_n]/D");
  //_tree->Branch("e1t",_b_e1t,"e1t[cluster_n]/D");
  //_tree->Branch("e2t",_b_e2t,"e2t[cluster_n]/D");
  //_tree->Branch("e3t",_b_e3t,"e3t[cluster_n]/D");
  //_tree->Branch("e4t",_b_e4t,"e4t[cluster_n]/D");
//
  //_tree->Branch("chase_e1t",_b_chase_e1t,"chase_e1t[cluster_n]/D");
  //_tree->Branch("chase_e2t",_b_chase_e2t,"chase_e2t[cluster_n]/D");
  //_tree->Branch("chase_e3t",_b_chase_e3t,"chase_e3t[cluster_n]/D");
  //_tree->Branch("chase_e4t",_b_chase_e4t,"chase_e4t[cluster_n]/D");

  _tree->Branch("clusterTower_towers",&_b_clusterTower_towers);
  _tree->Branch("clusterTower_eta",_b_clusterTower_eta,"clusterTower_eta[clusterTower_towers]/D");
  _tree->Branch("clusterTower_phi",_b_clusterTower_phi,"clusterTower_phi[clusterTower_towers]/D");
  _tree->Branch("clusterTower_energy",_b_clusterTower_energy,"clusterTower_energy[clusterTower_towers]/D");
  std::cout<<"cTTree initialized"<<std::endl;
 return 0;
}


int TreeMaker::process_event(PHCompositeNode *topNode)
{
  /////////////////////////////////////find truth particle information /////////////////////////////////////////////
  _b_particle_n = 0;

  //PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  //PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
 
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  vx = NAN;
  vy = NAN;
  vz = NAN;
  if (vertexmap)
  {
    if (!vertexmap->empty())
    {
      GlobalVertex* vertex = (vertexmap->begin()->second);

      vx = vertex->get_x();
      vy = vertex->get_y();
      vz = vertex->get_z();
    }
  }
  std::cout<<"Analyzing event clusters with vtx=("<<vx<<','<<vy<<','<<vz<<')'<<std::endl; 
  //for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
  //  PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
  //  if ( truthinfo->isEmbeded( g4particle->get_track_id() ) != _embed_id ) continue;
  //  TLorentzVector t; t.SetPxPyPzE( g4particle->get_px(), g4particle->get_py(), g4particle->get_pz(), g4particle->get_e() );
  //  
  //  float truth_pt = t.Pt();
  //  float truth_et = t.Et();
  //  //if (truth_pt < 1) continue;
  //  float truth_eta = t.Eta();
  //  if (fabs(truth_eta) > 1.1) continue;
  //  float truth_phi = t.Phi();
  //  int truth_pid = g4particle->get_pid();
  //  
  //  _b_particle_pt[ _b_particle_n ] = truth_pt;
  //  _b_particle_eta[ _b_particle_n ] = truth_eta;
  //  _b_particle_phi[ _b_particle_n ] = truth_phi;
  //  _b_particle_pid[ _b_particle_n ] = truth_pid;
  //  _b_particle_et[ _b_particle_n ] = truth_et;
  //  
  //  _b_particle_n++;
  //  
  //}

  

  //////////////////////////////////////Find cluster information/////////////////////////////////////////////////////
  _b_cluster_n = 0;
  _b_clusterTower_towers = 0;

  RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawClusterContainer *clusters = conversionModule->getClusters();
  cout<<Name()<<" cluster size:"<<clusters->size()<<endl;  
  //find correct vertex
  vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap"); 
  vx=vy=vz=0;
  if (vertexmap&&!vertexmap->empty())
  {
     GlobalVertex* vertex = (vertexmap->begin()->second);
     vx = vertex->get_x();
     vy = vertex->get_y();
     vz = vertex->get_z();
     //std::cout<<"Event Vertex Calculated in ClusterIso x:"<<vx<<" y:"<<vy<<" z:"<<vz<<'\n';
  }
  for (RawClusterContainer::Iterator clusiter = clusters->getClusters().first; clusiter !=  clusters->getClusters().second; ++clusiter) 
  {
    /*const unsigned int clusid= clusiter->first;
    cout<<"got clusid:"<<clusid<<endl;
    RawCluster *cluster = clusters->getCluster(clusid); //what about just using clusiter->second??*/
    RawCluster *cluster = clusiter->second;
    cout<<"got cluster"<<endl;
    CLHEP::Hep3Vector vertex( vx, vy, vz); //set new correct vertex for eta calculation
    cout<<"making vector"<<endl;
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
    cout<<"madevector"<<endl;
    double cluster_energy = E_vec_cluster.mag();
    double cluster_eta = E_vec_cluster.pseudoRapidity(); 
    double cluster_phi = E_vec_cluster.phi();
    double et = cluster_energy / cosh( cluster_eta );
    double prob = cluster->get_prob();

    if (et < 1) continue;
    _b_cluster_eta[ _b_cluster_n ] = cluster_eta;
    _b_cluster_phi[ _b_cluster_n ] = cluster_phi;
    _b_cluster_et[ _b_cluster_n ] = et;
    _b_cluster_prob[ _b_cluster_n ] = prob;    

    //arguments are (cone radiusx10, subtract event = true, use calotowers for isolation = true)
    //_b_et_iso_calotower_sub_R01[ _b_cluster_n ] = cluster->get_et_iso(1,1,1);
    //_b_et_iso_calotower_R01[ _b_cluster_n ] = cluster->get_et_iso(1,0,1);
    //_b_et_iso_calotower_sub_R02[ _b_cluster_n ] = cluster->get_et_iso(2,1,1);
    //_b_et_iso_calotower_R02[ _b_cluster_n ] = cluster->get_et_iso(2,0,1);
    //_b_et_iso_calotower_sub_R03[ _b_cluster_n ] = cluster->get_et_iso(3,1,1);
    //_b_et_iso_calotower_R03[ _b_cluster_n ] = cluster->get_et_iso(3,0,1);
    //_b_et_iso_calotower_sub_R04[ _b_cluster_n ] = cluster->get_et_iso(4,1,1);
    //_b_et_iso_calotower_R04[ _b_cluster_n ] = cluster->get_et_iso(4,0,1);

    //now we get tower information for ID purposes, find "Center of Energy", get 4 central towers
    std::cout<<"Towers in cluster: "<<cluster->getNTowers()<<std::endl;
    _b_NTowers[_b_cluster_n] = cluster->getNTowers();
    //std::cout<<"Towers in cluster after saved: "<<_b_NTowers[_b_cluster_n]<<std::endl;

    std::vector <ChaseTower> clusterTowers;

    RawCluster::TowerConstRange clusterrange = cluster->get_towers();
    for (RawCluster::TowerConstIterator rtiter = clusterrange.first; rtiter != clusterrange.second; ++rtiter) 
    {
      RawTower *tower = towersEM3old->getTower(rtiter->first);
      RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
      ChaseTower temp;
      temp.setEta(tower_geom->get_eta());
      _b_clusterTower_eta[_b_clusterTower_towers] = tower_geom->get_eta();
      temp.setPhi(tower_geom->get_phi());
      _b_clusterTower_phi[_b_clusterTower_towers] = tower_geom->get_phi();
      temp.setEnergy(tower->get_energy());
      _b_clusterTower_energy[_b_clusterTower_towers] = tower->get_energy();
      temp.setKey(tower->get_key());
      clusterTowers.push_back(temp);
      _b_clusterTower_towers++;
    }

    ////////////////////now that we have all towers from cluster, find max tower//////////////////////////
   // ChaseTower MaxTower = findMaxTower(clusterTowers);
//
//
   // ////////////////////Find 49 towers around max tower, Sasha style/////////////////////////////////////
   // std::vector<ChaseTower> Sasha49Towers;
//
   // RawTowerContainer::ConstRange towerrange = towersEM3old->getTowers();
   // for (RawTowerContainer::ConstIterator rtiter = towerrange.first; rtiter != towerrange.second; ++rtiter) 
   // {
   //   RawTower *tower = rtiter->second;
   //   RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
   //   double this_phi = tower_geom->get_phi();
   //   double this_eta = tower_geom->get_eta();
   //   double this_energy = tower->get_energy();
   //   double dif_eta = this_eta - MaxTower.getEta();
   //   double dif_phi = this_phi - MaxTower.getPhi();
//
   //   if(dif_phi > TMath::Pi()){dif_phi -= 2*TMath::Pi();} //make sure dif_phi is between -pi and pi
   //   else if(dif_phi < -1*TMath::Pi()){dif_phi += 2*TMath::Pi();}
//
   //   if(fabs(dif_eta) < 0.08 and fabs(dif_phi) < 0.08 )
   //   {
   //     Sasha49Towers.push_back(ChaseTower(this_eta, this_phi, this_energy, tower->get_key()));
   //   }
   // }
//
   // /////////////Find Center of energy for cluster, get tower info of 4 towers around CoE////////////////
   // EtaPhiPoint CoE = CenterOfEnergy_BazilevskyStyle(Sasha49Towers);
   // 
   // cutValues clusterCuts = CutValues_BazilevskyStyle(Sasha49Towers, CoE);
   // cutValues chase_clusterCuts = CutValues_ChaseStyle(Sasha49Towers, CoE);
//
   // _b_e1t[_b_cluster_n] = clusterCuts.e1t;
   // _b_e2t[_b_cluster_n] = clusterCuts.e2t;
   // _b_e3t[_b_cluster_n] = clusterCuts.e3t;
   // _b_e4t[_b_cluster_n] = clusterCuts.e4t;
//
   // _b_chase_e1t[_b_cluster_n] = chase_clusterCuts.e1t;
   // _b_chase_e2t[_b_cluster_n] = chase_clusterCuts.e2t;
   // _b_chase_e3t[_b_cluster_n] = chase_clusterCuts.e3t;
   // _b_chase_e4t[_b_cluster_n] = chase_clusterCuts.e4t;

    _b_cluster_n++;
  }


  _tree->Fill();

  return 0; 
}

int TreeMaker::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}

