#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"
#include "interface/SetTDRStyle.h"
#include "interface/Plotter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TCanvas.h" 
#include "TH1F.h"
#include "TH2F.h"
#include "TEfficiency.h"
#include "TLorentzVector.h"
#include "TGraph.h"
#include "TLegend.h"



int main(int argc, char** argv)
{
  if( argc < 2 )
  {
    std::cerr << ">>>>> drawComparisonPlots.cpp::usage:   " << argv[0] << " configFileName" << std::endl;
    return -1;
  }

  
  //----------------------
  // parse the config file
  CfgManager opts;
  opts.ParseConfigFile(argv[1]);
  
  //--- open files and get trees
  std::vector<std::string> vars = opts.GetOpt<std::vector<std::string> >("Input.vars");
  std::string outputFolder = opts.GetOpt<std::string>("Output.outputFolder");
  
  setTDRStyle();
  
  Plotter myPlotter(opts,vars,outputFolder);
  myPlotter.DrawPlots();
  
  
  return 0;
}
