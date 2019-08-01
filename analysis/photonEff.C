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
  float rephoton_m;
  float tphoton_m;
  std::vector<TH1F*> plots;
  ttree->SetBranchAddress("photon_m",     &photon_m   );
  ttree->SetBranchAddress("rephoton_m",     &rephoton_m   );
  plots.push_back(new TH1F("m^{#gamma}_{reco}","",40,-2,10));
  plots.push_back(new TH1F("m^{#gamma}_{recoRefit}","",40,-2,10));
  
  plots[1]->SetLineColor(kRed);
  for (int i = 0; i < 2; ++i)
  {
    plots[i]->Sumw2();
  }

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(photon_m);
    plots[1]->Fill(rephoton_m);
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
  
  plots[0]->Sumw2();
  plots[1]->Sumw2();

  double calc=0;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(vtxr);
    plots[1]->Fill(tvtxr);
    calc+=TMath::Abs(vtxr-tvtxr);
  }
  calc/=ttree->GetEntries();
  for (int i = 0; i < 2; ++i)
  {
    plots[i]->Scale(1./ttree->GetEntries(),"width");
  }
  out_file->Write();
  std::cout<<"mean deviation="<<calc<<std::endl;
}

void makeRefitDist(TChain* ttree, TFile *out_file){
  float diffx;
  float diffy;
  float diffz;
  ttree->SetBranchAddress("refitdiffx",&diffx);
  ttree->SetBranchAddress("refitdiffy",&diffz);
  ttree->SetBranchAddress("refitdiffz",&diffz);
  TH1F *diffplotx= new TH1F("x_{0}-x_{refit}","",40,-10,10);
  TH1F *diffploty= new TH1F("y_{0}-y_{refit}","",40,-10,10);
  TH1F *diffplotz= new TH1F("z_{0}-z_{refit}","",40,-10,10);

  diffplotx->Sumw2();
  diffploty->Sumw2();
  diffplotz->Sumw2();

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    diffplotx->Fill(diffx);
    diffploty->Fill(diffy);
    diffplotz->Fill(diffz);
  }
  
  diffplotx->Scale(1./ttree->GetEntries(),"width");
  diffploty->Scale(1./ttree->GetEntries(),"width");
  diffplotz->Scale(1./ttree->GetEntries(),"width");

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
  //makeVtxR(ttree2,out_file);
  makeRefitDist(ttree,out_file);
}
