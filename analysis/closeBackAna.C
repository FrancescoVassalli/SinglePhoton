#include <iostream>
using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TLegend.h"
#include "math.h"
#include "TH1.h"
#include "TH2.h"
#include "TEfficiency.h"
#include "TLine.h"
#include "TGraphAsymmErrors.h"

TChain* handleFile(string name, string extension, string treename, unsigned int filecount){
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

void plotGoodBack(TChain* ttree,TFile* out_file){
  float _bb_track_deta ;
  float _bb_cluster_deta ;
  float _bb_cluster_dphi ;
  float _bb_vtx_radius ;
  float _bb_track_dca ;
  int _bb_track_layer ;
  int _bb_track_dlayer ;
  float _bb_track_pT;
  double _bb_approach ;
  float _bb_vtxTrackRZ_dist;
  float _bb_vtxTrackRPhi_dist;
  float _bb_vtx_chi2;
  float _bb_photon_m;
  float _bb_photon_pT;
  float _bb_cluster_prob;
  float _bb_track_dphi;
  int _bb_pid;

  unsigned goodCount=0;

  ttree->Branch("track_deta", &_bb_track_deta);
  ttree->Branch("cluster_deta", &_bb_cluster_deta);
  ttree->Branch("cluster_dphi", &_bb_cluster_dphi);
  ttree->Branch("track_dca", &_bb_track_dca);
  ttree->Branch("track_dphi", &_bb_track_dphi);
  ttree->Branch("track_dlayer", &_bb_track_dlayer);
  ttree->Branch("track_layer", &_bb_track_layer);
  ttree->Branch("track_pT", &_bb_track_pT);
  ttree->Branch("vtx_radius", &_bb_vtx_radius);
  ttree->Branch("vtx_chi2", &_bb_vtx_chi2);
  ttree->Branch("approach_dist", &_bb_approach);
  //ttree->Branch("vtxTrackRZ_dist", &_bb_vtxTrackRZ_dist);
  //ttree->Branch("vtxTrackRPhi_dist", &_bb_vtxTrackRPhi_dist);
  ttree->Branch("photon_m", &_bb_photon_m);
  ttree->Branch("photon_pT", &_bb_photon_pT);
  ttree->Branch("cluster_prob", &_bb_cluster_prob);
  //ttree->Branch("pid", &_bb_pid);

  std::vector<TH1F*> plots;
  plots.push_back(new TH1F("photon_m","",40,0,30));
  plots.push_back(new TH1F("photon_pT","",40,0,30));
  plots.push_back(new TH1F("vtx_radius","",40,0,30));
  plots.push_back(new TH1F("vtx_chi2","",40,0,30));
  //plots.push_back(new TH1F("vtxTrackRPhi_dist","",40,0,3));
  //plots.push_back(new TH1F("vtxTrackRZ_dist","",40,0,3));

  for (std::vector<TH1F*>::iterator i = plots.begin(); i != plots.end(); ++i)
  {
    (*i)->Sumw2();
  }

  unsigned count=0;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if (_bb_track_layer>0&&_bb_track_pT>.6&&_bb_track_dca>0&&-bb_cluster_prob>0&&_bb_track_deta<.3884&&TMath::Abs(_bb_track_dlayer)<=2)
    {
      plots[0]->Fill(_bb_photon_m);
      plots[1]->Fill(_bb_photon_pT);
      plots[2]->Fill(_bb_vtx_radius);
      plots[3]->Fill(_bb_vtx_chi2);
      plots[4]->Fill(_bb_vtxTrackRPhi_dist);
      plots[5]->Fill(_bb_vtxTrackRZ_dist);
      count++;
    }
  }
  for (int i = 0; i < plots.size(); ++i)
  {
    plots[i]->Scale(1./count,"width");
  }
  cout<<"Total good back: "<<count<<'\n';
  out_file->Write();
}

void closeBackAna()
{
  string treePath = "/sphenix/user/vassalli/gammasample/conversionembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TFile *out_file = new TFile("backplots.root","RECREATE");
  TChain *ttree = handleFile(treePath,treeExtension,"cutTreeBacke",nFiles);
  //TChain *ttree2 = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  plotGoodBack(ttree,out_file);
}
