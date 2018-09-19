#ifndef SINGLEPHOTONAFTER_H__
#define SINGLEPHOTONAFTER_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <vector>
#include <list>
#include <TTree.h>
#include <TFile.h>

class PHCompositeNode;

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
  int _b_particle_n;
  int _b_nVtx;
  int _b_event;
  float _b_particle_pt[100];
  float _b_particle_eta[100];
  int _b_particle_id[100];
  float _b_particle_phi[100];
};

inline int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo){
  return truthinfo->isEmbeded(particle->get_track_id());
}
inline bool withinR(PHG4VtxPoint* vtx,double r){
  return sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y())<r;
}
inline int numUnique(std::list<int> l){
  l.sort();
  int last=-1;
  int r=0;
  for (std::list<int>::iterator i = l.begin(); i != l.end(); ++i) {
    if(*i!=last){
      r++;
      last=*i;
    }
  }
  return r;
}
#endif // __SINGLEPHOTONAFTER_H__

