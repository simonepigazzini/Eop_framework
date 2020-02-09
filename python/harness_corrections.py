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
                icfile.write("%i\t%i\t0\t%f\t0.\n"%(ieta,iphi,IC))
            

def GetGraph(tree,selection,xname,yname,xuncname="",yuncname=""):
    Npoints = chain.Draw( "%s:%s"%(yname,xname), selection, "goff")
    y = chain.GetV1(); y.SetSize(Npoints)        #trick to handle c-pointers AKA PyDoubleBuffer
    x = chain.GetV2(); x.SetSize(Npoints)
    a_x = array('d',x)
    a_y = array('d',y)

    if(xuncname!=""):
        chain.Draw( xuncname, selection, "goff")
        ex  = chain.GetV1(); ex.SetSize(Npoints)
        a_ex = array('d',ex)
    else:
        a_ex = array('d',(0,)*Npoints)

    if(yuncname!=""):
        chain.Draw( yuncname, selection, "goff")
        ey  = chain.GetV1(); ey.SetSize(Npoints)
        a_ey = array('d',ey)
    else:
        a_ey = array('d',(0,)*Npoints)

    graph = ROOT.TGraphAsymmErrors(Npoints, a_x, a_y, a_ex, a_ex, a_ey, a_ey)
    return graph

def NormalizeGraph(graph,refpointlist):
    #compute mean
    mean=0.
    for ipoint in refpointlist:
        x = ROOT.Double(0) 
        y = ROOT.Double(0) 
        graph.GetPoint(ipoint-1,x,y)
        mean += y
    mean /= len(refpointlist)
    
    #rescale y value of all point by mean value
    for ipoint in range(0,graph.GetN()):
        x = ROOT.Double(0) 
        y = ROOT.Double(0) 
        graph.GetPoint(ipoint,x,y)
        eyl = graph.GetErrorYlow(ipoint)
        eyh = graph.GetErrorYhigh(ipoint)
        graph.SetPoint(ipoint, x, y/mean)
        graph.SetPointEYlow(ipoint, eyl/mean)
        graph.SetPointEYhigh(ipoint, eyh/mean)
        

#parse arguments
parser = OptionParser()
parser.add_option('--DrawDefaultPlots',  action='store_true',        dest='DrawDefaultPlots',   default=False,      help='draw default plots')
parser.add_option('--DrawPlots',      action='store_true',        dest='DrawPlots',   default=False,      help='make per harness plots')
parser.add_option("-i", "--inputdir", action="store", type="str", dest="inputdir",                    help="input directory")
parser.add_option("-x", "--xname",    action="store", type="str", dest="xname",    default="0.5*(timemin+timemax)",  help="name in the tree of the x variable")
parser.add_option("--t_min",    action="store", type="float", dest="t_min",    help="time min")
parser.add_option("--t_max",    action="store", type="float", dest="t_max",    help="time max")
parser.add_option("--xlowname",       action="store", type="str", dest="xlowname", default="timemin",  help="name in the tree of the xlow variable")
parser.add_option("--xupname",        action="store", type="str", dest="xupname",  default="timemax",  help="name in the tree of the xup variable")
parser.add_option("-y", "--yname",    action="store", type="str", dest="yname",  default="scale_Eop_mean", help="name in the tree of the y variable")
parser.add_option("--yuncname",           action="store", type="str", dest="yuncname", default="",     help="name in the tree of the y uncertainty variable")
#parser.add_option("--NormalizetoIOV",     action="store", type="str", dest="NormalizetoIOV", default="0",  help="normalize scale to the average of the scale in the given IOV (e.g. 1,2,3) the default is 0 --> not normalized")
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
ROOT.gErrorIgnoreLevel = ROOT.kWarning #switch off 'Info in <TCanvas::Print>: blabla'

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
        print "[WARNING]: %s chain is empty --> skip it"%harnessname
        continue
    graph_dict[harnessname] = GetGraph(chain,
                                       "timemin>%f && timemax<%f"%(options.t_min,options.t_max),#selection
                                       options.xname, options.yname,                            #x,y variables names
                                       "",            options.yuncname)                         #ex,ey, variables names

print "%i graphs created"%len(graph_dict)

