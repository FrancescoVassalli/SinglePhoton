#include <TVector3.h>
#include <TLorentzVector.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <cmath>
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

void makeHists(TTree* truth, TTree* recovery, const string& outname){
  TFile *outfile = new TFile(outname.c_str(),"RECREATE");

  TLorentzVector *recotlv, *truthtlv;
  TVector3 *recoVert,*truthVert;

  recovery->SetBranchAddress("reco_tlv",&recotlv);
  recovery->SetBranchAddress("truth_tlv",&truthtlv);
  recovery->SetBranchAddress("reco_vertex",&recoVert);
  recovery->SetBranchAddress("truth_vertex",&truthVert);

  TH1F *pTR = new TH1F("pTR","",60,0,2);
  TH1F *matchAngle =new TH1F("matchAngle","",200,0,.1);
  TH2F *anglespace = new TH2F("ranglespace","",20,0,.005,20,0,.1);
  TH2F *responseR = new TH2F("tresR","",200,0,25,60,0,2);
  TH2F *responseZ = new TH2F("tresZ","",100,-20,20,200,0.6,1.8);
  TH1F *truthVEta= new TH1F("trutheta","",200,-1,1);
  TH1F *recoVEta= new TH1F("recoeta","",200,-1,1);
  TH1F *truthVRadius = new TH1F("truthRadius","",200,0,25);
  TH1F *recoVRadius = new TH1F("recoRadius","",200,0,40);
  TH2F *truthVrz = new TH2F("truthconZdepend","",200,-20,20,200,0,20);
  TH2F *recoVrz = new TH2F("recoconZdepend","",200,-20,20,200,0,20);
  TH2F *truthplotXY = new TH2F("tpXY","",100,-20,20,100,-20,20);
  TH2F *recoplotXY = new TH2F("rpXY","",100,-20,20,100,-20,20);

  TH1F* VR1 = new TH1F("VR1","",100,0,10);
  TH1F* VR2 = new TH1F("VR2","",100,0,10);
  TH1F* VR3 = new TH1F("VR3","",100,0,10);

  TH1F* nullVertR = new TH1F("nullVertR","",100,0,60);

  TH1F* tRHighres = new TH1F("tRHighres","",100,0,60);

  for (int i = 0; i < recovery->GetEntries(); ++i)
  {
    recovery->GetEntry(i);
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
    responseR->Fill(truthVert->XYvector().Mod(),recotlv->Pt()/truthtlv->Pt());
    responseZ->Fill(truthVert->Z(),recotlv->Pt()/truthtlv->Pt());

    if (truthVert->XYvector().Mod()<5)
    {
      VR1->Fill(TMath::Abs(truthVert->XYvector().Mod()-recoVert->XYvector().Mod()));
    }
    else if (truthVert->XYvector().Mod()<15)
    {
      VR2->Fill(TMath::Abs(truthVert->XYvector().Mod()-recoVert->XYvector().Mod()));
    }
    else
    {
      VR3->Fill(TMath::Abs(truthVert->XYvector().Mod()-recoVert->XYvector().Mod()));
    }
    if (recotlv->Pt()/truthtlv->Pt()>1.2)
    {
      tRHighres->Fill(truthVert->XYvector().Mod());
    }
    if (recoVert->XYvector().Mod()==0)
    {
      nullVertR->Fill(truthVert->XYvector().Mod());
    }
  }

  int truthN;
  int ids[100];
  truth->SetBranchAddress("particle_id",&ids[0]);
  truth->SetBranchAddress("particle_n",&truthN);

  int truthConversionCount=0;


  for (int i = 0; i < truth->GetEntries(); ++i)
  {
    truth->GetEntry(i);
    cout<<"Truth N:"<<truthN<<endl;
    if(truthN>8){
      for (int j = 0; j < truthN; ++j)
      {
        if (ids[j]!=22)
        { 
          truthConversionCount++;
        }
      }
    }
  }
  cout<<"Truth electron:"<<truthConversionCount<<endl;
  //TH1F *efficency = new TH1F("efficency","",1000,0,1);
  //efficency->Fill(recovery->GetEntries()/truthConversionCount);

  outfile->Write();
  outfile->Close();
  delete outfile;

}

void onlineHister(){
  const string location ="/sphenix/user/vassalli/singlesamples/Photon5/";
  string outname = "onlineTrackFile.root";
	string intruth ="test/truth.root";
	string inreco ="test/reco.root";
	TFile *f_truth = new TFile((location+intruth).c_str(),"READ");
	TFile *f_reco = new TFile((location+inreco).c_str(),"READ");
	TTree *truthInfo, *recoveryTree;
	truthInfo = (TTree*) f_truth->Get("ttree");
	recoveryTree = (TTree*) f_reco->Get("convertedphotontree");
	makeHists(truthInfo,recoveryTree,location+outname);
}
