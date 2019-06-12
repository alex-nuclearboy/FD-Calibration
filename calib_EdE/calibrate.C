// Calibration sript for WASA FD calibration
// for pd reactions.

#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>
#include <TGStatusBar.h>

#include <TFile.h>
#include <TH2.h>

class MyMainFrame;

MyMainFrame* gWindow = 0;

class MyMainFrame {
  RQ_OBJECT("MyMainFrame")

private:
    TGMainFrame            *fMain;
    TGCompositeFrame       *f2dFrame;
    TRootEmbeddedCanvas    *fEcanvas;
    TGHorizontalFrame      *fInputFrame1;
    TGHorizontalFrame      *fInputFrame2;

    //widgets
    TGNumberEntry          *fwElemNrFTH;
    TGNumberEntry          *fwCalib_c0[11];
    TGNumberEntry          *fwCalib_c0exp[11];
    TGNumberEntry          *fwCalib_c1[11];
    TGNumberEntry          *fwCalib_c1exp[11];
    TGComboBox             *fwCalib_fct[11];
    TGComboBox             *fwCalibSet;
    TGComboBox             *fwThetabin;
    TGCheckButton          *fwMCDisplayFlag;


    TString fRDfilename, fMCfilename, fMCfilenameeta;
    TFile *fRDfile, *fMCfile, *fMCfileeta;
    Double_t fCalib_c0[11][49];   // linear parameter
    Double_t fCalib_c1[11][49];   // nonlinear parameter
    Int_t    fCalib_fct[11];      // calibfct

public:
    MyMainFrame(const TGWindow *p, TString RDfilename="");
    virtual ~MyMainFrame();

    void DoExit();
    void DoReadDatabase();
    void DoReadCalib();
    void DoWriteCalib();
    void DoDraw();
    void DoChangeCalibNrs();
    void DoNextDisplay();

    void PlotSlice(TH2D* histom2d_c, TH2D* histo2d_rd,
		   Int_t protype, Double_t slicelow, Double_t sliceup, Double_t axislow, Double_t axisup,
		   Double_t scalerd);
    void plot(Int_t selector, Int_t elemnrFTH, Int_t thetabin);
    void RecalibrateAxis(TAxis* axis,Double_t c0,Double_t c1);
    void ReadDatabaseFile(TString cfilename);
    void WriteCalibFile(TString cfilename);
    void ReadCalibFile(TString cfilename);
    //Double_t he3missingmass(Double_t EdepFRH1, Double_t Theta)

    TGMainFrame* GetMain() { return fMain; }

};


