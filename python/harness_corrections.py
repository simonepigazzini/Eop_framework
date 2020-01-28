#!/bin/python
import os
import glob
import math
from array import array
import sys
from optparse import OptionParser

def HarnessLimits(harnessname):
    
    folder_args = ROOT.TString(harnessname.replace("/","_")).Tokenize("_");
    #folder_args.Print();
    s_ietamin = ROOT.TString()
    s_ietamax = ROOT.TString()
    s_iphimin = ROOT.TString()
    s_iphimax = ROOT.TString()
    for i in range(0,folder_args.GetEntries()):
        if folder_args.At(i).String() == "IEta":
            s_ietamin = folder_args.At(i+1).String()
            s_ietamax = folder_args.At(i+2).String()
        elif folder_args.At(i).String() == "IPhi":
            s_iphimin = folder_args.At(i+1).String()
            s_iphimax = folder_args.At(i+2).String()
    return s_ietamin.Atoi(), s_ietamax.Atoi(), s_iphimin.Atoi(), s_iphimax.Atoi()

def SetAxisTitle(axis,name):
    if "time" in name:
        axis.SetTitle("date")
    elif "lumi" in name:
        axis.SetTitle("Integrated luminosity (fb^{-1})")
    elif "median" in name:
        axis.SetTitle("E/p scale (median)")
    elif "mean" in name:
        axis.SetTitle("E/p scale (mean)")
    elif "template" in name:
        axis.SetTitle("E/p scale (template fit)")

def writeIC(icfilename,ICmap):
    with open(icfilename,'w') as icfile:
        for ieta,iphi_ICmap in ICmap.items():
            for iphi,IC in iphi_ICmap.items():
                icfile.write("%i\t%i\t0\t%f\n"%(ieta,iphi,IC))
            

#parse arguments
parser = OptionParser()
parser.add_option('--DrawDefaultPlots',  action='store_true',        dest='default_plots',   default=False,      help='draw default plots')
parser.add_option('--DrawPlots',      action='store_true',        dest='DrawPlots',   default=False,      help='make per harness plots')
parser.add_option("-i", "--inputdir", action="store", type="str", dest="inputdir",                    help="input directory")
parser.add_option("-x", "--xname",    action="store", type="str", dest="xname",    default="0.5*(timemin+timemax)",  help="name in the tree of the x variable")
parser.add_option("--t_min",    action="store", type="float", dest="t_min",    help="time min")
parser.add_option("--t_max",    action="store", type="float", dest="t_max",    help="time max")
parser.add_option("--xlowname",       action="store", type="str", dest="xlowname", default="timemin",  help="name in the tree of the xlow variable")
parser.add_option("--xupname",        action="store", type="str", dest="xupname",  default="timemax",  help="name in the tree of the xup variable")
parser.add_option("-y", "--yname",    action="store", type="str", dest="yname",  default="scale_Eop_mean", help="name in the tree of the y variable")
parser.add_option("--yuncname",           action="store", type="str", dest="yuncname", default="",     help="name in the tree of the y uncertainty variable")
parser.add_option("--GetPointCorrections", action='store_true',        dest='GetPointCorrections',   default=False,
                  help='extract an IC set for each time bin and save it in a output txt file')
parser.add_option("-o", "--outdir",    action="store",      type="str", dest="outdir",          default="",       help="output directory")

(options, args) = parser.parse_args()

#create outdir
if(options.outdir == ""):
    outdir=options.inputdir
else:
    outdir=options.outdir
    os.system("mkdir -p "+outdir)

import ROOT as ROOT
ROOT.gROOT.SetBatch(1)

#build the dictionary of the TChains
print "building the chains"
chain_dict = {}
for dirname in os.listdir(options.inputdir):
    fullpath = os.path.join(options.inputdir,dirname)
    if not os.path.isdir(fullpath):
        continue
    if dirname.find("IEta")!=-1 and dirname.find("IPhi")!=-1:
        harnessname = dirname
        chain_dict[harnessname] = ROOT.TChain(harnessname,harnessname)
        chain_dict[harnessname].Add(fullpath+"/out_file_*_scalemonitoring.root")#to be properly modified 

print "%i chains created"%len(chain_dict)

#build the dictionary of the TGraphAsymmErrors
print "building y vs x graphs"
graph_dict = {}
for harnessname, chain in chain_dict.items():
    #print harnessname+" - "+str(chain.GetEntries())
    if chain.GetEntries()<=0: 
        continue
    if(options.yuncname!=""):
        Npoints = chain.Draw( "%s:%s:%s"%(options.yname,options.xname,options.yuncname), 
                              "timemin>%f && timemax<%f"%(options.t_min,options.t_max),
                              "goff")
        y   = chain.GetV1()
        x   = chain.GetV2()
        ey  = chain.GetV3()
        #trick to handle c-pointers AKA PyDoubleBuffer
        x.SetSize(Npoints)
        y.SetSize(Npoints)
        ey.SetSize(Npoints)
        graph_dict[harnessname] = ROOT.TGraphAsymmErrors(Npoints, 
                                                         array('d',x), array('d',y), 
                                                         array('d',(0,)*Npoints), array('d',(0,)*Npoints), 
                                                         array('d',ey), array('d',ey))
    else:
        Npoints = chain.Draw( "%s:%s"%(options.yname,options.xname), 
                              "timemin>%f && timemax<%f"%(options.t_min,options.t_max),
                              "goff")
        y   = chain.GetV1()
        x   = chain.GetV2()
        #trick to handle c-pointers AKA PyDoubleBuffer
        x.SetSize(Npoints)
        y.SetSize(Npoints)
        graph_dict[harnessname] = ROOT.TGraphAsymmErrors(Npoints, 
                                                         array('d',x), array('d',y), 
                                                         array('d',(0,)*Npoints), array('d',(0,)*Npoints),
                                                         array('d',(0,)*Npoints), array('d',(0,)*Npoints))

