#!/bin/bash
#script to remove the unmerged files given the parent folder

echo 'rm '$1'/EopEta_loop_*_file_*_*.root'
rm $1/EopEta_loop_*_file_*_*.root 

echo 'rm '$1'/IC_loop_*_file_*_*.root'
rm $1/IC_loop_*_file_*_*.root
