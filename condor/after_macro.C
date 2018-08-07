int after_macro( std::string infile = "XjPhi3_pT5_98_dst.root")
{
  
	std::string outfile = infile+"truth.root";
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libcalotrigger.so");
  gSystem->Load("libSinglePhotonAnalysis.so");
  gSystem->Load("libConvertedPhotonReconstructor.so");
  int verbosity = 0;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity( 0 );
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->fileopen( infile );
  se->registerInputManager(hitsin);
 
  SinglePhotonAfter *truther = new SinglePhotonAfter(outfile);
  se->registerSubsystem(truther);
  se->registerSubsystem(new ConvertedPhotonReconstructor(outfile);
  se->run();

  se->End();
  std::cout << "All done" << std::endl;
  delete se;

  gSystem->Exit(0);
}
