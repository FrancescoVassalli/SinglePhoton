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

void makephotonM(TChain* tree,TFile* out_file){
  float photon_m;
  float tphoton_m
  ttree->SetBranchAddress("photon_m",     &photon_m   );
  ttree->SetBranchAddress("tphoton_m",&tphoton_m    );

  std::vector<TH1F*> plots;
  plots.push_back(new TH1F("m_{#gamma}_reco","",40,-10,10));
  plots.push_back(new TH1F("m_{#gamma}_truth","",40,-10,10));

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(photon_m);
    plots[1]->Fill(tphoton_m);
  }
  for (int i = 0; i < 2; ++i)
  {
    plots[i]->Scale(1./ttree->GetEntries(),"width");
  }
  out_file->Write();
}

void makeVtxR(TChain* tree,TFile* out_file){
  float vtxr;
  float tvtxr;
  ttree->SetBranchAddress("vtx_radius",&vtxr);
  tree->SetBranchAddress("tvtx_radius",&tvtxr);

  std::vector<TH1F*> plots;
  plots.push_back(new TH1F("vtx_reco","",40,0,30));
  plots.push_back(new TH1F("vtx_truth","",40,0,30));

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(vtxr);
    plots[1]->Fill(tvtxr);
  }
  for (int i = 0; i < 2; ++i)
  {
    plots[i]->Scale(1./ttree->GetEntries(),"width");
  }
  out_file->Write();

}

void photonEff()
{
  string treePath = "/sphenix/user/vassalli/gammasample/conversionembedonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TFile *out_file = new TFile("effplots.root","RECREATE");
  TChain *ttree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *ttree2 = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  makephotonM(ttree,out_file);
  makeVtxR(ttree2,out_file);



  TH1F *h_TepT = new TH1F("TepT","",100,0,30);
  TH1F *h_RepT = new TH1F("RepT","",100,0,30);

  TH1F *h_rvtx = new TH1F("rvtx","",100,0,30);
  TH1F *h_layer = new TH1F("layer","",24,-.5,23.5);

  h_TepT->GetXaxis()->SetTitle("pT");
  h_TepT->GetYaxis()->SetTitle("N");
  h_RepT->GetXaxis()->SetTitle("pT");
  h_RepT->GetYaxis()->SetTitle("N");

  h_rvtx->GetXaxis()->SetTitle("radius [cm]");
  h_rvtx->GetYaxis()->SetTitle("N");
  
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    for (int i = 0; i < truth_n; ++i)
    {
      h_RepT->Fill(electron_reco_pt[i]);
      h_RepT->Fill(positron_reco_pt[i]);
    }
    for (int i = 0; i < nVtx; ++i)
    {
      h_TepT->Fill(electron_pt[i]);
      h_TepT->Fill(positron_pt[i]);
      h_rvtx->Fill(rVtx[i]);
    }
    for (int i = 0; i < reco_n; ++i)
    {
      h_layer->Fill(b_layer[i]);
    }
  }  
  out->Write();
  out->Close();
  delete ttree;
  delete out;
}
