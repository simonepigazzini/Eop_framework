#ifndef calibratorEE__
#define calibratorEE__

#include <iostream>
#include <string>
#include <vector>

#include "CfgManager.h"
#include "CfgManagerT.h"
#include "TEndcapRings.h"
#include "calibrator.h"

#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TTreeFormula.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TFile.h"
#include "TObject.h"

class calibratorEE : public calibrator
{

 public:
  //---ctors---
  calibratorEE(CfgManager conf);
  //---dtor---
  ~calibratorEE();
  //---utils--
  Float_t  GetPcorrected(const Int_t &i);
  Float_t  GetRegression(const Int_t &i);
  Float_t  GetICEnergy(const Int_t &i);
  
 private:
  TEndcapRings* eeRing_; //for now useless but maybe useful in the future
  bool EScorrection_;    //if true, correct energy and momentum for preshower effect. By default it should be true for intercalibration and false for momentum calibration and monitoring. The effectiveness of this correction should be actually studied in future...
};

#endif
