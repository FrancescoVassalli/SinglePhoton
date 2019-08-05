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

void make(TChain* ttree,TFile* out_file){
  cout<<ttree->GetEntries()<<endl;
  /*float pT;
  float tpT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);
  
  TH1F *pTeffPlot = new TH1F("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}","",40,-2,2);
  TH2F *pTefffuncPlot = new TH2F("pT_resolution_to_truthpt","",40,1,35,40,-1.5,1.5);
  pTeffPlot->Sumw2();

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    pTeffPlot->Fill((pT-tpT)/tpT);
    pTefffuncPlot->Fill(tpT,(pT-tpT)/tpT);
  }
  pTeffPlot->Scale(1./ttree->GetEntries(),"width");
  pTefffuncPlot->Scale(1./ttree->GetEntries(),"width");
  out_file->Write();*/
}

void minBiasRecoAna()
{
  string treePath = "/sphenix/user/vassalli/minBiasPythia/conversionembededminBiasanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=200;
  TFile *out_file = new TFile("minBiasplots.root","RECREATE");
  TChain *ttree = handleFile(treePath,treeExtension,"recoSignal",nFiles);
  make(ttree,out_file);
}
