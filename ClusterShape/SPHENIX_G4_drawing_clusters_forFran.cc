

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

void addAllTheTrees(TChain *tree, string treename, int numTrees)
/* ( exampleTreeChain, "Tree_", 50) for 50 trees called Tree_1.root, Tree_2.root, etc.)*/
{
  string tempname;
  string temp;

  for(int i = 0; i < numTrees; i++) //read all files into TChain
  {
    temp = to_string(i); //convert integer to string
    tempname = treename + temp + ".root"; //add strings together to form filename
    //tree->Add(tempname.c_str());
    if(doesItExist(tempname) == true)
    {
     tree->Add(tempname.c_str()); //convert string to const char string and read in file
    }
  }
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

  TH2F *photon_cluster = new TH2F("photon_cluster","Plot of Photon Clusters",80,-1,1,251,-1*TMath::Pi(),TMath::Pi());
  photon_cluster->SetStats(kFALSE);
  photon_cluster->GetXaxis()->SetTitle("eta");
  photon_cluster->GetYaxis()->SetTitle("phi");

  double clusterTower_eta[500];
  double clusterTower_phi[500];
  double clusterTower_energy[500];
  int    cluster_n;
  int    NTowers[20];

  TChain *ttree = new TChain("ttree");
  addAllTheTrees(ttree, "./single_particles_TMVA/photon_output_", 100);

  ttree->SetBranchAddress("clusterTower_eta",    &clusterTower_eta    );
  ttree->SetBranchAddress("clusterTower_phi",    &clusterTower_phi    );
  ttree->SetBranchAddress("clusterTower_energy", &clusterTower_energy );
  ttree->SetBranchAddress("NTowers",             &NTowers             );

  int clusterNum = 1; // number of clusters you wish to draw on a single canvas

  ttree->GetEvent(0);
  for(int i = 0; i < clusterNum; i++)
  {
    for(int event = 0; event < NTowers[i]; event++)
    {
      photon_cluster->Fill(clusterTower_eta[event],clusterTower_phi[event],clusterTower_energy[event]);
    }
  }

  TCanvas *canvas1 = new TCanvas("canvas1","2D Histogram Canvas",800,600);
  canvas1->cd(1);
  //gPad->SetGrid();
  //photon_cluster->GetXaxis()->SetNdivisions(20);
  //photon_cluster->GetYaxis()->SetNdivisions(62);
  photon_cluster->Draw("colz");

}








