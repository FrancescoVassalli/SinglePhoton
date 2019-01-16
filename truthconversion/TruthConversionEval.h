
/**
 * @TruthConversionEval.h
 * @author Francesco Vassalli <Francesco.Vassalli@colorado.edu>
 * @version 1.0
 *
 * @section Uses truth particle information to find photon conversions. 
 * Infomation about the conversions is recored in a TTree.
 * Finally they are associated with clusters for latter analysis
 */
#ifndef TRUTHCONVERSIONEVAL_H__
#define TRUTHCONVERSIONEVAL_H__

#include "TLorentzVector.h"
#include "Conversion.h"
#include "../PhotonConversion/RaveVertexingAux.h"

#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <fun4all/SubsysReco.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>

#include <calobase/RawClusterContainer.h>

#include <TTree.h>
#include <TFile.h>

#include <vector>
#include <queue>
#include <set>


class PHCompositeNode;
class Conversion;
class SvtxTrackEval;

class TruthConversionEval: public SubsysReco
{

  public:
    /**
     *
     * @param name name of the output file
     * @param runnumber printed in TTree for condor jobs
     * @param particleEmbed the embedID of particles embeded by Fun4All
     * @param pythiaEmbed the embedID for an embeded pythia event
     * @param makeTTree true-writes the TTree false-won't make TTree will still find clusters
     */
    TruthConversionEval(const std::string &name,unsigned int runnumber, 
        int particleEmbed, int pythiaEmbed,bool makeTTree);
    ~TruthConversionEval();
    int InitRun(PHCompositeNode*);
    /**
     * Find the conversions pass them to numUnique.
     * Fill the TTree
     * @return event status */
    int process_event(PHCompositeNode*);
    int End(PHCompositeNode*);
    /** get the clusters associated with converions*/
    inline const RawClusterContainer* getClusters()const {return &_conversionClusters;} 

  private:
    inline float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
      float deta = eta1 - eta2;
      float dphi = phi1 - phi2;
      if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
      if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
      return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
    }
    inline void doNodePointers(PHCompositeNode* topNode){
      _mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
      _truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
      _svtxClusterMap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap");
      _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");
      _vertexer= new RaveVertexingAux(topNode);
      _vertexer->Verbosity(10);
      _topNode=topNode;
    }
    /** helper function for process_event
     * fills the member fields with information from the conversions 
     * finds the clusters associated with the conversions
     * @return currently will return nothing 
     * but can easily be changed to return a structure for the converions with only 1 truth associated track*/
    std::queue<std::pair<int,int>> numUnique(std::map<int,Conversion>* map,SvtxTrackEval* trackEval,RawClusterContainer* mainClusterContainer);
    /** attempts to find other truth associated tracks for conversions with only 1 truth associated track*/
    void findChildren(std::queue<std::pair<int,int>> missing,PHG4TruthInfoContainer* truthinfo);
    /** @param map should contain Conversion objects which hold background events i.e. not conversions
     * fills the fields for {@link _backgroundCutTree*/
    void processBackground(std::map<int,Conversion>* map,SvtxTrackEval* trackEval);

    const static int s_kMAXParticles=200; //< increase this number if arrays go out of bounds
    const unsigned int _kRunNumber;
    const int _kParticleEmbed;
    const int _kPythiaEmbed;
    const bool _kMakeTTree;
    int _runNumber; ///<for the TTree do not change
    TFile *_f=NULL;
    TTree *_tree=NULL; ///< stores most of the data about the conversions
    TTree *_signalCutTree=NULL; //<signal data for making track pair cuts
    TTree *_backgroundCutTree=NULL; //<background data for making track pair cuts
    RawClusterContainer *_mainClusterContainer;
    PHG4TruthInfoContainer *_truthinfo;
    SvtxClusterMap* _svtxClusterMap;
    SvtxHitMap *_hitMap;
    std::string _foutname;
    PHCompositeNode *_topNode=NULL;
    RaveVertexingAux *_vertexer=NULL;
    /** \defgroup mainTreeVars Variables for {@link _tree}
      @{*/
    int _b_event;
    int _b_nVtx;  ///<total conversions
    int _b_Tpair; ///<count acceptance e pairs in truth
    int _b_Rpair; ///<count acceptance e pairs in reco
    float _b_rVtx[s_kMAXParticles];  ///<truth conversion radius used for the signal tree
    bool _b_pythia[s_kMAXParticles];  ///<record if the conversion is from pythia or G4 particle
    float _b_electron_pt[s_kMAXParticles];
    float _b_positron_pt[s_kMAXParticles];
    float _b_parent_pt  [s_kMAXParticles];
    float _b_parent_eta [s_kMAXParticles];
    float _b_parent_phi [s_kMAXParticles];
    int _b_grandparent_id [s_kMAXParticles];
    /**@}*/
    /** \defgroup signalTreeVars Variables for {@link _signalCutTree}
      @{*/
    float _b_track_deta ;
    bool _b_track_layer ;
    int _b_track_dlayer ;
    double _b_approach ;
    float _b_vtx_radius;
    float _b_vtx_chi2;
    float _b_photon_m;
    float _b_photon_pT;
    /**@}*/
    /** \defgroup backTreeVars Variables for {@link _signalCutTree}
      @{*/
    float _bb_track_deta ;
    int _bb_track_layer ;
    int _bb_track_dlayer ;
    double _bb_approach ;
    float _bb_vtx_radius;
    float _bb_vtx_chi2;
    float _bb_photon_m;
    float _bb_photon_pT;
    /**@}*/
    /** RawClusters associated with truth conversions
     * processed by other modules*/
    RawClusterContainer _conversionClusters;

    const static int s_kTPCRADIUS=21; //in cm there is a way to get this from the simulation I should implement?
    ///<TPC radius currently hardcoded
    float _kRAPIDITYACCEPT=1; //<acceptance rapidity currently hard coded to |1|
};

inline int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo){
  return truthinfo->isEmbeded(particle->get_track_id());
}
inline float vtoR(PHG4VtxPoint* vtx){
  return (float) sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
}

#endif // __TRUTHCONVERSIONEVAL_H__



