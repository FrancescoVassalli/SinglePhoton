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

void makephotonM(TChain* ttree,TFile* out_file){
  float photon_m;
  float tphoton_m;
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

void makeVtxR(TChain* ttree,TFile* out_file){
  float vtxr;
  float tvtxr;
  ttree->SetBranchAddress("vtx_radius",&vtxr);
  ttree->SetBranchAddress("tvtx_radius",&tvtxr);

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
  string treePath = "/sphenix/user/vassalli/gammasample/conversionembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TFile *out_file = new TFile("effplots.root","RECREATE");
  TChain *ttree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *ttree2 = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  makephotonM(ttree,out_file);
  makeVtxR(ttree2,out_file);
}
