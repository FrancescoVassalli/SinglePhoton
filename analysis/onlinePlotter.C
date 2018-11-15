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
void plot(TH1* plot){
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
	const int kNUM1DPLOTS= 3;
	string plotNames1d[kNUM1DPLOTS] = {"TR","R#frac{dtrack}{dR}","T#frac{dtrack}{dpT}"};
	string plotXNames1d[kNUM1DPLOTS] = {"truth R","reco R","truth #gamma pT"};

	TH1F* h1plots[kNUM1DPLOTS];
	for (int i = 0; i < kNUM1DPLOTS; ++i)
	{
		h1plots[i] = (TH1F*) ef->Get(plotNames1d[i].c_str());
		string title = plotNames1d[i]+";"+plotXNames1d[i]+";";
		h1plots[i]->SetTitle(title.c_str());
		plot(h1plots[i]);
	} 

	const int kNUM2DPLOTS= 2;
	TH2F* h2plots[kNUM2DPLOTS];
	string plotNames2d[kNUM2DPLOTS] = {"RRvTRnS","RRvTRS"};
	string plotNamesX2d[kNUM2DPLOTS] = {"truth radius"};
	string plotNamesY2d[kNUM2DPLOTS] = {"reco radius"};
	for (int j = 0; j < kNUM2DPLOTS; ++j)
	{
		h2plots[j] = (TH2F*) ef->Get(plotNames2d[j].c_str());
		string title = plotNames2d[j]+";"+plotNamesX2d[j]+";"+plotNamesY2d[j];
		h2plots[j]->SetTitle(title.c_str());
		plot(h2plots[j]);
	}
}
