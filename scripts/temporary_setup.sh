#!/bin/sh
if [[ $PWD == /afs/* ]];
then
    cd /afs/cern.ch/user/f/fmonti/work/EoP_harness/CMSSW_10_1_2/src/
    eval `scram runtime -sh`
    cd -
fi
alias cmake_clean='rm -r /afs/cern.ch/user/f/fmonti/work/Eop_framework/build/*'
