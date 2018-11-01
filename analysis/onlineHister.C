#include <TVector3.h>
#include <TMap.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TFile.h>
#include <TChain.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <cmath>
#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include "RecoData.h"


using namespace std;

namespace {
  const float kEmass = 0.000511;
  int plotcount=0;
  template<class T>
    T quadrature(T d1, T d2){
      return sqrt((double)d1*d1+d2*d2);
    }
  inline float pToE(TVector3 v, float mass){
    return quadrature((float) quadrature(v.x(),v.y()),(float) quadrature((float)v.z(),mass));
  }
  inline float deltaPhi(float i1, float i2){
    float r = TMath::Abs(i1-i2);
    if (r>TMath::Pi())
    {
      r= 2*TMath::Pi()-r;
    }
    return r;
  }
}
/*
   void makeHists(TTree* truth, TTree* recovery, const string& outname){
   TFile *outfile = new TFile(outname.c_str(),"RECREATE");

   TLorentzVector *recotlv1, *truthtlv1, *recotlv2, *truthtlv2,*recotlv,*truthtlv;
   TVector3 *recoVert,*truthVert;

   bool failed;
   string *hash;
   int recocount=recovery->GetEntries();

   recovery->SetBranchAddress("reco_tlv1",    &recotlv1 );
   recovery->SetBranchAddress("truth_tlv1",   &truthtlv1 );
   recovery->SetBranchAddress("reco_tlv2",    &recotlv2 );
   recovery->SetBranchAddress("truth_tlv2",   &truthtlv2 );
   recovery->SetBranchAddress("reco_vertex", &recoVert );
   recovery->SetBranchAddress("truth_vertex",&truthVert);
   recovery->SetBranchAddress("status",&failed);
   recovery->SetBranchAddress("hash",&hash);

   TH1F *pTR = new TH1F("pTR","",60,0,2);
   TH1F *matchAngle =new TH1F("matchAngle","",200,0,.1);
   TH2F *anglespace = new TH2F("ranglespace","",20,0,.005,20,0,.1);
   TH2F *responseR = new TH2F("tresR","",200,0,25,60,0,2);
   TH2F *responseZ = new TH2F("tresZ","",100,-20,20,200,0.6,1.8);
   TH1F *truthVEta= new TH1F("trutheta","",200,-1,1);
   TH1F *recoVEta= new TH1F("recoeta","",200,-1,1);
   TH1F *truthVRadius = new TH1F("truthRadius","",200,0,25);
   TH1F *recoVRadius = new TH1F("recoRadius","",200,0,40);
   TH2F *truthVrz = new TH2F("truthconZdepend","",200,-20,20,200,0,40);
   TH2F *recoVrz = new TH2F("recoconZdepend","",200,-20,20,200,0,40);
   TH2F *truthplotXY = new TH2F("tpXY","",100,-20,20,100,-20,20);
   TH2F *recoplotXY = new TH2F("rpXY","",100,-20,20,100,-20,20);

   TH1F* VR1 = new TH1F("VR1","",100,0,10);
   TH1F* VR2 = new TH1F("VR2","",100,0,20);
   TH1F* VR3 = new TH1F("VR3","",30,0,40);

   TH1F* tRHighres = new TH1F("tRHighres","",100,0,60);
   recotlv= new TLorentzVector(*recotlv1+*recotlv2);
   truthtlv= new TLorentzVector(*truthtlv1+*truthtlv2);
   std::map<string, float> recomap;
   for (int i = 0; i < recovery->GetEntries(); ++i)
   {
   recovery->GetEntry(i);
   cout<<hash<<'\n';
   if (!failed)
   {
//recomap[hash]=(float)recoVert->XYvector().Mod();
recotlv= new TLorentzVector(*recotlv1+*recotlv2);
truthtlv= new TLorentzVector(*truthtlv1+*truthtlv2);
pTR->Fill(recotlv->Pt()/truthtlv->Pt());
truthVEta->Fill(truthVert->Eta());
recoVEta->Fill(recoVert->Eta());
matchAngle->Fill(truthtlv->Angle(recotlv->Vect()));
truthVRadius->Fill(truthVert->XYvector().Mod());
recoVRadius->Fill(recoVert->XYvector().Mod());
truthVrz->Fill(truthVert->Z(),truthVert->XYvector().Mod());
recoVrz->Fill(recoVert->Z(),recoVert->XYvector().Mod());
anglespace->Fill(TMath::Abs(truthtlv->Eta()-recotlv->Eta()),deltaPhi(truthtlv->Phi(),recotlv->Phi()));
truthplotXY->Fill(truthVert->X(),truthVert->Y());
recoplotXY->Fill(recoVert->X(),recoVert->Y());
responseR->Fill(truthVert->XYvector().Mod(),recotlv1->Pt()/truthtlv1->Pt());
responseZ->Fill(truthVert->Z(),recotlv1->Pt()/truthtlv1->Pt());
responseR->Fill(truthVert->XYvector().Mod(),recotlv2->Pt()/truthtlv2->Pt());
responseZ->Fill(truthVert->Z(),recotlv2->Pt()/truthtlv2->Pt());

if (truthVert->XYvector().Mod()<5)
{
VR1->Fill(TMath::Abs(recoVert->XYvector().Mod()));
}
else if (truthVert->XYvector().Mod()<15)
{
  VR2->Fill(TMath::Abs(recoVert->XYvector().Mod()));
}
else
{
  VR3->Fill(TMath::Abs(recoVert->XYvector().Mod()));
}
if (recotlv->Pt()/truthtlv->Pt()>1.2)
{
  tRHighres->Fill(truthVert->XYvector().Mod());
}
delete recotlv;
delete truthtlv;
}
else{
  recocount--;
  //cout<<"failed event:"<<event<<endl;
}

}

TH1F *noreco = new TH1F("Rnoreco","",100,0,35);

int truthN;
int nVtx;
int ids[100];
int rVtx[100];
truth->SetBranchAddress("particle_id",&ids[0]);
truth->SetBranchAddress("particle_n",&truthN);
truth->SetBranchAddress("hash",&hash);
truth->SetBranchAddress("nVtx",&nVtx);
truth->SetBranchAddress("rVtx",&rVtx);
int truthConversionCount=0;
cout<<"Totals, truth:"<<truth->GetEntries()<<", reco:"<<recovery->GetEntries()<<endl;
int recomapSize=recomap.size();
for (int i = 0; i < truth->GetEntries(); ++i)
{
  truth->GetEntry(i);
  if (nVtx==1)
  {
    truthConversionCount+=nVtx;
    cout<<"truth hash:"<<hash<<'\n';
    // recomap[event];
    // if (recomap.size()!=recomapSize)
    // {
    //   cout<<"no reco event"<<endl;
    //   noreco->Fill(rVtx[0]);
    //   recomapSize++;
    // }
  }
}
TH1F *efficency = new TH1F("efficency","",1000,0,1);
efficency->Fill(recocount/(float)truthConversionCount);
cout<<"Goods, truth:"<<truthConversionCount<<", reco:"<<recocount<<endl;
outfile->Write();
outfile->Close();
delete outfile;

}
*/

