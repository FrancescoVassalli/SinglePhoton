#include <TVector3.h>
#include <TLorentzVector.h>
#include "Track.h"
#include <iostream>
using namespace std;

namespace {
	const float kEmass = 0.000511;
	int plotcount=0;
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

std::vector<std::vector<Photon>> makePhotons(TTree* file){
	const int kEntries = file->GetEntries();
	int N;
	float pT[100];
	float eta[100];
	float phi[100];
	file->SetBranchAddress("particle_n",&N);
	file->SetBranchAddress("particle_pt",pT);
	file->SetBranchAddress("particle_eta",eta);
	file->SetBranchAddress("particle_phi",phi);
	std::vector<std::vector<Photon>> r;
	cout<<"Making Truth Photons"<<endl;
	for (int i = 0; i < kEntries; ++i)
	{
		file->GetEvent(i);
		std::vector<Photon> eventPhotons;
		for (int j = 0; j < N; ++j)
		{
			eventPhotons.push_back(Photon(i,pT[j],phi[j],eta[j]));
		}
		r.push_back(eventPhotons);
	}
	return r;
}

std::map<int, Photon> map matchTracks(TNtuple* tracks,TNtuple* verticies){
	const int kVertexTupleLength=verticies->GetEntries();
	
	float vx,vy,vz,vevent,ntracks;
	verticies->SetBranchAddress("vx",&vvx);
	verticies->SetBranchAddress("vy",&vvy);
	verticies->SetBranchAddress("vz",&vvz);
	verticies->SetBranchAddress("event",&vevent);
	verticies->SetBranchAddress("ntracks",&ntracks);
	
	float tevent,tvx,tvy,tvz,rpx,rpy,rpz,tpt,tpx,tpy,tpz;
	tracks->SetBranchAddress("gvx",&tvx);
	tracks->SetBranchAddress("gvy",&tvy);
	tracks->SetBranchAddress("gvz",&tvz);
	tracks->SetBranchAddress("gpt",&tpt);
	tracks->SetBranchAddress("event",&tevent);
	tracks->SetBranchAddress("px",&rpx);
	tracks->SetBranchAddress("py",&rpy);
	tracks->SetBranchAddress("pz",&rpz);
	tracks->SetBranchAddress("gpx",&tpx);
	tracks->SetBranchAddress("gpy",&tpy);
	tracks->SetBranchAddress("gpz",&tpz);

	TH1F *ptR = new TH1F("pTR","",60,0,2);
	TH1F *matchAngle =new TH1F("match1","",200,0,.1);
	TH1F *truthVRadius = new TH1F("conRad","",200,0,25);
	TH2F *rvz = new TH2F("conZdepend","",200,0,25,200,0,20);
	TH2F *anglespace = new TH2F("anglespace","",20,0,.003,20,0,.1);
	TH2F *anglespaceTruth = new TH2F("anglespaceTruth","",20,0,.003,20,0,.1);
	//TH2F *responseR = new TH2F("resR","",20,2,25,60,0,2);
	std::map<int, Photon> map;
	int slide=0;
	for (int i = 0; i < kVertexTupleLength; ++i)
	{
		verticies->GetEvent(i);
		if (vvx!=NAN&&ntracks==2)
		{
			do{
				tracks->GetEvent(slide);
				slide++;
			}while(tevent!=vevent);

			TVector3 p1(rpx,rpy,rpz);
			TLorentzVector lv1(p1,pToE(p1,kEmass));
			float truthpT1=tpt;
			TVector3 truthVertex(tvx,tvy,tvz);
			TVector3 p1Truth(tpx,tpy,tpz);

			tracks->GetEvent(slide);
			TVector3 p2(rpx,rpy,rpz);
			TLorentzVector lv2(p2,pToE(p2,kEmass));
			float truthpT2=tpt;
			TVector3 p2Truth(tpx,tpy,tpz);
			map[i]=Photon(lv2+lv1);

			pTR->Fill(p1.Pt()/truthpT1);
			pTR->Fill(p2.Pt()/truthpT2);
			matchAngle->Fill((float)p1.Angle(p2));
			truthVRadius->Fill(truthVertex.XYvector().Norm());
			rvz->Fill(truthVertex.XYvector().Norm(),tvz);
			anglespace->Fill((float)TMath::Abs(p2.Eta()-p1.Eta()),deltaPhi(p2.Phi(),p1.Phi()));
			anglespaceTruth->Fill((float)TMath::Abs(p2Truth.Eta()-p1Truth.Eta()),deltaPhi(p2Truth.Phi(),p1Truth.Phi()))
		}
	}
	plot(pTR,"Track pT #frac{reco}{truth}");
	plot(matchAngle,"matching track angle reco");
	plot(truthVRadius,"truth conversion radius [cm]");
	plot(rvz,"truth conversion radius [cm]","z [cm]");
	plot(anglespace,"reco #Delta#eta","#Delta#phi");
	plot(anglespaceTruth,"truth #Delta#eta","#Delta#phi");
	return map;
}

void makeRatios(std::vector<Pair<Photon>> pairs){
	TH1F *pTratio = new TH1F("ratio","",20,0,2);
	for (std::vector<Pair<Photon>>::iterator i = pairs.begin(); i != pairs.end(); ++i)
	{
		pTratio->Fill((*i).y.getpT().value/(*i).x.getpT().value);
	}
	plot(pTratio,"#frac{reco pT #gamma}{truth pT #gamma}");
}

void evalAnalysis(){
	string location ="/home/user/Droptemp/SinglePhoton/";
	string name ="eval.root";
	TFile *ef = new TFile((location+name).c_str(),"READ");
	TNtuple *track, *vertex;
	track = (TNtuple*) ef->Get("ntp_track");
	vertex = (TNtuple*) ef->Get("ntp_vertex");
	name="truth.root";
	TFile *tf = new TFile((location+name).c_str(),"READ");
	TTree *truthInfo;
	truthInfo = (TTree*) tf->Get("ttree");
	matchTracks(track,vertex);

}