#include <TVector3.h>
#include <TLorentzVector.h>
#include <TFile.h>
#include <TChain.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <cmath>
#include <sstream>
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

  TLorentzVector *recotlv1, *truthtlv1, *recotlv2, *truthtlv2,*recotlv,*truthtlv;
  TVector3 *recoVert,*truthVert;

  recovery->SetBranchAddress("reco_tlv1",    &recotlv1 );
  recovery->SetBranchAddress("truth_tlv1",   &truthtlv1 );
  recovery->SetBranchAddress("reco_tlv2",    &recotlv2 );
  recovery->SetBranchAddress("truth_tlv2",   &truthtlv2 );
  recovery->SetBranchAddress("reco_vertex", &recoVert );
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
  TH1F* VR3 = new TH1F("VR3","",30,0,40);

  TH1F* nullVertR = new TH1F("nullVertR","",100,0,60);
  TH1F* tRHighres = new TH1F("tRHighres","",100,0,60);
  cout<<recovery->GetEntries()<<endl;
  recotlv= new TLorentzVector(*recotlv1+*recotlv2);
  truthtlv= new TLorentzVector(*truthtlv1+*truthtlv2);
  for (int i = 0; i < recovery->GetEntries(); ++i)
  {
    recovery->GetEntry(i);
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
    if(recoVert->X()==0&&recoVert->Y()==0){
      cout<<"Truth R:"<<truthVert->XYvector().Mod()<<", Reco z:"<<recoVert->Z()<<endl;
    }
    responseR->Fill(truthVert->XYvector().Mod(),recotlv1->Pt()/truthtlv1->Pt());
    responseZ->Fill(truthVert->Z(),recotlv1->Pt()/truthtlv1->Pt());
    responseR->Fill(truthVert->XYvector().Mod(),recotlv2->Pt()/truthtlv2->Pt());
    responseZ->Fill(truthVert->Z(),recotlv2->Pt()/truthtlv2->Pt());

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
    delete recotlv;
    delete truthtlv;
  }

  int truthN;
  int ids[100];
  int nVtx;
  truth->SetBranchAddress("particle_id",&ids[0]);
  truth->SetBranchAddress("particle_n",&truthN);
  truth->SetBranchAddress("nVtx",&nVtx);
  int truthConversionCount=0;

  cout<<truth->GetEntries()<<endl;
  for (int i = 0; i < truth->GetEntries(); ++i)
  {
    truth->GetEntry(i);
    truthConversionCount+=nVtx;
  }
  TH1F *efficency = new TH1F("efficency","",1000,0,1);
  efficency->Fill(recovery->GetEntries()/(float)truthConversionCount);

  outfile->Write();
  outfile->Close();
  delete outfile;

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
  const string location ="/sphenix/user/vassalli/singlesamples/Photon5/";
  string outname = "onlineTrackFile.root";
	string in ="onlineanalysis";
	string reco =".rootrecovered.root";
  string truth =".root";
  int numFiles=100;
  TChain* truthchain=handleFile(location+in,truth,"ttree",numFiles);
  TChain* recochain=handleFile(location+in,reco,  "convertedphotontree",numFiles);
	/*TFile *f_truth = new TFile((location+intruth).c_str(),"READ");
	TFile *f_reco = new TFile((location+inreco).c_str(),"READ");
	TTree *truthInfo, *recoveryTree;
	truthInfo = (TTree*) f_truth->Get("ttree");
	recoveryTree = (TTree*) f_reco->Get("convertedphotontree");*/
	makeHists(truthchain,recochain,outname);
  //ostringsteam s;
  //s<<numFiles;
  //string num(s)
}
