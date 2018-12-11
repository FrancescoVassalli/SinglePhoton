#ifndef TreeMaker_h 
#define TreeMaker_h

#include <fun4all/SubsysReco.h>
#include <vector>
#include "TTree.h"
#include "TFile.h"
#include <string>
#include <SinglePhotonAfter.h>

class PHCompositeNode;

class ChaseTower
{
public:
  //constructors
  ChaseTower(){}; 
  ChaseTower(double eta_in, double phi_in, double energy_in, unsigned int keytype_in)
  {
    eta = eta_in;
    phi = phi_in;
    energy = energy_in;
    keytype = keytype_in;
  }
  void setEta(double eta2){eta = eta2;}
  void setPhi(double phi2){phi = phi2;}
  void setEnergy(double energy2){energy = energy2;}
  void setKey(unsigned int keytype2){keytype = keytype2;}
  double getEta(){return eta;}
  double getPhi(){return phi;}
  double getEnergy(){return energy;}
  unsigned int getKey(){return keytype;}
private:
  double eta;
  double phi;
  double energy;
  unsigned int keytype;
};

class EtaPhiPoint
{
public:
  //constructors
  EtaPhiPoint(){}; 
  EtaPhiPoint(double eta_in, double phi_in)
  {
    eta = eta_in;
    phi = phi_in;
  }
  void setEta(double eta2){eta = eta2;}
  void setPhi(double phi2){phi = phi2;}
  double getEta(){return eta;}
  double getPhi(){return phi;}
  double eta;
  double phi;
};

class cutValues
{
public:
  cutValues(){};
  cutValues(double e1t_in, double e2t_in, double e3t_in, double e4t_in)
  {
    e1t = e1t_in;
    e2t = e2t_in;
    e3t = e3t_in;
    e4t = e4t_in;
  }
  double e1t;
  double e2t;
  double e3t;
  double e4t;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class TreeMaker: public SubsysReco
{

 public:

  TreeMaker(const std::string &name="TreeMaker.root", int embed_id = 0);

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

 private:

  float deltaR( float eta1, float eta2, float phi1, float phi2 ) 
  {
    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
    return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
  }

  TFile *_f;

  TTree *_tree;

  std::string _foutname;

  float vx;
  float vy;
  float vz;

  int _embed_id;

  int _b_particle_n;
  int _b_particle_pid[1000];
  float _b_particle_pt[1000];
  float _b_particle_et[1000];
  float _b_particle_eta[1000];
  float _b_particle_phi[1000];

  int _b_cluster_n;
  double _b_cluster_et[500];
  double _b_cluster_eta[500];
  double _b_cluster_phi[500];
  double _b_cluster_prob[500];
  double _b_et_iso_calotower_sub_R01[500];
  double _b_et_iso_calotower_R01[500];
  double _b_et_iso_calotower_sub_R02[500];
  double _b_et_iso_calotower_R02[500];
  double _b_et_iso_calotower_sub_R03[500];
  double _b_et_iso_calotower_R03[500];
  double _b_et_iso_calotower_sub_R04[500];
  double _b_et_iso_calotower_R04[500];

  int _b_NTowers[500];
  double _b_etot[500];
  //sasha's variables
  double _b_e1t[500];
  double _b_e2t[500];
  double _b_e3t[500];
  double _b_e4t[500];
  //chase's changed variables
  double _b_chase_e1t[500];
  double _b_chase_e2t[500];
  double _b_chase_e3t[500];
  double _b_chase_e4t[500];

  int _b_clusterTower_towers;
  double _b_clusterTower_eta[500];
  double _b_clusterTower_phi[500];
  double _b_clusterTower_energy[500];

};
#endif


