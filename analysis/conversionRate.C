
TChain* handleFile(string name, string extension, string treename, unsigned int filecount){
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
TEfficiency* makepTRes(TFile* out_file,TChain* ttree=NULL,TTree* allTree=NULL){
  out_file->ReOpen("READ");
  if(out_file->Get("converted_photon_truth_pT")&&out_file->Get("all_photon_truth_pT")) 
    return new TEfficiency(*(TH1F*)out_file->Get("converted_photon_truth_pT"),*(TH1F*)out_file->Get("all_photon_truth_pT"));
  else if(!ttree||!allTree){
    return NULL;
  }
  out_file->ReOpen("UPDATE");
  float pT;
  float tpT;
  float track_pT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);

  vector<float> *allpT=NULL;
  allTree->SetBranchAddress("photon_pT",&allpT);

  TH1F *pTeffPlot = new TH1F("#frac{#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}","",40,-2,2);
  TH2F *pTefffuncPlot = new TH2F("pT_resolution_to_truthpt","",40,1,35,40,-1.5,1.5);
  TH1F *converted_pTspec = new TH1F("converted_photon_truth_pT","",20,5,25);
  TH1F *all_pTspec = new TH1F("all_photon_truth_pT","",20,5,25);
  //TH1F *trackpTDist = new TH1F("truthpt","",40,0,35);
  pTeffPlot->Sumw2();
  converted_pTspec->Sumw2();
  all_pTspec->Sumw2();
  pTefffuncPlot->Sumw2();
  //TODO need to turn off other branches 
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(pT>0)pTeffPlot->Fill(pT/tpT);
    converted_pTspec->Fill(tpT);
    if(pT>0)pTefffuncPlot->Fill(tpT,pT/tpT);
    //trackpTDist->Fill(track_pT); 
  }
  for (int event = 0; event < allTree->GetEntries(); ++event)
  {
    allTree->GetEvent(event);
    for(auto i : *allpT){
      all_pTspec->Fill(i);
    }
  }
  TEfficiency* uni_rate = new TEfficiency(*converted_pTspec,*all_pTspec);
  pTeffPlot->Scale(1./ttree->GetEntries(),"width");
  pTefffuncPlot->Scale(1./ttree->GetEntries());
  TProfile* resProfile = pTefffuncPlot->ProfileX("func_prof",5,30);
  resProfile->Write();
  //trackpTDist->Scale(1./ttree->GetEntries(),"width");
  out_file->Write();
  ttree->ResetBranchAddresses();
  return uni_rate;
}

void calculateRate(TEfficiency* rate,TFile* file){
  //get the combined pythiaspec from the file then clone it to rate
  TH1F* conversion_rate = (TH1F*)((TH1F*) file->Get("combinedpythia"))->Clone("rate");
  TH1* uni_rate = (TH1F*)rate->GetPassedHistogram()->Clone("uni_rate");
  uni_rate->Divide(rate->GetTotalHistogram());
  conversion_rate->Multiply(uni_rate);
  conversion_rate->Scale(1./365209);
  file->ReOpen("UPDATE");
  file->Write();
}

void conversionRate(){
  TFile *out_file = new TFile("effplots.root","UPDATE");
  TEfficiency* uni_rate=makepTRes(out_file);
  if(!uni_rate){
    string treePath = "/sphenix/user/vassalli/gammasample/truthconversiononlineanalysis";
    string treeExtension = ".root";
    unsigned int nFiles=200;
    TChain *ttree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
    TChain *observations = handleFile(treePath,treeExtension,"observTree",nFiles);
    cout<<"Got tree: "<<ttree->GetName()<<" and "<<ttree->GetEntries()<<" entries"<<endl;
    cout<<"Got tree: "<<observations->GetName()<<" and "<<observations->GetEntries()<<" entries"<<endl;
    uni_rate=makepTRes(out_file,ttree,observations);
  }
  out_file->ReOpen("READ");
  calculateRate(uni_rate,out_file);
}
