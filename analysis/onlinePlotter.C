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
	string name ="onlineTrackFile4.root";
	TFile *ef = new TFile((name).c_str(),"READ");

	bool plotAll=false;
	if (plotAll)
	{
		//track plots 
	const int kNUM1DPLOTS= 9;
	string plotNames1d[kNUM1DPLOTS] = {"TR","R#frac{dtrack}{dR}","T#frac{dtrack}{dpT} all",
	"TRnr","TRr","T#frac{dtrack}{dpT} matched","T#frac{dtrack}{dpT} silicone","Tr_c","recoTrackTruthR"};
	string plotXNames1d[kNUM1DPLOTS] = {"truth R","reco R","truth #gamma pT","truth R",
	"truth R","truth #gamma pT","truth #gamma pT","truth R","truth R"};

	TH1F* h1plots[kNUM1DPLOTS];
	for (int i = 0; i < kNUM1DPLOTS; ++i)
	{
		h1plots[i] = (TH1F*) ef->Get(plotNames1d[i].c_str());
		string title = plotNames1d[i]+";"+plotXNames1d[i]+";";
		h1plots[i]->SetTitle(title.c_str());
		plot(h1plots[i]);
		cout<<i<<endl;
	} 

	const int kNUM2DPLOTS= 2;
	TH2F* h2plots[kNUM2DPLOTS];
	string plotNames2d[kNUM2DPLOTS] = {"RRvTRnS","RRvTRS"};
	string plotNamesX2d[kNUM2DPLOTS] = {"truth radius","truth radius"};
	string plotNamesY2d[kNUM2DPLOTS] = {"reco radius","reco radius"};
	for (int j = 0; j < kNUM2DPLOTS; ++j)
	{
		h2plots[j] = (TH2F*) ef->Get(plotNames2d[j].c_str());
		string title = plotNames2d[j]+";"+plotNamesX2d[j]+";"+plotNamesY2d[j];
		h2plots[j]->SetTitle(title.c_str());
		plot(h2plots[j]);
	}
	}
	else{
		TH1F* accepttruth =(TH1F*) ef->Get("TR");
		TH1F* alltruth =(TH1F*) ef->Get("allTruthR");
		TH1F* recoTrack =(TH1F*) ef->Get("recoTrackTruthR");
		TH1F* recoVert =(TH1F*) ef->Get("TRr");
		TCanvas *tcR = new TCanvas();
		TLegend *tlR = new TLegend(.4,.4,.6,.6);
		alltruth->Draw();
		accepttruth->SetLineColor(kGreen);
		recoTrack->SetLineColor(kRed);
		recoVert->SetLineColor(kOrange);
		accepttruth->Draw("same");
		recoTrack->Draw("same");
		recoVert->Draw("same");
		tlR->AddEntry(alltruth,"All Truth","l");
		tlR->AddEntry(accepttruth,"Truth in Acceptance","l");
		tlR->AddEntry(recoTrack,"Reco Tracks","l");
		tlR->AddEntry(recoVert,"reco Vertex","l");
		tlR->Draw();

		TCanvas *tcpT = new TCanvas();
		TH1F* accepttruthpT =(TH1F*) ef->Get("TpT acceptance");
		TH1F* alltruthpT =(TH1F*) ef->Get("allTruthpT");
		TH1F* recoTrackpT =(TH1F*) ef->Get("recoTrackTruthpT");
		TH1F* recoVertpT =(TH1F*) ef->Get("TpT reco vert");
		alltruthpT->Draw();
		accepttruthpT->SetLineColor(kGreen);
		recoTrackpT->SetLineColor(kRed);
		recoVertpT->SetLineColor(kOrange);
		accepttruthpT->Draw("same");
		recoTrackpT->Draw("same");
		recoVertpT->Draw("same");
		TLegend *tlpT = new TLegend(.4,.4,.6,.6);
		tlpT->AddEntry(alltruthpT,"All Truth","l");
		tlpT->AddEntry(accepttruthpT,"Truth in Acceptance","l");
		tlpT->AddEntry(recoTrackpT,"Reco Tracks","l");
		tlpT->AddEntry(recoVertpT,"reco Vertex","l");
		tlpT->Draw();
	}
}
