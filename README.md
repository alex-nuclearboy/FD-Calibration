FD-Calibration
==============
Calibration of the Forward Detector of WASA-at-COSY facility

Nonuniformity calibration
=========================
It is very important to have CalibCards with proper time calibration coefficients for ADC and TDC. 

Then, we have to use the FRHCalibAnalysis.cc module on a few runs. The program name is slightly misleading because it is a module for the calibration of the whole forward detector (excluding only the FVH), not only for the FRH. We can, in principle, use any run but the most accurate is to take runs with some elastic trigger (TR#21 = frha1|psc1) in order to have a lot of events with minimal ionizing particles. The reason is that those events have an almost constant linear energy deposition in all FD detectors. This leads to a better separation of the nonuniformity and nonlinearity effects. 

For the May/June 2014 eta beamtime, we use runs 46050-46074 46275-46424 and 46750-46774 to generate a calib_NU.root file (joining the 200 .root files with hadd.C function). In order to do this just run RunCalibration.sh [RunCalibration_460.sh, RunCalibration_462.sh, RunCalibration_464.sh and RunCalibration_467.sh] scripts.

Each time you use FRHCalibAnalysis.cc remember to make sure that the trigger number you use for non-uniformity inside the program is corresponding to an elastic trigger number. We used trigger 21.

Then we open ROOT environment load fitNU.C and execute it with the generated root file:
     
     .L fitNU.C
     .x fitNU.C("calib_NU.root",layer_number)
  
where layer_number is:

     1 = FTH
     4-6 = FRH  
     10-11 = FWC
  
This action fits Theta vs ADC*theta spectra for each element of each plane/layer (push enter after each element output) and in the end generates non-uniformity calibration constants table (for the given layer).

What we have to do now is to include those new parameter values to the corresponding CalibCards. Those changes will be made on the temporal verions of the CalibCards for safety reasons. First of all copy the CalibCards from wasa/Database in RootSorter to your local working directory and link them properly in .sorterrc:

     Sorter.ParSource.CalibCardFDScint_FRHdefault: calib_tmp/CalibCardFDScint_FRHdefault
     Sorter.ParSource.CalibCardFDScint_FTHdefault: calib_tmp/CalibCardFDScint_FTHdefault
     Sorter.ParSource.CalibCardFDScint_FWCdefault: calib_tmp/CalibCardFDScint_FWCdefault
  
The parameter table (skip the last column with 0 contents) has to be filled inside (after the column with "6") the corresponding CalibCardFDScint_x, where x = FRHdefault/FTHdefault/FWCdefault.

     Eg.:
     1  1  4902.2  0.09497 0.   321.1  1.56250 0.  100 6 +8.257650e-01 -2.156795e-01 +7.332738e+00 -2.161129e+00 0.045 0.314 0 0 0 0 0 5 2.140e-05  1.000e-15  0 0 0
  
Note:

     - All three FRH layers have 24 elements (theta sectors, pizza-like structure);
     - FTH layer has 48 elements;
     - FWC has two layers with 24 elements each.
     
Nonlinearity calibration
========================
After the nonuniformity corrections to the calibration files (locally) the FRHCalibAnalysis.cc module is used again. We have to run the analysis module for data runs (200) and for MC data. Here we use MC of pd->pd pi0 reaction. In the same way as for non-uniformity, first of all, we have to check that the correct trigger number is used by FRHCalibAnalysis.cc. For example, our trigger was TR29.

Then we execute the RunCalibration-data.sh, RunCalibration_460.sh, RunCalibration_462.sh, RunCalibration_464.sh and RunCalibration_467.sh scripts for data and RunCalibration-mc.sh for MC.

Before going further we have to join the generated 200 calib_EdE_<given_run_for_NL>.root files with hadd.C function. 

Then, we open the ROOT environment and we run calibrate.C. Inside the calibrate.C program should be written the correct names of data.root and MC.root files. The nonlinearity calibration consist in adjusting two parameters for each element and layer that correspond to the formula:

     E_{dep} = ADC*C_{0}\frac{1}{1-ADC*\frac{C_{1}}{C_{0}}}

This is done fitting real data to MC simulation on a set of spectra representing the energy deposition of one layer vs another layer ("bananas"). For example, FRH1 vs FRH2, FTH1 vs FRH1 etc...



