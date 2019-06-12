/*

  This macro will add histograms from a list of root files and write them
  to a target root file. The target file is newly created and must not be
  identical to one of the source files.

  Author: Sven A. Schmidt, sven.schmidt@cern.ch
  Date:   13.2.2001

  This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
  which had a problem with directories more than one level deep.
  (see macro hadd_old.C for this previous implementation).
  
  The macro from Sven has been enhanced by 
     Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
   to automatically add Trees (via a chain of trees).
  
  To use this macro, modify the file names in function hadd.
  
  NB: This macro is provided as a tutorial.
      Use $ROOTSYS/bin/hadd to merge many histogram files

 */


#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

TList *FileList;
TFile *Target;

void MergeRootfile( TDirectory *target, TList *sourcelist );


void hadd() {
   // in an interactive ROOT session, edit the file names
   // Target and FileList, then
   // root > .L hadd.C
   // root > hadd()

  Target = TFile::Open( "../calib_NU.root", "RECREATE" );

  FileList = new TList();

  FileList->Add( TFile::Open("calib_NU_46050.root"));
  FileList->Add( TFile::Open("calib_NU_46051.root"));
  FileList->Add( TFile::Open("calib_NU_46052.root"));
  FileList->Add( TFile::Open("calib_NU_46053.root"));
  FileList->Add( TFile::Open("calib_NU_46054.root"));
  FileList->Add( TFile::Open("calib_NU_46055.root"));
  FileList->Add( TFile::Open("calib_NU_46056.root"));
  FileList->Add( TFile::Open("calib_NU_46057.root"));
  FileList->Add( TFile::Open("calib_NU_46058.root"));
  FileList->Add( TFile::Open("calib_NU_46059.root"));

  FileList->Add( TFile::Open("calib_NU_46060.root"));
  FileList->Add( TFile::Open("calib_NU_46061.root"));
  FileList->Add( TFile::Open("calib_NU_46062.root"));
  FileList->Add( TFile::Open("calib_NU_46063.root"));
  FileList->Add( TFile::Open("calib_NU_46064.root"));
  FileList->Add( TFile::Open("calib_NU_46065.root"));
  FileList->Add( TFile::Open("calib_NU_46066.root"));
  FileList->Add( TFile::Open("calib_NU_46067.root"));
  FileList->Add( TFile::Open("calib_NU_46068.root"));
  FileList->Add( TFile::Open("calib_NU_46069.root"));

  FileList->Add( TFile::Open("calib_NU_46070.root"));
  FileList->Add( TFile::Open("calib_NU_46071.root"));
  FileList->Add( TFile::Open("calib_NU_46072.root"));
  FileList->Add( TFile::Open("calib_NU_46073.root"));
  FileList->Add( TFile::Open("calib_NU_46074.root"));

  FileList->Add( TFile::Open("calib_NU_46275.root"));
  FileList->Add( TFile::Open("calib_NU_46276.root"));
  FileList->Add( TFile::Open("calib_NU_46277.root"));
  FileList->Add( TFile::Open("calib_NU_46278.root"));
  FileList->Add( TFile::Open("calib_NU_46279.root"));

  FileList->Add( TFile::Open("calib_NU_46280.root"));
  FileList->Add( TFile::Open("calib_NU_46281.root"));
  FileList->Add( TFile::Open("calib_NU_46282.root"));
  FileList->Add( TFile::Open("calib_NU_46283.root"));
  FileList->Add( TFile::Open("calib_NU_46284.root"));
  FileList->Add( TFile::Open("calib_NU_46285.root"));
  FileList->Add( TFile::Open("calib_NU_46286.root"));
  FileList->Add( TFile::Open("calib_NU_46287.root"));
  FileList->Add( TFile::Open("calib_NU_46288.root"));
  FileList->Add( TFile::Open("calib_NU_46289.root"));

  FileList->Add( TFile::Open("calib_NU_46290.root"));
  FileList->Add( TFile::Open("calib_NU_46291.root"));
  FileList->Add( TFile::Open("calib_NU_46292.root"));
  FileList->Add( TFile::Open("calib_NU_46293.root"));
  FileList->Add( TFile::Open("calib_NU_46294.root"));
  FileList->Add( TFile::Open("calib_NU_46295.root"));
  FileList->Add( TFile::Open("calib_NU_46296.root"));
  FileList->Add( TFile::Open("calib_NU_46297.root"));
  FileList->Add( TFile::Open("calib_NU_46298.root"));
  FileList->Add( TFile::Open("calib_NU_46299.root"));

  FileList->Add( TFile::Open("calib_NU_46300.root"));
  FileList->Add( TFile::Open("calib_NU_46301.root"));
  FileList->Add( TFile::Open("calib_NU_46302.root"));
  FileList->Add( TFile::Open("calib_NU_46303.root"));
  FileList->Add( TFile::Open("calib_NU_46304.root"));
  FileList->Add( TFile::Open("calib_NU_46305.root"));
  FileList->Add( TFile::Open("calib_NU_46306.root"));
  FileList->Add( TFile::Open("calib_NU_46307.root"));
  FileList->Add( TFile::Open("calib_NU_46308.root"));
  FileList->Add( TFile::Open("calib_NU_46309.root"));

  FileList->Add( TFile::Open("calib_NU_46310.root"));
  FileList->Add( TFile::Open("calib_NU_46311.root"));
  FileList->Add( TFile::Open("calib_NU_46312.root"));
  FileList->Add( TFile::Open("calib_NU_46313.root"));
  FileList->Add( TFile::Open("calib_NU_46314.root"));
  FileList->Add( TFile::Open("calib_NU_46315.root"));
  FileList->Add( TFile::Open("calib_NU_46316.root"));
  FileList->Add( TFile::Open("calib_NU_46317.root"));
  FileList->Add( TFile::Open("calib_NU_46318.root"));
  FileList->Add( TFile::Open("calib_NU_46319.root"));

  FileList->Add( TFile::Open("calib_NU_46320.root"));
  FileList->Add( TFile::Open("calib_NU_46321.root"));
  FileList->Add( TFile::Open("calib_NU_46322.root"));
  FileList->Add( TFile::Open("calib_NU_46323.root"));
  FileList->Add( TFile::Open("calib_NU_46324.root"));
  FileList->Add( TFile::Open("calib_NU_46325.root"));
  FileList->Add( TFile::Open("calib_NU_46326.root"));
  FileList->Add( TFile::Open("calib_NU_46327.root"));
  FileList->Add( TFile::Open("calib_NU_46328.root"));
  FileList->Add( TFile::Open("calib_NU_46329.root"));

  FileList->Add( TFile::Open("calib_NU_46330.root"));
  FileList->Add( TFile::Open("calib_NU_46331.root"));
  FileList->Add( TFile::Open("calib_NU_46332.root"));
  FileList->Add( TFile::Open("calib_NU_46333.root"));
  FileList->Add( TFile::Open("calib_NU_46334.root"));
  FileList->Add( TFile::Open("calib_NU_46335.root"));
  FileList->Add( TFile::Open("calib_NU_46336.root"));
  FileList->Add( TFile::Open("calib_NU_46337.root"));
  FileList->Add( TFile::Open("calib_NU_46338.root"));
  FileList->Add( TFile::Open("calib_NU_46339.root"));

  FileList->Add( TFile::Open("calib_NU_46340.root"));
  FileList->Add( TFile::Open("calib_NU_46341.root"));
  FileList->Add( TFile::Open("calib_NU_46342.root"));
  FileList->Add( TFile::Open("calib_NU_46343.root"));
  FileList->Add( TFile::Open("calib_NU_46344.root"));
  FileList->Add( TFile::Open("calib_NU_46345.root"));
  FileList->Add( TFile::Open("calib_NU_46346.root"));
  FileList->Add( TFile::Open("calib_NU_46347.root"));
  FileList->Add( TFile::Open("calib_NU_46348.root"));
  FileList->Add( TFile::Open("calib_NU_46349.root"));

  FileList->Add( TFile::Open("calib_NU_46350.root"));
  FileList->Add( TFile::Open("calib_NU_46351.root"));
  FileList->Add( TFile::Open("calib_NU_46352.root"));
  FileList->Add( TFile::Open("calib_NU_46353.root"));
  FileList->Add( TFile::Open("calib_NU_46354.root"));
  FileList->Add( TFile::Open("calib_NU_46355.root"));
  FileList->Add( TFile::Open("calib_NU_46356.root"));
  FileList->Add( TFile::Open("calib_NU_46357.root"));
  FileList->Add( TFile::Open("calib_NU_46358.root"));
  FileList->Add( TFile::Open("calib_NU_46359.root"));

  FileList->Add( TFile::Open("calib_NU_46360.root"));
  FileList->Add( TFile::Open("calib_NU_46361.root"));
  FileList->Add( TFile::Open("calib_NU_46362.root"));
  FileList->Add( TFile::Open("calib_NU_46363.root"));
  FileList->Add( TFile::Open("calib_NU_46364.root"));
  FileList->Add( TFile::Open("calib_NU_46365.root"));
  FileList->Add( TFile::Open("calib_NU_46366.root"));
  FileList->Add( TFile::Open("calib_NU_46367.root"));
  FileList->Add( TFile::Open("calib_NU_46368.root"));
  FileList->Add( TFile::Open("calib_NU_46369.root"));

  FileList->Add( TFile::Open("calib_NU_46370.root"));
  FileList->Add( TFile::Open("calib_NU_46371.root"));
  FileList->Add( TFile::Open("calib_NU_46372.root"));
  FileList->Add( TFile::Open("calib_NU_46373.root"));
  FileList->Add( TFile::Open("calib_NU_46374.root"));
  FileList->Add( TFile::Open("calib_NU_46375.root"));
  FileList->Add( TFile::Open("calib_NU_46376.root"));
  FileList->Add( TFile::Open("calib_NU_46377.root"));
  FileList->Add( TFile::Open("calib_NU_46378.root"));
  FileList->Add( TFile::Open("calib_NU_46379.root"));

  FileList->Add( TFile::Open("calib_NU_46380.root"));
  FileList->Add( TFile::Open("calib_NU_46381.root"));
  FileList->Add( TFile::Open("calib_NU_46382.root"));
  FileList->Add( TFile::Open("calib_NU_46383.root"));
  FileList->Add( TFile::Open("calib_NU_46384.root"));
  FileList->Add( TFile::Open("calib_NU_46385.root"));
  FileList->Add( TFile::Open("calib_NU_46386.root"));
  FileList->Add( TFile::Open("calib_NU_46387.root"));
  FileList->Add( TFile::Open("calib_NU_46388.root"));
  FileList->Add( TFile::Open("calib_NU_46389.root"));

  FileList->Add( TFile::Open("calib_NU_46390.root"));
  FileList->Add( TFile::Open("calib_NU_46391.root"));
  FileList->Add( TFile::Open("calib_NU_46392.root"));
  FileList->Add( TFile::Open("calib_NU_46393.root"));
  FileList->Add( TFile::Open("calib_NU_46394.root"));
  FileList->Add( TFile::Open("calib_NU_46395.root"));
  FileList->Add( TFile::Open("calib_NU_46396.root"));
  FileList->Add( TFile::Open("calib_NU_46397.root"));
  FileList->Add( TFile::Open("calib_NU_46398.root"));
  FileList->Add( TFile::Open("calib_NU_46399.root"));

  FileList->Add( TFile::Open("calib_NU_46400.root"));
  FileList->Add( TFile::Open("calib_NU_46401.root"));
  FileList->Add( TFile::Open("calib_NU_46402.root"));
  FileList->Add( TFile::Open("calib_NU_46403.root"));
  FileList->Add( TFile::Open("calib_NU_46404.root"));
  FileList->Add( TFile::Open("calib_NU_46405.root"));
  FileList->Add( TFile::Open("calib_NU_46406.root"));
  FileList->Add( TFile::Open("calib_NU_46407.root"));
  FileList->Add( TFile::Open("calib_NU_46408.root"));
  FileList->Add( TFile::Open("calib_NU_46409.root"));

  FileList->Add( TFile::Open("calib_NU_46410.root"));
  FileList->Add( TFile::Open("calib_NU_46411.root"));
  FileList->Add( TFile::Open("calib_NU_46412.root"));
  FileList->Add( TFile::Open("calib_NU_46413.root"));
  FileList->Add( TFile::Open("calib_NU_46414.root"));
  FileList->Add( TFile::Open("calib_NU_46415.root"));
  FileList->Add( TFile::Open("calib_NU_46416.root"));
  FileList->Add( TFile::Open("calib_NU_46417.root"));
  FileList->Add( TFile::Open("calib_NU_46418.root"));
  FileList->Add( TFile::Open("calib_NU_46419.root"));

  FileList->Add( TFile::Open("calib_NU_46420.root"));
  FileList->Add( TFile::Open("calib_NU_46421.root"));
  FileList->Add( TFile::Open("calib_NU_46422.root"));
  FileList->Add( TFile::Open("calib_NU_46423.root"));
  FileList->Add( TFile::Open("calib_NU_46424.root"));

  FileList->Add( TFile::Open("calib_NU_46750.root"));
  FileList->Add( TFile::Open("calib_NU_46751.root"));
  FileList->Add( TFile::Open("calib_NU_46752.root"));
  FileList->Add( TFile::Open("calib_NU_46753.root"));
  FileList->Add( TFile::Open("calib_NU_46754.root"));
  FileList->Add( TFile::Open("calib_NU_46755.root"));
  FileList->Add( TFile::Open("calib_NU_46756.root"));
  FileList->Add( TFile::Open("calib_NU_46757.root"));
  FileList->Add( TFile::Open("calib_NU_46758.root"));
  FileList->Add( TFile::Open("calib_NU_46759.root"));

  FileList->Add( TFile::Open("calib_NU_46760.root"));
  FileList->Add( TFile::Open("calib_NU_46761.root"));
  FileList->Add( TFile::Open("calib_NU_46762.root"));
  FileList->Add( TFile::Open("calib_NU_46763.root"));
  FileList->Add( TFile::Open("calib_NU_46764.root"));
  FileList->Add( TFile::Open("calib_NU_46765.root"));
  FileList->Add( TFile::Open("calib_NU_46766.root"));
  FileList->Add( TFile::Open("calib_NU_46767.root"));
  FileList->Add( TFile::Open("calib_NU_46768.root"));
  FileList->Add( TFile::Open("calib_NU_46769.root"));

  FileList->Add( TFile::Open("calib_NU_46770.root"));
  FileList->Add( TFile::Open("calib_NU_46771.root"));
  FileList->Add( TFile::Open("calib_NU_46772.root"));
  FileList->Add( TFile::Open("calib_NU_46773.root"));
  FileList->Add( TFile::Open("calib_NU_46774.root"));
  
  MergeRootfile( Target, FileList );

}   

