#!/bin/bash
SCRIPTDIRECTORY=$(cd `dirname ${BASH_SOURCE[0]}` && pwd)
BASEDIRECTORY=$SCRIPTDIRECTORY/..
#echo 'script directory:' $SCRIPTDIRECTORY
echo 'base directory:  ' $BASEDIRECTORY
export SCRIPTDIRECTORY
export BASEDIRECTORY

if [[ $BASEDIRECTORY == /afs/* ]];
then
    cd /afs/cern.ch/user/f/fmonti/work/flashggNew/CMSSW_10_5_0/src/
    #cd /afs/cern.ch/user/f/fmonti/work/EoP_harness/CMSSW_10_1_2/src/
    eval `scram runtime -sh`
    cd -
fi

alias cmake_clean='rm -r '$BASEDIRECTORY/build/*
alias cmake_reset='rm -r '$BASEDIRECTORY/build/*'; cd '$BASEDIRECTORY/build'; cmake ..; cd -'
#alias cmake_build='cd '$BASEDIRECTORY/build'; make; cd -'
alias cmake_build=$SCRIPTDIRECTORY/cmake_build.sh
alias rm_unmerged=$SCRIPTDIRECTORY/rm_unmerged.sh
