#ifndef _FRHCalibAnalysis_HH_
#define _FRHCalibAnalysis_HH_

#include <TROOT.h>
#include <TObject.h>

#include <TH1.h>
#include <TH2.h>

#include "CAnalysisModule.hh"
#include "WCluster.hh"
#include "FDFTHTracks.hh"
#include "WTrack.hh"
#include "WTrackBank.hh"
#include "WHitScint.hh"
#include "REventHeader.hh"
#include "FRHCalibAnalysis.hh"
#include "WKineSimple.hh"

class FRHCalibAnalysis : public CAnalysisModule {

public:

    FRHCalibAnalysis();
    explicit FRHCalibAnalysis(const char * name);
    virtual ~FRHCalibAnalysis();
    virtual void                Clear(Option_t *option = "");
    virtual void                Print(Option_t *option = "");
    virtual void                UserCommand(CCommand * command);
    virtual void                ProcessEvent();

private:
    Bool_t              fMCmode;

    WHitBank            *fHitBankFRH;
    REventHeader        *fHeader;
    CalibCardFDScint    *fCalibCard;
    WKineSimple         *fKineSimple;

    TH1F                *hStatistics;
    TH1F                *hPhiNU;
    TH1F                *hThetaNU;
    TH2F                *hPlanevsElem;

    TH2F                *hThetavsADCcos[12][49];
    TH2F                *hThetavsADCcos_cor[12][49];

    TH2F                *hEdEFRH1vsFWC1[25][6], *hEdEFTH1vsFWC1[25][6];
    TH2F                *hEdEFRH1vsFWC2[25][6], *hEdEFTH1vsFWC2[25][6];

    TH2F                *hEdEFRH1vsFTH1[49][6], *hEdEFRH1vsFTH2[25][6], *hEdEFRH1vsFTH3[25][6];
    TH2F                *hEdEFTH1vsFTH2[25][6], *hEdEFTH1vsFTH3[25][6];
    TH2F                *hEdEFRH1vsFRH2[25][6], *hEdEFRH2vsFRH3[25][6],  *hEdEFRH3vsFRH4[25][6], *hEdEFRH4vsFRH5[25][6];

    TH2F                *hThetavsFRH1[49];
    TH2F                *hThetavsFRH1_eta[49];

protected:
    void                        SetupRawSpectra(const char * path);
    virtual void                FillNUplots();
    virtual void                FillEdEplots();

    ClassDef(FRHCalibAnalysis,0)
};

#endif
