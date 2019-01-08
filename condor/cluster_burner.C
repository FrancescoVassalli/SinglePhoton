int cluster_burner( std::string infile = "XjPhi3_pT5_98_dst.root",std::string outfile)
{
  
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");

  gSystem->Load("libg4eval.so");
  gSystem->Load("libcalotrigger.so");
  gSystem->Load("libtruthconversion.so");

  int verbosity = 0;
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(10);
  recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->fileopen( infile );
  se->registerInputManager(hitsin);
 
  TruthConversionEval *truther = new TruthConversionEval(outfile);
  se->registerSubsystem(truther);
//  ConvertedPhotonReconstructor *converter=new ConvertedPhotonReconstructor(outfile);
  //se->registerSubsystem(converter);
  outfile+="cTtree.root";

  TreeMaker *tt = new TreeMaker( outfile,3,truther);
  se->registerSubsystem( tt );
  
  se->run();
  se->End();
  delete se;
  std::cout << "All done" << std::endl;

  gSystem->Exit(0);
}
