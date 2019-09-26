
void myText(Double_t x,Double_t y,Color_t color, const char *text, Double_t tsize) {
	TLatex l; //l.SetTextAlign(12); 
	l.SetTextSize(tsize); 
	l.SetNDC();
	l.SetTextColor(color);
	l.DrawLatex(x,y,text);
}

void photon_m(TFile* thisFile){
	gStyle->SetOptStat(0);
	std::vector<TH1F*> plots;
	plots.push_back((TH1F*) thisFile->Get("m^{#gamma}_{recoRefit}"));
	plots.push_back((TH1F*) thisFile->Get("m^{#gamma}_{reco}"));

	TCanvas* tc = new TCanvas();
	tc->Draw();
	TLegend* tl = new TLegend(.7,.7,.9,.9);
	plots[0]->SetLineColor(kRed);
	plots[1]->SetLineColor(kGreen+2);
	for (int i = 0; i < plots.size(); ++i)
	{
		plots[i]->SetYTitle("#frac{dN}{dm *N_{#gamma}}");
		plots[i]->SetXTitle("invarient mass GeV/c^{2}");
		if(i==0) plots[i]->Draw("e1");
		else plots[i]->Draw("e1 same");
		tl->AddEntry(plots[i],plots[i]->GetName(),"l");
	}
	tl->Draw();
	tc->SaveAs("plots/gamma_dm_eff.pdf");	
}

