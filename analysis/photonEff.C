#include <iostream>
#include <fstream>
using namespace std;

#include "TFile.h"
#include "Scalar.h"
#include "TTree.h"
#include "TChain.h"
#include "TLegend.h"
#include "math.h"
#include "TH1.h"
#include "TH2.h"
#include "TEfficiency.h"
#include "TLine.h"
#include "TGraphAsymmErrors.h"

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

void makephotonM(TChain* ttree,TFile* out_file){
  float photon_m;
  float tphoton_m;
  std::vector<TH1F*> plots;
  ttree->SetBranchAddress("photon_m",     &photon_m   );
  //ttree->SetBranchAddress("rephoton_m",     &rephoton_m   );
  plots.push_back(new TH1F("m^{#gamma}_{reco}","",60,0,.18));
  //plots.push_back(new TH1F("m^{#gamma}_{recoRefit}","",40,-2,10));
  
  for (int i = 0; i < plots.size(); ++i)
  {
    plots[i]->Sumw2();
  }
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(photon_m);
   // plots[1]->Fill(rephoton_m);
  }
  for (int i = 0; i < plots.size(); ++i)
  {
    plots[i]->Scale(1./ttree->GetEntries(),"width");
  }
  out_file->Write();
  ttree->ResetBranchAddresses();
}

void makeVtxR(TChain* ttree,TFile* out_file){
  float vtxr;
  float tvtxr;
  ttree->SetBranchAddress("vtx_radius",&vtxr);
  ttree->SetBranchAddress("tvtx_radius",&tvtxr);

  std::vector<TH1F*> plots;
  plots.push_back(new TH1F("vtx_reco","",40,0,30));
  plots.push_back(new TH1F("vtx_truth","",40,0,30));
  
  plots[0]->Sumw2();
  plots[1]->Sumw2();

  double calc=0;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    plots[0]->Fill(vtxr);
    plots[1]->Fill(tvtxr);
    calc+=TMath::Abs(vtxr-tvtxr);
  }
  calc/=ttree->GetEntries();
  for (int i = 0; i < 2; ++i)
  {
    plots[i]->Scale(1./ttree->GetEntries(),"width");
  }
  out_file->Write();
  std::cout<<"mean deviation="<<calc<<std::endl;
}

void makeVtxRes(TChain* ttree,TFile* out_file){
  float r;
  float tr;
  ttree->SetBranchAddress("vtx_radius",&r);
  ttree->SetBranchAddress("tvtx_radius",&tr);
  TH1F *vtxeffPlot = new TH1F("#frac{#Deltar_{vtx}_^{#it{reco}}}{r_{vtx}^{#it{truth}}}","",40,-2,2);
  TH2F *vtxefffuncPlot = new TH2F("vtx_resolution_to_truthvtx","",20,0,21,40,-1.5,1.5);
  vtxeffPlot->Sumw2();
  vtxefffuncPlot->Sumw2();
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(r<0) continue;
    vtxeffPlot->Fill((r-tr)/tr);
    vtxefffuncPlot->Fill(tr,(r-tr)/tr);
  }
  vtxeffPlot->Scale(1./ttree->GetEntries(),"width");
  vtxefffuncPlot->Scale(1./ttree->GetEntries(),"width");
  out_file->Write();
  ttree->ResetBranchAddresses();
}

void makeVtxEff(TChain* ttree,TFile* out_file){
  float r;
  float tr;
  ttree->SetBranchAddress("vtx_radius",&r);
  ttree->SetBranchAddress("tvtx_radius",&tr);
  TEfficiency *vtxEff;
  TH1F *recoR= new TH1F("vtxrecoR","",20,0,21);
  TH1F *truthR= new TH1F("vtxtruthR","",20,0,21);
  recoR->Sumw2();
  truthR->Sumw2();
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(r>0) recoR->Fill(tr);
    truthR->Fill(tr);
  }
  vtxEff = new TEfficiency(*recoR,*truthR);
  vtxEff->SetName("vtxEff");
  vtxEff->Write();
  out_file->Write();
  ttree->ResetBranchAddresses();
}

void pTResFunction(TH2F* plot2d){
  
}

