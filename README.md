# Eop_framework 
Code for the monitoring and calibration of the ECAL with E/p    


NOTE     
 - skeleton of this repository is forked from https://github.com/abenagli/TTHAnalysis.git    
 - algorithm and function are basically a reorganization of ECALELF/EOverPCalibration codes displaced in different branches:      
    - more updated version of calibration code: https://github.com/lbrianza/ECALELF      
    - more updated version of	monitoring code:  https://gitlab.cern.ch/vciriolo/ECALELF/tree/eop      
    - master of ECALELF:                        https://gitlab.cern.ch/shervin/ECALELF      
 - this code uses the very useful CfgManager class cloned from https://github.com/simonepigazzini/CfgManager to parse options from file 


## Setup
compilation with cmake should be fine with much more compiler/o.s./rootversion 
   ```
   git clone git@github.com:fabio-mon/Eop_framework.git
   cd Eop_framework
   mkdir build lib bin    
   cd build       
   cmake ..     
   make     
   ```
NOTE: if you add a new class/header/source you have to manually cancel the cmake cache:    
   ```
   cd Eop_framework/build
   rm -r ./*    
   cmake ..   	   
   make	 
   ```
ENJOY :)   