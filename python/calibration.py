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
ntupleName = "Run2017"
current_dir = os.getcwd();
ntuple_dir = "/eos/cms/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/UltraRereco2017_2feb2019_AllCorrections/"#parent folder containing the ntuples for the monitoring
#ntuple_dir="/home/fabio/work/Eop_framework/data/"
tag_list = ["Run2017B","Run2017C","Run2017D","Run2017E","Run2017F"]#tag for the monitoring
#tag_list = ["Run2017C"] #tag for the monitoring
ignored_ntuples_label_list = ["obsolete"]#ntuples containing anywhere in the path these labels will be ignored (eg ntuples within a tag for the monitoring containing some error)
tasklist = ["BuildEopEta_EB","ComputeIC_EB"]
splitstat = ["odd","even"]

#parse arguments
parser = OptionParser()
parser.add_option('--submit',    action='store_true',           dest='submit', default=False,      help='submit jobs')
parser.add_option("-v", "--verbosity", action="store", type="int",    dest="verbosity",    default=1,          help="verbosity level")
parser.add_option("-o", "--outdir",    action="store", type="str", dest="outdir",       default="./",       help="output directory")
parser.add_option("-e", "--exedir",    action="store", type="str", dest="exedir",       default="./build/", help="executable directory")
parser.add_option("-c", "--cfg",       action="store", type="str", dest="configFile",                       help="template config file")
parser.add_option("-l", "--label",     action="store", type="str", dest="label",                            help="job label")
parser.add_option("-N", "--Nloop",     action="store", type="int",    dest="Nloop",        default=15,         help="number of loop")
(options, args) = parser.parse_args()

#create outdir
os.system("mkdir -p "+str(options.outdir))

#get ntuples for the calibration
selected_filelist = []
extracalibtree_filelist = []
for root, dirs, files in os.walk(ntuple_dir):
    for file in files:
        #NOTE in the following line "DoubleEG" has to be replaced with "EGamma" for >=2018 ntuples
        if file.endswith(".root") and file.startswith("DoubleEG") & (file.find("WSkim")!=-1 or file.find("ZSkim")!=-1): 
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
job_parent_folder=current_dir+"/jobs/"+str(options.label)+"/"
os.system("mkdir -p "+job_parent_folder)

#create the log folder
os.system("mkdir -p "+job_parent_folder+"/log/")

#create DAGMan file to manage submitting hierarchy
dagFilename=job_parent_folder+"/submit_manager.dag"
dagFile = open( dagFilename,"w")

#make the monitoring script
job_Nb={}

