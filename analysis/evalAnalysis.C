#include "Track.h"
#include <Photon.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <iostream>
#include <cmath>
#ifndef __CINT__
#include <map>
#include <vector>
#endif //CINT

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
//  typedef std::map<int,Photon> mapEventPhoton;
}

std::map<int, Photon> matchTracks(TNtuple* tracks,TNtuple* verticies){
	const int kVertexTupleLength=verticies->GetEntries();
	
	float vx,vy,vz,vevent,ntracks;
	verticies->SetBranchAddress("vx",&vx);
	verticies->SetBranchAddress("vy",&vy);
	verticies->SetBranchAddress("vz",&vz);
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

  TFile *file = new TFile("trackFile.root","RECREATE");
	TH1F *pTR = new TH1F("pTR","",60,0,2);
	TH1F *matchAngle =new TH1F("match1","",200,0,.07);
	TH1F *matchAngleTruth = new TH1F("truthmatchangle","",200,0,.035);
	TH1F *truthVRadius = new TH1F("conRad","",200,0,25);
	TH1F *recoVRadius = new TH1F("recoconRad","",200,0,25);
	TH2F *rvz = new TH2F("conZdepend","",200,0,25,200,0,20);
	TH2F *anglespace = new TH2F("anglespace","",20,0,.005,20,0,.1);
	TH2F *anglespaceTruth = new TH2F("anglespaceTruth","",20,0,.005,20,0,.1);
	TH2F *plotXY = new TH2F("pXY","",100,-20,20,100,-20,20);
	TH2F *plotXYTruth = new TH2F("pXYT","",100,-20,20,100,-20,20);
	TH2F *anglepT = new TH2F("daangle","",40,0,10,200,0,.2);
	TH2F *responseR = new TH2F("resR","",200,0,25,60,0,1.8);
	TH2F *responseZ = new TH2F("resZ","",100,0,10,200,0.6,1.4);
	
	std::map<int, Photon> map; //return value
	int slide=0;
	int total=kVertexTupleLength;
	int toomanytrackscounter=0;
	int nancount=0;
  int goodVerticies=0;
  int recoPhotons=0;
	for (int i = 0; i < kVertexTupleLength; ++i)
	{
    if(i%10000==0){
      std::cout<<i<<" verticies processed"<<'\n';
    }
		verticies->GetEvent(i);
		if (ntracks==2)
		{
      goodVerticies++;
			do{
				tracks->GetEvent(slide);
				slide++;
			}while(tevent!=vevent);

			TVector3 p1(rpx,rpy,rpz);
			TLorentzVector lv1(p1,pToE(p1,kEmass));
			float truthpT1=tpt;
			TVector3 truthVertex(tvx,tvy,tvz);
			TVector3 recoVertex(vx,vy,vz);
			TVector3 p1Truth(tpx,tpy,tpz);

			tracks->GetEvent(slide);
			TVector3 p2(rpx,rpy,rpz);
			TLorentzVector lv2(p2,pToE(p2,kEmass));
			float truthpT2=tpt;
			TVector3 p2Truth(tpx,tpy,tpz);
			map[(int)vevent]=Photon(lv2+lv1);

			pTR->Fill((float)p1.Pt()/truthpT1);
			pTR->Fill((float)p2.Pt()/truthpT2);
			matchAngle->Fill((float)p1.Angle(p2));
			matchAngleTruth->Fill((float)p1Truth.Angle(p2Truth));
			truthVRadius->Fill((float)truthVertex.XYvector().Mod());
			recoVRadius->Fill((float)recoVertex.XYvector().Mod());
			rvz->Fill(truthVertex.XYvector().Mod(),tvz);
			anglespace->Fill((float)TMath::Abs(p2.Eta()-p1.Eta()),deltaPhi(p2.Phi(),p1.Phi()));
			anglespaceTruth->Fill((float)TMath::Abs(p2Truth.Eta()-p1Truth.Eta()),deltaPhi(p2Truth.Phi(),p1Truth.Phi()));
			plotXY->Fill(vx,vy);
			plotXYTruth->Fill(tvx,tvy);
			anglepT->Fill((float)(lv1+lv2).Pt(),(float)p1.Angle(p2));
			responseR->Fill((float)truthVertex.XYvector().Mod(),(float)p1.Pt()/truthpT1);
			responseR->Fill((float)truthVertex.XYvector().Mod(),(float)p2.Pt()/truthpT2);
			responseZ->Fill(tvz,(float)p2.Pt()/truthpT2);
			responseZ->Fill(tvz,(float)p1.Pt()/truthpT1);
		}
		else{
			if(ntracks>2){
				toomanytrackscounter++;
			}
			if(vx!=vx){
				nancount++;
			}
		}
  }
  cout<<total<<"=totalverticies\n";
  cout<<(float)goodVerticies/(float)total*100<<"\% Reconstructed\n";
//	cout<<"Rejected "<<toomanytrackscounter<<" verticies out of "<<total<<" due to too many tracks \n"<<endl;
//	cout<<nancount<<" nan verticies \n";
//	cout<<map.size()/(float)total*100<<"\% of conversions reconstructed \n";
	file->Write();
  file->Close();
  delete file;
  return map;
}

