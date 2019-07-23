#include "Conversion.h"
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterv1.h>
#include <assert.h>

Conversion::Conversion(SvtxTrackEval* trackeval,int verbosity){
  this->trackeval=trackeval;
  this->verbosity=verbosity;
}
Conversion::Conversion(PHG4VtxPoint* vtx,int verbosity){
  this->vtx=vtx;
  this->verbosity=verbosity;
}
Conversion::Conversion(PHG4VtxPoint* vtx,SvtxTrackEval *trackeval,int verbosity){
  this->trackeval=trackeval;
  this->vtx=vtx;
  this->verbosity=verbosity;
}

Conversion::~Conversion(){
  if(recoVertex) delete recoVertex;
  if(recoPhoton) delete recoPhoton;
  //dont delete the points as you are not the owner and did not make your own copies
}
void Conversion::setElectron(PHG4Particle* e){
  if (e1)
  {
    if (e2&&verbosity>0)
    {
      std::cout<<"WARNING in Conversion oversetting conversion electrons"<<std::endl;
    }
    else{
      e2=e;
    }
  }
  else{
    e1=e;
  }
}


bool Conversion::setElectron(){
  if (hasPair())
  {
    if (e1->get_pid()<0)
    {
      PHG4Particle *temp = e1;
      e1=e2;
      e2=temp;
    }
    if (e1->get_pid()<0)
    {
      if (verbosity>0)
      {
        std::cout<<"Warning in Conversion bad charges in conversion"<<std::endl;
      }
      return false;
    }
  }
  else{
    if (!e1)
    {
      if (verbosity>0)
      {
        std::cout<<"Warning in Conversion no truth tracks set"<<std::endl;
      }
      return false;
    }
    else if (e1->get_pid()<0) return false;
  }
  return true;
}

PHG4Particle* Conversion::getElectron(){
  setElectron();
  return e1;
}

PHG4Particle* Conversion::getPositron(){
  if(setElectron()){
    return e2;
  }
  else{
    return e1;
  }
}

int Conversion::setRecoTracks(SvtxTrackEval* trackeval){	
  this->trackeval=trackeval;
  setElectron();
  if (e1)
  {
    reco1=trackeval->best_track_from(e1);  
  }
  if (e2)
  {
    reco2=trackeval->best_track_from(e2);
  }
  if(reco2==reco1){
    reco2=NULL;
  }
  int r=0;
  if (reco1)
  {
    r++;
  }
  if (reco2)
  {
    r++;
  }
  setRecoPhoton();
  return r;
}

int Conversion::setRecoTracks(){  
  assert(trackeval);
  if (e1)
  {
    reco1=trackeval->best_track_from(e1); // have not checked that these are in range 
  }
  if (e2)
  {
    reco2=trackeval->best_track_from(e2);
  }
  if(reco2==reco1){
    reco2=NULL;
  }
  int r=0;
  if (reco1)
  {
    r++;
  }
  if (reco2)
  {
    r++;
  }
  setRecoPhoton();
  return r;
}

TLorentzVector* Conversion::setRecoPhoton(){
  if (reco1&&reco2)
  {
    TLorentzVector tlv1(reco1->get_px(),reco1->get_py(),reco1->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco1->get_p()*reco1->get_p()));
    TLorentzVector tlv2(reco2->get_px(),reco2->get_py(),reco2->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco2->get_p()*reco2->get_p()));
    if (recoPhoton) delete recoPhoton;
    recoPhoton= new TLorentzVector(tlv1+tlv2);
  }
  return recoPhoton;
}


int Conversion::get_cluster_id(){
  if(!reco1){
    assert(trackeval);
    reco1=trackeval->best_track_from(e1);
    if(!reco1){
      return -1;
    }
  }
  return reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1));//id of the emcal
}

int Conversion::get_cluster_id()const{
  if(!reco1){
    return -1;
  }
  else return reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1));//id of the emcal
}

int Conversion::get_cluster_id(SvtxTrackEval *trackeval){
  this->trackeval=trackeval;
  if (!reco1)
  {
    reco1=trackeval->best_track_from(e1);
    if(!reco1){
      cout<<"bad reco"<<endl;
      return -1;
    }
  }
  return reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1));//id of the emcal
}

std::pair<int,int> Conversion::get_cluster_ids(){
  switch(recoCount()){
    case 2:
      return std::pair<int,int>(reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)),reco2->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      break;
    case 1:
      if (reco1)
      {
        return std::pair<int,int>(reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)),-1);
      }
      else return std::pair<int,int>(reco2->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)),-1);
      break;
    default:
      return std::pair<int,int>(-1,-1);
      break;
  }
}



bool Conversion::hasSilicon(SvtxClusterMap* svtxClusterMap){
  switch(recoCount()){
    case 2:
      {
        SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
        SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
        return c1->get_layer()<=_kNSiliconLayer||c2->get_layer()<=_kNSiliconLayer;
      }
      break;
    case 1:
      {
        SvtxCluster *c1;
        if (reco1)
        {
          c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
        }
        else{
          c1 = svtxClusterMap->get(*(reco2->begin_clusters()));
        }
        return c1->get_layer()<=_kNSiliconLayer;
      }
      break;
    default:
      return false;
      break;
  }
}

