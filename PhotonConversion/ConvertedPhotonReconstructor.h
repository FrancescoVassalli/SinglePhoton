#ifndef CONVERTEDPHOTONRECONSTRUCTOR_H__
#define CONVERTEDPHOTONRECONSTRUCTOR_H__

//===============================================
/// \file CONVERTEDPHOTONRECONSTRUCTOR.h
/// \brief Reconstructes electron positron tracks into photons
/// \author Francesco Vassalli
//===============================================


#include <fun4all/SubsysReco.h>
#include <g4eval/SvtxEvalStack.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include<iostream>
#include <string>

class PHCompositeNode;

class ReconstructedConvertedPhoton
{
public:
  ReconstructedConvertedPhoton(const std::string& name, int event, const TLorentzVector& reco,const TVector3& recoVert,const TLorentzVector& truth, const TVector3& truthVert)
  : event(event), recovec(reco), recoVertex(recoVert),truthvec(truth), truthVertex(truthVert){}
  
  ~ReconstructedConvertedPhoton(){}

  inline friend std::ostream& operator<<(std::ostream& os, ReconstructedConvertedPhoton const & tc) {
       return os <<"Converted Photon: \n \t pvec:" << tc.recovec.Pt()
        <<"\n \t truth pvec:"<<tc.truthvec.Pt()<<'\n';
    }

private:
  int event;
  //probably some stuff about the tracks 
  TLorentzVector recovec;
  TLorentzVector truthvec;
  TVector3 truthVertex;
  TVector3 recoVertex;
};

class ConvertedPhotonReconstructor : public SubsysReco {
  
public:
 
  ConvertedPhotonReconstructor(const std::string &name = "CONVERTEDPHOTONRECONSTRUCTOR");
		
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
  std::vector<ReconstructedConvertedPhoton> getPhotons() const {return reconstructedConvertedPhotons;}
 
protected:
  const float kEmass = 0.000511;

private:
  int event;
  string name;
  SvtxEvalStack* _svtxevalstack; 
  std::vector<ReconstructedConvertedPhoton> reconstructedConvertedPhotons;
  TFile *_file;
  TTree *_tree;
  TLorentzVector b_recovec;
  TLorentzVector b_truthvec;
  TVector3       b_truthVertex;
  TVector3       b_recoVertex;
  
  void reconstruct(SvtxEvalStack *stack,PHCompositeNode *topNode);
  inline float pToE(TVector3 v, float mass){
    return quadrature((float) quadrature(v.x(),v.y()),(float) quadrature((float)v.z(),mass));
  }
  template<class T>
  T quadrature(T d1, T d2){
      return TMath::Sqrt((double)d1*d1+d2*d2);
  }
};

#endif // __CONVERTEDPHOTONRECONSTRUCTOR_H__
