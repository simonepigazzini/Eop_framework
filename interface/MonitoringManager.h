#ifndef MONITORINGMANAGER__
#define MONITORINGMANAGER__

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "CfgManager.h"
#include "CfgManagerT.h"
#include "TimeBin.h"

#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"
#include "TObject.h"
#include "calibrator.h"

class MonitoringManager: public calibrator
{

 public:
  //---ctors---
  MonitoringManager(CfgManager conf);
  //---dtor---
  ~MonitoringManager();
  //---utils--
  TH1F* BuildTemplate();
  void  RunDivide();
  void  SaveTimeBins(std::string outfilename, std::string writemethod="RECREATE");
  void  LoadTimeBins(std::vector<UInt_t>& runs, std::vector<UInt_t>& times, std::string option="");
  void  LoadTimeBins(string inputfilename, string objname="", std::string option="");
  void  LoadIntegratedLuminosity(string intlumi_vs_time_filename);
  void  FillTimeBins();
  void  fitScale();
  std::vector<TimeBin>::iterator FindBin(const UInt_t &run, const UShort_t &ls);
  std::vector<TimeBin>::iterator FindBin(const UInt_t &run, const UShort_t &ls, const UInt_t &time );

  //void  AddScale(std::string variable, int Nbin, float xmin, float xmax){};
  void  RunTemplateFit(string scale);
  void  RunComputeMean(string scale);
  void  RunComputeMedian(string scale);
  //void  SaveScales(TFile* outfile){};
  //void  saveHistos(TFile* outfile){};
  void  PrintScales();
  float GetScaleVariableValue(const int &iEle);
  
 protected:
  enum kvariabletype {kregular, kICenergy_over_p,kICMee};
  int variabletype_;
  std::vector<TimeBin>::iterator last_accessed_bin_;
  std::vector<TimeBin> timebins;
  std::string variablename_;
  std::string label_;
  TH1F* h_template_;
  CfgManager conf_;
  bool BookHistos();
  void SetScaleVariable(const string &variablename);
};

#endif