float Conversion::vtxTrackRZ(TVector3 vertpos){
  float d1=vtxTrackRZ(vertpos,reco1);
  float d2=vtxTrackRZ(vertpos,reco2);
  return d1>d2?d1:d2;
}

float Conversion::vtxTrackRPhi(TVector3 vertpos){
  float d1=vtxTrackRPhi(vertpos,reco1);
  float d2=vtxTrackRPhi(vertpos,reco2);
  return d1>d2?d1:d2;
}

float Conversion::vtxTrackRZ(TVector3 recoVertPos,SvtxTrack *track){
  float dR = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y())-sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
  float dZ = recoVertPos.z()-track->get_z();
  return sqrt(dR*dR+dZ*dZ);
}

float Conversion::vtxTrackRPhi(TVector3 recoVertPos,SvtxTrack *track){
  float vtxR=sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
  float trackR=sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
  return sqrt(vtxR*vtxR+trackR*trackR-2*vtxR*trackR*cos(recoVertPos.Phi()-track->get_phi()));
}

int Conversion::trackDLayer(SvtxClusterMap* svtxClusterMap,SvtxHitMap *hitmap){
  if (recoCount()==2){
    SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
    SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
    SvtxHit *h1 = hitmap->get(*(c1->begin_hits()));
    SvtxHit *h2 = hitmap->get(*(c2->begin_hits()));
    int l1 = h1->get_layer();
    int l2 = h2->get_layer();
    return abs(l1-l2);
  }
  else return -1;
}

float Conversion::minDca(){
  if(reco1->get_dca()>reco2->get_dca()) return reco2->get_dca();
  else return reco1->get_dca();
}

int Conversion::trackDLayer(TrkrClusterContainer* clusterMap){
  if (recoCount()==2){
    TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
    TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
    unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
    unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
    return abs(l1-l2);
  }
  else return -1;
}
int Conversion::trackDLayer(TrkrClusterContainer* clusterMap,SvtxTrack* reco1, SvtxTrack* reco2){
    TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
    TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
    unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
    unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
    return abs(l1-l2);
}

int Conversion::firstLayer(SvtxClusterMap* svtxClusterMap,SvtxHitMap *hitmap){
  switch(recoCount()){
    case 2:
      {
        SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
        SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
        SvtxHit *h1 = hitmap->get(*(c1->begin_hits()));
        SvtxHit *h2 = hitmap->get(*(c2->begin_hits()));
        if(h1->get_layer()>h2->get_layer()){
          return h2->get_layer();
        }
        else return h1->get_layer();
      }
    case 1:
      {
        if (reco1)return svtxClusterMap->get(*(reco1->begin_clusters()))->get_layer();
        else return svtxClusterMap->get(*(reco2->begin_clusters()))->get_layer();
      }
    default:
      return -1;
  }
}

int Conversion::firstLayer(TrkrClusterContainer* clusterMap){
  switch(recoCount()){
    case 2:
      {
        TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
        TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
        unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
        unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
        //maybe i need this? TrkrDefs::hitsetkey hitsetkey = TrkrDefs::getHitSetKeyFromClusKey(cluskey);
        if(l1>l2){
          return l1;
        }
        else return l2;
      }
    case 1:
      {
        if (reco1)return TrkrDefs::getLayer(*(reco1->begin_cluster_keys()));
        else return TrkrDefs::getLayer(*(reco2->begin_cluster_keys()));
      }
    default:
      return -1;
  }
}

double Conversion::dist(PHG4VtxPoint *recovtx,SvtxClusterMap* svtxClusterMap){
  SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
  SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
  double r1 = sqrt(abs(c1->get_x()-recovtx->get_x())+abs(c1->get_y()-recovtx->get_y())+abs(c1->get_z()-recovtx->get_z()));
  double r2 = sqrt(abs(c2->get_x()-recovtx->get_x())+abs(c2->get_y()-recovtx->get_y())+abs(c2->get_z()-recovtx->get_z()));
  if (r1>r2)
  {
    return r1;
  }
  else return r2;
}

double Conversion::dist(TVector3 *recovtx,SvtxClusterMap* svtxClusterMap){
  SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
  SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
  double r1 = sqrt(abs(c1->get_x()-recovtx->x())+abs(c1->get_y()-recovtx->y())+abs(c1->get_z()-recovtx->z()));
  double r2 = sqrt(abs(c2->get_x()-recovtx->x())+abs(c2->get_y()-recovtx->y())+abs(c2->get_z()-recovtx->z()));
  if (r1>r2)
  {
    return r1;
  }
  else return r2;
}

double Conversion::dist(TVector3 *recovtx,TrkrClusterContainer* clusterMap){
  TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
  TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
  double r1 = sqrt(abs(c1->getX()-recovtx->x())+abs(c1->getY()-recovtx->y())+abs(c1->getZ()-recovtx->z()));
  double r2 = sqrt(abs(c2->getX()-recovtx->x())+abs(c2->getY()-recovtx->y())+abs(c2->getZ()-recovtx->z()));
  if (r1>r2)
  {
    return r1;
  }
  else return r2;
}

