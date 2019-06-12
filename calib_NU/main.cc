/******************************************************************************
 *
 * This is an example main() 
 * 
 *****************************************************************************/

#include "Wasa.hh"
#include "SorterConfig.hh"
#include "SorterOption.hh"
#include "CLog.hh"

static SorterOption O_outputfile("fmultiout","-fmultiout","","NoOutput");

int main(int argc, char** argv) {
  // read command line arguments, mandatory for proper operation
    gSorterConfig->ReadCmdLine(argc,argv);
    

  // register Sorter ID
    Wasa::Initialize("FRHCalibration","","RootSorter.log");
  
    // register analysis module
    //    gWasa->AddAnalysis("FRHRawAnalysis","Raw");
    gWasa->AddAnalysis("FRHCalibAnalysis","Raw");
    
  gWasa->Run();
  
  delete gWasa; 
}



