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

void cluster2Plotter()
{
  float eTpT[200];
  float pTpT[200];
  float eRpT[200];
  float pRpT[200];
  int    truth_n;

  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TChain *ttree = handleFile(treePath,treeExtension,"ttree",nFiles);
  ttree->SetBranchAddress("electron_pt",     &eTpT    );
  ttree->SetBranchAddress("electron_reco_pt",&eRpT    );
  ttree->SetBranchAddress("positron_pt",     &pTpT    );
  ttree->SetBranchAddress("positron_reco_pt",&pRpT    );
  ttree->SetBranchAddress("nTpair",          &truth_  );
  

  string outfilename = "pTeffdists.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  TH1F *h_TpT_NR = new TH1F("TpT_NR","",2,0.5,2.5);
  
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    for (int i = 0; i < nTpair; ++i)
    {
      if (electron_reco_pt[nTpair]>0) h_TpT_NR->Fill(electron_pt[nTpair]);
      if (positron_reco_pt[nTpair]>0) h_TpT_NR->Fill(positron_pt[nTpair]);

    }
  }  
  out->Write();
  out->Close();
  delete ttree;
  delete out;
}
