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

void clusterPlotter(){
	gStyle->SetOptStat(0);
	string name ="onlineClusterFile.root";
	TFile *ef = new TFile((name).c_str(),"READ");

	int numPlots =5;
	string plotname = "photon_cluster";
	TH1F* cluster;
	for (int i = 0; i < numPlots; ++i)
	{
		string thisplotname = plotname+to_string(i);
		cluster=(TH1F*) ef->Get(thisplotname.c_str());
		TCanvas *tcR = new TCanvas();
		cluster->Draw("colz");
		cluster->SetTitle("Cluster;#eta;#phi");
	}
	 

}
