#!/usr/bin/python
import os
import glob
import math
from array import array
import sys
import time
import subprocess
from optparse import OptionParser
import time
import datetime

#print date
print("----------------------------------------------------------------------------------")
print(datetime.datetime.now())
print("----------------------------------------------------------------------------------")

#parameters
ntupleName = "Run2018"
current_dir = os.getcwd();
ntuple_dir = "/eos/cms/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/Cal_Oct2017_cand_v7/" #"/eos/cms/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/2018/"#parent folder containing the ntuples for the monitoring
#tag_list = ["EcalPedestals_10May2018_collisions_blue_laser_Rereco","101X_dataRun2_Prompt_v9","101X_dataRun2_Prompt_v10","101X_dataRun2_Prompt_v11"]#tag for the monitoring
#ntuple_dir = "/home/fabio/Eop_framework/data/" #parent folder containing the ntuples for the monitoring
tag_list = ["Run2017C"] #tag for the monitoring
ignored_ntuples_label_list = ["obsolete"]#ntuples containing anywhere in the path these labels will be ignored (eg ntuples within a tag for the monitoring containing some error)
tasklist = ["BuildEopEta","UpdateIC"]

#parse arguments
parser = OptionParser()
parser.add_option('--generateOnly',    action='store_true',           dest='generateOnly', default=False,      help='generate jobs only, without submitting them')
parser.add_option("-v", "--verbosity", action="store", type="int",    dest="verbosity",    default=1,          help="verbosity level")
parser.add_option("-o", "--outdir",    action="store", type="string", dest="outdir",       default="./",       help="output directory")
parser.add_option("-e", "--exedir",    action="store", type="string", dest="exedir",       default="./build/", help="executable directory")
parser.add_option("-c", "--cfg",       action="store", type="string", dest="configFile",                       help="template config file")
parser.add_option("-l", "--label",     action="store", type="string", dest="label",                            help="job label")
parser.add_option("-N", "--Nloop",     action="store", type="int",    dest="Nloop",        default=15,         help="number of loop")
(options, args) = parser.parse_args()

#eventually get new files for the monitoring
selected_filelist = []
extracalibtree_filelist = []
for root, dirs, files in os.walk(ntuple_dir):
    for file in files:
        if file.endswith(".root") and file.startswith("DoubleEG") & (file.find("WSkim")!=-1 or file.find("ZSkim")!=-1): #NOTE DoubleEG has to be replaced with EGamma for 2018 data
            if (any (tag in os.path.join(root, file) for tag in tag_list)):
                if(not any (ignored_ntuples_label in os.path.join(root, file) for ignored_ntuples_label in ignored_ntuples_label_list)):
                    if file.find("extraCalibTree-DoubleEG")==-1 :
                        selected_filelist.append(os.path.join(root, file))
                        extracalibtree_filelist.append(os.path.join(root, "extraCalibTree-"+file))
                    
if (len(selected_filelist)>0):
    print
    print("Run calibration on files:")
    print("-----------------------")
    print(selected_filelist)
    print("-----------------------")
    print("auto-generated extraCalibTree filelist")
    print(extracalibtree_filelist)
    print("-----------------------")

else:
    print
    print("NOT any file found --> EXIT")
    sys.exit()

#create folder for the job
job_folder=current_dir+"/jobs/"+options.label
os.system("mkdir -p "+job_folder)

#make the monitoring script
job_Nb={}

