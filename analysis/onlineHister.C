#include <TVector3.h>
#include <TMap.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TFile.h>
#include <TChain.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <cmath>
#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include "RecoData.h"

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

TMap* makeRecoMap(TTree* recoveryTree){
	TMap *recoMap= new TMap(recoveryTree->GetEntries(),0); 
	bool charge,silicone;
	string *hash= new string();
	TLorentzVector *recotlv1= new TLorentzVector();
	TLorentzVector *truthtlv1= new TLorentzVector();
	TLorentzVector  *recotlv2= new TLorentzVector();
	TLorentzVector  *truthtlv2= new TLorentzVector();
	TVector3 *recoVert= new TVector3();
	TVector3 *truthVert= new TVector3();
	recoveryTree->SetBranchAddress("charge",&charge);
	recoveryTree->SetBranchAddress("silicone",&silicone);
	recoveryTree->SetBranchAddress("hash", &hash);
	recoveryTree->SetBranchAddress("reco_tlv1",    &recotlv1 );
	recoveryTree->SetBranchAddress("truth_tlv1",   &truthtlv1 );
	recoveryTree->SetBranchAddress("reco_tlv2",    &recotlv2 );
	recoveryTree->SetBranchAddress("truth_tlv2",   &truthtlv2 );
	recoveryTree->SetBranchAddress("reco_vertex", &recoVert );
	recoveryTree->SetBranchAddress("truth_vertex",&truthVert);
	cout<<"Starting reco map with "<<recoveryTree->GetEntries()<<" entries"<<endl;
	for (int i = 0; i < recoveryTree->GetEntries()-5; ++i)
	{
		recoveryTree->GetEntry(i);
		TNamed *key=new TNamed(hash->c_str(),hash->c_str());
		RecoData *value=new RecoData(charge,silicone,*hash,*recotlv1,*recotlv2,*truthtlv1,*truthtlv2,*recoVert,*truthVert);
		recoMap->Add(key,value);
	}
	return recoMap;
}

