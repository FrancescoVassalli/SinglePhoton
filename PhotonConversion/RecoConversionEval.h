#ifndef RecoConversionEval_H__
#define RecoConversionEval_H__

//===============================================
/// \file RecoConversionEval.h
/// \brief Use reco info to tag photon conversions
/// \author Francesco Vassalli
//===============================================


#include <fun4all/SubsysReco.h>
#include <phool/getClass.h>
#include <phool/PHDataNode.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
class PHCompositeNode;
class SvtxEvalStack;
class SvtxClusterMap;

class RecoConversionEval : public SubsysReco {

	public:

		RecoConversionEval(const std::string &name);
		~RecoConversionEval();	
		int Init(PHCompositeNode *topNode);
		int InitRun(PHCompositeNode *topNode);
		int process_event(PHCompositeNode *topNode);
		int End(PHCompositeNode *topNode);

	private:
    SvtxTrackMap* _allTracks;
    RawClusterContainer* _mainClusterContainer;
    SvtxClusterMap* _svtxClusterMap;
    SvtxHitMap *_hitMap;
		std::string _fname;
		TFile *_file;
		TTree *_tree;

    inline void doNodePointers(PHCompositeNode *topNode){
      _allTracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
      _mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
      _svtxClusterMap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap");
      _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");
    }

    inline bool hasNodePointers(){
      return _allTracks &&_mainClusterContainer && _svtxClusterMap&&_hitMap;
    }

    inline float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
      float deta = eta1 - eta2;
      float dphi = phi1 - phi2;
      if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
      if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
      return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
    }
		void process_recoTracks(PHCompositeNode *topNode);
		template<class T>
			T quadrature(T d1, T d2){
				return TMath::Sqrt((double)d1*d1+d2*d2);
			}

      inline bool pairCuts(SvtxTrack* t1, SvtxTrack* t2){
        std::cout<<"polar:"<<fabs(t1->get_eta()-t2->get_eta())<<'\n';
        return fabs(t1->get_eta()-t2->get_eta())<_kPolarCut && hitCuts(t1,t2);

      }
      /* Check that the radial distance between the first hit of both tracks is less the cut 
      * cut should be stricter for pairs with no silicone hits
      * also need to check the approach distance
      */
      inline bool hitCuts(SvtxTrack* t1, SvtxTrack* t2){
        SvtxCluster *c1 = _svtxClusterMap->get(*(t1->begin_clusters()));
        SvtxCluster *c2 = _svtxClusterMap->get(*(t2->begin_clusters()));
        SvtxHit *h1 = _hitMap->get(*(c1->begin_hits()));
        SvtxHit *h2 = _hitMap->get(*(c2->begin_hits()));
        //check that the first hits are close enough
        if (c1->get_layer()>_kNSiliconLayer&&c2->get_layer()>_kNSiliconLayer)
        {
          std::cout<<"No silicon hits layers diff="<<abs(h1->get_layer()-h2->get_layer())<<'\n';
          if (abs(h1->get_layer()-h2->get_layer())>_kFirstHitStrict)
          {
            return false;
          }
        }
        else{
          std::cout<<"Hits! layers diff="<<abs(h1->get_layer()-h2->get_layer())<<'\n';
          if (abs(h1->get_layer()-h2->get_layer())>_kFirstHit)
          {
            return false;
          }
        }
        //check the approach distance
        return true;
      }
      // I want these to be static constexpr
     unsigned int _kNSiliconLayer=7;
     float _kEMProbCut=.5;
     float _kPolarCut=.1;
     float _kFirstHit=3;
     float _kFirstHitStrict=1;
     float _kTrackPtCut=.4; //MeV


};

#endif // __RecoConversionEval_H__
