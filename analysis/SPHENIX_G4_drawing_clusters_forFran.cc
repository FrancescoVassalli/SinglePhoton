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

bool doesItExist(const std::string& name) //checks to see if a file exists in the current directory
{
  if (FILE *file = fopen(name.c_str(), "r")) 
  {
    fclose(file);
    return true;
  } 
  else 
  {
    return false;
  }   
}

TChain* handleFile(string name, string extension, string treename, int filecount){
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


// FRANCESCO:
// Hope this works for you, what I did was in my afterburner for the G4 event  
// I saved all the towers in a given cluster into the arrays above such that 
// each event contained arrays of tower information for a single cluster.
// I produced events specifically for this purpose so I'm not sure it'll work 
// for what you want

//////////////////////////////////////function called by root////////////////////////////////////////////////
void SPHENIX_G4_drawing_clusters_forFran()
{
  double clusterTower_eta[500];
  double clusterTower_phi[500];
  double clusterTower_energy[500];
  int    cluster_n;
  int    NTowers[200];

  string treePath = "/sphenix/user/vassalli/gammasample/test/fourembededonlineanalysis";
  string treeExtension = ".rootcTtree.root";
  TChain *ttree = handleFile(treePath,treeExtension,"ttree",1);
  ttree->SetBranchAddress("clusterTower_eta",    &clusterTower_eta    );
  ttree->SetBranchAddress("clusterTower_phi",    &clusterTower_phi    );
  ttree->SetBranchAddress("clusterTower_energy", &clusterTower_energy );
  ttree->SetBranchAddress("NTowers",             &NTowers             );

  string outfilename = "onlineClusterFile.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  TH2F *photon_cluster = new TH2F("photon_cluster","Plot of Photon Clusters",80,-1,1,251,-1*TMath::Pi(),TMath::Pi());
  photon_cluster->SetStats(kFALSE);
  photon_cluster->GetXaxis()->SetTitle("eta");
  photon_cluster->GetYaxis()->SetTitle("phi");


  int clusterNum = 1; // number of clusters you wish to draw on a single canvas
  ttree->GetEvent(0);
  for(int i = 0; i < clusterNum; i++)
  {
    for(int event = 0; event < NTowers[i]; event++)
    {
      photon_cluster->Fill(clusterTower_eta[event],clusterTower_phi[event],clusterTower_energy[event]);
    }
  }
  out->Write();
  out->Close();
  delete ttree;
  delete out;

}
