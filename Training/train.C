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


void makeFactory(TTree* signalTree, TTree* back1Tree,TTree* back2Tree,std::string outfile)
{
  using namespace TMVA;
  TString jobname("pairCuts");
  TFile *targetFile = new TFile(outfile.c_str(),"RECREATE");
  Factory *factory = new Factory(jobname,targetFile);
  factory->AddSignalTree(signalTree,1.0);
  factory->AddBackgroundTree(back1Tree,1.0);
 // factory->AddBackgroundTree(back2Tree,0.5);
  factory->AddVariable("track_deta",'F');
  factory->AddVariable("track_dlayer",'I');
  factory->AddVariable("track_layer",'I');
  factory->AddVariable("track_pT",'F');
  factory->AddVariable("approach_dist",'F');
  factory->AddVariable("vtx_radius",'F');
  //factory->AddVariable("vtx_chi2",'F'); //until reco vtx works this is meaningless
  factory->AddVariable("vtxTrack_dist",'F');
  factory->AddVariable("photon_m",'F');
  factory->AddVariable("photon_pT",'F');
  factory->AddVariable("cluster_prob",'F');

  factory->AddSpectator("vtx_chi2",'F'); //until reco vtx works this is meaningless


  string track_pT_cut = "track_pT>0";
  string vtx_radius_cut = "vtx_radius>0";
  string em_prob_cut = "cluster_prob>=0";
  //do I need photon cuts? 
  string tCutInitializer = em_prob_cut+"&&"+ vtx_radius_cut+"&&"+track_pT_cut+"&&track_dlayer>=0&&track_layer>=0&&approach_dist>0&&vtxTrack_dist>0";
  TCut preTraingCuts(tCutInitializer.c_str());


  factory->PrepareTrainingAndTestTree(preTraingCuts,"nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0");
  factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD",
      "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
}


int train(){
  using namespace std;
  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".root";
  string outname = "cutTrain.root";
  unsigned int nFiles=100;

  TChain *signalTree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *backHTree = handleFile(treePath,treeExtension,"cutTreeBackh",nFiles);
  TChain *backETree = handleFile(treePath,treeExtension,"cutTreeBacke",nFiles);
  makeFactory(signalTree,backHTree,backETree,outname);
}