for iLoop in range(0,options.Nloop):
    print("> Generating job for loop "+str(iLoop))

    for task in tasklist:
        if(options.verbosity>=1):
            print(">> Generating job for "+task)

        for iFile in range(0,len(selected_filelist)):
            selected_filename=selected_filelist[iFile]
            extracalibtree_filename=extracalibtree_filelist[iFile]
            if(options.verbosity>=1):
                print(">>> Generating job for file "+selected_filename)
                
            jobdir=job_parent_folder+"/job_loop_"+str(iLoop)+"_file_"+str(iFile)+"_"+task+"/"
            os.system("mkdir "+jobdir)
            
            with open(str(options.configFile)) as fi:
                contents = fi.read()
                replaced_contents = contents.replace("SELECTED_INPUTFILE", selected_filename).replace("EXTRACALIBTREE_INPUTFILE", extracalibtree_filename)
            cfgfilename=jobdir+"/config.cfg"
            with open(cfgfilename, "w") as fo:
                fo.write(replaced_contents)

            for split in splitstat:
                ##### creates executable options #######
                BUILDEOPETA_OUTPUT= str(options.outdir)+"/EopEta_loop_"+str(iLoop)+"_file_"+str(iFile)+"_"+split+".root"
                UPDATEIC_OUTPUT= str(options.outdir)+"/IC_loop_"+str(iLoop)+"_file_"+str(iFile)+"_"+split+".root"
                BUILDEOPETA_INPUT_OPTION=""
                UPDATEIC_INPUT_OPTION=""
                EOPWEIGHTRANGE_OPTION=""
                if iLoop==0:
                    if task=="BuildEopEta_EB":
                        BUILDEOPETA_INPUT_OPTION=""
                        UPDATEIC_INPUT_OPTION=""
                        EOPWEIGHTRANGE_OPTION="--Eopweightrange 0.9"
                    if task=="ComputeIC_EB":
                        BUILDEOPETA_INPUT_OPTION="--Eopweight TH2F EopEta "+str(options.outdir)+"/EopEta_loop_"+str(iLoop)+".root"
                        UPDATEIC_INPUT_OPTION=""
                        EOPWEIGHTRANGE_OPTION=""
                else:
                    if task=="BuildEopEta_EB":
                        BUILDEOPETA_INPUT_OPTION="--Eopweight TH2F EopEta "+str(options.outdir)+"/EopEta_loop_"+str(iLoop-1)+".root"
                        UPDATEIC_INPUT_OPTION="--inputIC IC "+str(options.outdir)+"/IC_loop_"+str(iLoop-1)+".root"                    
                        EOPWEIGHTRANGE_OPTION="--Eopweightrange 0.15"
                    if task=="ComputeIC_EB":
                        BUILDEOPETA_INPUT_OPTION="--Eopweight TH2F EopEta "+str(options.outdir)+"/EopEta_loop_"+str(iLoop)+".root"
                        UPDATEIC_INPUT_OPTION="--inputIC IC "+str(options.outdir)+"/IC_loop_"+str(iLoop-1)+".root"                    
                        EOPWEIGHTRANGE_OPTION=""

                ##### creates script #######
                outScriptName=jobdir+"/job_file_"+str(iFile)+"_"+split+".sh"
                outScript = open(outScriptName,"w")
                outScript.write("#!/bin/bash\n")
                #outScript.write('source setup.sh\n')
                outScript.write("cd /afs/cern.ch/user/f/fmonti/work/EoP_harness/CMSSW_10_1_2/\n")
                outScript.write('eval `scram runtime -sh`\n');
                outScript.write("cd -\n");
                outScript.write("echo $PWD\n");
                outScript.write(
                    str(options.exedir)+"/"+task+
                    " --cfg "+cfgfilename+
                    " "+UPDATEIC_INPUT_OPTION+
                    " "+BUILDEOPETA_INPUT_OPTION+
                    " --BuildEopEta_output "+BUILDEOPETA_OUTPUT+
                    " "+EOPWEIGHTRANGE_OPTION+
                    " --ComputeIC_output "+UPDATEIC_OUTPUT+
                    " --"+split+"\n")
                outScript.write("echo finish\n") 
                outScript.close();
                os.system("chmod 777 "+outScriptName)

        #generate condor multijob submitfile for each task
        condorsubFilename=job_parent_folder+"/submit_"+task+"_loop_"+str(iLoop)+".sub"
        condorsub = open( condorsubFilename,"w")
        condorsub.write("executable            = $(scriptname)\n")
        condorsub.write("output                = $(scriptname).$(ClusterId).out\n")
        condorsub.write("error                 = $(scriptname).$(ClusterId).err\n")
        condorsub.write("log                   = "+job_parent_folder+"/log/log.$(ClusterId).log\n")
        condorsub.write('+JobFlavour           = "workday"\n')
        condorsub.write("queue scriptname matching "+job_parent_folder+"/job_loop_"+str(iLoop)+"_file_*_"+task+"/*.sh\n")
        condorsub.close()
        #fill the submitting manager file
        dagFile.write("JOB "+task+"_loop_"+str(iLoop)+" "+condorsubFilename+"\n")

        #submit the merging step
        if task=="BuildEopEta_EB":
            mergescriptName=job_parent_folder+"/merge_BuildEopEta_EB_loop_"+str(iLoop)+".sh"
            mergescript = open( mergescriptName,"w")
            mergescript.write("#!/bin/bash\n")
            mergescript.write("cd /afs/cern.ch/user/f/fmonti/work/EoP_harness/CMSSW_10_1_2/\n")
            mergescript.write('eval `scram runtime -sh`\n');
            mergescript.write("cd -\n");
            mergescript.write("hadd -f "+str(options.outdir)+"/EopEta_loop_"+str(iLoop)+".root "+str(options.outdir)+"/EopEta_loop_"+str(iLoop)+"_file_*_*.root\n")
            mergescript.close()
            os.system("chmod 777 "+mergescriptName)
        if task=="ComputeIC_EB":
            mergescriptName=job_parent_folder+"/merge_ComputeIC_EB_loop_"+str(iLoop)+".sh"
            mergescript = open( mergescriptName,"w")
            mergescript.write("#!/bin/bash\n")
            mergescript.write("cd /afs/cern.ch/user/f/fmonti/work/EoP_harness/CMSSW_10_1_2/\n")
            mergescript.write('eval `scram runtime -sh`\n');
            mergescript.write("cd -\n");
            mergescript.write("hadd -f "+str(options.outdir)+"/IC_loop_"+str(iLoop)+".root "+str(options.outdir)+"/IC_loop_"+str(iLoop)+"_file_*_*.root\n")
            if iLoop==0:
                mergescript.write(str(options.exedir)+"/UpdateIC --newIC IC "+str(options.outdir)+"/IC_loop_"+str(iLoop)+".root\n")
            else:
                mergescript.write(str(options.exedir)+"/UpdateIC --oldIC IC "+str(options.outdir)+"/IC_loop_"+str(iLoop-1)+".root --newIC IC "+str(options.outdir)+"/IC_loop_"+str(iLoop)+".root\n")
            mergescript.close()
            os.system("chmod 777 "+mergescriptName)

        mergesubFilename=job_parent_folder+"/submit_merge"+task+"_loop_"+str(iLoop)+".sub"
        mergesub = open( mergesubFilename,"w")
        mergesub.write("executable            = "+mergescriptName+"\n")
        mergesub.write("output                = "+mergescriptName+".$(ClusterId).out\n")
        mergesub.write("error                 = "+mergescriptName+".$(ClusterId).err\n")
        mergesub.write("log                   = "+job_parent_folder+"/log/log.$(ClusterId).log\n")
        mergesub.write('+JobFlavour           = "longlunch"\n')
        mergesub.write("queue 1\n")
        mergesub.close()

        #fill the submitting manager file
        dagFile.write("JOB merge"+task+"_loop_"+str(iLoop)+" "+mergesubFilename+"\n")
            
