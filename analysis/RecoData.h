#ifndef RecoData__h
#define RecoData__h
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TObject.h>

class RecoData :public TObject
{
public:
  RecoData(){
    recoPhoton = new TLorentzVector();
    recoPhoton = new TLorentzVector();
  }
  RecoData(bool charge, string hash, TLorentzVector* recotlv1,
   TLorentzVector *recotlv2,TLorentzVector *truthtlv1,TLorentzVector *truthtlv2,
   TVector3* recoVert,TVector3* truthVert):gcharge(charge),hash(hash),recotlv1(recotlv1),
   recotlv2(recotlv2),truthtlv1(truthtlv1),truthtlv2(truthtlv2),recoVert(recoVert),truthVert(truthVert){
    recoPhoton = new TLorentzVector();
    truthPhoton = new TLorentzVector();
    *recoPhoton= *recotlv2+ *recotlv2;
    *truthPhoton = *truthtlv1+ *truthtlv2;
  }
  ~RecoData(){
    delete recoPhoton;
    delete truthPhoton;
  }
  inline bool getGoodCharge(){
    return gcharge;
  }
  inline string getHash(){
    return hash;
  }
  inline int getNtracks(){
    int r=0;
    if(recotlv1)r++;
    if(recotlv2)r++;
    return r;
  }
  inline double getRecoR(){
    return recoVert->XYvector().Mod();
  }
  //need to handle NULL pointer
  inline pair<TLorentzVector,TLorentzVector> getRecoTracks(){
    pair<TLorentzVector,TLorentzVector> r;
    r.first= *recotlv1;
    r.second= *recotlv2;
    return r;
  }
  //need to handle NULL pointer
  inline pair<TLorentzVector,TLorentzVector> getTruthTracks(){
    pair<TLorentzVector,TLorentzVector> r;
    r.first= *truthtlv1;
    r.second= *truthtlv2;
    return r;
  }
  //need to handle NULL pointer
  inline TLorentzVector getRecoPhoton(){
    return *recoPhoton;
  }
  //need to handle NULL pointer
  inline TLorentzVector getTruthPhoton(){
    return *truthPhoton;
  }
  //need to handle NULL pointer
  inline TVector3 getRecoVert(){
    return *recoVert;
  }
  //need to handle NULL pointer
  inline TVector3 getTruthVert(){
    return *truthVert;
  }

private:
  bool gcharge;
  string hash;
  TLorentzVector *recotlv1, *recotlv2,*truthtlv1,*truthtlv2,*recoPhoton,*truthPhoton;
  TVector3 *truthVert, *recoVert;
};

#endif
