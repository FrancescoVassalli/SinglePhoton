#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include "TruthConversionEval.h"
#include "RecoConversionEval.h"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtruthconversion.so)
int after_embeded( std::string infile = "XjPhi3_pT5_98_dst.root",std::string outfile="embedanalysis.root",int runNumber=0)
{
  
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
//  gSystem->Load("libg4hough.so");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libcalotrigger.so");
  gSystem->Load("libtruthconversion.so");
//  gSystem->Load("libConvertedPhotonReconstructor.so");
 // gSystem->Load("libtreemaker.so");

  int verbosity = 10;
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);
  //recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->fileopen( infile );
  se->registerInputManager(hitsin);
  
  string tmvapath="/direct/phenix+u/vassalli/sphenix/single/Training/condorout/training/weights/vtxFactory_kNN14.weights.xml";
  string tmvaname="kNN14";
  TruthConversionEval *truther = new TruthConversionEval(outfile,runNumber,2,3,true,tmvaname,tmvapath);
  se->registerSubsystem(truther);
  /*RecoConversionEval *rCE = new RecoConversionEval(outfile);
  se->registerSubsystem( rCE );*/

  se->run();
  se->End();
  delete se;
  std::cout << "All done" << std::endl;

  gSystem->Exit(0);
  return 0;
}
