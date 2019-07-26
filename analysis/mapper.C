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

void makeMaps(TChain* ttree,TFile* out_file){
  float vtxX;
  float tvtxX;
  float vtxY;
  float tvtxY;
  ttree->SetBranchAddress("vtx_x",&vtxX);
  ttree->SetBranchAddress("tvtx_x",&tvtxX);
  ttree->SetBranchAddress("vtx_y",&vtxY);
  ttree->SetBranchAddress("tvtx_y",&tvtxY);

  TH2F *map = new TH2F("recoMap","",100,-30,30,100,-30,30);
  TH2F *tmap = new TH2F("truthMap","",100,-30,30,100,-30,30);
  map->Sumw2();
  tmap->Sumw2();

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    map->Fill(vtxX,vtxY);
    map->Fill(tvtxX,tvtxY);
  }
  out_file->Write();
}

void mapper()
{
  string treePath = "/sphenix/user/vassalli/gammasample/conversionembededonlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=100;
  TFile *out_file = new TFile("maps.root","RECREATE");
  TChain *ttree = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  makeMaps(ttree,out_file);
}
