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


/*void makeRatios(std::vector<Pair<Photon>> pairs){
	TH1F *pTratio = new TH1F("ratio","",20,0,2);
	for (std::vector<Pair<Photon>>::iterator i = pairs.begin(); i != pairs.end(); ++i)
	{
		pTratio->Fill((*i).y.getpT().value/(*i).x.getpT().value);
	}
	plot(pTratio,"#frac{reco pT #gamma}{truth pT #gamma}");
}*/

void nicePlotter(){
	string name ="trackFile.root";
	TFile *ef = new TFile((name).c_str(),"READ");
	//track plots 
	//reco/truth comparison
	TH1F* pTR =(TH1F*) ef->Get("pTR");T
	H2F* responsedepR=(TH2F*) ef->Get("resR"); //response x axis measures truth
	TH2F* responsedepX=(TH2F*) ef->Get("resZ");
	//track reco and truth data
	TH1F* recomatchAngle =(TH1F*) ef->Get("match1");
	TH1F* truthRadius=(TH1F*) ef->Get("conRad");
	TH2F* truthRz=(TH2F*) ef->Get("conRadZdepend");
	TH2F* precoanglespace=(TH2F*) ef->Get("anglespace");
	TH2F* ptruthanglespace=(TH2F*) ef->Get("anglespaceTruth");
	TH2F* recoRadio=(TH2F*) ef->Get("pXY");
	TH2F* recomatchAngleDeppT=(TH2F*) ef->Get("daangle");
	//photon plots 
	TH1F* photMatchAng=(TH1F*) ef->Get("rtDrg");
	TH1F* photResponse=(TH1F*) ef->Get("pTgfrt");

}
