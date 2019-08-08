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

int make(TChain* ttree,TFile* out_file){
  cout<<ttree->GetEntries()<<endl;
  return ttree->GetEntries();
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

void reportBackground(TChain* _treeBackground,int signal){
  unsigned totalTracks;
  unsigned passedpTEtaQ;
  unsigned passedCluster;
  unsigned passedPair;
  unsigned passedVtx;

  int sum_totalTracks=-1*signal;
  int sum_passedpTEtaQ=-1*signal;
  int sum_passedCluster=-1*signal;
  int sum_passedPair=-1*signal;
  int sum_passedVtx=-1*signal;

  _treeBackground->SetBranchAddress("total",   &totalTracks);
  _treeBackground->SetBranchAddress("tracksPEQ",  &passedpTEtaQ);
  _treeBackground->SetBranchAddress("tracks_clus", &passedCluster);
  _treeBackground->SetBranchAddress("pairs", &passedPair);
  _treeBackground->SetBranchAddress("vtx", 	  &passedVtx);

  for(unsigned i=0; i<_treeBackground->GetEntries();i++){
    _treeBackground->GetEntry(i);
    sum_totalTracks+=totalTracks;
    sum_passedpTEtaQ+=passedpTEtaQ;
    sum_passedCluster+=passedCluster;
    sum_passedPair+=passedPair;
    sum_passedVtx+=passedVtx;
  }
  cout<<"Did RecoConversionEval with "<<sum_totalTracks<<" total tracks\n\t";
  cout<<1-(float)sum_passedpTEtaQ/sum_totalTracks<<"+/-"<<sqrt((float)sum_passedpTEtaQ)/sum_totalTracks<<" of tracks were rejected by pTEtaQ\n\t";
  cout<<1-(float)sum_passedCluster/sum_passedpTEtaQ<<"+/-"<<sqrt((float)sum_passedCluster)/sum_passedpTEtaQ<<" of remaining tracks were rejected by cluster\n\t";
  cout<<1-(float)sum_passedPair/sum_passedCluster<<"+/-"<<sqrt((float)sum_passedPair)/sum_passedCluster<<" of pairs were rejected by pair cuts\n\t";
  cout<<1-(float)sum_passedVtx/sum_passedPair<<"+/-"<<sqrt((float)sum_passedVtx)/sum_passedPair<<" of vtx were rejected by vtx cuts\n\t";
}

void minBiasRecoAna()
{
  string treePath = "/sphenix/user/vassalli/minBiasPythia/conversionembededminBiasanalysis";
  string truthTreePath = "/sphenix/user/vassalli/minBiasPythia/conversionembededminBiasTruthanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=600;
  TFile *out_file = new TFile("minBiasplots.root","RECREATE");
  TChain *ttree = handleFile(treePath,treeExtension,"recoSignal",nFiles);
  TChain *back_tree = new TChain("recoBackground");
  string filename = "/sphenix/user/vassalli/minBiasPythia/allRecoMinBias.root";
  back_tree->Add(filename.c_str());
  TChain *truth_ttree = handleFile(truthTreePath,treeExtension,"cutTreeSignal",nFiles);
  //make(ttree,out_file);
  reportBackground(back_tree,make(truth_ttree,out_file));
}
