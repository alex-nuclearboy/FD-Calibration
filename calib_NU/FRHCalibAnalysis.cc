#include <iostream>
#include <fstream>
#include <TString.h>
#include "Wasa.hh"
#include "AdminEntry.hh"
#include "CConst.hh"
#include "SorterOption.hh"
#include "CDataManager.hh"
#include "CParameterManager.hh"
#include "time.h"
#include "CHistoManager.hh"
#include "CLog.hh"
#include "string"
#include "FRHCalibAnalysis.hh"
#include "WKineSimple.hh"

//FWC1,2 and FRH1 are in ADC channels axis ****similar to "_last" for 3He****** otherwise is not possible to calibrate
//because have contradiction between MC spectra (in energy unit) and data spectra

//************ I take trigger N21 for non-uniformity corection *************//

static SorterOption so("usecalib","-usecalib","",false);

ClassImp(FRHCalibAnalysis);

FRHCalibAnalysis::FRHCalibAnalysis() {}

FRHCalibAnalysis::FRHCalibAnalysis(const char * name):CAnalysisModule(name) {

    fHeader = dynamic_cast<REventHeader*>(gDataManager->GetDataObject("REventHeader","Header"));

// FDN, FDC, CDN, CDC, PS threshold
// FDC tmin/tmax
// FDCCDN t0, maxtdiff
// FDCPS t0, maxtdiff
// PSSE thetamax, PSSE phi max

   fKineSimple=dynamic_cast<WKineSimple*>(gDataManager->GetAnalysisModule("WKineSimple","notrackfinder"));
   fKineSimple->SetParameters(	0.02,0.02,0.020,0.020,0.0005,
				1930,2070,50,
				5,20,
				20,22,
				30,30);

   fHitBankFRH = dynamic_cast<WHitBank*>
       (gDataManager->GetDataObject("WHitBank","FRHHitBankNG"));

   fMCmode=gWasa->IsAnalysisMode(Wasa::kMC);
   if (so.GetBoolValue())
       fMCmode=1;

   SetupRawSpectra(name);
}

FRHCalibAnalysis::~FRHCalibAnalysis() {}

