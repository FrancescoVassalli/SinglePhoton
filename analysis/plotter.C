
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
	//plots.push_back((TH1F*) thisFile->Get("m^{#gamma}_{recoRefit}"));
	plots.push_back((TH1F*) thisFile->Get("m^{#gamma}_{reco}"));
	plots.push_back((TH1F*) thisFile->Get("m^{bkgd}_{reco}"));

	TCanvas* tc = new TCanvas();
	tc->Draw();
	TLegend* tl = new TLegend(.7,.7,.9,.9);
	//plots[0]->SetLineColor(kRed);
	plots[0]->SetMarkerStyle(kFullCircle);
	plots[1]->SetMarkerStyle(kFullTriangleUp);
	plots[1]->SetLineColor(kGreen+2);
	plots[1]->SetMarkerColor(kGreen+2);

	for (int i = 0; i < plots.size(); ++i)
	{
		plots[i]->SetYTitle("#frac{1}{dN_{#gamma}}#frac{dN}{dm}");
		plots[i]->GetYaxis()->SetTitleOffset(1);
		plots[i]->GetXaxis()->SetRangeUser(0,.1);
		plots[i]->SetXTitle("m_{ee} [#frac{GeV}{c^{2}}]");
		if(i==0) plots[i]->Draw("e1");
		else plots[i]->Draw("e1 same");
	}
	tl->AddEntry(plots[0],"Photon Candidates","p");
	tl->AddEntry(plots[1],"Background","p");
	tl->Draw();
	//tc->SaveAs("plots/gamma_dm_eff.pdf");	
}

void pTRes(TFile *thisFile){
	gStyle->SetOptStat(0);
	TH1F *plot  = (TH1F*) thisFile->Get("#frac{#it{p}^{T}_{reco}}{#it{p}_{#it{truth}}^{T}}");
	TGraph *graph  = (TGraph*) thisFile->Get("calopTRes");
	graph->Print("all");
	double graphMax = 0;
	cout<<"here: "<<graphMax<<endl;
	for (int i=0;i<graph->GetN();i++) if(graph->GetY()[i]>graphMax) graphMax = graph->GetY()[i];
	double scale  = graphMax / plot->GetMaximum();
	for (int i=0;i<graph->GetN();i++) graph->GetY()[i] /= scale;
	graph->SetMarkerSize(.4);
	TCanvas* tc = new TCanvas();
	TLegend *tl = new TLegend(.7,.7,.9,.9);
	tl->AddEntry(plot,"Converted","l");
	tl->AddEntry(graph,"Unconverted", "p");
	tc->Draw();
	plot->GetXaxis()->SetTitle(plot->GetName());
	plot->GetYaxis()->SetTitle("1/N");
	plot->DrawCopy("e1");
	graph->Draw("P*");
	graph->Print("all");
	tl->Draw();
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
	plot->SetYTitle("#frac{r_{#it{reco}}-r_{#it{truth}}}{r_{#it{truth}}}");
	//plot->SetZTitle("1/N #Delta#it{p}^{T}");
	plot->Draw("colz");
}