void MergeRootfile( TDirectory *target, TList *sourcelist ) {

  cout << "Target path: " << target->GetPath() << endl;
  
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );

  TFile *first_source = (TFile*)sourcelist->First();
  first_source->cd( path );
  TDirectory *current_sourcedir = gDirectory;

  // loop over all keys in this directory
  TChain *globChain = 0;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key;
  while ( (key = (TKey*)nextkey())) {

    // read object from first source file
    first_source->cd( path );
    TObject *obj = key->ReadObj();

    if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
      // descendant of TH1 -> merge it

      cout << "Merging histogram " << obj->GetName() << endl;
      TH1 *h1 = (TH1*)obj;

      // loop over all source files and add the content of the
      // correspondant histogram to the one pointed to by "h1"
      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      while ( nextsource ) {
        
        // make sure we are at the correct directory level by cd'ing to path
        nextsource->cd( path );
        TH1 *h2 = (TH1*)gDirectory->Get( h1->GetName() );
        if ( h2 ) {
          h1->Add( h2 );
          delete h2; // don't know if this is necessary, i.e. if 
                     // h2 is created by the call to gDirectory above.
        }

        nextsource = (TFile*)sourcelist->After( nextsource );
      }
    }
    else if ( obj->IsA()->InheritsFrom( "TTree" ) ) {
      
      // loop over all source files create a chain of Trees "globChain"
      const char* obj_name= obj->GetName();

      globChain = new TChain(obj_name);
      globChain->Add(first_source->GetName());
      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      //      const char* file_name = nextsource->GetName();
      // cout << "file name  " << file_name << endl;
     while ( nextsource ) {
     	  
       globChain->Add(nextsource->GetName());
       nextsource = (TFile*)sourcelist->After( nextsource );
     }

    } else if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) {
      // it's a subdirectory

      cout << "Found subdirectory " << obj->GetName() << endl;

      // create a new subdir of same name and title in the target file
      target->cd();
      TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );

      // newdir is now the starting point of another round of merging
      // newdir still knows its depth within the target file via
      // GetPath(), so we can still figure out where we are in the recursion
      MergeRootfile( newdir, sourcelist );

    } else {

      // object is of no type that we know or can handle
      cout << "Unknown object type, name: " 
           << obj->GetName() << " title: " << obj->GetTitle() << endl;
    }

    // now write the merged histogram (which is "in" obj) to the target file
    // note that this will just store obj in the current directory level,
    // which is not persistent until the complete directory itself is stored
    // by "target->Write()" below
    if ( obj ) {
      target->cd();

      //!!if the object is a tree, it is stored in globChain...
	if(obj->IsA()->InheritsFrom( "TTree" ))
	  globChain->Write( key->GetName() );
	else
	obj->Write( key->GetName() );
    }

  } // while ( ( TKey *key = (TKey*)nextkey() ) )

  // save modifications to target file
  target->Write();

}

