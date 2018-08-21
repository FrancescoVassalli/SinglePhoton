#ifndef CONVERTEDPHOTONRECONSTRUCTOR_H__
#define CONVERTEDPHOTONRECONSTRUCTOR_H__

//===============================================
/// \file CONVERTEDPHOTONRECONSTRUCTOR.h
/// \brief Reconstructes electron positron tracks into photons
/// \author Francesco Vassalli
//===============================================


#include <fun4all/SubsysReco.h>
#include <g4hough/SvtxTrack.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <string>
#include <vector>

class PHCompositeNode;
class SvtxEvalStack;

class ReconstructedConvertedPhoton
{
public:
  ReconstructedConvertedPhoton(int event, const TLorentzVector& reco,const TVector3& recoVert,const TLorentzVector& truth, const TVector3& truthVert): event(event){
    recovec    =reco;
    truthvec   = truth;
    truthVertex= truthVert;
    recoVertex = recoVert;
  }
  ReconstructedConvertedPhoton(int event, const TLorentzVector& reco,const TVector3& recoVert,const TLorentzVector& truth, const TVector3& truthVert,SvtxTrack* ptrack,SvtxTrack* etrack)
    : event(event), positron(ptrack),electron(etrack){
    recovec    =reco;
    truthvec   = truth;
    truthVertex= truthVert;
    recoVertex = recoVert;
    subtracted = false;
  }
  
  ~ReconstructedConvertedPhoton(){
    delete positron;
    delete electron;
  }

  inline friend std::ostream& operator<<(std::ostream& os, ReconstructedConvertedPhoton const & tc) {
       return os <<"Converted Photon: \n \t pvec:" << tc.recovec.Pt()
        <<"\n \t truth pvec:"<<tc.truthvec.Pt()<<'\n';
  }
  void setPositron(SvtxTrack* track){positron=track;}
  void setElectron(SvtxTrack* track){electron=track;}
  inline SvtxTrack* get_positron() const{return positron;}
  inline SvtxTrack* get_electron() const{return electron;}
  void subtractEClusters();
  void subtractEClusters(SvtxTrack* track,SvtxTrack* track2);
private:
  int event;
  bool subtracted;
  //probably some stuff about the tracks 
  TLorentzVector recovec;
  TLorentzVector truthvec;
  TVector3 truthVertex;
  TVector3 recoVertex;
  SvtxTrack* positron;
  SvtxTrack* electron;
};

class ConvertedPhotonReconstructor : public SubsysReco {
  
public:
 
  ConvertedPhotonReconstructor(const std::string &name = "CONVERTEDPHOTONRECONSTRUCTOR");
		
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
  //std::vector<ReconstructedConvertedPhoton> getPhotons() const {return reconstructedConvertedPhotons;}//i removed this as a class member but I may re add it.  It is declared in the reconstruct method
 
protected:
  const float kEmass = 0.000511;

private:
  int event;
  std::string name;
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
