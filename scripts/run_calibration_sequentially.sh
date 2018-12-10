#!/bin/bash

#script to run sequentially the calibration loop
#script requires as input the number of loops: default=15
#at each iteration the output is overwritten so it is previously copied
#3 cfg are required :
#   + 1 to run the first BuilEopEta
#   + 1 to run the first UpdateIC
#   + 1 for all the other operations after the first loop
workdir=/home/fabio/Eop_framework/
Nloop=`expr $1 - 1`

echo ---------------------------
echo START
echo ---------------------------

#first loop
echo 
echo loop 0
echo $workdir/build/BuildEopEta $workdir/cfg/calib_BuildEopEta_firstloop.cfg
$workdir/build/BuildEopEta $workdir/cfg/calib_BuildEopEta_firstloop.cfg
echo $workdir/build/UpdateIC $workdir/cfg/calib_UpdateIC_firstloop.cfg
$workdir/build/UpdateIC $workdir/cfg/calib_UpdateIC_firstloop.cfg
echo cp EopEta_.root EopEta_0.root
cp EopEta_.root EopEta_0.root
echo cp IC_.root IC_0.root
cp IC_.root IC_0.root

#other loops
for i in $(seq 1 $Nloop); do
    echo
    echo loop $i
    echo $workdir/build/BuildEopEta $workdir/cfg/calib_after1loop.cfg
    $workdir/build/BuildEopEta $workdir/cfg/calib_after1loop.cfg
    echo $workdir/build/UpdateIC $workdir/cfg/calib_after1loop.cfg
    $workdir/build/UpdateIC $workdir/cfg/calib_after1loop.cfg
    echo cp EopEta_.root EopEta_$i.root
    cp EopEta_.root EopEta_$i.root
    echo cp IC_.root IC_$i.root
    cp IC_.root IC_$i.root
done

echo ---------------------------
echo END
echo ---------------------------