float Conversion::setRecoVtx(SvtxVertex *recovtx,SvtxClusterMap* svtxClusterMap){
  recoVtx=recovtx;
  SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
  SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
  float r1 = sqrt(abs(c1->get_x()-recovtx->get_x())+abs(c1->get_y()-recovtx->get_y())+abs(c1->get_z()-recovtx->get_z()));
  float r2 = sqrt(abs(c2->get_x()-recovtx->get_x())+abs(c2->get_y()-recovtx->get_y())+abs(c2->get_z()-recovtx->get_z()));
  if (r1>r2)
  {
    return r1;
  }
  else return r2;
}

double Conversion::approachDistance()const{
  if (recoCount()==2)
  {
    static const double eps = 0.000001;
    TVector3 u(reco1->get_px(),reco1->get_py(),reco1->get_pz());
    TVector3 v(reco2->get_px(),reco2->get_py(),reco2->get_pz());
    TVector3 w(reco1->get_x()-reco2->get_x(),reco1->get_x()-reco2->get_y(),reco1->get_x()-reco2->get_z());

    double a = u.Dot(u);
    double b = u.Dot(v);
    double c = v.Dot(v);
    double d = u.Dot(w);
    double e = v.Dot(w);

    double D = a*c - b*b;
    double sc, tc;
    // compute the line parameters of the two closest points
    if (D < eps) {         // the lines are almost parallel
      sc = 0.0;
      tc = (b>c ? d/b : e/c);   // use the largest denominator
    }
    else {
      sc = (b*e - c*d) / D;
      tc = (a*e - b*d) / D;
    }
    // get the difference of the two closest points
    u*=sc;
    v*=tc;
    w+=u;
    w-=v;
    return w.Mag();   // return the closest distance 
  }
  else return -1; 
}

float Conversion::trackDEta()const{
  if (recoCount()==2)
  {
    return fabs(reco1->get_eta()-reco2->get_eta());
  }
  else return -1.;
}

double Conversion::approachDistance(SvtxTrack* reco1, SvtxTrack* reco2){
    static const double eps = 0.000001;
    TVector3 u(reco1->get_px(),reco1->get_py(),reco1->get_pz());
    TVector3 v(reco2->get_px(),reco2->get_py(),reco2->get_pz());
    TVector3 w(reco1->get_x()-reco2->get_x(),reco1->get_x()-reco2->get_y(),reco1->get_x()-reco2->get_z());

    double a = u.Dot(u);
    double b = u.Dot(v);
    double c = v.Dot(v);
    double d = u.Dot(w);
    double e = v.Dot(w);

    double D = a*c - b*b;
    double sc, tc;
    // compute the line parameters of the two closest points
    if (D < eps) {         // the lines are almost parallel
      sc = 0.0;
      tc = (b>c ? d/b : e/c);   // use the largest denominator
    }
    else {
      sc = (b*e - c*d) / D;
      tc = (a*e - b*d) / D;
    }
    // get the difference of the two closest points
    u*=sc;
    v*=tc;
    w+=u;
    w-=v;
    return w.Mag();   // return the closest distance 
}

float Conversion::trackDEta(SvtxTrack* reco1, SvtxTrack* reco2){
    return fabs(reco1->get_eta()-reco2->get_eta());
}

float Conversion::minTrackpT(){
  switch(recoCount()){
    case 2:
      if (reco1->get_pt()<reco2->get_pt())
      {
        return reco1->get_pt();
      }
      else return reco2->get_pt();
      break;
    case 1:
      if (reco1)
      {
        return reco1->get_pt();
      }
      else return reco2->get_pt();
      break;
    default:
      return -1;
      break;
  }
}

std::pair<float,float> Conversion::getTrackpTs(){
  switch(recoCount()){
    case 2:
      return std::pair<float,float>(reco1->get_pt(),reco2->get_pt());
    case 1:
      if (reco1) return std::pair<float,float>(reco1->get_pt(),-1);
      else return std::pair<float,float>(-1,reco2->get_pt());
      break;
    default:
      return std::pair<float,float>(-1,-1);
      break;
  }
}

std::pair<float,float> Conversion::getTrackEtas(){
  switch(recoCount()){
    case 2:
      return std::pair<float,float>(reco1->get_eta(),reco2->get_eta());
    case 1:
      if (reco1) return std::pair<float,float>(reco1->get_eta(),-1);
      else return std::pair<float,float>(-1,reco2->get_eta());
      break;
    default:
      return std::pair<float,float>(-1,-1);
      break;
  }
}

std::pair<float,float> Conversion::getTrackPhis(){
  switch(recoCount()){
    case 2:
      return std::pair<float,float>(reco1->get_phi(),reco2->get_phi());
    case 1:
      if (reco1) return std::pair<float,float>(reco1->get_phi(),-1);
      else return std::pair<float,float>(-1,reco2->get_phi());
      break;
    default:
      return std::pair<float,float>(-1,-1);
      break;
  }
}

