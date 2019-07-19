#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include "VtxTest.h"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtruthconversion.so)

int vtxTest_burner( std::string infile = "XjPhi3_pT5_98_dst.root",std::string outfile="cluster_burner_out.root", int runNumber=0)
{
  
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  //gSystem->Load("libg4hough.so");
  gSystem->Load("libg4calo.so");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libcalotrigger.so");
  gSystem->Load("libtruthconversion.so");

  int verbosity = 10;
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);
  //recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->fileopen( infile );
  se->registerInputManager(hitsin);
 
  VtxTest *test = new VtxTest(outfile,runNumber,2,3,true);
  se->registerSubsystem(test);
//  ConvertedPhotonReconstructor *converter=new ConvertedPhotonReconstructor(outfile);
  //se->registerSubsystem(converter);
/*  outfile+="cTtree.root";

  TreeMaker *tt = new TreeMaker( outfile,3,truther);
  se->registerSubsystem( tt );
  */
  se->run();
  se->End();
  delete se;
  std::cout << "All done" << std::endl;

  gSystem->Exit(0);
  return 0;
}
