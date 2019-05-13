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
  Float_t  GetRegression(const Int_t &i) {return energySCEle_[i]/rawEnergySCEle_[i];}
  void     LoadMomentumCorrection(std::string filename);
  void     LoadEopWeight(const std::vector<std::string> &weightcfg);

 protected:
  bool useRegression_;
  TGraphErrors* electron_momentum_correction_;
  TGraphErrors* positron_momentum_correction_;
  TH2F* weight_;
};

#endif