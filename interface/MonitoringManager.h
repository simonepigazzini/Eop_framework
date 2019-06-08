#ifndef MONITORINGMANAGER__
#define MONITORINGMANAGER__

#include <iostream>
#include <string>
#include <vector>

#include "CfgManager.h"
#include "CfgManagerT.h"

#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TTreeFormula.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"
#include "TObject.h"
#include "ECALELFInterface.h"

class MonitoringManager: public ECALELFInterface
{

 public:
  //---ctors---
  MonitoringManager(CfgManager conf);
  //---dtor---
  ~MonitoringManager();
  //---utils--
  void  SetTemplateModel(int Nbin, float xmin, float xmax);
  TH1F* BuildTemplate();
  void  RunDivide(){};
  void  LoadTimeBins(){};
  void  AddScale(std::string variable, int Nbin, float xmin, float xmax){};
  void  FillTimeBins(){};
  void  RunTemplateFit(string scale){};
  void  RunComputeMean(string scale){};
  void  RunComputeMedian(string scale){};
  void  SaveScales(TFile* outfile){};
  void  saveHistos(TFile* outfile){};

  
 protected:
  std::string variable_;
  std::string label_;
  TH1F* h_template_;
  CfgManager conf_;
};

#endif
