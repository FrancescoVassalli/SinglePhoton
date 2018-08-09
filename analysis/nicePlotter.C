#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TMath.h>
#include <TCanvas.h>
#include <iostream>
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

void plot(TH1F* plot,string x){
  TCanvas* tc = new TCanvas(getNextPlotName(&plotcount).c_str(),x.c_str());
  //plot->Scale(1/plot->Integral());
  fixOffset(plot);
  axisTitles(plot,x.c_str(),"");
  plot->Draw();
}
void plotLog(TH1F* plot,string x){
  TCanvas* tc = new TCanvas(getNextPlotName(&plotcount).c_str(),x.c_str());
  gPad->SetLogy();
  //plot->Scale(1/plot->Integral());
  fixOffset(plot);
  axisTitles(plot,x.c_str(),"");
  plot->Draw();
}

void plot(TH2F* plot,string x, string y){
  TCanvas* tc = new TCanvas(getNextPlotName(&plotcount).c_str(),x.c_str());
  tc->SetRightMargin(.15);
  tc->SetLeftMargin(.1);
  gPad->SetLogz();
  plot->Scale(1/plot->Integral());
  axisTitles(plot,x.c_str(),y.c_str());
  fixOffset(plot);
  plot->Draw("colz");
}

void nicePlotter(){
	string name ="trackFile.root";
	TFile *ef = new TFile((name).c_str(),"READ");
	//track plots 
	

	TH1F *pTR = (TH1F*) ef->Get("pTR");
	TH1F *matchAngle =(TH1F*) ef->Get("matchAngle");
	TH2F *anglespace = (TH1F*) ef->Get("ranglespace");
	TH2F *responseR = (TH1F*) ef->Get("tresR"); 
	TH2F *responseZ = (TH1F*) ef->Get("tresZ");
	TH1F *truthVEta= (TH1F*) ef->Get("trutheta");
	TH1F *recoVEta= (TH1F*) ef->Get("recoeta"); 
	TH1F *truthVRadius = (TH1F*) ef->Get("truthRadius");
	TH1F *recoVRadius = (TH1F*) ef->Get("recoRadius"); 
	TH2F *truthVrz = (TH1F*) ef->Get("truthconZdepend");
	TH2F *recoVrz = (TH1F*) ef->Get("recoconZdepend"); 
	TH2F *truthplotXY = (TH1F*) ef->Get("tpXY"); 
	TH2F *recoplotXY = (TH1F*) ef->Get("rpXY"); 

}
