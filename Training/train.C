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


void makeFactory(TTree* signalTree, TTree* backTree,std::string outfile)
{
  using namespace TMVA;
  TString jobname("pairCuts");
  TFile *targetFile = new TFile(outfile.c_str(),"RECREATE");
  Factory *factory = new Factory(jobname,targetFile);
  factory->AddSignalTree(signalTree,1.);
  factory->AddBackgroundTree(backTree,1.);
  factory->AddVariable("track_deta",'F');
  factory->AddVariable("track_dlayer",'I');
  factory->AddVariable("track_silicon",'I');
}


int train(){
  using namespace std;
  string treePath = "/sphenix/user/vassalli/gammasample/fourembededonlineanalysis";
  string treeExtension = ".rootcTtree.root";
  string outname = "";
  unsigned int nFiles=100;

  TChain *signalTree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *backTree = handleFile(treePath,treeExtension,"cutTreeBack",nFiles);
  makeFactory(signalTree,backTree,outname);
}
