void fitNU(TString filename="result.root", Int_t plane=0, Int_t elem=0) {

    TCanvas *cc=new TCanvas("fitNU","",600,600);
    cc->Divide(2,2);

    TFile *file=new TFile(filename);

    Int_t planefirst=4;
    Int_t planelast=11;
    if (plane!=0)
	planefirst=planelast=plane;

    Int_t elemfirst=1;
    Int_t elemlast=24; if (plane==1) elemlast=48;
    if (elem!=0)
	elemfirst=elemlast=elem;
    Bool_t cont=false;

    TF1* fitfunction=new TF1("fitfunction","landau");
    TF1* fitfunction2=new TF1("fitfunction2","pol3");
    Double_t NUparams[12][49][5];

    fitfunction->SetLineWidth(1);
    fitfunction->SetLineColor(kRed);
    fitfunction2->SetLineWidth(1);
    fitfunction2->SetLineColor(kRed);

    TH2F* display=new TH2F("2ddisplay","",10,0,20,10,0,5000);
    TH2F* displayrad=new TH2F("2ddisplayrad","",10,0,0.35,10,0.,5.);

    for (Int_t iplane=planefirst; iplane<=planelast; iplane++) {
       if (plane==9) continue; // plane 9 is FVH, that can not be treated here
       for (Int_t ielem=elemfirst; ielem<=elemlast; ielem++) {
	    printf("Fitting plane %i, elem %i \n",iplane,ielem);

	    for (Int_t i=0; i<5; i++) NUparams[iplane][ielem][i]=0.;

	    TString histo2dname=Form("Histograms/NUcor/plane_%i/Theta vs ADC*cos%02i",iplane,ielem);
	    TH2F* histo2d=file->Get(histo2dname);
	    cc->cd(1);
	    histo2d->Draw("colz");

	    // fit all slices in histogram
	    Int_t NrSlices=histo2d->GetNbinsX();

	    TGraph *NUcourve=new TGraph(NrSlices);
	    NUcourve->SetMarkerStyle(21);
	    TGraph *NUcourvemean=new TGraph(NrSlices);
	    NUcourvemean->SetMarkerStyle(22);
	    NUcourvemean->SetMarkerColor(kBlue);
	    for (Int_t bin=1; bin<=NrSlices; bin++) {
		TH1D* histo1d=histo2d->ProjectionY(Form("bin %i",bin),bin,bin);
		cliphisto(histo1d,0.02);

		cc->cd(2);
		fitfunction->SetParameters(histo1d->GetMaximum(),
					   histo1d->GetBinCenter(histo1d->GetMaximumBin()),
					   5);
		histo1d->Fit(fitfunction,"wq");
		NUcourve->SetPoint(bin-1,
				   histo2d->GetXaxis()->GetBinCenter(bin),
				   fitfunction->GetParameter(1));
		NUcourvemean->SetPoint(bin-1,
				       histo2d->GetXaxis()->GetBinCenter(bin),
				       histo1d->GetMean());

		histo1d->DrawCopy();
		cc->Update();
		//for (Int_t wait=0; wait<2000000; wait++);
		delete histo1d;
	    }

	    cc->cd(3);
	    display->Draw();
	    NUcourve->Draw("Psame");
	    NUcourvemean->Draw("Psame");

	    // now lets fit the NU courve
	    // NUcourvemean->Fit(fitfunction2,"","",2,17);
	    NUcourve->Fit(fitfunction2,"","",2,17);
	    fitfunction2->SetLineWidth(5);
	    fitfunction2->DrawCopy("same");
	    fitfunction2->SetRange(1,19);
	    fitfunction2->SetLineWidth(1);
	    fitfunction2->DrawCopy("same");
	    Double_t scale=fitfunction2->Eval(10.);
	    // if (iplane<=3) scale=1.;
	    NUparams[iplane][ielem][0]=fitfunction2->GetParameter(0)*1/scale*pow(TMath::RadToDeg(),0);
	    NUparams[iplane][ielem][1]=fitfunction2->GetParameter(1)*1/scale*pow(TMath::RadToDeg(),1);
	    NUparams[iplane][ielem][2]=fitfunction2->GetParameter(2)*1/scale*pow(TMath::RadToDeg(),2);
	    NUparams[iplane][ielem][3]=fitfunction2->GetParameter(3)*1/scale*pow(TMath::RadToDeg(),3);
	    NUparams[iplane][ielem][4]=fitfunction2->GetParameter(4)*1/scale*pow(TMath::RadToDeg(),4);

	    cc->cd(4);
	    displayrad->Draw();
	    for (Int_t i=0; i<4; i++) fitfunction2->SetParameter(i,NUparams[iplane][ielem][i]);
	    fitfunction2->SetRange(0,0.35);
	    fitfunction2->DrawCopy("same");

	    cc->Update();
	    if (!cont) TString input=Getline("next element...");
	    if (input.Contains("c")) cont=1;

	    delete histo2d;

	    if (input=="q\n") exit;
	}

	if (input=="q\n") exit;
    }

    printf("Paramaters: \n");
    for (Int_t iplane=planefirst; iplane<=planelast; iplane++) {
	for (Int_t ielem=elemfirst; ielem<=elemlast; ielem++) {
	    printf("%+8e %+8e %+8e %+8e %+8e \n",NUparams[iplane][ielem][0],NUparams[iplane][ielem][1],
		   NUparams[iplane][ielem][2],NUparams[iplane][ielem][3],
		   NUparams[iplane][ielem][4]);
	}
    }
}

void cliphisto(TH1* histo, Double_t threshold, Double_t min=0) {
    Double_t thr=histo->GetMaximum()*threshold;
    if (thr<min)
      thr=min;

    for (Int_t ibin=1; ibin<=histo->GetNbinsX(); ibin++) {
        Double_t newcontent=histo->GetBinContent(ibin)-thr;
        if (newcontent<0) newcontent=0;
        histo->SetBinContent(ibin,newcontent);
    }
}