void vtxEff(TFile *thisFile){
	gStyle->SetOptStat(0);
	TEfficiency *plot  = (TEfficiency*) thisFile->Get("vtxEff");
	TCanvas* tc = new TCanvas();
	tc->Draw();
	plot->SetTitle(";radius_{#it{truth}} [cm];#frac{N_{reco}}{N_{truth}}");
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

void compareDeta(TFile *thisFile, bool zoom=true){
	gStyle->SetOptStat(0);
	TH1F *sig  = (TH1F*) thisFile->Get("detaS");
	TH1F *back  = (TH1F*) thisFile->Get("detaB");
	TCanvas* tc = new TCanvas();
	tc->SetTicky();
	TLegend *tl =new TLegend(.6,.8,.9,.9);
	tc->Draw();
	sig->SetXTitle("#Delta#eta");
	sig->SetYTitle("1/N");
	if(zoom)sig->GetXaxis()->SetRangeUser(0,.005);
	else{
		sig= (TH1F*) sig->Rebin(100);
		back= (TH1F*) back->Rebin(100);
	}
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
	TH1F *plotTruth  = (TH1F*) thisFile->Get("vtx_truth");
	TH1F *plotReco  = (TH1F*) thisFile->Get("vtx_reco");
	TH1F *plotCorr = (TH1F*) thisFile->Get("vtx_corrected");
	TEfficiency *eff = (TEfficiency*) thisFile->Get("vtxEff");
	plotTruth->Multiply(eff->GetPassedHistogram());
	plotTruth->Divide(eff->GetTotalHistogram());
	plotReco->SetLineColor(kRed);
	plotReco->SetMarkerColor(kRed);
	plotCorr->SetLineColor(kGreen+2);
	plotCorr->SetMarkerColor(kGreen+2);
	plotReco->SetMarkerStyle(kOpenSquare);
	plotTruth->SetMarkerStyle(kFullCircle);
	plotCorr->SetMarkerStyle(kFullTriangleUp);
	TCanvas* tc = new TCanvas();
	tc->Draw();
	tc->SetTicky();
	TLegend *tl = new TLegend(.8,.8,.9,.9);
	tl->AddEntry(plotTruth,"Truth * Vertex Efficiency","p");
	tl->AddEntry(plotReco,"Reco","p");
	tl->AddEntry(plotCorr,"Corrected Reco","p");
	plotTruth->SetYTitle("1/N");
	plotTruth->SetXTitle("r_{vtx} [cm]");
	plotTruth->Draw("e1");
	plotReco->Draw("e1 same");
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
	tmap->SetTitle("Truth Conversion Location XY Plane;x [cm];y [cm]");
	TH2F* rmap = (TH2F*) thisFile->Get("recoMap");
	rmap = (TH2F*) rmap->Rebin2D();
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

void uni_rate(TFile* MBFile){
	gStyle->SetOptStat(0);
	TEfficiency* uni_rate = (TEfficiency*) MBFile->Get("uni_rate");
	TCanvas* tc = new TCanvas();
	tc->SetTicky();
	uni_rate->SetTitle("Uniform pT convertion rate;pT [GeV];#frac{N_{#it{converted}}}{N_{#it{photon}}}");
	uni_rate->Draw();
	TH1F* passed = (TH1F*) uni_rate->GetPassedHistogram();
	TH1F* total= (TH1F*) uni_rate->GetTotalHistogram();
	double e1,e2,i1,i2;
	i1=passed->IntegralAndError(1,passed->GetNbinsX(),e1);
	i2=total->IntegralAndError(1,total->GetNbinsX(),e2);
	double rerror = sqrt(e1/i1*e1/i1+e2/i2*e2/i2);
	cout<<"average uni_rate: "<<i1/i2<<"#pm"<<i1/i2*rerror<<'\n';
}

void real_rate(TFile *MBFile){
	gStyle->SetOptStat(0);
	TCanvas *tc = new TCanvas();
	tc->SetLogy();
	TH1F* rate= (TH1F*) MBFile->Get("rate");
	rate->SetTitle("sPHENIX MinBias Conversion Rate;pT[GeV];#frac{dN_{#it{converted}}}{dN_{MB}}");
	rate->Draw();
}

void conversion_derivative(TFile *MBFile){
	gStyle->SetOptStat(0);
	TCanvas *tc = new TCanvas();
	tc->SetLogy();
	TH1F* derivative= (TH1F*) MBFile->Get("derivative");
	derivative->SetTitle(";pT[GeV];#frac{1}{N_{MB}} #int_{p_{T,min}}^{25} #frac{dN_{converted}}{dp_{T}} dp_{T}");
	derivative->Draw();
}

void rateWithSystematic(TFile* MBFile, TFile* MB2){
	gStyle->SetOptStat(0);
	TCanvas *tc = new TCanvas();
	tc->SetLogy();
	TH1F* rate1= (TH1F*) MBFile->Get("rate");
	TH1F* rate2= (TH1F*) MB2->Get("rate");
	rate2->SetLineColor(kRed);
	rate1->SetTitle("sPHENIX MinBias Conversion Rate;pT[GeV];#frac{dN_{#it{converted}}}{dN_{MB}}");
	rate1->Draw();
	rate2->Draw("same");
}

void plotter(){
	TFile *thisFile = new TFile("effplots2.root","READ");
	TFile *MBFile = new TFile("effhighmatch.root","READ");
	TFile *MBFile2 = new TFile("efflowmatch.root","READ");
	TFile *thisOtherFile = new TFile("maps.root","READ");
	//photon_m(thisFile);
	cout<<"mass done"<<endl;
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
	//vtxR(thisFile);
	//truthPtMix(thisFile);
	compareDeta(thisFile);
	//compareDeta(thisFile,false);
	//compareDeta(thisFile,false);
	//drawMaps(thisOtherFile);
	//uni_rate(MBFile);
	//real_rate(MBFile2);
	//conversion_derivative(MBFile);
	//rateWithSystematic(MBFile,MBFile2);
	//TFile *backFile = new TFile("backplots.root","READ");

}
