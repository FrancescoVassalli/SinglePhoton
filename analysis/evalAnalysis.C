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

std::vector<std::vector<Photon>> makeRecoPhotons(TNtuple *file){
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

	std::vector<std::vector<Photon>> recoPhotons; // the return value 
	std::vector<Photon> eventPhotons;
	/*plots */ 
	TH1F *ptR = new TH1F("pTR","",60,0,2);
	//TH1F *matchAngle =new TH1F("match1","",200,0,.1);
	TH1F *truthVRadius = new TH1F("conRad","",200,0,25);
	TH2F *rvz = new TH2F("conZdepend","",200,0,25,200,0,20);
	//TH2F *anglespace = new TH2F("anglespace","",20,0,.003,20,0,.1);
	//TH2F *anglespaceTruth = new TH2F("anglespaceTruth","",20,0,.003,20,0,.1);
	//TH2F *responseR = new TH2F("resR","",20,2,25,60,0,2);  INTT doesn't seem to be fully implemented 
	
	std::cout<<"Starting reco track extraction"<<std::endl;
	
	/*locating the event in the tuple*/
	file->GetEvent(0);
	int thisEvent=event;
	int slide=0;
	do{
		slide++;
		file->GetEvent(slide);
	}while(thisEvent==event&&slide<kEntries);
	Pair<int> range(0,slide);


	for (int i = 0; i < kEntries; ++i) // tuple for loop
	{
		file->GetEvent(i);
		
		TVector3 p1(rpx,rpy,rpz);
		TLorentzVector p2(p1,pToE(p1,kEmass));  //the lepton
		ptR->Fill(rpt/tpt); // ratio of reco track pT to truth track pT 
		
		if(TMath::Abs((int)charge)!=1) cout<<"Not charge 1: "<<i<<endl;
		if(TMath::Abs((int)flavor)!=11) cout<<"Not flavor 11: "<<i<<endl;
		
		/*reconstructed conversion vertex*/ // this iswrong need to use vertex finder 
		//float thisEta = reta; // this is a momentum in polar coordinates 
		//float thisPhi = rphi;
//
		///*used to match the lepton pairs*/
		//int thischarge = (int) charge;
		//float thisDR = -1;  
		////cout<<"First track: "<<thisEta<<","<<thisPhi<<endl;
		//
		//slide=-1;
		//for (int j = range.x; j < range.y; ++j) //finding the pair
		//{
		//	if(j==i)continue; //can't pair with yourself
		//	file->GetEvent(j);
		//	float nextDR = deltaR(thisEta,thisPhi,reta,rphi);
		//	if ((int)charge==thischarge*-1 &&(nextDR<thisDR ||thisDR<0)) //pair must have opposite charge and smallest dR
		//	{
		//		thisDR=nextDR;
		//		thisEta=reta;
		//		thisPhi=rphi;
		//		slide=j;
		//	}
		//}

		//if (thisDR>.1) cout<<"Large dR at: "<<slide<<" ="<<thisDR<<endl;
		//
		///*making the pair*/
		//file->GetEvent(slide);
		//TVector3 pMatch(rpx,rpy,rpz);
		////file->GetEvent(i); //once the pair has been made switch the values back to the original lepton
		//TLorentzVector pMatch2(pMatch,pToE(pMatch,kEmass));
		//cout<<(float)pMatch.Angle(p1)<<'\n';
		//matchAngle->Fill((float)pMatch.Angle(p1));
		//anglespace->Fill((float)TMath::Abs(pMatch.Eta()-p1.Eta()),deltaPhi(pMatch.Phi(),p1.Phi()));
		int truthRadius =quadrature(vx,vy);
		truthVRadius->Fill(truthRadius);
		rvz->Fill(truthRadius,vz);
		eventPhotons.push_back(Photon(p2+pMatch2),event); //preping the return 
		
		/*more event location stuff*/
		if(i==range.y-1){
			thisEvent=event;
			slide=range.y;
			do{
				slide++;
				file->GetEvent(slide);
			}while(thisEvent==event&&slide<kEntries);
			range.x=range.y;
			range.y=slide;
			recoPhotons.push_back(eventPhotons);
			eventPhotons.clear();
		}
	}
	std::cout<<"Done with reco extraction now plotting"<<std::endl;
	plot(ptR,"Track pT reco/ pT truth");
	//plot(matchAngle,"Matching Angle");
	//plot(anglespace,"#Delta#eta","#Delta#phi");
	plotLog(truthVRadius, "conversion radius [cm]");
	plot(rvz,"conversion radius [cm]","z [cm]")
	return recoPhotons;
}

std::vector<Pair<Photon>> makeMatches(std::vector<std::vector<Photon>> truth,std::vector<std::vector<Photon>> reco){
	std::vector<Pair<Photon>> matches;
	cout<<"Matching"<<endl;
	for (unsigned int i = 0; i < reco.size(); ++i) //for every reco event 
	{
		int event = reco[i][0].getPosition(); //get the event number 
		for (int k = 0; k < reco[i].size(); ++k) //for every reco photon in the event 
		{
			int spot=-1;
		float dR= reco[i].deltaR(truth[0]);
		for (unsigned int j = 0; j < truth.size(); ++j) // for every truth photon in the evet 
		{
			float thisdR = reco[i].deltaR(truth[j]);
			if (thisdR<dR)
			{
				dR=thisdR;
				spot=j;
			}
		}
		}
		
		//remove the truth photon after pairing 
		matches.push_back(Pair<Photon>(truth[spot],reco[i]));
	}
	cout<<"Done Matching"<<endl;
	return matches;
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
	TNtuple *track;
	track = (TNtuple*) ef->Get("ntp_track");
	name="truth.root";
	TFile *tf = new TFile((location+name).c_str(),"READ");
	TTree *truthInfo;
	truthInfo = (TTree*) tf->Get("ttree");
	std::vector<std::vector<Photon>> recoPhotons =makeTracks(track);
	//std::vector<Photon> truthPhotons =makePhotons(truthInfo) ;
	//makeRatios(makeMatches(truthPhotons,recoPhotons));
	//cout<<truthTrack->GetEntries()<<endl;
	/* out of 10k events I got 940 tracks or equivently 470 converted photons*/

}