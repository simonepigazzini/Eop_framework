#!/bin/python
import os
import glob
from optparse import OptionParser
import harness_definition
#parse arguments
parser = OptionParser()
parser.add_option('--submit',          action='store_true',             dest='submit',          default=False,      help='submit jobs')
parser.add_option("--expectedoutput",  action="store",      type="str", dest="expectedoutput",                      help="expected output filename syntax (keywords are HARNESS and FILE)")
parser.add_option("--Nfiles",          action="store",      type="int", dest="Nfile",           default=1,          help="number of output files for each harness (keywords are HARNESS and FILE)")
parser.add_option("--condorsub",       action="store",      type="str", dest="condorsub",                           help="condor submission file")
parser.add_option("--script",          action="store",      type="str", dest="script",                              help="script syntax")

(options, args) = parser.parse_args()

missing_harness = []
missing_scripts = []
for harness_range in harness_definition.GetHarnessRanges():
    etamin = harness_range[0]
    etamax = harness_range[1]
    phimin = harness_range[2]
    phimax = harness_range[3]
    harnessname = "IEta_%i_%i_IPhi_%i_%i"%(etamin,etamax,phimin,phimax)
    for ifile in range(0,options.Nfile):
        outfilename = options.expectedoutput.replace("HARNESS",harnessname).replace("FILE",str(ifile))
        if not (os.path.isfile(outfilename)):
            scriptname = options.script.replace("HARNESS",harnessname).replace("FILE",str(ifile))
            missing_harness.append(harnessname)
            missing_scripts.append(scriptname)

if(len(missing_harness)==0):
    print "all jobs succeded --> exit"
    exit()

else:
    print "%i jobs to resubmit"%len(missing_harness)
    print missing_harness
    condorResubFilename = options.condorsub.replace(".sub","_resubmit.sub")
    job_parent_folder = os.path.dirname(options.condorsub)
    condorsub = open( condorResubFilename,"w")
    condorsub.write("executable            = $(scriptname)\n")
    condorsub.write("output                = $(scriptname).$(ClusterId).out\n")
    condorsub.write("error                 = $(scriptname).$(ClusterId).err\n")
    condorsub.write("log                   = "+job_parent_folder+"/log/log.$(ClusterId).log\n")
    condorsub.write('+JobFlavour           = "workday"\n')
    condorsub.write("queue scriptname matching ( \\ \n")
    for scriptname in missing_scripts:
        condorsub.write("%s \\ \n"%scriptname)
    condorsub.write(")\n")
    condorsub.close()
    print "SUBMIT COMMAND: condor_submit "+condorResubFilename
    if options.submit:
        os.system("condor_submit "+condorResubFilename)
