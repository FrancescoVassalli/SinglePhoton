#include <TFile.h>
#include <TTree.h>
#include <TString.h>


#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif

int train( std::string infile = "XjPhi3_pT5_98_dst.root",std::string outfile, int runNumber=0)
{
  using namespace TMVA;
  TString jobname("pairCuts");
  TFile *targetFile = new TFile(outfile.c_str(),"RECREATE");
  Factory *factory = new Factory(jobname,targetFile);
  TFile *inputFile = new TFile(infile.c_str(),"OPEN");
  TTree *signalTree = (TTree*) inputFile->Get("cutTreeSignal");
  TTree *backTree = (TTree*) inputFile->Get("cutTreeBack");
  factory->AddSignalTree(signalTree,1.);
  factory->AddBackgroundTree(backTree,1.);
  factory->AddVariable("track_deta",'F');
  factory->AddVariable("track_dlayer",'I');
  factory->AddVariable("track_silicon",'I');
}
