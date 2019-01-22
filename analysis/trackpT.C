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

void trackpT()
{
  float electron_pt[200];
  float positron_pt[200];
  float electron_reco_pt[200];
  float positron_reco_pt[200];
  int    truth_n;

  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TChain *ttree = handleFile(treePath,treeExtension,"ttree",nFiles);
  ttree->SetBranchAddress("electron_pt",     &electron_pt    );
  ttree->SetBranchAddress("electron_reco_pt",&electron_reco_pt    );
  ttree->SetBranchAddress("positron_pt",     &positron_pt    );
  ttree->SetBranchAddress("positron_reco_pt",&positron_reco_pt    );
  ttree->SetBranchAddress("nTpair",          &truth_n  );
  

  string outfilename = "pTeffdists.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  TH1F *h_TpT_NR = new TH1F("TpT_NR","",60,0,30);
  
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    for (int i = 0; i < truth_n; ++i)
    {
      if (electron_reco_pt[truth_n]>0) h_TpT_NR->Fill(electron_pt[truth_n]);
      if (positron_reco_pt[truth_n]>0) h_TpT_NR->Fill(positron_pt[truth_n]);

    }
  }  
  out->Write();
  out->Close();
  delete ttree;
  delete out;
}