void pTRes(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	//TGraph *graph  = (TH1F*) thisFile->Get("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->GetXaxis()->SetTitle(plot->GetName());
	plot->GetYaxis()->SetTitle("1/N #frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	plot->Draw("e1");
}

void pTRes2D(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH2F *plot  = (TH2F*) thisFile->Get("pT_resolution_to_truthpt");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle("#it{p}^{T}_{#it{truth}}");
	plot->SetYTitle("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	plot->SetZTitle("1/N #Delta#it{p}^{T}");
	plot->Draw("colz");
}

void vtxRes(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("#frac{#Deltar_{vtx}_^{#it{reco}}}{r_{vtx}^{#it{truth}}}");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle(plot->GetName());
	//plot->SetYTitle("1/N #frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	plot->Draw("e1");
}

void vtxRes2D(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH2F *plot  = (TH2F*) thisFile->Get("vtx_resolution_to_truthvtx");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle("radius_{#it{truth}} [cm]");
	plot->SetYTitle("conversion vertex radius resolution");
	//plot->SetZTitle("1/N #Delta#it{p}^{T}");
	plot->Draw("colz");
}

void vtxEff(TFile *thisFile){
	gStyle->SetOptStat(0);
	TEfficiency *plot  = (TEfficiency*) thisFile->Get("vtxEff");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetTitle(";radius_{#it{truth}} [cm];conversion vertex reco efficiency");
	plot->Draw("");
}

void layer(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("layer");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle("Index of First Tracking Layer");
	plot->SetYTitle("1/N");
	plot->Draw("e1");
}

void deta(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("deta");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle("#DeltaEta");
	plot->SetYTitle("1/N");
	plot->Draw("e1");
}

void compareDeta(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *sig  = (TH1F*) thisFile->Get("detaS");
	TH1F *back  = (TH1F*) thisFile->Get("detaB");
	TCanvas* tc = new TCanvas();
	tc->SetTicky();
	TLegend *tl =new TLegend(.6,.8,.9,.9);
	tc->Draw();
	sig->SetXTitle("#Delta#eta");
	sig->SetYTitle("1/N");
	sig->GetXaxis()->SetRangeUser(0,.005);
	sig->GetYaxis()->SetTitleOffset(1);
	sig->Draw("p");
	back->SetLineColor(kRed);
	back->SetMarkerColor(kRed);
	back->SetMarkerStyle(kFullTriangleUp);
	sig->SetMarkerStyle(kFullCircle);
	tl->AddEntry(sig,"Conversions","p");
	tl->AddEntry(back,"Pythia Photon-Jet Background","p");
	back->Draw("p same");
	tl->Draw();
}

void dlayer(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("dlayer");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetYTitle("1/N");
	plot->Draw("e1");
}
void signalVtxR(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("signal_vtx_radius_dist");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetYTitle("1/N");
	plot->SetXTitle("r_{vtx} [cm]");
	plot->Draw("e1");
}

void vtxR(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plotTruth  = (TH1F*) thisFile->Get("truthDist");
	TH1F *plotReco  = (TH1F*) thisFile->Get("recoDist");
	TH1F *plotCorr = (TH1F*) thisFile->Get("correctedDist");
	plotReco->SetLineColor(kRed);
	plotReco->SetMarkerColor(kRed);
	plotCorr->SetLineColor(kGreen+2);
	plotCorr->SetMarkerColor(kGreen+2);
	TCanvas* tc = new TCanvas();
	tc->Draw();
	TLegend *tl = new TLegend(.8,.8,.9,.9);
	tl->AddEntry(plotTruth,"Truth","l");
	tl->AddEntry(plotReco,"Reco","l");
	tl->AddEntry(plotCorr,"Corrected Reco","l");
	plotTruth->SetYTitle("1/N");
	plotTruth->SetXTitle("r_{vtx} [cm]");
	plotReco->Draw("e1 ");
	plotTruth->Draw("e1 same");
	plotCorr->Draw("e1 same");
	tl->Draw();
}

void recoRefit(TFile* thisFile){
	gStyle->SetOptStat(0);
	std::vector<TH1F*> plots;
	plots.push_back((TH1F*) thisFile->Get("y_{0}-y_{refit}"));
	plots.push_back((TH1F*) thisFile->Get("x_{0}-x_{refit}"));
	plots.push_back((TH1F*) thisFile->Get("z_{0}-z_{refit}"));

	TCanvas* tc = new TCanvas();
	tc->Draw();
	TLegend* tl = new TLegend(.7,.7,.9,.9);
	plots[1]->SetLineColor(kRed);
	plots[2]->SetLineColor(kGreen+2);
	for (int i = 0; i < plots.size(); ++i)
	{
		plots[i]->SetYTitle("1/N");
		plots[i]->SetXTitle("#Delta#it{p}");
		if(i==0) plots[i]->Draw("e1");
		else plots[i]->Draw("e1 same");
		tl->AddEntry(plots[i],plots[i]->GetName(),"l");
	}
	tl->Draw();
	tc->SaveAs("plots/deltaRefit.pdf");	
}

unsigned topFilledBin(TH1* hard){
	unsigned r= hard->GetNbinsX();
	while(hard->GetBinContent(r--)==0);
	return r;
}

unsigned getMatchingBin(TH1* hard, TH1* soft){
	unsigned r=1;
	while(soft->GetBinLowEdge((r++)+1)!=hard->GetBinLowEdge(1));
	return r;
}

/*void truthPtMix(TFile* thisFile){
	gStyle->SetOptStat(0);
	TH1F*  soft= (TH1F*) thisFile->Get("soft_photon_truth_pT");
	TH1F*  hard= (TH1F*) thisFile->Get("hard4_photon_truth_pT");
	TCanvas* tc = new TCanvas();
	tc->SetLogy();
	soft->Draw();
	hard->SetLineColor(kRed);
	hard->Draw("same");
	TH1F* combined = new TH1F("combinedpythia","",soft->GetNbinsX(),soft->GetBinLowEdge(1),hard->GetBinLowEdge(topFilledBin(hard)));
	unsigned matchBin = 11;//getMatchingBin(hard,soft);
	for (int i = 1; i < matchBin; ++i)
	{
		combined->SetBinContent(i,soft->GetBinContent(i));
		combined->SetBinError(i,soft->GetBinError(i));
	}
	hard->Scale(hardWeightFactor(hard,soft,matchBin));
	for (int i = matchBin; i < combined->GetNbinsX()+1; ++i)
	{
		combined->SetBinContent(i,hard->GetBinContent(i));
		combined->SetBinError(i,hard->GetBinError(i));
	}
	TCanvas* tc2 = new TCanvas();
	tc2->Draw();
	tc2->SetLogy();
	combined->Draw();
}*/

void drawMaps(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH2F* tmap = (TH2F*) thisFile->Get("truthMap");
	TH2F* rmap = (TH2F*) thisFile->Get("recoMap");
	TCanvas *tc;

	if (tmap)
	{
		tc=new TCanvas();
		tmap->Draw("colz");
	}
	if (rmap)
	{
		if (!tc)
		{
			tc=new TCanvas();
			rmap->Draw("colz");
		}
		else{
			TCanvas *tc2 = new TCanvas();
			rmap->Draw("colz");
		}
	}
}





void plotter(){
	TFile *thisFile = new TFile("effplots.root","READ");
	TFile *thisOtherFile = new TFile("maps.root","READ");
	//photon_m(thisFile);
	//recoRefit(thisFile);
	//pTRes(thisFile);
	//pTRes2D(thisFile);
	//vtxRes(thisFile);
	//vtxRes2D(thisFile);
	//vtxEff(thisFile);
	//layer(thisFile);
	//dlayer(thisFile);
	//deta(thisFile);
	//signalVtxR(thisFile);
	//vtxR(thisOtherFile);
	//truthPtMix(thisFile);
	compareDeta(thisFile);
	drawMaps(thisOtherFile);
	//TFile *backFile = new TFile("backplots.root","READ");

}
