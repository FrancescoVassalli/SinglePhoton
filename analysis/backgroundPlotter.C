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

float kPurityCutProb=.8; //for high purity make cluster_prob<.8
float kEffCutProb=.9; //for high efficiency make cluster_prob<.9
int kPurityCutLayer=1; //for high purity make layer==1
int kEffCutLayer =1; //for high efficiency make layer <=1
int kCutLayer=kPurityCutLayer;

void backgroundPlotter()
{
  int pid;
  float cluster_prob;
  float deta;
  int layer;

  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;

  TChain *backTree = handleFile(treePath,treeExtension,"cutTreeBacke",nFiles);
  backTree->SetBranchAddress("pid",    &pid    );
  backTree->SetBranchAddress("cluster_prob",    &cluster_prob );
  backTree->SetBranchAddress("track_deta", &deta);
  backTree->SetBranchAddress("track_layer", &layer);

  TChain *signalTree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  signalTree->SetBranchAddress("track_deta", &deta);
  signalTree->SetBranchAddress("cluster_prob",    &cluster_prob );
  signalTree->SetBranchAddress("track_layer", &layer);

  string outfilename = "backgroundProb.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  TH1F *h_pip_prob = new TH1F("pip","",50,0.,1.); 
  TH1F *h_pim_prob = new TH1F("pim","",50,0.,1.);
  TH1F *h_p_prob = new TH1F("p","",50,0.,1.);
  TH1F *h_mu_prob = new TH1F("mu","",50,0.,1.);
  //TH1F *counts = new TH1F("pid_counts","",4,-.5,4.5);
  TH1F *s_deta = new TH1F("s_deta","Signal #Delta#eta",100,0,.1);
  TH1F *b_deta = new TH1F("b_deta","Background #Delta#eta",100,0,.1);
  TH1F *s_layer = new TH1F("s_layer","Signal Layer",4,-.5,3.5);
  TH1F *b_layer = new TH1F("b_layer","Background Layer",4,-.5,3.5);
  TH1F *s_prob = new TH1F("s_prob","",50,0.,1.);

  unsigned long pip=0;
  unsigned long pim=0;
  unsigned int p=0;
  unsigned int mu=0;
  unsigned rejection=0;
  for (int event = 0; event < backTree->GetEntries(); ++event)
  {
    backTree->GetEvent(event);
    if (true)
    {
      switch(pid){
        case 211:
          h_pip_prob->Fill(cluster_prob);
          //counts->Fill(0);
          pip++;
          break;
        case -211:
          h_pim_prob->Fill(cluster_prob);
          //counts->Fill(1);
          pim++;
          break;
        case 2212:
          h_p_prob->Fill(cluster_prob);
          //counts->Fill(2);
          p++;
          break;
        case -2212:
          h_p_prob->Fill(cluster_prob);
          //counts->Fill(2);
          p++;
          break;
        case 13:
          h_mu_prob->Fill(cluster_prob);
          //counts->Fill(3);
          mu++;
          break;
        case -13:
          h_mu_prob->Fill(cluster_prob);
          //counts->Fill(3);
          mu++;
          break;
        default:
          break;
      }
      b_deta->Fill(deta);
      b_layer->Fill(layer);
      rejection++;
    }
  } 
  //counts->Scale(1./(float)backTree->GetEntries());
  float total=backTree->GetEntries();
  std::cout<<Form("Pi+=%0.3f	Pi-=%0.3f	p/pbar=%0.3f	mu=%0.3f",pip/total,pim/total,p/total,mu/total)<<endl;
  unsigned efficiency=0;
  for (int i = 0; i < signalTree->GetEntries(); ++i)
  {
    signalTree->GetEvent(i);
    if (layer==kCutLayer)
    {
      s_deta->Fill(deta);
      s_layer->Fill(layer);
      s_prob->Fill(cluster_prob);
      efficiency++;
    }
  }
  b_deta->Scale(1/b_deta->Integral());
  b_layer->Scale(1/b_layer->Integral());
  s_deta->Scale(1/s_deta->Integral());
  s_layer->Scale(1/s_layer->Integral());
  s_prob->Scale(1/s_prob->Integral());
  out->Write();
  out->Close();
  delete backTree;
  delete out;
  std::cout<<Form("efficiency:%.04f rejection:%0.3f",efficiency/(float)signalTree->GetEntries(),1-rejection/total)<<endl;
  cout<<Form("n signal=%i, n background=%0.1f",signalTree->GetEntries(),total)<<endl;
}