for iLoop in range(0,options.Nloop):
    print("Generating job for loop "+str(iLoop))
    for iFile in range(0,len(selected_filelist)):
        selected_filename=selected_filelist[iFile]
        extracalibtree_filename=extracalibtree_filelist[iFile]
        if(options.verbosity>=1):
            print("Generating job for file "+selected_filename)
        for task in tasklist:
            if(options.verbosity>=1):
                print("Generating job for "+task)
            
            jobdir=job_folder+"/job_loop_"+str(iLoop)+"_file_"+str(iFile)+"_"+task
            os.system("mkdir "+jobdir)

            ##### creates config file #######
            BUILDEOPETA_OUTPUT= options.outdir+"/EopEta_"+str(iLoop)+".root"
            UPDATEIC_OUTPUT= options.outdir+"/IC_"+str(iLoop)+".root"
            if task=="BuildEopEta":
                BUILDEOPETA_INPUT=" EopEta_"+str(iLoop-1)+" "+options.outdir+"/EopEta_"+str(iLoop-1)+".root"
                UPDATEIC_INPUT=" IC_eta_phi "+options.outdir+"/IC_"+str(iLoop-1)+".root"                    
            if task=="UpdateIC":
                BUILDEOPETA_INPUT=" EopEta_"+str(iLoop)+" "+options.outdir+"/EopEta_"+str(iLoop)+".root"
                UPDATEIC_INPUT=" IC_eta_phi "+options.outdir+"/IC_"+str(iLoop-1)+".root"                    
            with open(options.configFile) as fi:
                contents = fi.read()
                replaced_contents = contents.replace("SELECTED_INPUTFILE", selected_filename).replace("EXTRACALIBTREE_INPUTFILE", extracalibtree_filename)
                replaced_contents=replaced_contents.replace("BUILDEOPETA_OUTPUT",BUILDEOPETA_OUTPUT)
                replaced_contents=replaced_contents.replace("UPDATEIC_OUTPUT",UPDATEIC_OUTPUT)
                if iLoop==0 :
                    replaced_contents=replaced_contents.replace("  inputIC UPDATEIC_INPUT\n","")
                    if task=="BuildEopEta":
                        replaced_contents=replaced_contents.replace("  Eopweight TH2F BUILDEOPETA_INPUT\n","")
                    else:
                        replaced_contents=replaced_contents.replace("BUILDEOPETA_INPUT",BUILDEOPETA_INPUT)
                else:
                    replaced_contents=replaced_contents.replace("BUILDEOPETA_INPUT",BUILDEOPETA_INPUT)
                    replaced_contents=replaced_contents.replace("UPDATEIC_INPUT",UPDATEIC_INPUT)
            with open(jobdir+"/config.cfg", "w") as fo:
                fo.write(replaced_contents)

            ##### create script #######
            outScript = open( jobdir+"/job_"+str(iFile)+".sh","w")
            outScript.write("#!/bin/bash\n")
            #outScript.write('source setup.sh\n')
            outScript.write("echo $PWD\n");
            outScript.write(options.exedir+"/"+task+"\n") 
            outScript.write("echo finish\n") 
            outScript.close();
            os.system("chmod 777 "+jobdir+"/job_"+str(iFile)+".sh")
            submit_command = "bsub -q 8nh "+jobdir+"/job_"+str(iFile)+".sh"
            if(options.verbosity>=1):
                print(">>SUBMIT COMMAND: "+submit_command)
            if not options.generateOnly:
                command = submit_command.split()
                p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
                for line in iter(p.stdout.readline, b''):
                    out=line.split()
                    job_Nb[categories[iFile]] = out[1].replace("<","").replace(">","")
                    print(">>Job number = "+job_Nb[categories[iFile]])

    #generate condor submitfile
    condorsub = open( job_folder+"/submit_BuilEopEta_loop"+str(iLoop)+".sub","w")
    condorsub.write("executable            = "+job_folder+"/job_loop_"+str(iLoop)+"_file_$(ProcId)_BuilEopEta/job_$(ProcId).sh\n")
    condorsub.write("output                = output/hello.$(ClusterId).$(ProcId).out\n")
    condorsub.write("error                 = error/hello.$(ClusterId).$(ProcId).err\n")
    condorsub.write("log                   = log/hello.$(ClusterId).log\n")
    condorsub.write("queue "+str(len(selected_filelist))+"\n")
    condorsub.close()
    
if options.generateOnly:
    sys.exit()

print
print
waiting_time=5 #in minutes 
elaps_time = 0
Ntotjobs=len(categories)
while((len(categories)>0) & (elaps_time<24*60*60) ):
      print("Check job termination in "+str(waiting_time)+" min")
      time.sleep(60*waiting_time)
      for category in categories:
          checkjob_command = "bjobs "+job_Nb[category]
          command = checkjob_command.split()
          p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
          for line in iter(p.stdout.readline, ''):
              #print line
              if((line.count("not found")>0) | (line.count("DONE")>0)):
                  print("job #"+job_Nb[category]+" for category "+category+ " terminated")
                  os.system("mv LSFJOB_"+job_Nb[category]+" "+job_folder)
#                 print(">>updating txt and plots")
#                 os.system(exedir+"/UpdateHistory "+outdir+" "+newpoints_dir+" "+category+"__")
#                 os.system(exedir+"/UpdateHistoryPlots "+outdir+" "+category+"__ 0.95 1.05 zoom1percent")
#                 os.system(exedir+"/UpdateHistoryPlots "+outdir+" "+category+"__ 0.3 1.2 zoom90percent")
                  os.system("cp "+index_file+" "+outdir+"/"+category+"__/")
                  os.system("cp "+index_file+" "+outdir+"/"+category+"__/templatefit/")
                  categories.remove(category)
                  break
      elaps_time = elaps_time+60*waiting_time
      print("Total elapsed time = "+str(elaps_time/60)+" min")
      print(str(Ntotjobs-len(categories))+" / "+str(Ntotjobs)+" terminated")
              
if(elaps_time<24*60*60):
    print("updating datfile")
    newpoints_datfile = open( datfile,"r")
    filestr = newpoints_datfile.read()
    newpoints_datfile.close()
    history_datfile = open(history_fname, 'a')
    history_datfile.write(filestr)
    history_datfile.close()
else:
    print("TIMEOUT --> killing")


