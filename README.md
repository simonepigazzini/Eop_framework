# Eop_framework 
Code for the monitoring and calibration of the ECAL with E/p    


NOTE     
 - skeleton of this repository is forked from https://github.com/abenagli/TTHAnalysis.git    
 - algorithm and function are basically a reorganization of ECALELF/EOverPCalibration codes displaced in different branches:      
    - more updated version of calibration code: https://github.com/lbrianza/ECALELF      
    - more updated version of	monitoring code:  https://gitlab.cern.ch/vciriolo/ECALELF/tree/eop      
    - master of ECALELF:                        https://gitlab.cern.ch/shervin/ECALELF      



## Setup
compilation with cmake should be fine with much more compiler/o.s./rootversion 
   ```
   git clone git@github.com:fabio-mon/Eop_framework.git
   cd Eop_framework
   mkdir build
   cd build && cmake ..
   make
   ```
NOTE: if you add a new class you have to re-run `cd build && cmake ..`    
NOTE: if you add a new executable you have to properly modify the CMakeList.txt and then re-run `cd build && cmake ..`