TH1F* makepTRes(TChain* ttree,TFile* out_file){
  float pT;
  float tpT;
  float track_pT;
  ttree->SetBranchAddress("photon_pT",&pT);
  ttree->SetBranchAddress("tphoton_pT",&tpT);
  
  TH1F *pTeffPlot = new TH1F("#frac{#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}","",40,-2,2);
  TH2F *pTefffuncPlot = new TH2F("pT_resolution_to_truthpt","",40,1,35,40,-1.5,1.5);
  TH1F *tpTspec = new TH1F("converted_photon_truth_pT","",25,5,30);
  //TH1F *trackpTDist = new TH1F("truthpt","",40,0,35);
  pTeffPlot->Sumw2();
  tpTspec->Sumw2();
  pTefffuncPlot->Sumw2();
  //t cout<<"here"<<endl;rackpTDist->Sumw2();
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(pT>0)pTeffPlot->Fill(pT/tpT);
    tpTspec->Fill(tpT);
    if(pT>0)pTefffuncPlot->Fill(tpT,pT/tpT);
    //trackpTDist->Fill(track_pT); 
  }
  pTeffPlot->Scale(1./ttree->GetEntries(),"width");
  tpTspec->Scale(1./300000); //total number of embeded photon in nobgrd simulation 15 per event 100 events per run 200 runs 
  cout<<"Conversion pT spectrum integral: "<<tpTspec->Integral()<<'\n';
  pTefffuncPlot->Scale(1./ttree->GetEntries());
  TProfile* resProfile = pTefffuncPlot->ProfileX("func_prof",5,30);
  resProfile->Write();
  //trackpTDist->Scale(1./ttree->GetEntries(),"width");
  out_file->Write();
  ttree->ResetBranchAddresses();
  return tpTspec;
}

void testCuts(TChain* ttree,TFile* out_file){
  float dphi;
  float prob;
  float track_pT;
  float deta;
  float radius;
  int layer;
  int dlayer;
  ttree->SetBranchAddress("cluster_dphi",&dphi);
  ttree->SetBranchAddress("cluster_prob",&prob);
  ttree->SetBranchAddress("track_layer",&layer);
  ttree->SetBranchAddress("track_dlayer",&dlayer);
  ttree->SetBranchAddress("track_pT",&track_pT);
  ttree->SetBranchAddress("track_deta",&deta);
  ttree->SetBranchAddress("vtx_radius",&radius);
  
  TH1F *layerDist = new TH1F("layer","",16,-.5,15.5);
  TH1F *probDist = new TH1F("clust_prob","",30,-.5,1.);
  TH1F *deta_plot = new TH1F("deta","",30,-.001,.01);
  TH1F *dlayer_plot = new TH1F("dlayer","",11,-.5,10.5);
  TH1F *r_plot = new TH1F("signal_vtx_radius_dist","",26,-.5,25.5);
  layerDist->Sumw2();
  probDist->Sumw2();
  deta_plot->Sumw2();
  dlayer_plot->Sumw2();
  r_plot->Sumw2();
  unsigned badLayCount=0;
  unsigned badClusCount=0;
  unsigned bigDetaCount=0;
  unsigned shortRadiusCount=0;

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    if(layer==0)badLayCount++;
    if(dphi<0&&track_pT>.6){
      badClusCount++;
    }
    if(track_pT>.6&&dphi>=0){
      layerDist->Fill(layer);
      probDist->Fill(prob);
      deta_plot->Fill(deta);
      dlayer_plot->Fill(TMath::Abs(dlayer));
      if(deta>.0082||TMath::Abs(dlayer)>9)bigDetaCount++;
      else{
        r_plot->Fill(radius);
        if(radius<1.84) shortRadiusCount++;
      }
    }

  }
  layerDist->Scale(1./ttree->GetEntries());
  deta_plot->Scale(1./ttree->GetEntries());
  dlayer_plot->Scale(1./ttree->GetEntries());
  r_plot->Scale(1./ttree->GetEntries());
  cout<<"Signal rejection through layer cut= "<<(float)badLayCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)badLayCount)/ttree->GetEntries()<<endl;
  cout<<"Signal rejection through clus cut= "<<(float)badClusCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)badClusCount)/ttree->GetEntries()<<endl;
  cout<<"Signal rejection through deta cut= "<<(float)bigDetaCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)bigDetaCount)/ttree->GetEntries()<<endl;
  cout<<"Signal rejection through radius cut= "<<(float)shortRadiusCount/ttree->GetEntries()<<endl;
  cout<<"error= "<<sqrt((float)shortRadiusCount)/ttree->GetEntries()<<endl;
  out_file->Write();
}