if options.DrawDefaultPlots:
    c = ROOT.TCanvas()
    os.system("mkdir %s/defaultplots"%outdir)
    for harnessname, chain in chain_dict.items():
        #print harnessname+" - "+str(chain.GetEntries())
        if chain.GetEntries()<=0: 
            continue

        templatefit_graph = GetGraph(chain,
                                     "timemin>%f && timemax<%f"%(options.t_min,options.t_max),#selection
                                     options.xname, "scale_Eop_templatefit",                  #x,y variables names
                                     "",            "scaleunc_Eop_templatefit")               #ex,ey, variables names
        mean_graph = GetGraph(chain,
                              "timemin>%f && timemax<%f"%(options.t_min,options.t_max),#selection
                              options.xname, "scale_Eop_mean",                  #x,y variables names
                              "",            "scaleunc_Eop_mean")               #ex,ey, variables names
        median_graph = GetGraph(chain,
                                "timemin>%f && timemax<%f"%(options.t_min,options.t_max),#selection
                                options.xname, "scale_Eop_median",                  #x,y variables names
                                "",            "scaleunc_Eop_median")               #ex,ey, variables names

        NormalizeGraph(templatefit_graph,[1])
        NormalizeGraph(mean_graph,[1])
        NormalizeGraph(median_graph,[1])

        templatefit_graph.SetMarkerStyle(20)
        mean_graph.SetMarkerStyle(21)
        median_graph.SetMarkerStyle(22)
        templatefit_graph.SetMarkerColor(1)
        mean_graph.SetMarkerColor(2)
        median_graph.SetMarkerColor(3)

        multigraph = ROOT.TMultiGraph()
        multigraph.Add(templatefit_graph)
        multigraph.Add(mean_graph)
        multigraph.Add(median_graph)

        multigraph.Draw("AP")
        if("time" in options.xname):
            multigraph.GetXaxis().SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
            multigraph.GetXaxis().SetTimeDisplay(1);
        SetAxisTitle(multigraph.GetXaxis(),options.xname)
        multigraph.GetYaxis().SetTitle("E/p scale")

        legend = ROOT.TLegend(0.12,0.12,0.48,0.3)
        legend.AddEntry(templatefit_graph, "template fit", "p")
        legend.AddEntry(mean_graph,        "mean",         "p")
        legend.AddEntry(median_graph,      "median",       "p")
        legend.Draw()

        c.Print("%s/defaultplots/%s.png"%(outdir,harnessname))
        #c.Print("%s/defaultplots/%s.pdf"%(outdir,harnessname))
        #c.SaveAs("%s/defaultplots/%s.root"%(outdir,harnessname))
        #c.SaveAs("%s/defaultplots/%s.C"%(outdir,harnessname))
        c.Clear()

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
        #print "doing harness", harnessname
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
                    if Npoints!=len(IOV_list):
                        print "[ERROR]: missing IOVs in harness "+harnessname
                        exit()
                    if ( runmin[iIOV]!=IOV[0] or lsmin[iIOV]!=IOV[1] or runmax[iIOV]!=IOV[2] or lsmax[iIOV]!=IOV[3] ):
                        print "[ERROR]: IOV mismatching in harness "+harnessname
                        exit()
                    else:
                        ietamin,ietamax,iphimin,iphimax = HarnessLimits(harnessname)
                        for ieta in range(ietamin,ietamax+1):
                            for iphi in range(iphimin,iphimax+1):
                                if(Escale[iIOV] != 0):
                                    ICvalue = 1./Escale[iIOV]
                                else:
                                    ICvalue = 1.
                                if(ICvalue<0 or ICvalue>10):
                                    ICvalue = 1.
                                IC[iIOV][ieta][iphi] = ICvalue

    print "writing output txt files"
    os.system("mkdir -p %s/PointCorrections/"%outdir)
    IOVdict_filename="%s/PointCorrections/IOVdictionary.txt"%outdir
    IOVdict_file = open(IOVdict_filename,"w")
    #IOVdict_file.write("RUNMIN\tLSMIN\tRUNMAX\tLSMAX\tICFILENAME\n")
    for iIOV in range(0,len(IOV_list)):
        icfilename = "%s/PointCorrections/IC%i.txt"%(outdir,iIOV)
        IOVdict_file.write("%i\t%i\t%i\t%i\t%s\n"%(IOV_list[iIOV][0],IOV_list[iIOV][1],IOV_list[iIOV][2],IOV_list[iIOV][3],icfilename))
        writeIC(icfilename,IC[iIOV])
    IOVdict_file.close()
                    
                    
                
