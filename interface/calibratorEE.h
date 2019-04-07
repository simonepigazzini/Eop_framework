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
  Float_t  GetRegression(const Int_t &i) {return energySCEle_[i]/(rawEnergySCEle_[i]+esEnergySCEle_[i]);}
  Float_t  GetICEnergy(const Int_t &i);
  
  Int_t    GetEERingSeed(const Int_t &i);
  void     GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i);
  int      GetietaSeed(const Int_t &i);
  int      GetiphiSeed(const Int_t &i);
  int      GetixSeed(const Int_t &i) {return xSeed_[i];}
  int      GetiySeed(const Int_t &i) {return ySeed_[i];}

 private:
  TEndcapRings* eeRing_;

};

#endif