TMap* makeRecoMap(TTree* recoveryTree){
  TMap *recoMap= new TMap(recoveryTree->GetEntries(),0); 
  bool status;
  string *hash= new string();
  TLorentzVector *recotlv1= new TLorentzVector();
  TLorentzVector *truthtlv1= new TLorentzVector();
  TLorentzVector  *recotlv2= new TLorentzVector();
  TLorentzVector  *truthtlv2= new TLorentzVector();
  TVector3 *recoVert= new TVector3();
  TVector3 *truthVert= new TVector3();
  recoveryTree->SetBranchAddress("status",&status);
  recoveryTree->SetBranchAddress("hash", &hash);
  recoveryTree->SetBranchAddress("reco_tlv1",    &recotlv1 );
  recoveryTree->SetBranchAddress("truth_tlv1",   &truthtlv1 );
  recoveryTree->SetBranchAddress("reco_tlv2",    &recotlv2 );
  recoveryTree->SetBranchAddress("truth_tlv2",   &truthtlv2 );
  recoveryTree->SetBranchAddress("reco_vertex", &recoVert );
  recoveryTree->SetBranchAddress("truth_vertex",&truthVert);
  cout<<"Starting reco map with "<<recoveryTree->GetEntries()<<" entries"<<endl;
  for (int i = 0; i < recoveryTree->GetEntries(); ++i)
  {
    recoveryTree->GetEntry(i);
    TNamed key(hash->c_str(),hash->c_str());
    RecoData value(status,*hash,recotlv1,recotlv2,truthtlv1,truthtlv2,recoVert,truthVert);
    recoMap->Add(&key,&value);
    cout<<*hash<<'\n';
  }
  return recoMap;
}

