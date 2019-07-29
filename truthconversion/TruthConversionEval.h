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

#include <fun4all/SubsysReco.h>
#include <calobase/RawClusterContainer.h>
#include <queue>

class PHCompositeNode;
class PHG4TruthInfoContainer;
class PHG4Particle;
class PHG4VtxPoint;
class Conversion;
class SvtxTrackEval;
class SvtxTrack;
class SvtxHitMap;
class SvtxHit;
class SvtxClusterMap;
class SvtxCluster;
class RawClusterContainer;
class TTree;
class TFile;
class SVReco;
class TrkrClusterContainer;

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
    const RawClusterContainer* getClusters()const;

  private:
    bool doNodePointers(PHCompositeNode* topNode);
    /** helper function for process_event
     * fills the member fields with information from the conversions 
     * finds the clusters associated with the truth conversions*/
    void numUnique(std::map<int,Conversion>* map,SvtxTrackEval* trackEval,RawClusterContainer* mainClusterContainer);
    ///fills the member fields for all the background trees
    void processTrackBackground(std::vector<SvtxTrack*>*v,TrkrClusterContainer*);

    int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo) const;
    float vtoR(PHG4VtxPoint* vtx)const;

    const static int s_kMAXParticles=200; ///< increase this number if arrays go out of bounds
    const unsigned int _kRunNumber;
    const int _kParticleEmbed; ///< primary embedID
    const int _kPythiaEmbed; ///< background event embedID i.e. pythia or AA
    const bool _kMakeTTree;//< if false no TTrees are output
    int _runNumber; ///<for the TTree do not change
    TFile *_f=NULL; ///< output file
    TTree *_signalCutTree=NULL; ///<signal data for making track pair cuts
    TTree *_trackBackTree=NULL;///< background for all possible single tracks
    TTree *_pairBackTree=NULL;///< background for all possible track pairs
    TTree *_vtxBackTree=NULL;///< background that passes existing track pair cuts
    TTree *_vtxingTree=NULL; ///<data for training vtxing
    RawClusterContainer *_mainClusterContainer; //< clusters from the node
    PHG4TruthInfoContainer *_truthinfo;
    TrkrClusterContainer* _clusterMap;
    SvtxHitMap *_hitMap;
    std::string _foutname; ///< name of the output file
    SVReco *_vertexer=NULL; ///< for reco vertex finding
    
    /** \defgroup  variables  for the TTrees
      @{*/
    /** # of clusters associated with each conversion that has 2 reco tracks
     * 1 indicates the reco tracks go to the same cluster ~15% of conversions*/
    int   _b_nCluster; 
    int _b_track1_pid;
    int _b_track2_pid;
    int _bb_track1_pid;
    int _bb_track2_pid;
    float _b_cluster_dphi ;
    float _b_cluster_deta;
    int   _bb_nCluster; 
    float _bb_cluster_dphi ;
    float _bb_cluster_deta;
    float _b_track1_pt;
    float _b_track1_eta;
    float _b_track1_phi;
    float _b_track2_pt;
    float _b_track2_eta;
    float _b_track2_phi;
    float _b_track_deta ;
    int _b_track_layer ;
    int _b_track_dlayer ;
    float _b_track_pT;
    float _b_track_dca;
    float _b_ttrack_pT;
    double _b_approach  ;
    float _b_vtx_radius ;
    float _b_vtx_phi ;
    float _b_vtx_eta ;
    float _b_vtx_x ;
    float _b_vtx_y ;
    float _b_vtx_z ;
    float _b_tvtx_eta ;
    float _b_tvtx_x ;
    float _b_tvtx_y ;
    float _b_tvtx_z ;
    float _b_tvtx_radius ;
    float _b_tvtx_phi ;
    float _b_vtxTrackRZ_dist;
    float _b_vtxTrackRPhi_dist;
    float _b_vtx_chi2;
    float _b_photon_m;
    float _b_tphoton_m;
    float _b_photon_pT;
    float _b_cluster_prob;
    float _b_track_dphi;
    //bb stands for background branch
    float _bb_track_deta ;
    float _bb_vtx_radius ;
    float _bb_track_dca ;
    int _bb_track_layer ;
    int _bb_track_dlayer ;
    float _bb_track_pT;
    double _bb_approach ;
    float _bb_vtxTrackRZ_dist;
    float _bb_vtxTrackRPhi_dist;
    float _bb_vtx_chi2;
    float _bb_photon_m;
    float _bb_photon_pT;
    float _bb_cluster_prob;
    float _bb_track_dphi;
    int _bb_pid;
    /**@}*/
    /** RawClusters associated with truth conversions
     * processed by other modules currently empty*/
    RawClusterContainer _conversionClusters;

    const static int s_kTPCRADIUS=21; //in cm there is a way to get this from the simulation I should implement?
    ///<TPC radius currently hardcoded
    float _kRAPIDITYACCEPT=1; //<acceptance rapidity currently hard coded to |1|
};



#endif // __TRUTHCONVERSIONEVAL_H__



