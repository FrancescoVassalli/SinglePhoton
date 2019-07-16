#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include "SVReco.h"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libSVReco.so)

int Run_SVReco(
		const int nEvents = 0,
		//const char * inputFile = "G4sPHENIXCells_test.root",
		const char * inputFile = "G4sPHENIX.root"
		)
{
  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  //gSystem->Load("libcemc.so");
  gSystem->Load("libg4eval.so");
	gSystem->Load("libSVReco.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  // just if we set some flags somewhere in this macro
  //recoConsts *rc = recoConsts::instance();
  // By default every random number generator uses
  // PHRandomSeed() which reads /dev/urandom to get its seed
  // if the RANDOMSEED flag is set its value is taken as seed
  // You ca neither set this to a random value using PHRandomSeed()
  // which will make all seeds identical (not sure what the point of
  // this would be:
  //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
  // or set it to a fixed value so you can debug your code
  // rc->set_IntFlag("RANDOMSEED", 12345);

	SVReco *svreco = new SVReco();
	svreco->set_do_eval(true);
	svreco->set_cut_jet(true);
	svreco->set_cut_jet_R(0.4);
	svreco->set_cut_ncluster(false);
	se->registerSubsystem( svreco );

	Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
	hitsin->fileopen(inputFile);
	se->registerInputManager(hitsin);

  //-----------------
  // Event processing
  //-----------------
	/*
  if (nEvents < 0)
    {
      return;
    }
  // if we run the particle generator and use 0 it'll run forever
  if (nEvents == 0 && !readhits && !readhepmc)
    {
      cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
      cout << "it will run forever, so I just return without running anything" << endl;
      return;
    }
	*/

  se->run(nEvents);

  //-----
  // Exit
  //-----

  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
