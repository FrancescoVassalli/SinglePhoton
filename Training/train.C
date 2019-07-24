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
  factory->AddSpectator("track_layer",'I');
  factory->AddSpectator("track_pT",'F');
  factory->AddSpectator("track_dca",'F');
  factory->AddSpectator("cluster_prob",'F');
  factory->AddVariable("track_deta",'F');
  factory->AddVariable("track_dlayer",'I');
  factory->AddVariable("approach_dist",'F');
  //factory->AddVariable("vtx_radius",'F');
  //factory->AddVariable("vtx_chi2",'F'); 
  //factory->AddVariable("vtxTrackRZ_dist",'F');
  //factory->AddVariable("abs(vtxTrackRPhi_dist-vtxTrackRZ_dist)",'F');
  //factory->AddVariable("photon_m",'F');
  //factory->AddVariable("photon_pT",'F');

  string track_layer_cut = "track_layer>0";
  string track_pT_cut = "track_pT>=0.84";
  string track_dca_cut = "30>track_dca>0";
  string em_prob_cut = "cluster_prob>=0";
  string track_deta_cut = ".0076>=track_deta>=0";
  string approach_dist_cut = "27.47>approach_dist>0";
  string vtx_radius_cut = "vtx_radius>0";
  //do I need photon cuts? 
  string tCutInitializer = track_pT_cut+"&&"+track_layer_cut+"&&"+track_dca_cut+"&&"+em_prob_cut;
  TCut preTraingCuts(tCutInitializer.c_str());

  factory->PrepareTrainingAndTestTree(preTraingCuts,"nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0");
  //factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD","!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );
  factory->BookMethod(Types::kCuts,"Cuts","CutRangeMin[0]=0:CutRangeMax[0]=1:CutRangeMin[1]=-100:CutRangeMax[1]=100:CutRangeMin[2]=0:CutRangeMax[2]=100");
  /*factory->BookMethod( Types::kKNN, "kNN", "" ); //>100k events
  factory->BookMethod( Types::kPDERS, "PDERS", "" );//>100k events*/
  /*factory->BookMethod( Types::kPDEFoam, "PDEFoam", "VolFrac=.0588i:SigBgSeparate=True" );//>10k events
  factory->BookMethod( Types::kFisher, "Fisher", "" );
  factory->BookMethod( Types::kLD, "LD" );*/
  /*would need to have the options tuned
   * factory->BookMethod( Types::kFDA, "FDA", "Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:\
      ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):\
      FitMethod=MINUIT:\
      ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );*/
/* nerual network that would need to be worked on 
 * factory->BookMethod( Types::kMLP, "MLP_ANN", "<options>" );*/
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
  TChain *backHTree = handleFile(treePath,treeExtension,"cutTreeBackh",nFiles);
  TChain *backETree = handleFile(treePath,treeExtension,"cutTreeBacke",nFiles);
  makeFactory(signalTree,backHTree,outname,"aback",backETree);
/*  outname="cutTrainE.root";
  makeFactory(signalTree,backETree,outname,"eback");*/
}