void FRHCalibAnalysis::SetupRawSpectra(const char * path) {

    TString hpath;

    hpath="NUcor";
    Int_t NBinsTheta=16;
    Double_t thetamin=2.;
    Double_t thetamax=18.;
    Int_t NBinsADC=75;
    Double_t ADCmin=1;
    Double_t ADCmax=5000;

    for (Int_t plane=1; plane<=11; plane++) {
	if (plane==9) continue;
	TString hpath2=hpath+Form("/plane_%i",plane);
	Int_t maxelem=24; if (plane==1) maxelem=48;
	for (Int_t elem=1; elem<=maxelem; elem++) {

	    // NU-uncorrected theta versus ADC*cos(theta)
	    hThetavsADCcos[plane][elem]=new TH2F(Form("Theta vs ADC*cos%02i",elem),"",
						 NBinsTheta,thetamin,thetamax,
						 NBinsADC,ADCmin,ADCmax);
	    gHistoManager->Add(hThetavsADCcos[plane][elem],hpath2);

	    // NU-corrected theta versus ADC*cos(theta)
	    hThetavsADCcos_cor[plane][elem]=new TH2F(Form("Theta vs NU*ADC*cos_%02i",elem),"",
						 NBinsTheta,thetamin,thetamax,
						 NBinsADC,ADCmin,ADCmax);
	    gHistoManager->Add(hThetavsADCcos_cor[plane][elem],hpath2);
	}
    }

    hpath="CalibPlots";
    Int_t NrBinsFTH=75;
    Double_t xminFTH=0;
    Double_t xmaxFTH=10000; if (fMCmode) xmaxFTH=0.01;
    Int_t NrBinsFRH=75;//original value
    //Int_t NrBinsFRH=225;//for p Dec 2007 only
    Double_t xminFRH=0;
    Double_t xmaxFRH=10000; if (fMCmode) xmaxFRH=0.2;
    Int_t NrBinsTheta=24;
    Double_t xmintheta=3;
    Double_t xmaxtheta=15;
    Int_t NrBinsFWC=75;
    Double_t xminFWC=0;
    Double_t xmaxFWC=5000; if (fMCmode) xmaxFWC=0.007;

    for (Int_t elem=0; elem<=48; elem++) {
	for(Int_t thetabin=0; thetabin<=5; thetabin++) {
	    hEdEFRH1vsFTH1[elem][thetabin]=new TH2F(Form("EdE FRH1 vs FTH1_%02i",elem),"",
						    NrBinsFRH,xminFRH,xmaxFRH,
						    NrBinsFTH,xminFTH,xmaxFTH);
	    gHistoManager->Add(hEdEFRH1vsFTH1[elem][thetabin],hpath+Form("/FRH/thetabin%i",thetabin));

	    if (elem<=24) {
		// FRH1 vs FRH2
		hEdEFRH1vsFRH2[elem][thetabin]=new TH2F(Form("EdE FRH1 vs FRH2_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFRH,xminFRH,xmaxFRH);
		gHistoManager->Add(hEdEFRH1vsFRH2[elem][thetabin],hpath+Form("/FRH/thetabin%i",thetabin));
		// FRH2 vs FRH3
		hEdEFRH2vsFRH3[elem][thetabin]=new TH2F(Form("EdE FRH2 vs FRH3_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFRH,xminFRH,xmaxFRH);
		gHistoManager->Add(hEdEFRH2vsFRH3[elem][thetabin],hpath+Form("/FRH/thetabin%i",thetabin));
		// FRH3 vs FRH4
		hEdEFRH3vsFRH4[elem][thetabin]=new TH2F(Form("EdE FRH3 vs FRH4_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFRH,xminFRH,xmaxFRH);
		gHistoManager->Add(hEdEFRH3vsFRH4[elem][thetabin],hpath+Form("/FRH/thetabin%i",thetabin));
		// FRH4 vs FRH5
		hEdEFRH4vsFRH5[elem][thetabin]=new TH2F(Form("EdE FRH4 vs FRH5_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFRH,xminFRH,xmaxFRH);
		gHistoManager->Add(hEdEFRH4vsFRH5[elem][thetabin],hpath+Form("/FRH/thetabin%i",thetabin));

		// FRH1 (calibrated) vs FTH2
		hEdEFRH1vsFTH2[elem][thetabin]=new TH2F(Form("EdE FRH1 vs FTH2_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFTH,xminFTH,xmaxFTH);
		gHistoManager->Add(hEdEFRH1vsFTH2[elem][thetabin],hpath+Form("/FTH2/thetabin%i",thetabin));
		// FRH1 (calibrated) vs FTH3
		hEdEFRH1vsFTH3[elem][thetabin]=new TH2F(Form("EdE FRH1 vs FTH3_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFTH,xminFTH,xmaxFTH);
		gHistoManager->Add(hEdEFRH1vsFTH3[elem][thetabin],hpath+Form("/FTH3/thetabin%i",thetabin));

		// FRH1 (calibrated) vs FWC1
		hEdEFRH1vsFWC1[elem][thetabin]=new TH2F(Form("EdE FRH1 vs FWC1_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFWC,xminFWC,xmaxFWC);
		gHistoManager->Add(hEdEFRH1vsFWC1[elem][thetabin],hpath+Form("/FWC1/thetabin%i",thetabin));
		// FRH1 (calibrated) vs FWC2
		hEdEFRH1vsFWC2[elem][thetabin]=new TH2F(Form("EdE FRH1 vs FWC2_%02i",elem),"",
							NrBinsFRH,xminFRH,xmaxFRH,
							NrBinsFWC,xminFWC,xmaxFWC);
		gHistoManager->Add(hEdEFRH1vsFWC2[elem][thetabin],hpath+Form("/FWC2/thetabin%i",thetabin));

		// FTH1 (calibrated) vs FTH2
		hEdEFTH1vsFTH2[elem][thetabin]=new TH2F(Form("EdE FTH1 vs FTH2_%02i",elem),"",
							NrBinsFTH,xminFTH,xmaxFTH,
							NrBinsFTH,xminFTH,xmaxFTH);
		gHistoManager->Add(hEdEFTH1vsFTH2[elem][thetabin],hpath+Form("/FTH2/thetabin%i",thetabin));
		// FTH1 (calibrated) vs FTH3
		hEdEFTH1vsFTH3[elem][thetabin]=new TH2F(Form("EdE FTH1 vs FTH3_%02i",elem),"",
							NrBinsFTH,xminFTH,xmaxFTH,
							NrBinsFTH,xminFTH,xmaxFTH);
		gHistoManager->Add(hEdEFTH1vsFTH3[elem][thetabin],hpath+Form("/FTH3/thetabin%i",thetabin));
		// FTH1 (calibrated) vs FWC1
		hEdEFTH1vsFWC1[elem][thetabin]=new TH2F(Form("EdE FTH1 vs FWC1_%02i",elem),"",
							NrBinsFTH,xminFTH,xmaxFTH,
							NrBinsFWC,xminFWC,xmaxFWC);
		gHistoManager->Add(hEdEFTH1vsFWC1[elem][thetabin],hpath+Form("/FWC1/thetabin%i",thetabin));
		// FTH1 (calibrated) vs FWC2
		hEdEFTH1vsFWC2[elem][thetabin]=new TH2F(Form("EdE FTH1 vs FWC2_%02i",elem),"",
							NrBinsFTH,xminFTH,xmaxFTH,
							NrBinsFWC,xminFWC,xmaxFWC);
		gHistoManager->Add(hEdEFTH1vsFWC2[elem][thetabin],hpath+Form("/FWC2/thetabin%i",thetabin));
	    }
	}

	if (elem<=24) {

	    hThetavsFRH1[elem]=new TH2F(Form("Theta vs FRH1_elem_%2i",elem),"",
					NrBinsTheta,xmintheta,xmaxtheta,NrBinsFRH,xminFRH,xmaxFRH);
	    gHistoManager->Add(hThetavsFRH1[elem],hpath);
	    hThetavsFRH1_eta[elem]=new TH2F(Form("Theta vs FRH1_elem_%2i, etaonly",elem),"",
					NrBinsTheta,xmintheta,xmaxtheta,NrBinsFRH,xminFRH,xmaxFRH);
	    gHistoManager->Add(hThetavsFRH1_eta[elem],hpath);

	}
    }

    hpath="general";
    hStatistics=new TH1F("Statistics","",20,-0.5,19.5);
    gHistoManager->Add(hStatistics,hpath);

    hPhiNU=new TH1F("Phi selected tracks NU","",24,-180.,180.);
    hPhiNU->SetMinimum(0);
    gHistoManager->Add(hPhiNU,hpath);
    hThetaNU=new TH1F("Theta selected tracks NU:","",20,0.,20.);
    hThetaNU->SetMinimum(0);
    gHistoManager->Add(hThetaNU,hpath);
    hPlanevsElem=new TH2F("plane vs elem","",12,0.5,12.5,48,0.5,48.5);
    gHistoManager->Add(hPlanevsElem,hpath);
}

void FRHCalibAnalysis::FillEdEplots() {

  //trigger N29 (2 charged in FD and 2 neutral in CD) || trigger N10 (1 charged in FD)
  //for calibration I use reaction pd -> pd pi0

  //if (fHeader->TriggerNumSet(14) || fHeader->TriggerNumSet(15))
  //gWasa->SaveEvent();

  if (!fMCmode && !fHeader->TriggerNumSet(29))
	return;
    hStatistics->Fill(1);

   if (!fMCmode && fKineSimple->IOKFDC()!=2)
	return;  // exactly two protons
    hStatistics->Fill(2);
    if (!fMCmode && fKineSimple->IOKCDN()<2)
	return;  // select pp->npi0 events, basically pp2pi0

    hStatistics->Fill(3);
    //gWasa->SaveEvent();

    for (Int_t trackid=0; trackid<fKineSimple->IOKFDC(); trackid++) {
	// loop over all charged tracks;
	WKineSimpleTrack* track=fKineSimple->FDC(trackid);
	hStatistics->Fill(5);

	if (! (track->IsInTrack(1) && track->IsInTrack(4)))
	    continue;  // for the time being we need FTH1 and FRH1
	hStatistics->Fill(6);

	if (track->GetCluster(1)->NHits()!=1 || track->GetCluster(4)->NHits()!=1)
	    continue;
	hStatistics->Fill(7);

	Int_t ElemNrFWC1=0; Double_t EdepFWC1=0;
	Int_t ElemNrFWC2=0; Double_t EdepFWC2=0;
	Int_t ElemNrFTH1=0; Double_t EdepFTH1=0; Double_t EdepFTH1e=0;
	Int_t ElemNrFTH2=0; Double_t EdepFTH2=0;
	Int_t ElemNrFTH3=0; Double_t EdepFTH3=0;
	Int_t ElemNrFRH1=0; Double_t EdepFRH1=0; Double_t EdepFRH1e=0;
	Int_t ElemNrFRH2=0; Double_t EdepFRH2=0; Double_t EdepFRH2e=0;
	Int_t ElemNrFRH3=0; Double_t EdepFRH3=0; Double_t EdepFRH3e=0;
	Int_t ElemNrFRH4=0; Double_t EdepFRH4=0; Double_t EdepFRH4e=0;
	Int_t ElemNrFRH5=0; Double_t EdepFRH5=0; Double_t EdepFRH5e=0;

	WHitScint* hit_FWC1=0;
	if (track->IsInTrack(10)) {
	    hit_FWC1=dynamic_cast<WHitScint*> (track->GetCluster(10)->GetHit(0));
	    ElemNrFWC1=hit_FWC1->ID().GetEl();
	    EdepFWC1=hit_FWC1->GetE();
	    if (!fMCmode) EdepFWC1=hit_FWC1->GetRAW_ADC()/hit_FWC1->GetRAW_NUcor();
	}
	WHitScint* hit_FWC2=0;
	if (track->IsInTrack(11)) {
	    hit_FWC2=dynamic_cast<WHitScint*> (track->GetCluster(11)->GetHit(0));
	    ElemNrFWC2=hit_FWC2->ID().GetEl();
	    EdepFWC2=hit_FWC2->GetE();
	    if (!fMCmode) EdepFWC2=hit_FWC2->GetRAW_ADC()/hit_FWC2->GetRAW_NUcor();
	}
	WHitScint* hit_FTH1=0;
	if (track->IsInTrack(1)) {
	    hit_FTH1=dynamic_cast<WHitScint*> (track->GetCluster(1)->GetHit(0));
	    ElemNrFTH1=hit_FTH1->ID().GetEl();
	    EdepFTH1=hit_FTH1->GetE();
	    EdepFTH1e=EdepFTH1;
	    if (!fMCmode) EdepFTH1=hit_FTH1->GetRAW_ADC()/hit_FTH1->GetRAW_NUcor();
	}
	WHitScint* hit_FTH2=0;
	if (track->IsInTrack(2)) {
	    hit_FTH2=dynamic_cast<WHitScint*> (track->GetCluster(2)->GetHit(0));
	    ElemNrFTH2=hit_FTH2->ID().GetEl();
	    EdepFTH2=hit_FTH2->GetE();
	    if (!fMCmode) EdepFTH2=hit_FTH2->GetRAW_ADC()/hit_FTH2->GetRAW_NUcor();
	}
	WHitScint* hit_FTH3=0;
	if (track->IsInTrack(3)) {
	    hit_FTH3=dynamic_cast<WHitScint*> (track->GetCluster(3)->GetHit(0));
	    ElemNrFTH3=hit_FTH3->ID().GetEl();
	    EdepFTH3=hit_FTH3->GetE();
	    if (!fMCmode) EdepFTH3=hit_FTH3->GetRAW_ADC()/hit_FTH3->GetRAW_NUcor();
	}
	WHitScint* hit_FRH1=0;
	if (track->IsInTrack(4)) {
	    hit_FRH1=dynamic_cast<WHitScint*> (track->GetCluster(4)->GetHit(0));
	    ElemNrFRH1=hit_FRH1->ID().GetEl();
	    EdepFRH1=hit_FRH1->GetE();
	    EdepFRH1e=EdepFRH1;
	    if (!fMCmode) EdepFRH1=hit_FRH1->GetRAW_ADC()/hit_FRH1->GetRAW_NUcor();
	    //cout<<"FRH1 "<<EdepFRH1<<" "<<EdepFRH1e<<" "<<hit_FRH1->GetRAW_ADC()<<" "<<hit_FRH1->GetRAW_NUcor()<<endl;

	}
	WHitScint* hit_FRH2=0;
	if (track->IsInTrack(5)) {
	    hit_FRH2=dynamic_cast<WHitScint*> (track->GetCluster(5)->GetHit(0));
	    ElemNrFRH2=hit_FRH2->ID().GetEl();
	    EdepFRH2=hit_FRH2->GetE();
	    EdepFRH2e=EdepFRH2;
	    if (!fMCmode) EdepFRH2=hit_FRH2->GetRAW_ADC()/hit_FRH2->GetRAW_NUcor();
	    //cout<<"FRH2 "<<EdepFRH2<<" "<<EdepFRH2e<<" "<<hit_FRH2->GetRAW_ADC()<<" "<<hit_FRH2->GetRAW_NUcor()<<endl;
	}
	WHitScint* hit_FRH3=0;
	if (track->IsInTrack(6)) {
	    hit_FRH3=dynamic_cast<WHitScint*> (track->GetCluster(6)->GetHit(0));
	    ElemNrFRH3=hit_FRH3->ID().GetEl();
	    EdepFRH3=hit_FRH3->GetE();
	    EdepFRH3e=EdepFRH3;
	    if (!fMCmode) EdepFRH3=hit_FRH3->GetRAW_ADC()/hit_FRH3->GetRAW_NUcor();
	    //cout<<"FRH3 "<<EdepFRH3<<" "<<EdepFRH3e<<" "<<hit_FRH3->GetRAW_ADC()<<" "<<hit_FRH3->GetRAW_NUcor()<<endl;
	}
	WHitScint* hit_FRH4=0;
	if (track->IsInTrack(7)) {
	    hit_FRH4=dynamic_cast<WHitScint*> (track->GetCluster(7)->GetHit(0));
	    ElemNrFRH4=hit_FRH4->ID().GetEl();
	    EdepFRH4=hit_FRH4->GetE();
	    EdepFRH4e=EdepFRH4;
	    if (!fMCmode) EdepFRH4=hit_FRH4->GetRAW_ADC()/hit_FRH4->GetRAW_NUcor();
	    //cout<<"FRH4 "<<EdepFRH4<<" "<<EdepFRH4e<<" "<<hit_FRH4->GetRAW_ADC()<<" "<<hit_FRH4->GetRAW_NUcor()<<endl;
	}
	WHitScint* hit_FRH5=0;
	if (track->IsInTrack(8)) {
	    hit_FRH5=dynamic_cast<WHitScint*> (track->GetCluster(8)->GetHit(0));
	    ElemNrFRH5=hit_FRH5->ID().GetEl();
	    EdepFRH5=hit_FRH5->GetE();
	    EdepFRH5e=EdepFRH5;
	    if (!fMCmode) EdepFRH5=hit_FRH5->GetRAW_ADC()/hit_FRH5->GetRAW_NUcor();
	    //cout<<"FRH5 "<<EdepFRH5<<" "<<EdepFRH5e<<" "<<hit_FRH5->GetRAW_ADC()<<" "<<hit_FRH5->GetRAW_NUcor()<<endl;
	}

	if (int((hit_FTH1->ID().GetEl()-1)/2)+1 != hit_FRH1->ID().GetEl()) 
	    continue;  // straight track in FTH1 and FRH1
	hStatistics->Fill(8);

	Double_t theta=track->Theta();
	Int_t thetabin=int((theta-0.045)/(0.314-0.045)*5)+1; 
	if (thetabin<1) thetabin=1; if (thetabin>5) thetabin=5;

	for (Int_t i=1; i<=2; i++) {
	    //FRH1 vs FTH1
	    hEdEFRH1vsFTH1[i==1?0:ElemNrFTH1][0]->Fill(EdepFRH1,EdepFTH1);
	    hEdEFRH1vsFTH1[i==1?0:ElemNrFTH1][thetabin]->Fill(EdepFRH1,EdepFTH1);
	    // FRH1 (NOT!!calibrated) vs FTH2
	    hEdEFRH1vsFTH2[i==1?0:ElemNrFTH2][0]->Fill(EdepFRH1,EdepFTH2);
	    hEdEFRH1vsFTH2[i==1?0:ElemNrFTH2][thetabin]->Fill(EdepFRH1,EdepFTH2);
	    // FRH1 (NOT!!calibrated) vs FTH3
	    hEdEFRH1vsFTH3[i==1?0:ElemNrFTH3][0]->Fill(EdepFRH1,EdepFTH3);
	    hEdEFRH1vsFTH3[i==1?0:ElemNrFTH3][thetabin]->Fill(EdepFRH1,EdepFTH3);
	    // FTH1 (NOT!!calibrated) vs FTH2
	    hEdEFTH1vsFTH2[i==1?0:ElemNrFTH2][0]->Fill(EdepFTH1,EdepFTH2);
	    hEdEFTH1vsFTH2[i==1?0:ElemNrFTH2][thetabin]->Fill(EdepFTH1,EdepFTH2);
	    // FTH1 (NOT!!calibrated) vs FTH3
	    hEdEFTH1vsFTH3[i==1?0:ElemNrFTH3][0]->Fill(EdepFTH1,EdepFTH3);
	    hEdEFTH1vsFTH3[i==1?0:ElemNrFTH3][thetabin]->Fill(EdepFTH1,EdepFTH3);

	    // FRH1/FTH1 (NOT!! calibrated) vs FWC1/FWC2
	    hEdEFRH1vsFWC1[i==1?0:ElemNrFWC1][0]->Fill(EdepFRH1,EdepFWC1);
	    hEdEFRH1vsFWC1[i==1?0:ElemNrFWC1][thetabin]->Fill(EdepFRH1,EdepFWC1);
	    hEdEFRH1vsFWC2[i==1?0:ElemNrFWC2][0]->Fill(EdepFRH1,EdepFWC2);
	    hEdEFRH1vsFWC2[i==1?0:ElemNrFWC2][thetabin]->Fill(EdepFRH1,EdepFWC2);
	    hEdEFTH1vsFWC1[i==1?0:ElemNrFWC1][0]->Fill(EdepFTH1,EdepFWC1);
	    hEdEFTH1vsFWC1[i==1?0:ElemNrFWC1][thetabin]->Fill(EdepFTH1,EdepFWC1);
	    hEdEFTH1vsFWC2[i==1?0:ElemNrFWC2][0]->Fill(EdepFTH1,EdepFWC2);
	    hEdEFTH1vsFWC2[i==1?0:ElemNrFWC2][thetabin]->Fill(EdepFTH1,EdepFWC2);

	    // FRHx vs FRHy
	    hEdEFRH1vsFRH2[i==1?0:ElemNrFRH1][0]->Fill(EdepFRH1,EdepFRH2);
	    hEdEFRH1vsFRH2[i==1?0:ElemNrFRH1][thetabin]->Fill(EdepFRH1,EdepFRH2);
	    hEdEFRH2vsFRH3[i==1?0:ElemNrFRH2][0]->Fill(EdepFRH2,EdepFRH3);
	    hEdEFRH2vsFRH3[i==1?0:ElemNrFRH2][thetabin]->Fill(EdepFRH2,EdepFRH3);
	    hEdEFRH3vsFRH4[i==1?0:ElemNrFRH3][0]->Fill(EdepFRH3,EdepFRH4);
	    hEdEFRH3vsFRH4[i==1?0:ElemNrFRH3][thetabin]->Fill(EdepFRH3,EdepFRH4);
	    hEdEFRH4vsFRH5[i==1?0:ElemNrFRH4][0]->Fill(EdepFRH4,EdepFRH5);
	    hEdEFRH4vsFRH5[i==1?0:ElemNrFRH4][thetabin]->Fill(EdepFRH4,EdepFRH5);
	}
    }
}

void FRHCalibAnalysis::FillNUplots() {
    if (!fHeader->TriggerNumSet(21))
    return;   // pp elastic scattering trigger
    hStatistics->Fill(11);

    if (fKineSimple->IOKFDC()!=1)
    return;   // exactly one charged track was reconstructed
    hStatistics->Fill(12);
    WKineSimpleTrack* track=fKineSimple->FDC(0);

    //cout<<"IsInTrack() "<<track->IsInTrack(4)<<" "<<track->IsInTrack(5)<<" "<<track->IsInTrack(6)<<" "<<track->IsInTrack(7)<<" "<<track->IsInTrack(8)<<endl;

    if (! (track->IsInTrack(4) && track->IsInTrack(5) && track->IsInTrack(6))) // && track->IsInTrack(7) && track->IsInTrack(8)))
    return;   // track has to go through all layers (min. ion.)

    hStatistics->Fill(13);

    hPhiNU->Fill(track->PhiD());
    hThetaNU->Fill(track->ThetaD());

    for (Int_t iplane=1; iplane<=11; iplane++) {
	if (iplane==9) continue;
	if (!track->IsInTrack(iplane)) continue;
	if (track->GetCluster(iplane)->NHits()!=1) continue;
	WHitScint* hit=dynamic_cast<WHitScint*>(track->GetCluster(iplane)->GetHit(0));
	if (!hit) continue;  // should not happen...

	Double_t Edep=track->Edep(iplane);
	Double_t ADC=hit->GetRAW_ADC();
	Double_t NU=hit->GetRAW_NUcor();
	Double_t theta=track->ThetaD();
	Int_t ElemNr=hit->ID().GetEl();

	hPlanevsElem->Fill(iplane,ElemNr);
	hThetavsADCcos[iplane][ElemNr]->Fill(theta,ADC*cos(theta*TMath::DegToRad()));
	hThetavsADCcos_cor[iplane][ElemNr]->Fill(theta,ADC*cos(theta*TMath::DegToRad()) / NU);
    }
}

void FRHCalibAnalysis::ProcessEvent() {
    if (fProcessed) return;
    fProcessed = true;

    fKineSimple->ProcessEvent();

    hStatistics->Fill(0);
    FillNUplots();
    FillEdEplots();
}

void FRHCalibAnalysis::Clear(Option_t *option) {
    fProcessed = kFALSE;
}

void FRHCalibAnalysis::Print(Option_t *option) {
    return;
}

void FRHCalibAnalysis::UserCommand(CCommand * command) {
    return;
}