void matchPhotons(TTree *truth,std::map<int, Photon> reco){
	int N;
	float pT[100];
	float eta[100];
	float phi[100];
	truth->SetBranchAddress("particle_n",&N);
	truth->SetBranchAddress("particle_pt",pT);
	truth->SetBranchAddress("particle_eta",eta);
	truth->SetBranchAddress("particle_phi",phi);

  TFile *file = new TFile("trackFile.root","UPDATE");
	TH1F *p_dR = new TH1F("gamMatchdR","",60,0,2);
	TH1F *ptr = new TH1F("pTgfrt","",60,.4,1.6);
  int loopcount=0;
	for(std::map<int, Photon>::iterator it =reco.begin(); it!=reco.end();it++){
		truth->GetEvent(it->first);
		int spot =0; //location of closet truth 
		float tdR = it->second.deltaR(eta[0],phi[0]);
		for (int i = 1; i < N; ++i)//loop over truth particles 
		{
			float ndR=it->second.deltaR(eta[i],phi[i]);
			if(ndR<tdR){
				spot=i;
				tdR=ndR;
			}
		}
		p_dR->Fill(tdR);
		ptr->Fill(it->second.getpT().value/pT[spot]);
	  loopcount++;
    if(loopcount%10000==0){
      std::cout<<loopcount<<"Photons matched"<<'\n';
    }
  }
//	plot(p_dR,"reco-truth #DeltaR #gamma");
//	plot(ptr,"pT #gamma #frac{reco}{truth}");
 // ptr->Write();
 // p_dR->Write();
		file->Write();
  file->Close();
  delete file;
}

/*void makeRatios(std::vector<Pair<Photon>> pairs){
	TH1F *pTratio = new TH1F("ratio","",20,0,2);
	for (std::vector<Pair<Photon>>::iterator i = pairs.begin(); i != pairs.end(); ++i)
	{
		pTratio->Fill((*i).y.getpT().value/(*i).x.getpT().value);
	}
	plot(pTratio,"#frac{reco pT #gamma}{truth pT #gamma}");
}*/

void evalAnalysis(){
  string location ="/sphenix/user/vassalli/singlesamples/Photon5/";
	string name ="track1.root";
	TFile *ef = new TFile((location+name).c_str(),"READ");
	TNtuple *track, *vertex;
	track = (TNtuple*) ef->Get("ntp_track");
	vertex = (TNtuple*) ef->Get("ntp_vertex");
	name="truth1.root";
	TFile *tf = new TFile((location+name).c_str(),"READ");
	TTree *truthInfo;
	truthInfo = (TTree*) tf->Get("ttree");
	//matchTracks(track,vertex);
	matchPhotons(truthInfo,matchTracks(track,vertex));
}