MyMainFrame::MyMainFrame(const TGWindow *p, TString RDfilename) 
{

  fRDfilename="calib_EdE_data.root"; // May-June 2014 pd->pd pi0 150 Runs

  if (RDfilename!="") fRDfilename=RDfilename;
  fMCfilename="calib_EdE_MC_pd-pdpi0.root"; // pd -> pd pi0 - MC for pd calibration
  fRDfile=new TFile(fRDfilename);
  fMCfile=new TFile(fMCfilename);

    // generate GUI
    fMain = new TGMainFrame(p,1000,1000,kHorizontalFrame);
    fMain->Connect("CloseWindow()", "MyMainFrame", this, "DoExit()");

    f2dFrame = new TGCompositeFrame(fMain,950,800,kVerticalFrame);
    fMain->AddFrame(f2dFrame, 0);

    // histogram canvas
    TCanvas *mycanvas;
    fEcanvas = new TRootEmbeddedCanvas("Ecanvas",f2dFrame,900,600);
    Int_t wid = fEcanvas->GetCanvasWindowId();
    mycanvas=new TCanvas("MyCanvas",10,10,wid);
    fEcanvas->AdoptCanvas(mycanvas);
    f2dFrame->AddFrame(fEcanvas,  new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,
						    10,10,10,1));
    mycanvas->SetCrosshair();

    // control buttons, collected in fInoutFrame1
    fInputFrame1 = new TGHorizontalFrame(f2dFrame,100,50);
    // Draw button
    TGTextButton *draw = new TGTextButton(fInputFrame1,"&Draw");
    fInputFrame1->AddFrame(draw,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
    draw->Connect("Clicked()","MyMainFrame",this,"DoDraw()");

    // Read Config button
    TGTextButton *readd = new TGTextButton(fInputFrame1,"Read &Database");
    fInputFrame1->AddFrame(readd,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
    readd->Connect("Clicked()","MyMainFrame",this,"DoReadDatabase()");

    TGTextButton *readf = new TGTextButton(fInputFrame1,"&Read Calibfile");
    fInputFrame1->AddFrame(readf,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
    readf->Connect("Clicked()","MyMainFrame",this,"DoReadCalib()");

    TGTextButton *write = new TGTextButton(fInputFrame1,"&Write Calibfile");
    fInputFrame1->AddFrame(write,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
    write->Connect("Clicked()","MyMainFrame",this,"DoWriteCalib()");

    // quit button
    TGTextButton *quit = new TGTextButton(fInputFrame1,"&Quit");
    fInputFrame1->AddFrame(quit,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
    quit->Connect("Clicked()","MyMainFrame",this,"DoExit()");    
    f2dFrame->AddFrame(fInputFrame1,new  TGLayoutHints(kLHintsExpandX, 10, 10, 0, 1));

    // control widgets, collected in fInputFrame2, split into several columns, "vframes"
    fInputFrame2 = new TGHorizontalFrame(f2dFrame,100,50);

    // first vframe:
    TGVerticalFrame* vframe = new TGVerticalFrame(fInputFrame2,10,10);
    TGLabel* label;

    // select histogram and element nr
    label = new TGLabel(vframe,"Calibration Set");
    vframe->AddFrame(label,new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
    hframe= new TGHorizontalFrame(vframe,10,10);
    fwCalibSet = new TGComboBox(hframe,10);
    char tmp[20];
    fwCalibSet->AddEntry("FRH1 vs FTH1",1);
    fwCalibSet->AddEntry("FTH2",2);
    fwCalibSet->AddEntry("FTH3",3);

    fwCalibSet->AddEntry("FRH1 vs FRH2",4);
    fwCalibSet->AddEntry("FRH2 vs FRH13",5);
    fwCalibSet->AddEntry("FRH3 vs FRH24",6);
    fwCalibSet->AddEntry("FRH4 vs FRH35",7);
    fwCalibSet->AddEntry("FRH5 vs FRH4",8);
    fwCalibSet->AddEntry("FWC1",9);
    fwCalibSet->AddEntry("FWC2",10);

    fwCalibSet->Resize(150,20);
    fwCalibSet->Select(1,kFALSE);
    fwCalibSet->Connect("Selected(Int_t)","MyMainFrame",this,"DoDraw()");
    hframe->AddFrame(fwCalibSet, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
    TGTextButton *next = new TGTextButton(hframe,"&next");
    next->Connect("Clicked()","MyMainFrame",this,"DoNextDisplay()");
    hframe->AddFrame(next,new TGLayoutHints(kLHintsTop | kLHintsLeft,3,3,3,3));
    vframe->AddFrame(hframe,0);
    TGTextButton *prev = new TGTextButton(hframe,"&prev");
    prev->Connect("Clicked()","MyMainFrame",this,"DoPrevDisplay()");
    hframe->AddFrame(prev,new TGLayoutHints(kLHintsTop | kLHintsLeft,3,3,3,3));

    // activate MC display in 2d plot
    fwMCDisplayFlag = new TGCheckButton(vframe, "Display MC",1);
    fwMCDisplayFlag->SetState(kButtonDown);
    fwMCDisplayFlag->Connect("Clicked()","MyMainFrame",this,"DoDraw()");
    vframe->AddFrame(fwMCDisplayFlag);

    // select element Nr
    label = new TGLabel(vframe,"FTH Element Nr");
    vframe->AddFrame(label, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
    fwElemNrFTH = new TGNumberEntry(vframe, 0., 6, 0,
				   TGNumberFormat::kNESInteger, //style
				   TGNumberFormat::kNEANonNegative, //input value filter
				   TGNumberFormat::kNELLimitMinMax, //specify limits
				   0.,48.); //limit values
    fwElemNrFTH->SetNumber(1);
    fwElemNrFTH->Connect("ValueSet(Long_t)","MyMainFrame",this,"DoDraw()");
    vframe->AddFrame(fwElemNrFTH, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));

    fInputFrame2->AddFrame(vframe, new TGLayoutHints(kLHintsExpandY,15,15,5,5));

    // select Theta Range
    label = new TGLabel(vframe,"Theta Range");
    vframe->AddFrame(label,new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
    fwThetabin = new TGComboBox(fInputFrame2,10);
    fwThetabin->AddEntry("full theta range",1);
    fwThetabin->AddEntry("bin 1:  2.5 -  5.5",2);
    fwThetabin->AddEntry("bin 2:  5.5 -  8.5",3);
    fwThetabin->AddEntry("bin 3:  8.5 - 11.5",4);
    fwThetabin->AddEntry("bin 4: 11.5 - 14.5",5);
    fwThetabin->AddEntry("bin 5: 14.5 - 17.5",6);
    fwThetabin->Resize(150,20);
    fwThetabin->Select(1,kFALSE);
    fwThetabin->Connect("Selected(Int_t)","MyMainFrame",this,"DoDraw()"); 
    vframe->AddFrame(fwThetabin, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));

     // c0, FTH1-FRH5
    // second vframe, co calib constants for all planes
     // c0, FTH1-FRH5

    vframe = new TGVerticalFrame(fInputFrame2,10,10);
    label = new TGLabel(vframe,"c0 FTH1-FRH5,FWC12");
    vframe->AddFrame(label, 0);

    for (Int_t iplane=1; iplane<=10; iplane++) {
	hframe= new TGHorizontalFrame(vframe,10,10);
	fwCalib_c0[iplane]= new TGNumberEntry(hframe,0.,12,0,
					      TGNumberFormat::kNESRealTwo, //style
					      TGNumberFormat::kNEANonNegative, //input value filter
					      TGNumberFormat::kNELNoLimits); //specify limits
	hframe->AddFrame(fwCalib_c0[iplane], new TGLayoutHints(kLHintsLeft,0,0,0,0));
	label = new TGLabel(hframe,"*10^");
	hframe->AddFrame(label,0);
	fwCalib_c0exp[iplane]= new  TGNumberEntry(hframe, 0., 2, 0,
						  TGNumberFormat::kNESInteger, //style
						  TGNumberFormat::kNEAAnyNumber, //input value filter
						  TGNumberFormat::kNELNoLimits, //specify limits
						  -13,0 .); //limit values
	hframe->AddFrame(fwCalib_c0exp[iplane], new TGLayoutHints(kLHintsLeft,0,0,0,0));
	fwCalib_c0[iplane]->Connect("ValueSet(Long_t)","MyMainFrame",this,"DoChangeCalibNrs()");
	fwCalib_c0exp[iplane]->Connect("ValueSet(Long_t)","MyMainFrame",this,"DoChangeCalibNrs()");

        vframe->AddFrame(hframe,0);
    }
    fInputFrame2->AddFrame(vframe, new TGLayoutHints(kLHintsExpandY,15,15,5,5));

    // c1, FTH1-FRH5
    vframe = new TGVerticalFrame(fInputFrame2,10,10);
    label = new TGLabel(vframe,"c1 FTH1-FRH5,FWC12");
    vframe->AddFrame(label, 0);
    for (Int_t iplane=1; iplane<=10; iplane++) {
	hframe = new TGHorizontalFrame(vframe,10,10);
	fwCalib_c1[iplane]= new TGNumberEntry(hframe,0.,12,0,
					      TGNumberFormat::kNESRealTwo, //style
					      TGNumberFormat::kNEANonNegative, //input value filter
					      TGNumberFormat::kNELNoLimits); //specify limits
	hframe->AddFrame(fwCalib_c1[iplane], new TGLayoutHints(kLHintsLeft,0,0,0,0));
	label = new TGLabel(hframe,"*10^");
	hframe->AddFrame(label,0);
	fwCalib_c1exp[iplane]= new  TGNumberEntry(hframe, 10., 3, 0,
						  TGNumberFormat::kNESInteger, //style
						  TGNumberFormat::kNEAAnyNumber, //input value filter
						  TGNumberFormat::kNELNoLimits, //specify limits
						  -12,12.); //limit values
	hframe->AddFrame(fwCalib_c1exp[iplane], new TGLayoutHints(kLHintsLeft,0,0,0,0));
	fwCalib_c1[iplane]->Connect("ValueSet(Long_t)","MyMainFrame",this,"DoChangeCalibNrs()");
	fwCalib_c1exp[iplane]->Connect("ValueSet(Long_t)","MyMainFrame",this,"DoChangeCalibNrs()");
	vframe->AddFrame(hframe,0);
    }
    fInputFrame2->AddFrame(vframe, new TGLayoutHints(kLHintsExpandY,15,15,5,5));

    // calibration function
    vframe = new TGVerticalFrame(fInputFrame2,10,10);
    label = new TGLabel(vframe,"calibration function");
    vframe->AddFrame(label,0);
    for (Int_t iplane=1; iplane<=10; iplane++) {
	fwCalib_fct[iplane]= new TGComboBox(vframe,10);
	fwCalib_fct[iplane]->AddEntry("c0*x*e^(c1*x)",1);
	fwCalib_fct[iplane]->AddEntry("c0*x*e^(c1*x**2)",2);
	fwCalib_fct[iplane]->AddEntry("c0*x*e^(c1*x**3)",3);
	fwCalib_fct[iplane]->AddEntry("c0*x*1/(1-c1/c0*x)",4);
	fwCalib_fct[iplane]->Resize(150,22);
	fwCalib_fct[iplane]->Select(1,kFALSE);
	fwCalib_fct[iplane]->SetEnabled(0);
	vframe->AddFrame(fwCalib_fct[iplane]);
	fwCalib_fct[iplane]->Connect("Selected(Int_t)","MyMainFrame",this,"DoChangeCalibNrs()");
    }
    fInputFrame2->AddFrame(vframe,new TGLayoutHints(kLHintsExpandY,15,15,5,5));

    f2dFrame->AddFrame(fInputFrame2,new  TGLayoutHints(kLHintsExpandX, 10, 10, 0, 1));

    fMain->SetWindowName("WASA FD Calibration");
    // Map all subwindows of main frame
    fMain->MapSubwindows();
    // Initialize the layout algorithm
    fMain->Resize(fMain->GetDefaultSize());
    // Map main frame
    fMain->MapWindow();
}

MyMainFrame::~MyMainFrame()
{
    fMain->CleanUp();
    delete fMain;
}

void MyMainFrame::DoChangeCalibNrs()
{
    Int_t elemnrFTH=fwElemNrFTH->GetNumber();
    Int_t elemnrFRH=(elemnrFTH-1)/2+1;
    if (elemnrFTH==0) elemnrFRH=0;
    for (Int_t i=1; i<=10; i++) {
	Int_t elemnr=(i==1?elemnrFTH:elemnrFRH);
	fCalib_c0[i][elemnr]=fwCalib_c0[i]->GetNumber()*pow(10,fwCalib_c0exp[i]->GetNumber());
	fCalib_c1[i][elemnr]=fwCalib_c1[i]->GetNumber()*pow(10,fwCalib_c1exp[i]->GetNumber());
	fCalib_fct[i]=fwCalib_fct[i]->GetSelected();
	// if elemnr==0, then change for all elements simultaneously
	if (elemnr==0){
	    for (Int_t elemall=1; elemall<=48; elemall++) {
		if (fCalib_c0[i][elemall]<=0 && fCalib_c1[i][elemall]<=0 ){
		    fCalib_c0[i][elemall]=fwCalib_c0[i]->GetNumber()*pow(10,fwCalib_c0exp[i]->GetNumber());
		    fCalib_c1[i][elemall]=fwCalib_c1[i]->GetNumber()*pow(10,fwCalib_c1exp[i]->GetNumber());
		}
	    }
	}
    }

    DoDraw();
}

void MyMainFrame::DoDraw()
{
    Int_t setnr=fwCalibSet->GetSelected();
    Int_t thetabin=fwThetabin->GetSelected();
    Int_t elemnrFTH=fwElemNrFTH->GetNumber();
    Int_t elemnrFRH=(elemnrFTH-1)/2+1;
    if (elemnrFTH==0) elemnrFRH=0;

    // Update CalibNr Widgets and deactivate all
    for (Int_t i=1; i<=10; i++) {
	Int_t elemnr=(i==1?elemnrFTH:elemnrFRH);
	Int_t c0exp=0; if (fCalib_c0[i][elemnr]!=0) c0exp=int(log10(fabs(fCalib_c0[i][elemnr])))-1;
	Double_t c0man=fCalib_c0[i][elemnr] / pow(10,c0exp);
	Int_t c1exp=0; if (fCalib_c1[i][elemnr]!=0) c1exp=int(log10(fabs(fCalib_c1[i][elemnr])))-1;
	Double_t c1man=fCalib_c1[i][elemnr] / pow(10,c1exp);

	fwCalib_c0[i]->SetNumber(c0man);
	fwCalib_c0exp[i]->SetNumber(c0exp);
	fwCalib_c1[i]->SetNumber(c1man);
	fwCalib_c1exp[i]->SetNumber(c1exp);
	fwCalib_fct[i]->Select(fCalib_fct[i],kFALSE);
	fwCalib_c0[i]->SetState(0); fwCalib_c0exp[i]->SetState(0);
	fwCalib_c1[i]->SetState(0); fwCalib_c1exp[i]->SetState(0);
	fwCalib_fct[i]->SetEnabled(0); if (elemnrFTH==0) fwCalib_fct[i]->SetEnabled(1);
    }

    // plot histograms, activate CalibNr widgets
    if (setnr==1) {
	fwCalib_c0[1]->SetState(1); fwCalib_c1[1]->SetState(1);	fwCalib_c0exp[1]->SetState(1); fwCalib_c1exp[1]->SetState(1);
	fwCalib_c0[4]->SetState(1); fwCalib_c1[4]->SetState(1); fwCalib_c0exp[4]->SetState(1); fwCalib_c1exp[4]->SetState(1);
	plot(1,elemnrFTH,thetabin-1);
    }

    if (setnr==2) {
	fwCalib_c0[2]->SetState(1);    fwCalib_c1[2]->SetState(1);
	fwCalib_c0exp[2]->SetState(1); fwCalib_c1exp[2]->SetState(1);
	plot(2,elemnrFTH,thetabin-1);
    }

    if (setnr==3) {
	fwCalib_c0[3]->SetState(1);    fwCalib_c1[3]->SetState(1);
	fwCalib_c0exp[3]->SetState(1); fwCalib_c1exp[3]->SetState(1);
	plot(3,elemnrFTH,thetabin-1);
    }

    if (setnr>=4 && setnr<=8) {
	for (Int_t i=1; i<=5; i++){
	    if (abs(i+3-setnr)>1) continue;
	    fwCalib_c0[i+3]->SetState(1);    fwCalib_c1[i+3]->SetState(1);
	    fwCalib_c0exp[i+3]->SetState(1); fwCalib_c1exp[i+3]->SetState(1);
	}
	plot(setnr,elemnrFTH,thetabin-1);
    }
    if (setnr>=9 && setnr<=10) {
	fwCalib_c0[setnr]->SetState(1);    fwCalib_c1[setnr]->SetState(1);
	fwCalib_c0exp[setnr]->SetState(1); fwCalib_c1exp[setnr]->SetState(1);
	plot(setnr,elemnrFTH,thetabin-1);
    }

}

void MyMainFrame::DoReadDatabase()
{
    ReadDatabaseFile("/home/WASA-software/rootsorter/wasa/Database/temp_calib/CalibCardFDScint_FRHdefault_44961_46884");
    ReadDatabaseFile("/home/WASA-software/rootsorter/wasa/Database/temp_calib/CalibCardFDScint_FTHdefault_44961_46884");
    ReadDatabaseFile("/home/WASA-software/rootsorter/wasa/Database/temp_calib/CalibCardFDScint_FWCdefault_44961_46884");
    //ReadDatabaseFile("/home/WASA-software/rootsorter/wasa/Database/CalibCardFDScint_FRHdefault");
    //ReadDatabaseFile("/home/WASA-software/rootsorter/wasa/Database/CalibCardFDScint_FTHdefault");
    //ReadDatabaseFile("/home/WASA-software/rootsorter/wasa/Database/CalibCardFDScint_FWCdefault");

    DoDraw();
}

void MyMainFrame::DoReadCalib()
{
    ReadCalibFile("/home/khreptak/Calibration/calib_EdE/temp_calib/calib_p_May_2014_old.tmp");

    DoDraw();
}

void MyMainFrame::DoWriteCalib()
{
    WriteCalibFile("/home/khreptak/Calibration/calib_EdE/temp_calib/calib_p_May_2014_new.tmp");
}

void MyMainFrame::DoExit()
{
    printf("Und tschuess \n");
    fEcanvas->Print("out.ps");
    gApplication->Terminate(0);
}

void MyMainFrame::DoNextDisplay()
{
    Int_t Displayset=fwCalibSet->GetSelected();
    Displayset++;
    if (Displayset>fwCalibSet->GetNumberOfEntries())
	Displayset=1;
    fwCalibSet->Select(Displayset,kFALSE);
    DoDraw();
}

void MyMainFrame::DoPrevDisplay()
{
    Int_t Displayset=fwCalibSet->GetSelected();
    Displayset--;
    if (Displayset<1)
	Displayset=fwCalibSet->GetNumberOfEntries();
    fwCalibSet->Select(Displayset,kFALSE);
    DoDraw();
}

void MyMainFrame::ReadDatabaseFile(TString cfilename)
{
    std::ifstream cfile;
    cfile.open(cfilename);

    Int_t planeoffset=0;
    if (cfilename.Contains("FTH")) planeoffset=0;
    if (cfilename.Contains("FRH")) planeoffset=3;
    if (cfilename.Contains("FWC")) planeoffset=8;

    printf("Reading calib constants from file %s...\n",cfilename.Data());

    Double_t xd1,xd2;
    cfile >> xd1 >> xd2;

    while (!cfile.eof()) {
	Int_t xpln, xelem;
	Double_t xt1,xt2,xt3,xt4,xt5,xt6;
	Double_t xadcthr;
	Int_t xnutype; Double_t xnu[11];
	Int_t xcaltype; Double_t xcal[5];
	cfile >> xpln >> xelem;
	cfile >> xt1 >> xt2 >> xt3 >> xt4 >> xt5 >> xt6 >> xadcthr;
	cfile >> xnutype;
	for (Int_t i=0; i<11; i++)
	    cfile >> xnu[i];
	cfile >> xcaltype;
	for (Int_t i=0; i<5; i++)
	    cfile >> xcal[i];
	if (xpln+planeoffset>0 &&
	    xpln+planeoffset<11 &&
	    xelem>0 && xelem<=48 &&
	    xcaltype==6) {
	    fCalib_c0[xpln+planeoffset][xelem]=xcal[0];
	    fCalib_c1[xpln+planeoffset][xelem]=xcal[1];
	    fCalib_fct[xpln+planeoffset] = xcal[2];
	}
	if (xpln+planeoffset>0 &&
	    xpln+planeoffset<11 &&
	    xelem>0 && xelem<=48 &&
	    xcaltype==5) {
	    fCalib_c0[xpln+planeoffset][xelem]=xcal[0];
	    fCalib_c1[xpln+planeoffset][xelem]=xcal[1];
	    fCalib_fct[xpln+planeoffset] = 4;
	}
    }
    printf("\ncalib file read in\n");

}

void MyMainFrame::WriteCalibFile(TString cfilename)
{
    std::ofstream cfile;
    cfile.open(cfilename,fstream::trunc);

    printf("Writing present config to file: %s \n",cfilename.Data());
    for (Int_t iplane=1; iplane<=10; iplane++) {
	Int_t NrElements=24;
	if (iplane==1) NrElements=48;

	for (Int_t ielem=1; ielem<=NrElements; ielem++){
	    TString out=Form("%i %2i %10.3e %10.3e %i 0 0\n",
			     iplane,ielem, fCalib_c0[iplane][ielem], fCalib_c1[iplane][ielem], fCalib_fct[iplane]);
	    cfile << out;
	}
    }
    cfile.close();
}


void MyMainFrame::ReadCalibFile(TString cfilename)
{
    std::ifstream cfile;
    cfile.open(cfilename);

    if (cfile.good()) {
	printf("Reading config from file: %s \n",cfilename.Data());
	for (Int_t iplane=1; iplane<=10; iplane++) {
	    Int_t NrElements=24;
	    if (iplane==1) NrElements=48;

	    for (Int_t ielem=1; ielem<=NrElements; ielem++){
		Int_t plane, element, d1, d2;
		cfile >> plane >> element >> fCalib_c0[iplane][ielem] >> fCalib_c1[iplane][ielem] >> fCalib_fct[iplane] >> d1 >> d2;
		if (plane !=iplane || element != ielem)
		    printf("Fehler beim Einlesen \n");
	    }
	}
    } else {
	printf("File %s does not exist. No parameter read. \n");
    }

    cfile.close();
}

void MyMainFrame::RecalibrateAxis(TAxis* axis,Double_t c0,Double_t c1, Int_t fct)
// Recalibrate a gives histogram axis from ADCcounts to Edep using constants
// c0 and c1, for a given calibration function seletor fct.
{
   if (axis->GetXmax() < 2.)
	return;   // obviously this axis was calibrated already

    Int_t NrBins=axis->GetNbins();
    Double_t *binlimits=new Double_t [NrBins+1];

    for (Int_t i=1; i<=NrBins+1; i++) {
	Double_t ADC=axis->GetBinLowEdge(i);
	Double_t binvalue = -1;

// implement different calibration methods
// c0*adc*exp(c1*ADC**fct)
	if (fct>0 && fct<=3)
	{
	    Double_t exponent=c1*pow(ADC,fct);
	    if (exponent>10){
		printf("exponent out of range: %f; set to 0 \n",exponent);
		exponent=0;
	    }
	    binvalue=c0*ADC*exp(exponent);
	}

// c0*ADC*1/(1-c1/c0*ADC)    (FRH proton calibration)
	if (fct==4)
	{
	    if (c0 > 0 && (1-c1/c0*ADC !=0))
		binvalue=c0*ADC*1/(1-c1/c0*ADC);
	}



	if (i>1 && ( binvalue<binlimits[i-2] ||
		     binvalue<0)) {
	    printf("clipping in calibration due to large non-uniformity...\n");
	    binvalue=binlimits[i-2];
	}
	binlimits[i-1]=binvalue;
    }

    axis->Set(NrBins,binlimits);
    delete[] binlimits;
}

void MyMainFrame::PlotSlice(TH2D* histo2d_mc, TH2D* histo2d_rd,
			    Int_t protype, Double_t slicelow, Double_t sliceup, Double_t axislow, Double_t axisup,
			    Double_t scalerd)
// Helper routin for the plot routine (next).
// provides easy way to plot slices of 2d histo, MC on top of RD with proper scaling
// in order t osimplify later the plot routine
// protype==1 : projection onto y axis of slice in x
// protype==2 : projection onto x axis of slice in y
{
    TH1D* histo1d_mc;
    TH1D* histo1d_rd;
    TText *titeltext=new TText();
    titeltext->SetTextSize(0.04);
    TString titel;
    if (protype==1) {
	Int_t slicelowmcbin=histo2d_mc->GetXaxis()->FindFixBin(slicelow);
	Int_t sliceupmcbin=histo2d_mc->GetXaxis()->FindFixBin(sliceup);
	Int_t slicelowrdbin=histo2d_rd->GetXaxis()->FindFixBin(slicelow);
	Int_t sliceuprdbin=histo2d_rd->GetXaxis()->FindFixBin(sliceup);

	histo1d_mc=histo2d_mc->ProjectionY(Form("MCslicex_%f_%f_%f",slicelow,sliceup,rand()),slicelowmcbin,sliceupmcbin);
	histo1d_rd=histo2d_rd->ProjectionY(Form("RDslicex_%f_%f_%f",slicelow,sliceup,rand()),slicelowrdbin,sliceuprdbin);
	titel=Form("Slice in x from %5.3f to %5.3f GeV",slicelow,sliceup);
    }
    else if (protype==2) {
	Int_t slicelowmcbin=histo2d_mc->GetYaxis()->FindFixBin(slicelow);
	Int_t sliceupmcbin=histo2d_mc->GetYaxis()->FindFixBin(sliceup);
	Int_t slicelowrdbin=histo2d_rd->GetYaxis()->FindFixBin(slicelow);
	Int_t sliceuprdbin=histo2d_rd->GetYaxis()->FindFixBin(sliceup);

	histo1d_mc=histo2d_mc->ProjectionX(Form("MCslicey_%f_%f_%f",slicelow,sliceup,rand()),slicelowmcbin,sliceupmcbin);
	histo1d_rd=histo2d_rd->ProjectionX(Form("RDslicey_%f_%f_%f",slicelow,sliceup,rand()),slicelowrdbin,sliceuprdbin);
	titel=Form("Slice in y from %5.3f to %5.3f GeV",slicelow,sliceup);
    }

    histo1d_mc->GetXaxis()->SetRangeUser(axislow,axisup);
    histo1d_mc->SetMaximum(histo1d_mc->GetMaximum()*1.3);
    histo1d_mc->SetLineColor(kRed);
    histo1d_mc->SetLineWidth(1);
    histo1d_mc->SetLineStyle(2);

    histo1d_rd->GetXaxis()->SetRangeUser(axislow,axisup);
    histo1d_rd->SetMaximum(histo1d_rd->GetMaximum()*1.3);
    histo1d_rd->SetLineColor(kBlack);
    histo1d_rd->SetLineWidth(1);
    histo1d_rd->SetLineStyle(1);
    if (scalerd>0)
	histo1d_rd->Scale(scalerd);
    else
	histo1d_rd->Scale(histo1d_mc->GetMaximum()/(histo1d_rd->GetMaximum()+0.0001) * 1.1);

    histo1d_mc->DrawCopy();
    histo1d_rd->DrawCopy("same");
    titeltext->DrawTextNDC(0.2,0.9,titel.Data());
    delete histo1d_mc;
    delete histo1d_rd;
    delete titeltext;
}

void MyMainFrame::plot(Int_t selector, Int_t elemnrFTH, Int_t thetabin)
// This is the plotting routine which plots the canvas with 2d and 1d slices
// for a given sheet selector number, FTH sector and thetabin

{
    gStyle->SetPalette(1,0);
    TCanvas *cc = fEcanvas->GetCanvas();
    cc->cd();
    cc->Clear();

//    TH2D *histo2d_RD, *histo2d_MC;

    if (selector==1) {
	// Calibration histos for FRH1 and FTH1
	Int_t elemnrFRH=(elemnrFTH-1)/2+1;
	if (elemnrFTH==0) elemnrFRH=0;
	TH2D *histo2d_RD, *histo2d_MC;

	// Read original histos from root file
	printf(Form("Histograms/CalibPlots/FRH/thetabin%i/EdE FRH1 vs FTH1_%02i \n",thetabin,elemnrFTH));
	histo2d_RD=(TH2D*)fRDfile->Get(Form("Histograms/CalibPlots/FRH/thetabin%i/EdE FRH1 vs FTH1_%02i",thetabin,elemnrFTH))->Clone();
	histo2d_RD->SetTitle("Edep FRH1 vs FTH1, RD");
	histo2d_RD->GetXaxis()->SetTitle("Edep FRH1 (GeV)");
	histo2d_RD->GetYaxis()->SetTitle("Edep FTH1 (GeV)");

	histo2d_MC=(TH2D*)fMCfile->Get(Form("Histograms/CalibPlots/FRH/thetabin%i/EdE FRH1 vs FTH1_%02i",thetabin,elemnrFTH))->Clone();
	histo2d_MC->SetTitle("Edep FRH1 vs FTH1, MC");
	histo2d_MC->GetXaxis()->SetTitle("Edep FRH1 (GeV)");
	histo2d_MC->GetYaxis()->SetTitle("Edep FTH1 (GeV)");

	// recalibrate RD axis
	RecalibrateAxis(histo2d_RD->GetXaxis(),fCalib_c0[4][elemnrFRH],fCalib_c1[4][elemnrFRH],fCalib_fct[4]);
	RecalibrateAxis(histo2d_RD->GetYaxis(),fCalib_c0[1][elemnrFTH],fCalib_c1[1][elemnrFTH],fCalib_fct[1]);

	// plot 2d and generate slices
	cc->Divide(3,3);

	cc->cd(1);
	gPad->SetLogz(1);
	histo2d_MC->SetContour(25);
	histo2d_MC->SetMinimum(histo2d_MC->GetMaximum()*0.002);
	histo2d_MC->SetMaximum(histo2d_MC->GetMaximum()*0.5);
	histo2d_MC->GetXaxis()->SetRangeUser(0,0.13);
	histo2d_MC->GetYaxis()->SetRangeUser(0,0.008);
	histo2d_MC->DrawCopy("AXIS");
	histo2d_RD->DrawCopy("colsame");
	if (fwMCDisplayFlag->GetState()==kButtonDown)
	    histo2d_MC->DrawCopy("Cont2same");

	// projections on FTH1 axis
	cc->cd(2);
	PlotSlice(histo2d_MC,histo2d_RD,
		  2,0.0025,0.0035,0.06,0.12,0.);

	cc->cd(4);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.028,0.030,0.000,0.004,0.);

	cc->cd(5);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.050,0.052,0.000,0.004,0.);

	cc->cd(6);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.07,0.072,0.001,0.004,0.);
	cc->cd(7);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.09,0.10,0.002,0.004,0.);

	// projections on FRH1 axis
	cc->cd(8);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.040,0.045,0.003,0.008,0.);
	cc->cd(9);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.025,0.030,0.004,0.007,0.);

    }

    if (selector==2 || selector==3) {
	// Calibrate FTH2 and FTH3
	Int_t elemnrFTH23=(elemnrFTH-1)/2+1;
	Int_t elemnrFRH=(elemnrFTH-1)/2+1;
	if (elemnrFTH==0) elemnrFTH23=0;
	TH2D *histo2d_RD, *histo2d_MC;
    	TH2D *histo2d_RD2, *histo2d_MC2;

	// histos: FRH1 vs FTH23
	histo2d_RD=(TH2D*)fRDfile->Get(Form("Histograms/CalibPlots/FTH%i/thetabin%i/EdE FRH1 vs FTH%i_%02i",selector,thetabin,selector,elemnrFTH23))->Clone();
	histo2d_RD->SetTitle("Edep FRH1 vs FTH23, RD");
	histo2d_RD->GetXaxis()->SetTitle("Edep FRH1 (GeV)");
	histo2d_RD->GetYaxis()->SetTitle("Edep FTH23 (GeV)");

	histo2d_MC=(TH2D*)fMCfile->Get(Form("Histograms/CalibPlots/FTH%i/thetabin%i/EdE FRH1 vs FTH%i_%02i",selector,thetabin,selector,elemnrFTH23))->Clone();
	histo2d_MC->SetTitle("Edep FRH1 vs FTH23, RD");
	histo2d_MC->GetXaxis()->SetTitle("Edep FRH1 (GeV)");
	histo2d_MC->GetYaxis()->SetTitle("Eep FTH23 (GeV)");

	// recalibrate RD axis
	RecalibrateAxis(histo2d_RD->GetYaxis(),
			fCalib_c0[selector][elemnrFTH23],
			fCalib_c1[selector][elemnrFTH23],
			fCalib_fct[selector]);

		RecalibrateAxis(histo2d_RD->GetXaxis(),
			fCalib_c0[4][elemnrFRH],
			fCalib_c1[4][elemnrFRH],
			fCalib_fct[4]);

	// histos FTH1 vs FTH23
	histo2d_RD2=(TH2D*)fRDfile->Get(Form("Histograms/CalibPlots/FTH%i/thetabin%i/EdE FTH1 vs FTH%i_%02i",selector,thetabin,selector,elemnrFTH23))->Clone();
	histo2d_RD2->SetTitle("Edep FTH1 vs FTH23, RD");
	histo2d_RD2->GetXaxis()->SetTitle("Edep FTH1 (GeV)");
	histo2d_RD2->GetYaxis()->SetTitle("Edep FTH23 (GeV)");

	histo2d_MC2=(TH2D*)fMCfile->Get(Form("Histograms/CalibPlots/FTH%i/thetabin%i/EdE FTH1 vs FTH%i_%02i",selector,thetabin,selector,elemnrFTH23))->Clone();
	histo2d_MC2->SetTitle("Edep FTH1 vs FTH23, RD");
	histo2d_MC2->GetXaxis()->SetTitle("Edep FTH1 (GeV)");
	histo2d_MC2->GetYaxis()->SetTitle("Eep FTH23 (GeV)");

	// recalibrate RD axis
	RecalibrateAxis(histo2d_RD2->GetYaxis(),
			fCalib_c0[selector][elemnrFTH23],
			fCalib_c1[selector][elemnrFTH23],
			fCalib_fct[selector]);
	RecalibrateAxis(histo2d_RD2->GetXaxis(),
			fCalib_c0[1][elemnrFTH],
			fCalib_c1[1][elemnrFTH],
			fCalib_fct[1]);


	// plot 2d and generate slices
	cc->Divide(3,3);
	cc->cd(1);

	gPad->SetLogz(1);
	histo2d_MC->DrawCopy("AXIS");
	histo2d_RD->DrawCopy("colsame");
	histo2d_MC->SetContour(13);
	histo2d_MC->SetLineColor(kBlack);
	histo2d_MC->SetMinimum(histo2d_MC->GetMaximum()*0.008);
	histo2d_MC->SetMaximum(histo2d_MC->GetMaximum()*0.1);
 	if (fwMCDisplayFlag->GetState()==kButtonDown)
	    histo2d_MC->DrawCopy("cont3same");

	cc->cd(2);
	gPad->SetLogz(1);
	histo2d_MC2->DrawCopy("AXIS");
	histo2d_RD2->DrawCopy("colsame");
	histo2d_MC2->SetContour(13);
	histo2d_MC2->SetLineColor(kBlack);
	histo2d_MC2->SetMinimum(histo2d_MC2->GetMaximum()*0.008);
	histo2d_MC2->SetMaximum(histo2d_MC2->GetMaximum()*0.1);
	if (fwMCDisplayFlag->GetState()==kButtonDown)
	    histo2d_MC2->DrawCopy("cont3same");

	cc->cd(4);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.034,0.036,0.00,0.003,0);
	cc->cd(5);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.058,0.060,0.001,0.006,0);

	cc->cd(6);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.080,0.082,0.001,0.004,0);

	cc->cd(7);
	PlotSlice(histo2d_MC2,histo2d_RD2,
		  1,0.0010,0.0014,0.0,0.004,0);
	cc->cd(8);
	PlotSlice(histo2d_MC2,histo2d_RD2,
		  1,0.0030,0.0034,0.002,0.005,0);
	cc->cd(9);
	PlotSlice(histo2d_MC2,histo2d_RD2,
		  1,0.0050,0.0055,0.003,0.008,0);

    }

    if (selector>=4 && selector<=8) {
	// calibrate FRH layers, FRH1,2,3,4,5

	Int_t elemnrFRH=(elemnrFTH-1)/2+1;

	TH2D *histo2d_RDpre=0, *histo2d_MCpre=0;
	TH2D *histo2d_RDnext=0, *histo2d_MCnext=0;
	Int_t FRHlayer=selector-3;
	Int_t FRHlayerpre=0;
	Int_t FRHlayernext=0;
	if (FRHlayer>1) FRHlayerpre=FRHlayer-1;
	if (FRHlayer<5) FRHlayernext=FRHlayer+1;

	// Read original histos from root file
	if (FRHlayerpre!=0){
	    TString histo2d_RDprename=Form("Histograms/CalibPlots/FRH/thetabin%i/EdE FRH%i vs FRH%i_%02i",
					   thetabin,FRHlayerpre,FRHlayer,elemnrFRH);
	    histo2d_RDpre=(TH2D*)fRDfile->Get(histo2d_RDprename)->Clone();

	    histo2d_RDpre->SetTitle(Form("Edep FRH%i vs FRH%i, RD",FRHlayerpre,FRHlayer));
	    histo2d_RDpre->GetXaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayerpre));
	    histo2d_RDpre->GetYaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayer));

	    TString histo2d_MCprename=Form("Histograms/CalibPlots/FRH/thetabin%i/EdE FRH%i vs FRH%i_%02i",
					   thetabin,FRHlayerpre,FRHlayer,elemnrFRH);
	    histo2d_MCpre=(TH2D*)fMCfile->Get(histo2d_MCprename)->Clone();
	    histo2d_MCpre->SetTitle(Form("Edep FRH%i vs FRH%i, RD",FRHlayerpre,FRHlayer));
	    histo2d_MCpre->GetXaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayerpre));
	    histo2d_MCpre->GetYaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayer));
	}
	if (FRHlayernext!=0){
	    TString histo2d_RDnextname=Form("Histograms/CalibPlots/FRH/thetabin%i/EdE FRH%i vs FRH%i_%02i",
					    thetabin,FRHlayer,FRHlayernext,elemnrFRH);
	    histo2d_RDnext=(TH2D*)fRDfile->Get(histo2d_RDnextname)->Clone();
	    histo2d_RDnext->SetTitle(Form("Edep FRH%i vs FRH%i, RD",FRHlayer,FRHlayernext));
	    histo2d_RDnext->GetXaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayer));
	    histo2d_RDnext->GetYaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayernext));

	    TString histo2d_MCnextname=Form("Histograms/CalibPlots/FRH/thetabin%i/EdE FRH%i vs FRH%i_%02i",
					    thetabin,FRHlayer,FRHlayernext,elemnrFRH);
	    histo2d_MCnext=(TH2D*)fMCfile->Get(histo2d_MCnextname)->Clone();
	    histo2d_MCnext->SetTitle(Form("Edep FRH%i vs FRH%i, RD",FRHlayer,FRHlayernext));
	    histo2d_MCnext->GetXaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayer));
	    histo2d_MCnext->GetYaxis()->SetTitle(Form("Edep FRH%i (GeV)",FRHlayernext));
    }

	// recalibrate RD axis
	if (histo2d_RDpre){
	    RecalibrateAxis(histo2d_RDpre->GetXaxis(),fCalib_c0[FRHlayerpre+3][elemnrFRH],
			    fCalib_c1[FRHlayerpre+3][elemnrFRH],fCalib_fct[FRHlayerpre+3]);
	    RecalibrateAxis(histo2d_RDpre->GetYaxis(),fCalib_c0[FRHlayer+3][elemnrFRH],
			    fCalib_c1[FRHlayer+3][elemnrFRH],fCalib_fct[FRHlayer+3]);
	}
	if (histo2d_RDnext) {
	    RecalibrateAxis(histo2d_RDnext->GetXaxis(),fCalib_c0[FRHlayer+3][elemnrFRH],
			    fCalib_c1[FRHlayer+3][elemnrFRH],fCalib_fct[FRHlayer+3]);
	    RecalibrateAxis(histo2d_RDnext->GetYaxis(),fCalib_c0[FRHlayernext+3][elemnrFRH],
			    fCalib_c1[FRHlayernext+3][elemnrFRH],fCalib_fct[FRHlayernext+3]);
	}

	// plot 2d and generate slices
	cc->Divide(3,3);


	cc->cd(1);
	// 2d plot versus prev FRH layer
	if (histo2d_RDpre){
	    gPad->SetLogz(1);
	    histo2d_MCpre->GetXaxis()->SetRangeUser(0,.14);
	    histo2d_MCpre->GetYaxis()->SetRangeUser(0,.14);

	    histo2d_MCpre->DrawCopy("AXIS");
	    histo2d_RDpre->DrawCopy("colsame");
	    gPad->SetLogz(1);
	    histo2d_MCpre->SetMinimum(histo2d_MCpre->GetMaximum()*0.008);
	    histo2d_MCpre->SetMaximum(histo2d_MCpre->GetMaximum()*0.1);

	    histo2d_MCpre->SetContour(5);
	    if (fwMCDisplayFlag->GetState()==kButtonDown)
		histo2d_MCpre->DrawCopy("CONT3same");
	}

	cc->cd(2);
	// 2d plot versus next FRH layer
	if (histo2d_RDnext){
	    gPad->SetLogz(1);
	    histo2d_MCnext->GetXaxis()->SetRangeUser(0,.14);
	    histo2d_MCnext->GetYaxis()->SetRangeUser(0,.14);
	    histo2d_MCnext->DrawCopy("AXIS");
	    histo2d_RDnext->DrawCopy("colsame");
	    gPad->SetLogz(1);
	    histo2d_MCnext->SetMinimum(histo2d_MCnext->GetMaximum()*0.008);
	    histo2d_MCnext->SetMaximum(histo2d_MCnext->GetMaximum()*0.1);
	    histo2d_MCnext->SetContour(5);
	    if (fwMCDisplayFlag->GetState()==kButtonDown)
		histo2d_MCnext->DrawCopy("CONT2same");
	}

	if (selector==4){
	    // FRH1 vs FRH2
	    cc->cd(4);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.028,0.032,0.01,0.06,0);
	    cc->cd(5);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.096,0.110,0.03,0.08,0);
	    cc->cd(6);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.012,0.020,0.05,0.12,0);
	}
	if (selector==5){
	    // FRH2 vs FRH1,FRH3
	    cc->cd(4);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.028,0.032,0.01,0.06,0);
	    cc->cd(5);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.084,0.088,0.01,0.08,0);
	    cc->cd(6);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.052,0.064,0.05,0.12,0);
	    cc->cd(7);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.028,0.032,0.01,0.06,0);
	    cc->cd(8);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.096,0.110,0.03,0.08,0);
	    cc->cd(9);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		      2,0.012,0.020,0.05,0.12,0);
	}
	if (selector==6){
	    // FRH3 vs FRH2,FRH4
	    cc->cd(4);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.028,0.032,0.01,0.06,0);
	    cc->cd(5);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.076,0.080,0.01,0.08,0);
	    cc->cd(6);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.052,0.064,0.05,0.12,0);
	    cc->cd(7);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.035,0.040,0.01,0.06,0);
	    cc->cd(8);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.120,0.140,0.02,0.08,0);
	    cc->cd(9);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		      2,0.018,0.025,0.05,0.12,0);
	}
	if (selector==7){
	    // FRH4 vs FRH3,FRH5
	    cc->cd(4);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.028,0.032,0.01,0.08,0);
	    cc->cd(5);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.075,0.078,0.01,0.08,0);
	    cc->cd(6);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		  1,0.047,0.050,0.05,0.15,0);
	    cc->cd(7);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.035,0.042,0.02,0.07,0);
	    cc->cd(8);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		  2,0.120,0.140,0.03,0.08,0);
	    cc->cd(9);
	    PlotSlice(histo2d_MCnext,histo2d_RDnext,
		      2,0.018,0.025,0.07,0.115,0);
	}
	if (selector==8){
	    // FRH5 vs FRH4
	    cc->cd(4);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		      1,0.040,0.045,0.02,0.08,0);
	    cc->cd(5);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		      1,0.084,0.090,0.01,0.11,0);
	    cc->cd(6);
	    PlotSlice(histo2d_MCpre,histo2d_RDpre,
		      1,0.065,0.075,0.05,0.15,0);
	}
    }

    if (selector==9 || selector==10) {
	// Calibrate FWC1 and FWC2
        //Int_t elemnrFWC12=(elemnrFTH-1)/2+1;
      if(selector==9){
	Int_t elemnrFWC12=elemnrFTH/2+1; // FWC1 el1 <-> FTH1 el. 48,1
	if(elemnrFWC12==25) elemnrFWC12=1;
	Int_t elemnrFRH=(elemnrFTH-1)/2+1;
      }
      if(selector==10){
        Int_t elemnrFWC12=(elemnrFTH-1)/2+1; //FWC2 el1 <-> FTH1 el. 1,2
	Int_t elemnrFRH=(elemnrFTH-1)/2+1;
      }

	if (elemnrFTH==0) elemnrFWC12=0;
	TH2D *histo2d_RD, *histo2d_MC;
    	TH2D *histo2d_RD2, *histo2d_MC2;

	// histos: FRH1 vs FWC23
	histo2d_RD=(TH2D*)fRDfile->Get(Form("Histograms/CalibPlots/FWC%i/thetabin%i/EdE FRH1 vs FWC%i_%02i",
					    selector-8,thetabin,selector-8,elemnrFWC12))->Clone();
	histo2d_RD->SetTitle("Edep FRH1 vs FWC12, RD");
	histo2d_RD->GetXaxis()->SetTitle("Edep FRH1 (GeV)");
	histo2d_RD->GetYaxis()->SetTitle("Edep FWC12 (GeV)");

	histo2d_MC=(TH2D*)fMCfile->Get(Form("Histograms/CalibPlots/FWC%i/thetabin%i/EdE FRH1 vs FWC%i_%02i",
					    selector-8,thetabin,selector-8,elemnrFWC12))->Clone();
	histo2d_MC->SetTitle("Edep FRH1 vs FWC12, RD");
	histo2d_MC->GetXaxis()->SetTitle("Edep FRH1 (GeV)");
	histo2d_MC->GetYaxis()->SetTitle("Eep FWC12 (GeV)");

	// recalibrate RD axis
	RecalibrateAxis(histo2d_RD->GetYaxis(),
			fCalib_c0[selector][elemnrFWC12],
			fCalib_c1[selector][elemnrFWC12],
			fCalib_fct[selector]);

	RecalibrateAxis(histo2d_RD->GetXaxis(),
			fCalib_c0[4][elemnrFRH],
			fCalib_c1[4][elemnrFRH],
			fCalib_fct[4]);

	// histos FTH1 vs FWC12
	histo2d_RD2=(TH2D*)fRDfile->Get(Form("Histograms/CalibPlots/FWC%i/thetabin%i/EdE FTH1 vs FWC%i_%02i",
					     selector-8,thetabin,selector-8,elemnrFWC12))->Clone();
	histo2d_RD2->SetTitle("Edep FTH1 vs FWC12, RD");
	histo2d_RD2->GetXaxis()->SetTitle("Edep FTH1 (GeV)");
	histo2d_RD2->GetYaxis()->SetTitle("Edep FWC12 (GeV)");

	histo2d_MC2=(TH2D*)fMCfile->Get(Form("Histograms/CalibPlots/FWC%i/thetabin%i/EdE FTH1 vs FWC%i_%02i",
					     selector-8,thetabin,selector-8,elemnrFWC12))->Clone();
	histo2d_MC2->SetTitle("Edep FTH1 vs FWC12, RD");
	histo2d_MC2->GetXaxis()->SetTitle("Edep FTH1 (GeV)");
	histo2d_MC2->GetYaxis()->SetTitle("Edep FWC12 (GeV)");

	// recalibrate RD axis
	RecalibrateAxis(histo2d_RD2->GetYaxis(),
			fCalib_c0[selector][elemnrFWC12],
			fCalib_c1[selector][elemnrFWC12],
			fCalib_fct[selector]);

	RecalibrateAxis(histo2d_RD2->GetXaxis(),
			fCalib_c0[1][elemnrFTH],
			fCalib_c1[1][elemnrFTH],
			fCalib_fct[1]);

	// plot 2d and generate slices
	cc->Divide(3,3);
	cc->cd(1);

	gPad->SetLogz(1);
	histo2d_MC->DrawCopy("AXIS");
	histo2d_RD->DrawCopy("colsame");
	histo2d_MC->SetContour(13);
	histo2d_MC->SetLineColor(kBlack);
	histo2d_MC->SetMinimum(histo2d_MC->GetMaximum()*0.008);
	histo2d_MC->SetMaximum(histo2d_MC->GetMaximum()*0.1);
 	if (fwMCDisplayFlag->GetState()==kButtonDown)
	    histo2d_MC->DrawCopy("cont3same");

	cc->cd(2);
	gPad->SetLogz(1);
	histo2d_MC2->DrawCopy("AXIS");
	histo2d_RD2->DrawCopy("colsame");
	histo2d_MC2->SetContour(13);
	histo2d_MC2->SetLineColor(kBlack);
	histo2d_MC2->SetMinimum(histo2d_MC2->GetMaximum()*0.008);
	histo2d_MC2->SetMaximum(histo2d_MC2->GetMaximum()*0.1);
	if (fwMCDisplayFlag->GetState()==kButtonDown)
	    histo2d_MC2->DrawCopy("cont3same");



	cc->cd(4);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.030,0.032,0.00,0.004,0);
	cc->cd(5);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.090,0.100,0.000,0.005,0);
	cc->cd(6);
	PlotSlice(histo2d_MC,histo2d_RD,
		  1,0.030,0.035,0.002,0.007,0);

	cc->cd(7);
	PlotSlice(histo2d_MC2,histo2d_RD2,
		  1,0.0015,0.0018,0.0,0.003,0);
	cc->cd(8);
	PlotSlice(histo2d_MC2,histo2d_RD2,
		  1,0.0030,0.0034,0.0,0.004,0);
	cc->cd(9);
	PlotSlice(histo2d_MC2,histo2d_RD2,
		  1,0.0055,0.0060,0.000,0.006,0);

    }

    cc->Update();
}



void calibrate(TString RDfilename="") {
  // Popup the GUI...
  gWindow = new MyMainFrame(gClient->GetRoot(),RDfilename);

}
