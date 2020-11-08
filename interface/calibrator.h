#ifndef CALIBRATOR__
#define CALIBRATOR__

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
#include "TFile.h"
#include "TObject.h"
#include "ICmanager.h"
#include "ECALELFInterface.h"

class calibrator: public ICmanager, public ECALELFInterface
{

 public:
  //---ctors---
  calibrator(CfgManager conf);
  //---dtor---
  ~calibrator();
  //---utils--
  Float_t  GetPcorrected(const Int_t &i);
  Float_t  GetICEnergy(const Int_t &i);
  Float_t  GetWeight(const Float_t &Eop,const Float_t &Eta);
  Float_t  GetRegression(const Int_t &i);
  void     LoadMomentumCorrection(std::string filename);
  void     LoadEopWeight(const std::vector<std::string> &weightcfg);
  void     PrintSettings();

 protected:
  bool EScorrection_;    //if true, correct energy and momentum for preshower effect. By default it should be true for intercalibration and false for momentum calibration and monitoring. The effectiveness of this correction should be actually studied in future...
  bool useRegression_;
  TGraphErrors* electron_momentum_correction_;
  TGraphErrors* positron_momentum_correction_;
  TH2F* weight_;

 private:
  Float_t  GetPcorrectedEB(const Int_t &i);
  Float_t  GetPcorrectedEE(const Int_t &i);
  Float_t  GetICEnergyEB(const Int_t &i);
  Float_t  GetICEnergyEE(const Int_t &i);
  Float_t  GetRegressionEB(const Int_t &i);
  Float_t  GetRegressionEE(const Int_t &i);
  
};

#endif
