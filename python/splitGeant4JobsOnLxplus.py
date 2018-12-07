#!/usr/bin/env python
import os, re
import commands
import math, time
import sys
import argparse
import subprocess

parser = argparse.ArgumentParser(description='This script splits Geant4 tasks in multiple jobs and sends them on LXBATCH')

parser.add_argument("-l", "--label",          required=True,     type=str,  help="job label")
parser.add_argument("-e", "--exe",            required=True,     type=str,  help="executable")
parser.add_argument("-o", "--outputFolder",   required=True,     type=str,  help="folder where to store output files")
#parser.add_argument("-f", "--outputFileName", required=True,     type=str,  help="base name of output files [outputFileName]_i.root")
parser.add_argument("-c", "--configFile",     required=True,     type=str,  help="config file to be run")
#parser.add_argument("-g", "--gpsFile",        required=True,     type=str,  help="gps.mac file to be run")
parser.add_argument("-n", "--nJobs",          required=True,     type=int,  help="number of jobs")
parser.add_argument("-N", "--nEvents",        required=True,     type=int,  help="number of events per job")
parser.add_argument("-q", "--queue",          default="1nd",     type=str,  help="hercules queue to use")
parser.add_argument("-s", "--submit",                                       help="submit jobs", action='store_true')
parser.add_argument("-v", "--verbose",                                      help="increase output verbosity", action='store_true')


args = parser.parse_args()


print 
print 'START'
print 

currDir = os.getcwd()

print

try:
   subprocess.check_output(['mkdir','jobs'])
except subprocess.CalledProcessError as e:
   print e.output
try:
   subprocess.check_output(['mkdir','jobs/'+args.label])
except subprocess.CalledProcessError as e:
   print e.output
try:
   subprocess.check_output(['mkdir',args.outputFolder+"/"+args.label+"/"])
except subprocess.CalledProcessError as e:
   print e.output


##### loop for creating and sending jobs #####
for x in range(1, args.nJobs+1):
   
   ##### creates directory and file list for job #######
   jobDir = currDir+'/jobs/'+args.label+'/job_'+str(x)
   os.system('mkdir '+jobDir)
   os.chdir(jobDir)
   
   ##### copy executable to the jobDir ######
   os.system('cp '+args.exe+' '+jobDir+"/executable.exe")
   
   ##### creates Geant4 gps.mac file #######
   #with open(args.gpsFile) as fi:
   #   contents = fi.read()
   #   replaced_contents = contents.replace('MAXEVENTS', str(args.nEvents))
   #with open(jobDir+"/gps.mac", "w") as fo:
   #   fo.write(replaced_contents)
      
   ##### creates Geant4 config file #######
   with open(args.configFile) as fi:
      contents = fi.read()
      replaced_contents = contents.replace('SEED', str(x))
      #replaced_contents = replaced_contents.replace('GPS', jobDir+"/gps.mac")
      replaced_contents = replaced_contents.replace('NEVENTS', str(args.nEvents))
      #replaced_contents = replaced_contents.replace('OUTPUT_PATH', str(args.outputFolder+args.label+"/"))
      #replaced_contents = replaced_contents.replace('/afs/cern.ch/user/f/fmonti', '~fmonti') #problems between geant4 and the absolute path, in particular cern.ch is automatically modified in cern_t0.ch by g4analysismanager
   with open(jobDir+"/config.cfg", "w") as fo:
      fo.write(replaced_contents)
   
   ##### creates jobs #######
   with open('job_'+str(x)+'.sh', 'w') as fout:
      fout.write("#!/bin/sh\n")
      fout.write("echo\n")
      fout.write("echo 'START---------------'\n")
      fout.write("echo 'current dir: ' ${PWD}\n")
      fout.write("source /afs/cern.ch/sw/lcg/external/gcc/4.9/x86_64-slc6-gcc49-opt/setup.sh\n")
      fout.write("source /afs/cern.ch/sw/lcg/external/geant4/10.1.p02/x86_64-slc6-gcc49-opt-MT/CMake-setup.sh\n")
      fout.write("mkdir "+str(args.outputFolder+args.label)+"\n")
      #fout.write("source ~/bin/myGeant4.sh\n")
      #fout.write("source ~/bin/myRoot.sh\n")
      fout.write("cd "+str(jobDir)+"\n")
      fout.write("echo 'current dir: ' ${PWD}\n")
      fout.write("./executable.exe config.cfg\n")#+args.outputFolder+"/"+args.label+"/"+args.outputFileName+"_"+str(x)+"\n")
      fout.write("mv *.root " +str(args.outputFolder+args.label)+"\n")
      fout.write("echo 'STOP---------------'\n")
      fout.write("echo\n")
      fout.write("echo\n")
   os.system("chmod 755 job_"+str(x)+".sh")
   
   ###### sends bjobs ######
   if args.submit:
      os.system("bsub -q "+args.queue+" job_"+str(x)+".sh")
      print "job nr. " + str(x) + " submitted"
   
   os.chdir("../..")
   
print
print "your jobs:"
os.system("bjobs")
print
print 'END'
print
