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

void backgroundPlotter()
{
  int pid;
  float cluster_prob;

  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TChain *ttree = handleFile(treePath,treeExtension,"cutTreeBackh",nFiles);
  ttree->SetBranchAddress("pid",    &pid    );
  ttree->SetBranchAddress("cluster_prob",    &cluster_prob );

  string outfilename = "backgroundProb.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  string plotname = "backgroundProb";

  TH1F *h_pip_prob = new TH1F("clus2plot","",50,0.,1.); 
  TH1F *h_pim_prob = new TH1F("cluscount","",50,0.,1.);
  TH1F *h_p_prob = new TH1F("clusSprob","",50,0.,1.);
  TH1F *h_mu_prob = new TH1F("clusMprob","",50,0.,1.);

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    switch(pid){
    	case 211:
    		h_pip_prob->Fill(cluster_prob);
    		break;
    	case -211:
    		h_pim_prob->Fill(cluster_prob);
    		break;
    	case 2212:
    		h_p_prob->Fill(cluster_prob);
    		break;
    	case -2212:
    		h_p_prob->Fill(cluster_prob);
    		break;
    	case 13:
    		h_mu_prob->Fill(cluster_prob);
    		break;
    	case -13:
    		h_mu_prob->Fill(cluster_prob);
    		break;
    	default:
    		break;
    }
  } 
   
  out->Write();
  out->Close();
  delete ttree;
  delete out;
}
