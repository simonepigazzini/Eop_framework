# Eop_framework 
NOTE 
skeleton of this repository is forked from repository TTHAnalsyis
https://github.com/abenagli/TTHAnalysis.git

algorithm and function are basically a reorganization of ECALELF/EOverPCalibration codes displaced in different branches:      
more updated version of calibration code: https://github.com/lbrianza/ECALELF      
more updated version of	monitoring code:  https://gitlab.cern.ch/vciriolo/ECALELF/tree/eop      
master of ECALELF:                        https://gitlab.cern.ch/shervin/ECALELF      

Code for the monitoring and calibration of the ECAL with E/p

## Setup
   ```
   git clone git@github.com:fabio-mon/Eop_framework.git
   cd Eop_framework
   git submodule update --init --recursive
   mkdir bin obj 
   make
   make exe
   ```
