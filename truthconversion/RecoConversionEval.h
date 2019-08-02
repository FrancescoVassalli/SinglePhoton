#ifndef RecoConversionEval_H__
#define RecoConversionEval_H__

//===============================================
/// \file RecoConversionEval.h
/// \brief Use reco info to tag photon conversions
/// \author Francesco Vassalli
//===============================================
#include <fun4all/SubsysReco.h>
#include <GenFit/GFRaveVertex.h>
#include <string>
#include <cmath>
namespace PHGenFit {
	class Track;
} /* namespace PHGenFit */
class PHCompositeNode;
class PHG4TruthInfoContainer;
class SvtxEvalStack;
class SvtxVertex;
class SvtxTrack;
class SvtxTrackMap;
class SvtxHitMap;
class TrkrClusterContainer;
class RawClusterContainer;
class TTree;
class TFile;
class TLorentzVector;
class SVReco;
class VtxRegressor;

class RecoConversionEval : public SubsysReco {

	public:

		RecoConversionEval(const std::string &name,std::string tmvamethod, std::string tmvapath);
		~RecoConversionEval();
		int Init(PHCompositeNode *topNode);
		int InitRun(PHCompositeNode *topNode);
		void doNodePointers(PHCompositeNode *topNode);
		int process_event(PHCompositeNode *topNode);
		int End(PHCompositeNode *topNode);

	private:
		SvtxTrackMap* _allTracks=NULL;
		RawClusterContainer* _mainClusterContainer=NULL;
		TrkrClusterContainer* _clusterMap=NULL;
		SvtxHitMap *_hitMap=NULL;
		SVReco *_vertexer=NULL;
		std::string _fname;
		TFile *_file=NULL;
		TTree *_tree=NULL;
		VtxRegressor *_regressor=NULL;

    	PHG4TruthInfoContainer *_truthinfo=NULL;


		bool hasNodePointers()const;
		///Uses the TMVA method to correct the vtx radius
		genfit::GFRaveVertex* correctSecondaryVertex(genfit::GFRaveVertex* vtx,SvtxTrack* reco1,SvtxTrack* reco2);
    ///Uses {@link _vertexer} to refit \param reco1 and \param reco2
    std::pair<PHGenFit::Track*,PHGenFit::Track*> refitTracks(genfit::GFRaveVertex* vtx,SvtxTrack* reco1,SvtxTrack* reco2);
		///Adds \param reco1 and \param reco2 as TLorentzVectors
		TLorentzVector* reconstructPhoton(SvtxTrack* reco1,SvtxTrack* reco2);
    ///Adds the {@link PHGenFit::Track}s into a TLorentVector
    TLorentzVector* reconstructPhoton(std::pair<PHGenFit::Track*,PHGenFit::Track*> recos);
		inline bool detaCut(float eta1, float eta2) const{
			return (eta1>eta2?eta1-eta2:eta2-eta1)<_kPolarCut;
		}

		bool pairCuts(SvtxTrack* t1, SvtxTrack* t2)const;
		/** Check that the radial distance between the first hit of both tracks is less the cut 
		 * cut should be stricter for pairs with no silicone hits
		 * also need to check the approach distance
		 */
		bool hitCuts(SvtxTrack* t1, SvtxTrack* t2)const ;
		bool vtxCuts(genfit::GFRaveVertex* vtxCan);
		//!track must be closer in RZ space to the vtx than the cut
		bool vtxTrackRZCut(TVector3 recoVertPos, SvtxTrack* track);
		//!track must be closer in RPhi space to the vtx than the cut
		bool vtxTrackRPhiCut(TVector3 recoVertPos, SvtxTrack* track);
		//! vtx radius must be greater than the cut
		bool vtxRadiusCut(TVector3 recoVertPos);
		/* cut on the distance between the closest point between the two tracks*/
		bool approachDistance(SvtxTrack *t1,SvtxTrack* t2)const;
		
		float _b_photon_m;
		float _b_photon_pT;
		float _b_photon_eta;
		float _b_photon_phi;
		float _b_tphoton_m;
		float _b_tphoton_pT;
		float _b_tphoton_eta;
		float _b_tphoton_phi;
		bool _b_fake;
		// I want these to be static constexpr
    // TODO confirm these numbers
		unsigned int _kNSiliconLayer=7;
		float _kEMProbCut=.5;
		float _kPolarCut=.1;
		float _kFirstHit=3;
		float _kFirstHitStrict=1;
		float _kTrackPtCut=.4; //MeV
		double _kApprochCut=.2;
    //these are guesses 
    float _kVtxRPhiCut=.4;
    float _kVtxRCut=1.;
    float _kVtxRZCut=.4;
    float _kVtxChi2Cut=.4;
		float _kElectronRestM=.5109989461;
};

#endif // __RecoConversionEval_H__
