
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
#include "Conversion.h"

#include <vector>
#include <list>
#include <queue>
#include <set>


class PHCompositeNode;
class Conversion;
class SvtxTrackEval;

class SinglePhotonAfter: public SubsysReco
{

 public:
  /*typedef std::set<unsigned int> ClusterSet;
  typedef ClusterSet::const_iterator ConstClusterIter;
  typedef ClusterSet::iterator       ClusterIter;*/

  SinglePhotonAfter(const std::string &name="SinglePhotonAfter.root");
  ~SinglePhotonAfter();
  int InitRun(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);
  inline RawClusterContainer* getClusters()const{return &conversionClusters;}

  /*ConstClusterIter conversionClusters_begin() const {return conversionClusterIDs.begin();}
  ClusterIter conversionClusters_begin() {return conversionClusterIDs.begin();}
  ConstClusterIter conversionClusters_end() const {return conversionClusterIDs.end();}
  ClusterIter conversionClusters_end() {return conversionClusterIDs.end();}*/

 private:
  inline float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
    return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
  }

  std::queue<std::pair<int,int>> numUnique(std::list<int>* l,std::map<int,Conversion>* map,SvtxTrackEval* trackEval,RawClusterContainer *mainClusterContainer);
  void findChildren(std::queue<std::pair<int,int>> missing,PHG4TruthInfoContainer* truthinfo);

  const static int kMAXParticles=1000;

  TFile *_f;
  TTree *_tree;
  std::string _foutname;
  std::string _b_hash;
  int _b_event;
  int _b_nVtx;  //total conversions
  int _b_Tpair; //count acceptance e pairs in truth
  int _b_Rpair; //count acceptance e pairs in reco
  float _b_rVtx[kMAXParticles];  //truth radius
  bool _b_pythia[kMAXParticles];  //tracks if the conversion is from pythia or G4 particle
  float _b_electron_pt[kMAXParticles];
  float _b_positron_pt[kMAXParticles];
  float _b_parent_pt  [kMAXParticles];
  float _b_parent_eta [kMAXParticles];
  float _b_parent_phi [kMAXParticles];

  RawClusterContainer conversionClusters;

  const static int kTPCRADIUS=21; //in cm there is a way to get this from the simulation I should implement
  float kRAPIDITYACCEPT=1;
};

inline int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo){
  return truthinfo->isEmbeded(particle->get_track_id());
}
inline float vtoR(PHG4VtxPoint* vtx){
  return (float) sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
}

#endif // __SINGLEPHOTONAFTER_H__



