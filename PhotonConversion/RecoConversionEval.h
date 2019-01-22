#ifndef RecoConversionEval_H__
#define RecoConversionEval_H__

//===============================================
/// \file RecoConversionEval.h
/// \brief Use reco info to tag photon conversions
/// \author Francesco Vassalli
//===============================================

#include "RaveVertexingAux.h"
#include <fun4all/SubsysReco.h>
#include <trackbase_historic/SvtxTrack.h>
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
class SvtxVertex;
class SvtxTrackMap;

class RecoConversionEval : public SubsysReco {

	public:

		RecoConversionEval(const std::string &name);
		~RecoConversionEval(){
			if (_auxVertexer) delete _auxVertexer;
		}	
		int Init(PHCompositeNode *topNode);
		int InitRun(PHCompositeNode *topNode);
		int process_event(PHCompositeNode *topNode);
		int End(PHCompositeNode *topNode);

	private:
		SvtxTrackMap* _allTracks;
		RawClusterContainer* _mainClusterContainer;
		SvtxClusterMap* _svtxClusterMap;
		SvtxHitMap *_hitMap;
		RaveVertexingAux *_auxVertexer;
		std::string _fname;
		TFile *_file;
		TTree *_tree;

		void doNodePointers(PHCompositeNode *topNode);
		bool hasNodePointers()const;
		void process_recoTracks(PHCompositeNode *topNode);

		inline detaCut(SvtxTrack *t1,SvtxTrack *t2) const{
			return fabs(t1->get_eta()-t2->get_eta())<_kPolarCut;
		}

		bool pairCuts(SvtxTrack* t1, SvtxTrack* t2)const;
		/* Check that the radial distance between the first hit of both tracks is less the cut 
		 * cut should be stricter for pairs with no silicone hits
		 * also need to check the approach distance
		 */
		bool hitCuts(SvtxTrack* t1, SvtxTrack* t2)const ;
		bool vtxCuts(SvtxVertex* vtx);
		/* cut on the distance between the closest point between the two tracks*/
		bool approachDistance(SvtxTrack *t1,SvtxTrack* t2)const;
		// I want these to be static constexpr
		unsigned int _kNSiliconLayer=7;
		float _kEMProbCut=.5;
		float _kPolarCut=.1;
		float _kFirstHit=3;
		float _kFirstHitStrict=1;
		float _kTrackPtCut=.4; //MeV
		double _kApprochCut=.2;


};

#endif // __RecoConversionEval_H__
