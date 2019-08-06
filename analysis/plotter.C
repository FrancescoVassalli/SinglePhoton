
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

void pTEff(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle(plot->GetName());
	plot->SetYTitle("dN/dN #frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	plot->Draw("e1");
}

void pTEff2D(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH2F *plot  = (TH2F*) thisFile->Get("pT_resolution_to_truthpt");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle("#{p}^{T}_{#it{truth}}");
	plot->SetYTitle("#frac{#Delta#it{p}^{T}}{#it{p}_{#it{truth}}^{T}}");
	plot->SetZTitle("dN/dN #Delta#it{p}^{T}");
	plot->Draw("colz");
}

void layer(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("layer");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle("Index of First Tracking Layer");
	plot->SetYTitle("dN/dN");
	plot->Draw("e1");
}

void deta(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("deta");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetXTitle("#DeltaEta");
	plot->SetYTitle("dN/dN");
	plot->Draw("e1");
}

void dlayer(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("dlayer");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetYTitle("dN/dN");
	plot->Draw("e1");
}
void signalVtxR(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("signal_vtx_radius_dist");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetYTitle("dN/dN");
	plot->SetXTitle("r_{vtx} [cm]");
	plot->Draw("e1");
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
		plots[i]->SetYTitle("dN/dN");
		plots[i]->SetXTitle("#Delta#it{p}");
		if(i==0) plots[i]->Draw("e1");
		else plots[i]->Draw("e1 same");
		tl->AddEntry(plots[i],plots[i]->GetName(),"l");
	}
	tl->Draw();
	tc->SaveAs("plots/deltaRefit.pdf");	
}



void plotter(){
	TFile *thisFile = new TFile("effplots.root","READ");
	//photon_m(thisFile);
	//recoRefit(thisFile);
	//pTEff(thisFile);
	//pTEff2D(thisFile);
	//layer(thisFile);
	//dlayer(thisFile);
	//deta(thisFile);
	signalVtxR(thisFile);
	//TFile *backFile = new TFile("backplots.root","READ");

}
