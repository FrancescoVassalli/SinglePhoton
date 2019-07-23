#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TChain.h>


#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif



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


void makeFactory(TTree* signalTree,std::string outfile,std::string factoryname)
{
  using namespace TMVA;
  TString jobname(factoryname.c_str());
  TFile *targetFile = new TFile(outfile.c_str(),"RECREATE");
  Factory *factory = new Factory(jobname,targetFile,"AnalysisType=Regression");
  DataLoader *loader = new DataLoader();
  loader->AddRegressionTree(signalTree,1.0);
  loader->AddVariable("track1_pt",'F');
  loader->AddVariable("track2_pt",'F');
  //loader->AddVariable("track1_phi",'F');
  //loader->AddVariable("track2_phi",'F');
  //loader->AddVariable("track1_eta",'F');
  //loader->AddVariable("track2_eta",'F');
  loader->AddVariable("vtx_eta",'F',"#eta");
  loader->AddVariable("vtx_phi",'F',"#phi"); 
  loader->AddTarget("vtx_radius","radius","[cm]");

  //string track_pT_cut = "track1_pT>0&&track2_pT>0";

  //string vtx_radius_cut = "vtx_radius>0"; //can I cut based on label?
  string tCutInitializer = "";
  TCut preTraingCuts(tCutInitializer.c_str());
  loader->PrepareTrainingAndTestTree(preTraingCuts,"nTrain_Regression=0:nTest_Regression=0");
  factory->BookMethod(loader,Types::kMLP,"MLP_ANN","HiddenLayers=2000");
  factory->BookMethod(loader,Types::kMLP,"MLP_ANN2","HiddenLayers=500,6");

  
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  targetFile->Write();
  targetFile->Close();
}


int vtxPredictionTraining(){
  using namespace std;
  string treePath = "/sphenix/user/vassalli/gammasample/conversiononlineanalysis";
  string treeExtension = ".root";
  string outname = "vtxTrain.root";
  unsigned int nFiles=200;

  TChain *signalTree = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  makeFactory(signalTree,outname,"vtxFactory");
/*  outname="cutTrainE.root";
  makeFactory(signalTree,backETree,outname,"eback");*/
  return 0;
}
