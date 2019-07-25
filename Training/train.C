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


void makeFactory(TTree* signalTree, TTree* backTree,std::string outfile,std::string factoryname, TTree* bgTree2=NULL)
{
  using namespace TMVA;
  TString jobname(factoryname.c_str());
  TFile *targetFile = new TFile(outfile.c_str(),"RECREATE");
  Factory *factory = new Factory(jobname,targetFile);
  factory->AddSignalTree(signalTree,1.0);
  factory->AddBackgroundTree(backTree,1.0);
  if(bgTree2){
    factory->AddBackgroundTree(bgTree2,1.0);
  }
  factory->AddVariable("track_layer",'I');
  factory->AddVariable("track_pT",'F');
  factory->AddVariable("track_dca",'F');
  factory->AddVariable("cluster_prob",'F');
  //factory->AddVariable("track_deta",'F');
  //factory->AddVariable("abs(track_dlayer)",'I');
  //factory->AddVariable("approach_dist",'F');
  //factory->AddVariable("vtx_radius",'F');
  //factory->AddVariable("vtx_chi2",'F'); 
  //factory->AddVariable("vtxTrackRZ_dist",'F');
  //factory->AddVariable("abs(vtxTrackRPhi_dist-vtxTrackRZ_dist)",'F');
  //factory->AddVariable("photon_m",'F');
  //factory->AddVariable("photon_pT",'F');

  string track_layer_cut = "track_layer>=0";
  string track_pT_cut = "track_pT>=0.84";
  string track_dca_cut = "30>track_dca>0";
  string em_prob_cut = "cluster_prob>=0";
  string track_deta_cut = ".0076>=track_deta>=0";
  string track_dlayer_cut = "3>=abs(track_dlayer)>=0";
  string approach_dist_cut = "21.54>approach_dist>0";
  string vtx_radius_cut = "vtx_radius>0";
  //do I need photon cuts? 
  string tCutInitializer = "track_pT>=.6&&track_layer>=0&&cluster_prob>=0&&track_dca>=0";
  TCut preTraingCuts(tCutInitializer.c_str());

  factory->PrepareTrainingAndTestTree(preTraingCuts,"nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0");
  //for track training
  factory->BookMethod(Types::kCuts,"Cuts");
  //for pair training
  //factory->BookMethod(Types::kCuts,"Cuts","CutRangeMin[0]=0:CutRangeMax[0]=1:CutRangeMin[1]=-100:CutRangeMax[1]=100:CutRangeMin[2]=0:CutRangeMax[2]=100");
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  targetFile->Write();
  targetFile->Close();
}


int train(){
  using namespace std;
  string treePath = "/sphenix/user/vassalli/gammasample/conversionembededonlineanalysis";
  string treeExtension = ".root";
  string outname = "cutTrainA.root";
  unsigned int nFiles=100;

  TChain *signalTree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *backtrackTree = handleFile(treePath,treeExtension,"_trackBackTree",nFiles);
  TChain *backpairTree = handleFile(treePath,treeExtension,"_pairBackTree",nFiles);
  makeFactory(signalTree,backtrackTree,outname,"trackback");
  //makeFactory(signalTree,backpairTree,outname,"pairback");
/*  outname="cutTrainE.root";
  makeFactory(signalTree,backETree,outname,"eback");*/
}
