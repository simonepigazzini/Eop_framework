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
  void  BuildRunRanges(){};
  void  RunMonitoring(){};
  
 protected:
  std::string scalename_;
  std::string label_;
  float xmin_template_, xmax_template_;
  int   Nbin_template_;
  TH1F* h_template_;

};

#endif
