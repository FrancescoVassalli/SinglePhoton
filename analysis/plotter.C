
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
	plots.push_back((TH1F*) thisFile->Get("m^{#gamma}_{reco}"));
	plots.push_back((TH1F*) thisFile->Get("m^{#gamma}_{recoRefit}"));

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
	photon_m(thisFile);
	recoRefit(thisFile);
	//TFile *backFile = new TFile("backplots.root","READ");

}
