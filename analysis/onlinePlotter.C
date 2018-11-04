#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TMath.h>
#include <TCanvas.h>
#include <iostream>
#include <cmath>
#include "NicePlots.C"
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
void plot(TH1F* plot){
	TCanvas* tc = new TCanvas(getNextPlotName(&plotcount).c_str());
	fixOffset(plot);
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

void plot(TH1F** plots, const int kSIZE, string* names=NULL){
	if (names)
	{
		for (int i = 0; i < kSIZE; ++i)
		{
			plot(plots[i],names[i]);
		}
	}
	else{
		for (int i = 0; i < kSIZE; ++i)
		{
			plot(plots[i]);
		}	
	}


}
void plot(TH2F** plots, const int kSIZE, string* namesX, string* namesY){
	for (int i = 0; i < kSIZE; ++i)
	{
		plot(plots[i],namesX[i],namesY[i]);
	}
}

void onlinePlotter(){
	gStyle->SetOptStat(0);
	string name ="onlineTrackFile.root";
	TFile *ef = new TFile((name).c_str(),"READ");
	//track plots 
	const int kNUM1DPLOTS= 4;
	string plotNames1d[kNUM1DPLOTS] = {"TdtrackdR","RdtrackdR","RdgRdR",
		"TdtrackdpT"};
	string plotXNames1d[kNUM1DPLOTS] = {"truth R","truth R","truth R","truth #Sigma track pT"};


	TH1F* h1plots[kNUM1DPLOTS];
	for (int i = 0; i < kNUM1DPLOTS; ++i)
	{
		h1plots[i] = (TH1F*) ef->Get(plotNames1d[i].c_str());
		string title = plotNames1d[i]+";"+plotXNames1d[i]+";";
		h1plots[i]->SetTitle(title.c_str());
		h1plots[i]->Scale(1/h1plots[i]->Integral());
	} 

	plot(&h1plots[0],kNUM1DPLOTS);

	/*
		 const int kNUM2DPLOTS= 0;
		 TH2F* h2plots[kNUM2DPLOTS];
		 string plotNames2d[kNUM2DPLOTS] = {"ranglespace","tresR","tresZ",
		 "truthconZdepend","recoconZdepend","tpXY","rpXY"};
		 string plotNamesX2d[kNUM2DPLOTS] = {"truth-reco #Delta#eta","truth R",
		 "truth z","truth z","reco z","truth x","reco x"};
		 string plotNamesY2d[kNUM2DPLOTS] = {"truth-reco #Delta#phi","pT track response",
		 "pT track response","truth R", "reco R","truth y","reco y"};
		 for (int j = 0; j < kNUM2DPLOTS; ++j)
		 {
		 h2plots[j] = (TH2F*) ef->Get(plotNames2d[j].c_str());
		 }
		 plot(&h2plots[0],kNUM2DPLOTS,plotNamesX2d,plotNamesY2d);*/
}