#setting hierarchy of the submitting manager file
for iLoop in range(0,options.Nloop):
    for iTask in range(0,len(tasklist)):
        dagFile.write("PARENT "+tasklist[iTask]+"_loop_"+str(iLoop)+" CHILD merge"+tasklist[iTask]+"_loop_"+str(iLoop)+"\n")
        if iTask<(len(tasklist)-1):
            dagFile.write("PARENT merge"+tasklist[iTask]+"_loop_"+str(iLoop)+" CHILD "+tasklist[iTask+1]+"_loop_"+str(iLoop)+"\n")
        else:
            if( iLoop < (options.Nloop-1) ):
                dagFile.write("PARENT merge"+tasklist[iTask]+"_loop_"+str(iLoop)+" CHILD "+tasklist[0]+"_loop_"+str(iLoop+1)+"\n")

#add possibility to re-submit failed jobs
for iLoop in range(0,options.Nloop):
    for iTask in range(0,len(tasklist)):
        dagFile.write("Retry "+tasklist[iTask]+"_loop_"+str(iLoop)+" 3\n")
        dagFile.write("Retry merge"+tasklist[iTask]+"_loop_"+str(iLoop)+" 3\n")

dagFile.close()

submit_command = "condor_submit_dag "+dagFilename
print("SUBMIT COMMAND: "+submit_command)
#submit in case the option is given
if(options.submit):
    os.system(submit_command)