void makeHists2(TTree* truthTree, TTree* recoveryTree, const string& outname){
	TFile *outfile = new TFile(outname.c_str(),"RECREATE");

	TMap* recoMap = makeRecoMap(recoveryTree);

	int t_nVtx,t_nconvert,t_npair,r_npair,event;
	string *hash = new string();
	const int kMAXPHOTNS=8;
	float t_rVtx[kMAXPHOTNS], t_photon_pt[kMAXPHOTNS],t_photon_eta[kMAXPHOTNS],t_photon_phi[kMAXPHOTNS],
				t_electron_pt[kMAXPHOTNS],t_positron_pt[kMAXPHOTNS];

	truthTree->SetBranchAddress("event",&event);
	truthTree->SetBranchAddress("hash",&hash);
	truthTree->SetBranchAddress("nVtx",&t_nVtx);
	truthTree->SetBranchAddress("nconvert",&t_nconvert);
	truthTree->SetBranchAddress("nTpair",&t_npair);
	truthTree->SetBranchAddress("nRpair",&r_npair);
	truthTree->SetBranchAddress("rVtx",&t_rVtx);
	truthTree->SetBranchAddress("electron_pt",&t_electron_pt);
	truthTree->SetBranchAddress("positron_pt",&t_positron_pt);
	truthTree->SetBranchAddress("photon_pt",&t_photon_pt);
	truthTree->SetBranchAddress("photon_eta",&t_photon_eta);
	truthTree->SetBranchAddress("photon_phi",&t_photon_phi);

	int t_totalconversions=0;
	int t_conversionsInRange=0;
	int t_recoMatchedTracks=0;
	int t_events=0;
	int tE_totalconversions=0;
  int e_silicon=0;
	int rE_recoMatchedEvents=0;
	int rE_chargePairs=0;
	int e_events=0;

	TH1F* h_r_dtrackMatcheddR = new TH1F("R#frac{dtrack}{dR}","",20,0,30);
	TH1F* h_t_dtrackMatcheddpT = new TH1F("T#frac{dtrack}{dpT}","",100,0,30);
  TH1F* h_t_R = new TH1F("TR","",20,0,30);
  TH1F* h_tr_R = new TH1F("TRr","",20,0,30);
	TH1F* h_tnr_R = new TH1F("TRnr","",20,0,30);

	TH2F* h_cns_R = new TH2F("RRvTRnS","",20,0,30,20,0,30);
	TH2F* h_cs_R = new TH2F("RRvTRS","",20,0,30,20,0,30);
  cout<<"Starting truth loop"<<endl;
  for (int i = 0; i < truthTree->GetEntries(); ++i)
  {
    truthTree->GetEntry(i);
    t_totalconversions+=t_nVtx;
    t_conversionsInRange+=t_npair;
    t_recoMatchedTracks+=r_npair;
    t_events++;
    if (t_npair==1&&t_nVtx==1)
    {
      e_events++;
      tE_totalconversions+=t_npair;
      h_t_R->Fill(t_rVtx[0]);
      if(recoMap->GetValue(hash->c_str())){
        RecoData* recodata= static_cast<RecoData*>(recoMap->GetValue(hash->c_str()));
        rE_recoMatchedEvents++;
        h_tr_R->Fill(t_rVtx[0]);
        if(recodata->getGoodCharge()){
          rE_chargePairs++;    
        }
        if (recodata->hasSilicone())
        {
          h_cs_R->Fill(t_rVtx[0],recodata->getRecoR());
          e_silicon++;
        }
        else{
          cout<<"no silicone"<<endl;
          h_cns_R->Fill(t_rVtx[0],recodata->getRecoR());
        }
        h_r_dtrackMatcheddR->Fill(t_rVtx[0]);
      }
      else{
        h_tnr_R->Fill(t_rVtx[0]);
      }
		}
	}
  h_r_dtrackMatcheddR->Scale(1/tE_totalconversions);
  h_t_R->Scale(1/tE_totalconversions);
  h_tr_R->Scale(1/tE_totalconversions);
  h_tnr_R->Scale(1/tE_totalconversions);
  h_cns_R->Scale(1/(tE_totalconversions-e_silicon));
  h_cs_R->Scale(1/(e_silicon));
	cout<<Form("For %i events of 8 photons there are %i total conversions.\n %i in the acceptance rapidity.\n %i truth matched reco tracks.\n",t_events,t_totalconversions,t_conversionsInRange,t_recoMatchedTracks);
	cout<<Form("For %i events of 8 photons with max 1 truth conversion and 2 tracks in the acceptance eta (reco and truth) there are %i total conversions.\n  %i reco matched conversions.\n %i reco charge paired events.",e_events,tE_totalconversions,rE_recoMatchedEvents,rE_chargePairs);
	outfile->Write();
	outfile->Close();
}

TChain* handleFile(string name, string extension, string treename, int filecount){
	TChain *all = new TChain(treename.c_str());
	string temp;
	for (int i = 0; i < filecount; ++i)
	{

		ostringstream s;
		s<<i;
		temp = name+string(s.str())+extension;
		all->Add(temp.c_str());
	}
	return all;
}

void onlineHister(){
	const string location ="/sphenix/user/vassalli/singlesamples/Photon5/";
	string outname = "onlineTrackFile.root";
	string in ="onlineanalysis";
	string reco =".rootrecovered.root";
	string truth =".root";
	int numFiles=100;
	TChain* truthchain=handleFile(location+in,truth,"ttree",numFiles);
	TChain* recochain=handleFile(location+in,reco,  "convertedphotontree",numFiles);
	/*TFile *f_truth = new TFile((location+intruth).c_str(),"READ");
		TFile *f_reco = new TFile((location+inreco).c_str(),"READ");
		TTree *truthInfo, *recoveryTree;
		truthInfo = (TTree*) f_truth->Get("ttree");
		recoveryTree = (TTree*) f_reco->Get("convertedphotontree");*/
	makeHists2(truthchain,recochain,outname);
	//ostringsteam s;
	//s<<numFiles;
	//string num(s)
}
