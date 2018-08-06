#ifndef CONVERTEDPHOTONRECONSTRUCTOR_H__
#define CONVERTEDPHOTONRECONSTRUCTOR_H__

//===============================================
/// \file CONVERTEDPHOTONRECONSTRUCTOR.h
/// \brief Reconstructes electron positron tracks into photons
/// \author Francesco Vassalli
//===============================================


#include <fun4all/SubsysReco.h>
#include <string>
#include <TVector3.h>

class ReconstructedConvertedPhoton
{
public:
  ReconstructedConvertedPhoton();
  ReconstructedConvertedPhoton(int event, const& TLorentzVector reco,const& TLorentzVector truth, const& TVector3 truthVert)
  : event(event), recovec(reco), truthvec(truth), truthVertex(truthVert){}
  //probably a track based contructor 
  ~ReconstructedConvertedPhoton();
inline friend std::ostream& operator<<(std::ostream& os, ReconstructedConvertedPhoton const & tc) {
       return os <<"Converted Photon: \n \t pvec:" << tc.recovec
        <<"\n \t truth pvec:"<<tc.truthvec<<'\n';
    }
private:
  int event;
  //probably some stuff about the tracks 
  TLorentzVector recovec;
  TLorentzVector truthvec;
  TVector3 truthVertex;
};

class PHCompositeNode;

class ConvertedPhotonReconstructor : public SubsysReco {
  
public:
 
  ConvertedPhotonReconstructor(const std::string &name = "CONVERTEDPHOTONRECONSTRUCTOR");
		
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
  std::vector<ReconstructedConvertedPhoton> getPhotons() const {return reconstructedConvertedPhotons;}
 private:
  int event;
  std::vector<ReconstructedConvertedPhoton> reconstructedConvertedPhotons;
  void reconstruct();
  static const float kEmass = 0.000511;
  inline float pToE(TVector3 v, float mass){
    return quadrature((float) quadrature(v.x(),v.y()),(float) quadrature((float)v.z(),mass));
  }
};

#endif // __CONVERTEDPHOTONRECONSTRUCTOR_H__