int after_embeded( std::string infile = "XjPhi3_pT5_98_dst.root",std::string outfile)
{
  
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libcalotrigger.so");
  gSystem->Load("libSinglePhotonAnalysis.so");
  gSystem->Load("libConvertedPhotonReconstructor.so");
  gSystem->Load("libtreemaker.so");

  int verbosity = 0;
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(10);
  recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->fileopen( infile );
  se->registerInputManager(hitsin);
  
  RecoConversionEval *rCE = new RecoConversionEval(outfile);
  se->registerSubsystem( rCE );

  se->run();
  se->End();
  delete se;
  std::cout << "All done" << std::endl;

  gSystem->Exit(0);
}
