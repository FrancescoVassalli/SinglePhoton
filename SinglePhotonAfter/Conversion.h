#ifndef CONVERSION_H__
#define CONVERSION_H__

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4hough/SvtxTrack.h>

class SvtxTrackEval;

class Conversion
{
public:
  Conversion(){}
  Conversion(PHG4VtxPoint* vtx){
    this->vtx=vtx;
  }
  ~Conversion(){
    //dont delete the points as you are not the owner and did not make your own copies
  }
  inline void setElectron(PHG4Particle* e){
    if (e1)
    {
      if (e2)
      {
        std::cout<<"WARNING: oversetting conversion electrons"<<std::endl;
      }
      else{
        e2=e;
      }
    }
    else{
      e1=e;
    }
  }
  inline void setVtx(PHG4VtxPoint* vtx){
    this->vtx=vtx;
  }
  inline PHG4VtxPoint* getVtx()const{
    return vtx;
  }
  inline bool isComplete()const{
    if (e1&&e2&&photon)
    {
      return true;
    }
    else{
      return false;
    }
  }
  inline bool hasPair()const{
    if (e1&&e2)
    {
      return true;
    }
    else{
      return false;
    }
  }
  int setRecoTracks(SvtxTrackEval* trackeval);

  inline int recoCount(){
    int r=0;
    if (reco1)
    {
      r++;
    }
    if (reco2)
    {
      r++;
    }
    return r;
  }
  /*bool acceptancePair(){

  }*/
private:
  PHG4Particle* e1=NULL;
  PHG4Particle* e2=NULL;
  PHG4Particle* photon=NULL;
  PHG4VtxPoint* vtx=NULL;
  SvtxTrack* reco1=NULL;
  SvtxTrack* reco2=NULL;
  /*inline bool inAcceptance(){

  }*/
};
#endif //CONVERSION_H__