void makeRefitDist(TChain* ttree, TFile *out_file){
  float diffx;
  float diffy;
  float diffz;
  ttree->SetBranchAddress("refitdiffx",&diffx);
  ttree->SetBranchAddress("refitdiffy",&diffy);
  ttree->SetBranchAddress("refitdiffz",&diffz);
  TH1F *diffplotx= new TH1F("x_{0}-x_{refit}","",40,-10,10);
  TH1F *diffploty= new TH1F("y_{0}-y_{refit}","",40,-10,10);
  TH1F *diffplotz= new TH1F("z_{0}-z_{refit}","",40,-10,10);

  diffplotx->Sumw2();
  diffploty->Sumw2();
  diffplotz->Sumw2();

  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    diffplotx->Fill(diffx);
    diffploty->Fill(diffy);
    diffplotz->Fill(diffz);
  }
  
  diffplotx->Scale(1./ttree->GetEntries(),"width");
  diffploty->Scale(1./ttree->GetEntries(),"width");
  diffplotz->Scale(1./ttree->GetEntries(),"width");

  out_file->Write();
  ttree->ResetBranchAddresses();
}

void makepTCaloGraph(string filename,TFile* outfile){
  ifstream caloFile;
  caloFile.open(filename.c_str());
  double x,y;
  string s;
  vector<double> xData, yData;
  /*if(!(caloFile >>x>>y)){
    cout<<"file error"<<endl;
    if(!caloFile.is_open()) cout<<"file not opened"<<endl;
  }*/
  while(caloFile >>x>>s>>y){
    xData.push_back(x);
    yData.push_back(y);
  }
  double *xArray, *yArray;
  xArray=&xData[0];
  yArray=&yData[0];
  TGraph *pTResCaloGraph = new TGraph(xData.size(),xArray,yArray);
  pTResCaloGraph->SetNameTitle("calopTRes","calopTRes");
  pTResCaloGraph->Sort();
  pTResCaloGraph->Write();
  outfile->Write();
}

TH1F* makePythiaSpec(TChain* ttree,TFile* out_file,string type=""){
  vector<float>* tpT= NULL;
  ttree->SetBranchAddress("photon_pT",&tpT);
  string title;
  if(!type.empty()){
    title=type;
    title+="_photon_truth_pT";
  }  
  else title="photon_truth_pT";
  TH1F *tpTspec = new TH1F(title.c_str(),"",25,5,30);
  tpTspec->Sumw2();
  cout<<"pythia tree with: "<<ttree->GetEntries()<<" entries"<<endl;
  for (int event = 0; event < ttree->GetEntries(); ++event)
  {
    ttree->GetEvent(event);
    for(auto i: *tpT){
      tpTspec->Fill(i);
    }
  }
  out_file->Write();
  ttree->ResetBranchAddresses();
  return tpTspec;
}

void calculateConversionRate(TH1F* converted, TH1F *pythia,TFile* out_file){
  TH1F* conversion_rate = NULL;
  conversion_rate = (TH1F*)  converted->Clone("rate");
  conversion_rate->Divide(pythia);
  conversion_rate->Scale(pythia->Integral());
  out_file->Write();
}

void photonEff()
{
  TFile *out_file = new TFile("effplots.root","UPDATE");
  //string treePath = "/sphenix/user/vassalli/RecoConversionTests/truthconversionembededonlineanalysis";
  string treePath = "/sphenix/user/vassalli/gammasample/truthconversiononlineanalysis";
  string treeExtension = ".root";
  unsigned int nFiles=200;
  TChain *ttree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *observations = handleFile(treePath,treeExtension,"observTree",nFiles);
  cout<<"Total events= "<<ttree->GetEntries()<<'\n';

  //string pythiaPath = "/sphenix/user/vassalli/minBiasPythia/softana.root";
  string pythiaPath = "/sphenix/user/vassalli/minBiasPythia/hardana.root";
//  string pythiaExtension = "_analysis.root";
  /*unsigned int nPythiaFiles=1700;
  TChain *pythiaTree = handleFile(pythiaPath,pythiaExtension,"photonTree",nPythiaFiles);*/
  TChain *pythiaTree = new TChain("photonTree");
  string haddname = pythiaPath;
  pythiaTree->Add(haddname.c_str());
  //TChain *ttree2 = handleFile(treePath,treeExtension,"vtxingTree",nFiles);
  //makephotonM(ttree,out_file);
  //makePythiaSpec(pythiaTree,out_file,"soft");
  makePythiaSpec(pythiaTree,out_file,"hard");
  //calculateConversionRate(makepTRes(ttree,out_file),makePythiaSpec(pythiaTree,out_file),out_file);
  //makeVtxRes(ttree,out_file);
  //makeVtxEff(ttree,out_file);
  //testCuts(ttree,out_file);
  //makepTCaloGraph("pTcalodata.csv",out_file);
  //makeVtxR(ttree2,out_file);
  //makeRefitDist(ttree,out_file);
}