void makeHists2(TTree* truthTree, TTree* recoveryTree, const string& outname){
  TFile *outfile = new TFile(outname.c_str(),"RECREATE");

  TMap* recoMap = makeRecoMap(recoveryTree);

  int t_nparticle,t_nVtx,t_nconvert,t_npair,r_npair,event;
  string *hash = new string();
  float t_rVtx[24], t_pt[24],t_eta[24],t_phi[24],t_id[24];

  truthTree->SetBranchAddress("particle_n",&t_nparticle);
  truthTree->SetBranchAddress("nVtx",&t_nVtx);
  truthTree->SetBranchAddress("nconvert",&t_nconvert);
  truthTree->SetBranchAddress("nTpair",&t_npair);
  truthTree->SetBranchAddress("nRpair",&r_npair);
  truthTree->SetBranchAddress("event",&event);
  truthTree->SetBranchAddress("hash",&hash);
  truthTree->SetBranchAddress("rVtx",&t_rVtx);
  truthTree->SetBranchAddress("particle_pt",&t_pt);
  truthTree->SetBranchAddress("particle_eta",&t_eta);
  truthTree->SetBranchAddress("particle_phi",&t_phi);

  int t_totalconversions=0;
  int t_conversionsInRange=0;
  int t_recoMatchedTracks=0;

  int tE_totalconversions=0;
  int tE_conversionsInRange=0;
  int tE_recoMatchedTracks=0;
  int rE_recoMatchedTracks=0;
  int e_events=0;
  cout<<"Starting truth loop"<<endl;
  for (int i = 0; i < truthTree->GetEntries(); ++i)
  {
    truthTree->GetEntry(i);
    t_totalconversions+=t_nVtx;
    t_conversionsInRange+=t_npair;
    t_recoMatchedTracks+=r_npair;
    if (t_nVtx<2)
    {
      e_events++;
      tE_totalconversions+=t_nVtx;
      tE_conversionsInRange+=t_npair;
      tE_recoMatchedTracks+=r_npair;
      cout<<hash->c_str()<<endl;
      if(recoMap->FindObject(hash->c_str())){
        recoMap->FindObject(hash->c_str())->Dump();
      }
      //TPair* mapEntry = static_cast<TPair*>(recoMap->FindObject(&thisHash));
      //if(mapEntry&&static_cast<RecoData*>(mapEntry->Value())->get_status()){
      //  rE_recoMatchedTracks++;
      //}

    }
  }
  cout<<Form("For %i events of 8 photons there are %i total conversions.\n %i in the acceptance rapidity.\n %i truth matched reco tracks.\n",event,t_totalconversions,t_conversionsInRange,t_recoMatchedTracks);
  cout<<Form("For %i events of 8 photons with max 1 truth conversion there are %i total conversions.\n %i in the acceptance rapidity.\n %i truth matched reco tracks and %i reco matched reco tracks.\n",e_events,tE_totalconversions,tE_conversionsInRange,tE_recoMatchedTracks,rE_recoMatchedTracks);


}

TChain* handleFile(string name, string extension, string treename, int filecount){
  TChain *all = new TChain(treename.c_str());
  string temp;
  for (int i = 0; i < filecount; ++i)
  {

    ostringstream s;
    s<<i;
    temp = name+string(s.str())+extension;
    all->Add(temp.c_str());
  }
  return all;
}

void onlineHister(){
  const string location ="/sphenix/user/vassalli/singlesamples/Photon5/test/";
  string outname = "onlineTrackFile.root";
  string in ="onlineanalysis";
  string reco =".rootrecovered.root";
  string truth =".root";
  int numFiles=1;
  TChain* truthchain=handleFile(location+in,truth,"ttree",numFiles);
  TChain* recochain=handleFile(location+in,reco,  "convertedphotontree",numFiles);
  /*TFile *f_truth = new TFile((location+intruth).c_str(),"READ");
    TFile *f_reco = new TFile((location+inreco).c_str(),"READ");
    TTree *truthInfo, *recoveryTree;
    truthInfo = (TTree*) f_truth->Get("ttree");
    recoveryTree = (TTree*) f_reco->Get("convertedphotontree");*/
  makeHists2(truthchain,recochain,outname);
  //ostringsteam s;
  //s<<numFiles;
  //string num(s)
}
