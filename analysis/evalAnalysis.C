#include <TVector3.h>
#include "Track.h"
#include <iostream>
using namespace std;

void plot(TH1F* plot,string x){
  TCanvas* tc = new TCanvas();
  gPad->SetLogy();
  //plot->Scale(1/plot->Integral());
  fixOffset(plot);
  axisTitles(plot,x.c_str(),"");
  plot->Draw();
}

void plot(TH2F* plot,string x, string y){
  TCanvas* tc = new TCanvas();
  tc->SetRightMargin(.15);
  tc->SetLeftMargin(.1);
  gPad->SetLogz();
  plot->Scale(1/plot->Integral());
  axisTitles(plot,x.c_str(),y.c_str());
  fixOffset(plot);
  plot->Draw("colz");
}

void makeTracks(TNtuple *file){
	const int kEntries = file->GetEntries();
	float event;
	float rID;
	float tID;
	float thits;
	float tmap;
	float tintt;
	float ttpc;
	float rhits;
	float rmap;
	float rintt;
	float rtpc;
	float flavor;
	float charge;
	float rpx;
	float rpy;
	float rpz;
	float reta;
	float rphi;
	float rpt;
	float tpx;
	float tpy;
	float tpz;
	float teta;
	float tphi;
	float tpt;
	float quality;
	float vx;
	float vy;
	float vz;
	float vt;
	file->SetBranchAddress("event",&event);
	file->SetBranchAddress("trackID",&rID);
	file->SetBranchAddress("gtrackID",&tID);
	file->SetBranchAddress("nhits",&thits);
	file->SetBranchAddress("nmaps",&tmap);
	file->SetBranchAddress("nintt",&tintt);
	file->SetBranchAddress("ntpc",&ttpc);
	file->SetBranchAddress("gnhits",&rhits);
	file->SetBranchAddress("gnmaps",&rmap);
	file->SetBranchAddress("gnintt",&rintt);
	file->SetBranchAddress("gntpc",&rtpc);
	file->SetBranchAddress("gflavor",&flavor);
	file->SetBranchAddress("charge",&charge);
	file->SetBranchAddress("px",&rpx);
	file->SetBranchAddress("py",&rpy);
	file->SetBranchAddress("pz",&rpz);
	file->SetBranchAddress("eta",&reta);
	file->SetBranchAddress("phi",&rphi);
	file->SetBranchAddress("pt",&rpt);
	file->SetBranchAddress("gpx",&tpx);
	file->SetBranchAddress("gpy",&tpy);
	file->SetBranchAddress("gpz",&tpz);
	file->SetBranchAddress("geta",&teta);
	file->SetBranchAddress("gphi",&tphi);
	file->SetBranchAddress("gpt",&tpt);
	file->SetBranchAddress("quality",&quality);
	file->SetBranchAddress("gvx",&vx);
	file->SetBranchAddress("gvy",&vy);
	file->SetBranchAddress("gvz",&vz);
	file->SetBranchAddress("gvt",&vt);

	Track tracks[kEntries];


	TH1F *ptR = new TH1F("pTR","",20,0,2);
	TH2F *matchAngle =new TH2F("match1","",10,-1.5,1.5,10,-1*TMath::Pi(),TMath::Pi());

	for (int i = 0; i < kEntries; ++i)
	{
		file->GetEvent(i);
		TVector3 p1(rpx,rpy,rpz);
		ptR->Fill(rpt/tpt);
		if(TMath::Abs((int)charge)!=1) cout<<"Not charge 1: "<<i<<endl;
		float thisEta = reta;
		float thisPhi = rphi;
		float thisDR =-1;
		int thischarge = (int) charge;
		//cout<<"First track: "<<thisEta<<","<<thisPhi<<endl;
		int spot=-1;
		for (int j = 0; j < kEntries; ++j)
		{
			if(j==i)continue;
			file->GetEvent(j);
			float nextDR = deltaR(thisEta,thisPhi,reta,rphi);
			if ((int)charge==thischarge*-1 &&(nextDR<thisDR ||thisDR<0))
			{
				thisDR=nextDR;
				thisEta=reta;
				thisPhi=rphi;
				spot=j;
			}
		}
		file->GetEvent(spot);
		TVector3 pMatch(rpx,rpy,rpz);
		pMatch-=p1;
		matchAngle->Fill(pMatch.Eta(),pMatch.Phi());
		//cout<<"Match index: "<<spot<<" with dR="<<thisDR<<" at: "<<thisEta<<","<<thisPhi<<endl;
	}
	plot(ptR,"Track pT reco/ pT truth");
	plot(matchAngle,"Matching Track #Delta#eta","#Delta#phi");
}

void makeRatios(TNtuple file){

}

void evalAnalysis(){
	string location ="/home/user/Droptemp/SinglePhoton/";
	string name ="eval.root";
	TFile *tf = new TFile((location+name).c_str(),"READ");
	TNtuple *truthTrack, *g4Track;
	truthTrack = (TNtuple*) tf->Get("ntp_track");
	makeTracks(truthTrack);
	//cout<<truthTrack->GetEntries()<<endl;
	/* out of 10k events I got 940 tracks or equivently 470 converted photons*/

}