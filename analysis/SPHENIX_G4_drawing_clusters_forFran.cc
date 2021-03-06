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
// Hope this works for you, what I did was in my afterburner for the G4 toweri  
// I saved all the towers in a given cluster into the arrays above such that 
// each toweri contained arrays of tower information for a single cluster.
// I produced toweris specifically for this purpose so I'm not sure it'll work 
// for what you want

//////////////////////////////////////function called by root////////////////////////////////////////////////
void SPHENIX_G4_drawing_clusters_forFran()
{
  double clusterTower_eta[500];
  double clusterTower_phi[500];
  double clusterTower_energy[500];
  int    cluster_n;
  int    NTowers[200];

  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".rootcTtree.root";
  unsigned int nFiles=100;
  TChain *ttree = handleFile(treePath,treeExtension,"ttree",nFiles);
  ttree->SetBranchAddress("clusterTower_eta",    &clusterTower_eta    );
  ttree->SetBranchAddress("clusterTower_phi",    &clusterTower_phi    );
  ttree->SetBranchAddress("clusterTower_energy", &clusterTower_energy );
  ttree->SetBranchAddress("NTowers",             &NTowers             );
  ttree->SetBranchAddress("cluster_n",           &cluster_n           );

  string outfilename = "onlineClusterFile.root";
  TFile *out = new TFile(outfilename.c_str(),"RECREATE");

  string plotname = "photon_cluster";

  TH2F *photon_cluster; 


  int clusterNum = 1; // number of clusters you wish to draw on a single canvas
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    cout<<"Drawing "<<clusterNum<<" of "<<cluster_n<<" clusters\n";
    if(cluster_n==0) continue;
    ostringstream oss;
    oss<<event;
    string thisPlotname = plotname +string(oss.str());
    photon_cluster= new TH2F(thisPlotname.c_str(),"Plot of Photon Clusters",80,-1,1,251,-1*TMath::Pi(),TMath::Pi());
    photon_cluster->SetStats(kFALSE);
    photon_cluster->GetXaxis()->SetTitle("eta");
    photon_cluster->GetYaxis()->SetTitle("phi");
    double maxTowerEnergy=-1.;
    double maxTowerEta=-2.;
    double maxTowerPhi=-2.*TMath::Pi();
    for(int i = 0; i < clusterNum; i++)
    {
      for(int toweri = 0; toweri < NTowers[i]; toweri++)
      {
        photon_cluster->Fill(clusterTower_eta[toweri],clusterTower_phi[toweri],clusterTower_energy[toweri]);
        if (clusterTower_energy[toweri]>maxTowerEnergy&&clusterTower_energy[toweri]>0)
        {
          maxTowerEnergy=clusterTower_energy[toweri];
          maxTowerEta=clusterTower_eta[toweri];
          maxTowerPhi=clusterTower_phi[toweri];
        }
      }
      if (maxTowerEnergy!=-1.)
       {
         photon_cluster->GetXaxis()->SetRangeUser(TMath::Max(maxTowerEta-.2,-1.),TMath::Min(maxTowerEta+.2,1.));
         photon_cluster->GetYaxis()->SetRangeUser(TMath::Max(maxTowerPhi-.2,-1*TMath::Pi()),TMath::Min(maxTowerPhi+.2,TMath::Pi()));
       } 
      
    }
  }
  
  out->Write();
  out->Close();
  delete ttree;
  delete out;

}
