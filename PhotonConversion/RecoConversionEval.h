#ifndef RecoConversionEval_H__
#define RecoConversionEval_H__

//===============================================
/// \file RecoConversionEval.h
/// \brief Use reco info to tag photon conversions
/// \author Francesco Vassalli
//===============================================


#include <fun4all/SubsysReco.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <string>
#include <vector>
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
		std::string _fname;
		TFile *_file;
		TTree *_tree;

		void process_recoTracks(PHCompositeNode *topNode);
		template<class T>
			T quadrature(T d1, T d2){
				return TMath::Sqrt((double)d1*d1+d2*d2);
			}
};

#endif // __RecoConversionEval_H__
