
#ifndef CONVERSION_H__
#define CONVERSION_H__
class Conversion
{
public:
  Conversion(){}
  Conversion(PHG4VtxPoint* vtx);
  ~Conversion(){
    //dont delete the points as you are not the owner and did not make your own copies
  }
  void setElectron(PHG4Particle* e){
    if (e1)
    {
      if (e2)
      {
        std::cout<<"WARNING: oversetting conversion electrons"<<std::endl;
      }
      else{
        e2=e;
      }
    }
    else{
      e1=e;
    }
  }
  void setVtx(PHG4VtxPoint* vtx){
    this->vtx=vtx;
  }
  PHG4VtxPoint* getVtx(){
    return vtx;
  }
  bool isComplete(){
    if (e1&&e2&&photon)
    {
      return true;
    }
    else{
      return false;
    }
  }
  bool hasPair(){
    if (e1&&e2)
    {
      return true;
    }
    else{
      return false;
    }
  }
  /*bool acceptancePair(){

  }*/
private:
  PHG4Particle* e1=NULL;
  PHG4Particle* e2=NULL;
  PHG4Particle* photon=NULL;
  PHG4VtxPoint* vtx=NULL;

  /*inline bool inAcceptance(){

  }*/
};
#endif //CONVERSION_H__



#ifndef SINGLEPHOTONAFTER_H__
#define SINGLEPHOTONAFTER_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <TTree.h>
#include <TFile.h>

#include "TLorentzVector.h"

#include <vector>
#include <list>


class PHCompositeNode;
class Conversion;

class SinglePhotonAfter: public SubsysReco
{

 public:
  SinglePhotonAfter(const std::string &name="SinglePhotonAfter.root");
  ~SinglePhotonAfter();
  int InitRun(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

 private:
  inline float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
    return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
  }

  TFile *_f;
  TTree *_tree;
  std::string _foutname;
  std::string _b_hash;
  int _b_particle_n;
  int _b_nVtx;
  int _b_pair; //count acceotance e pairs 
  int _b_nconvert;  //this is how I count how many truth conversions are in my acceptance
  int _b_event;
  float _b_rVtx[100];
  float _b_particle_pt[100];
  float _b_particle_eta[100];
  int _b_particle_id[100];
  float _b_particle_phi[100];

  static constexpr int kTPXRADIUS=21; //in cm there is a way to get this from the simulation I should implment
  static constexpr float kRAPIDITYACCEPT=1;
};

inline int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo){
  return truthinfo->isEmbeded(particle->get_track_id());
}
inline float vtoR(PHG4VtxPoint* vtx){
  return (float) sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
}
//should check this is really working
inline int numUnique(std::list<int> *l,std::map<int,Conversion> *mymap=NULL){
  l->sort();
  int last=-1;
  int r=0;
  for (std::list<int>::iterator i = l->begin(); i != l->end(); ++i) {
    if(*i!=last){
      r++;
      TLorentzVector t;
      PHG4VtxPoint *vtx =mymap[*i].getVtx();
      t.SetXYZM(vtx->get_x(),vtx->get_y(),vtx->get_z(),0);
      if (t.Vect().XYvector().Mod()<kTPCRADIUS&&t.Rapidity()<kRAPIDITYACCEPT)
      {
        _b_nconvert++;
        if (mymap[*i].hasPair())
        {
          _b_pair++;
        }
      }
      last=*i;
    }
  }
  return r;
}
#endif // __SINGLEPHOTONAFTER_H__



