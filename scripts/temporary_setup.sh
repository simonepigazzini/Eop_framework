#!/bin/bash
SCRIPTDIRECTORY=$(cd `dirname ${BASH_SOURCE[0]}` && pwd)
echo 'script directory:' $SCRIPTDIRECTORY
if [[ $SCRIPTDIRECTORY == /afs/* ]];
then
    cd /afs/cern.ch/user/f/fmonti/work/EoP_harness/CMSSW_10_1_2/src/
    eval `scram runtime -sh`
    cd -
fi

alias cmake_clean='rm -r '$SCRIPTDIRECTORY/../build/*
alias cmake_reset='rm -r '$SCRIPTDIRECTORY/../build/*' && cd '$SCRIPTDIRECTORY/../build' && cmake .. && cd -'
