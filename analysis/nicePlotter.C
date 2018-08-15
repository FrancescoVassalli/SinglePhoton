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

void plot(TH1F** plots, const int kSIZE, string* names){
	for (int i = 0; i < kSIZE; ++i)
	{
		plot(plots[i],names[i]);
	}
}
void plot(TH2F** plots, const int kSIZE, string* namesX, string* namesY){
	for (int i = 0; i < kSIZE; ++i)
	{
		plot(plots[i],namesX[i],namesY[i]);
	}
}

void nicePlotter(){
	string name ="trackFile.root";
	TFile *ef = new TFile((name).c_str(),"READ");
	//track plots 
	const int kNUM1DPLOTS= 5;
	const int kNUM2DPLOTS= 8;
	string 1dplotNames[kNUM1DPLOTS] = {"pTR","match1","truthmatchangle","truthRadius",
		"conRad","recoconRad","gamMatchdR","pTgfrt"};
	string 1dplotXNames[kNUM1DPLOTS] = {"track pT #frac{reco}{truth}","opening angle reco",
		"opening angle truth","truth conversion #eta","reco conversion #eta",
		"truth conversion r","reco conversion r","#Delta R","pT #gamma #frac{reco}{truth}"};
	string 2dplotNames[kNUM2DPLOTS] = {"conZdepend","anglespace","anglespaceTruth",
		"pXY","pXYT","daangle","resR","resZ"};
	string 2dplotXNames[kNUM2DPLOTS] = {"truth conversion r","reco #Delta#eta",
	"truth #Delta#eta","truth conversion r","reco conversion x","truth conversion x"
	"reco #gamma pT","truth conversion x","truth conversion z"};
	string 2dplotYNames[kNUM2DPLOTS] = {"truth conversion z","reco #Delta#phi",
		"truth #Delta#phi","reco conversion y", "truth conversion y","reco opening angle"
		"track pT #frac{reco}{truth}","track pT #frac{reco}{truth}"}
	TH1F* h1plots[kNUM1DPLOTS];
	TH2F* h2plots[kNUM2DPLOTS];
	for (int i = 0; i < kNUM1DPLOTS; ++i)
	{
		h1plots[i] = (TH1F*) ef->Get(1dplotNames[i]);
	} 
	for (int j = 0; j < kNUM2DPLOTS; ++j)
	{
		h2plots[j] = (TH2F*) ef->Get(2dplotNames[j]);
	}
	plot(&h1plots[0],kNUM1DPLOTS,1dplotXNames);
	plot(&h2plots[0],kNUM2DPLOTS,2dplotXNames,2dplotYNames);

}