print "%i graphs created"%len(graph_dict)

#draw the "scale vs time"-like plots
if options.DrawPlots:
    print "Drawing y vs x graphs"
    os.system("mkdir -p %s/fit/EBm/"%outdir)
    os.system("mkdir -p %s/fit/EBp/"%outdir)
    c = ROOT.TCanvas()
    for harnessname, graph in graph_dict.items():
        if graph.GetN()==0: continue
    
        xmin_graph = ROOT.TMath.MinElement(graph.GetN(),graph.GetX())
        xmax_graph = ROOT.TMath.MaxElement(graph.GetN(),graph.GetX())
        ietamin,ietamax,iphimin,iphimax = HarnessLimits(harnessname)
        graph.SetTitle("PN region: %i #leq i#eta #leq %i, %i #leq i#phi #leq %i"%(ietamin,ietamax,iphimin,iphimax)) 
        graph.SetMarkerStyle(20)
        graph.Draw("AP")
        graph.GetXaxis().SetLimits( xmin_graph-0.07*(xmax_graph-xmin_graph), xmax_graph+0.07*(xmax_graph-xmin_graph))
        if("time" in options.xname):
            graph.GetXaxis().SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
            graph.GetXaxis().SetTimeDisplay(1);

        SetAxisTitle(graph.GetXaxis(),options.xname)
        SetAxisTitle(graph.GetYaxis(),options.yname)
        graph.GetXaxis().SetTitleOffset(0.8)
        graph.GetXaxis().SetLabelSize(0.03)

        if ietamin<0:
            #        c.Print("%s/fit/EBm/%s.pdf"%(outdir,harnessname))
            c.Print("%s/fit/EBm/%s.png"%(outdir,harnessname))
            #        c.SaveAs("%s/fit/EBm/%s.root"%(outdir,harnessname))
            #        c.SaveAs("%s/fit/EBm/%s.C"%(outdir,harnessname))
        else: 
            #        c.Print("%s/fit/EBp/%s.pdf"%(outdir,harnessname))
            c.Print("%s/fit/EBp/%s.png"%(outdir,harnessname))
            #        c.SaveAs("%s/fit/EBp/%s.root"%(outdir,harnessname))
            #        c.SaveAs("%s/fit/EBp/%s.C"%(outdir,harnessname))
        c.Clear()

if options.GetPointCorrections:
    print "Creating point corrections"
    IOV_list = []
    IC = {} #IC is a list of dictionaries, i.e. IC [iIOV] [ix] [iy] 
    print "Loop over harnesses"
    for harnessname, chain in chain_dict.items():
        print "doing harness", harnessname
        if chain.GetEntries()!=0:
            Npoints = chain.Draw( "%s:runmin:lsmin:runmax:lsmax"%options.yname, 
                                  "timemin>%f && timemax<%f"%(options.t_min,options.t_max),
                                  "goff")
            
            Escale   = chain.GetVal(0)
            runmin   = chain.GetVal(1)    
            lsmin    = chain.GetVal(2)
            runmax   = chain.GetVal(3)
            lsmax    = chain.GetVal(4)
            Escale.SetSize(Npoints)
            runmin.SetSize(Npoints)
            lsmin.SetSize(Npoints)
            runmax.SetSize(Npoints)
            lsmax.SetSize(Npoints)
            if len(IOV_list)==0:
                for iIOV in range(0,Npoints):
                    #print "iIOV", iIOV
                    IOV = [runmin[iIOV],lsmin[iIOV],runmax[iIOV],lsmax[iIOV]]
                    IOV_list.append(IOV)
                    IC[iIOV] = {}
                    for ieta in range(-85,86):
                        IC[iIOV][ieta]={}
                    
            else:
                for iIOV in range(0,len(IOV_list)):
                    IOV = IOV_list[iIOV]
                    if ( runmin[iIOV]!=IOV[0] or lsmin[iIOV]!=IOV[1] or runmax[iIOV]!=IOV[2] or lsmax[iIOV]!=IOV[3] ):
                        print "[ERROR]: IOV mismatching"
                        exit()
                    else:
                        ietamin,ietamax,iphimin,iphimax = HarnessLimits(harnessname)
                        for ieta in range(ietamin,ietamax+1):
                            for iphi in range(iphimin,iphimax+1):
                                IC[iIOV][ieta][iphi] = Escale[iIOV]

    print "writing output txt files"
    os.system("mkdir -p %s/PointCorrections/"%outdir)
    IOVdict_filename="%s/PointCorrections/IOVdictionary.txt"%outdir
    IOVdict_file = open(IOVdict_filename,"w")
    IOVdict_file.write("RUNMIN\tLSMIN\tRUNMAX\tLSMAX\tICFILENAME\n")
    for iIOV in range(0,len(IOV_list)):
        icfilename = "%s/PointCorrections/IC%i.txt"%(outdir,iIOV)
        IOVdict_file.write("%i\t%i\t%i\t%i\t%s\n"%(IOV_list[iIOV][0],IOV_list[iIOV][1],IOV_list[iIOV][2],IOV_list[iIOV][3],icfilename))
        writeIC(icfilename,IC[iIOV])
    IOVdict_file.close()
                    
                    
                
