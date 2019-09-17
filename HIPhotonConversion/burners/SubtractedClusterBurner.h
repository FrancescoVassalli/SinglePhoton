/**
 * @SubtractedClusterBurner.h
 * @author Francesco Vassalli <Francesco.Vassalli@colorado.edu>
 * @version 1.0
 *
 * @section Analyze the SubtractedCEMC module
 */
#ifndef SUBTRACTEDCLUSTERBURNER_H__
#define SUBTRACTEDCLUSTERBURNER_H__

#include <fun4all/SubsysReco.h>

class RawClusterContainer.h

class SubtractedClusterBurner: public SubsysReco
{

	public:

		SubtractedClusterBurner(const std::string &name);
		~SubtractedClusterBurner();
		int InitRun(PHCompositeNode*);

		int process_event(PHCompositeNode*);
		int End(PHCompositeNode*);


	private:
		bool doNodePointers(PHCompositeNode* topNode);
		SvtxVertex* get_primary_vertex(PHCompositeNode* topNode)const;
		/** helper function for process_event
		 * fills the member fields with information from the conversions 
		 * finds the clusters associated with the truth conversions*/
		void numUnique(std::map<int,Conversion>* map,SvtxTrackEval* trackEval,RawClusterContainer* mainClusterContainer,std::vector<std::pair<SvtxTrack*,SvtxTrack*>>* tightBackground);
		///fills the member fields for all the background trees
		void processTrackBackground(std::vector<SvtxTrack*>*v,SvtxTrackEval*);
		void recordConversion(Conversion *conversion,TLorentzVector *tlv_photon,TLorentzVector *tlv_electron, TLorentzVector *tlv_positron);

		int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo) const;
		float vtoR(PHG4VtxPoint* vtx)const;

		const unsigned int _kRunNumber;
		TFile *_f=NULL; ///< output file
		TTree *_ttree=NULL; ///<signal data for making track pair cuts
		string _foutname;
};



#endif // __SUBTRACTEDCLUSTERBURNER_H__



