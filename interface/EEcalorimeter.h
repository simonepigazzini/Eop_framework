#ifndef EECALORIMETER__
#define EECALORIMETER__

#include <iostream>
#include <string>
#include <vector>

#include "CfgManager.h"
#include "CfgManagerT.h"
#include "crystal.h"
#include "TEndcapRings.h"
#include "calorimeter.h"

#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TTreeFormula.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TFile.h"
#include "TObject.h"

class EEcalorimeter : public calorimeter
{

 public:
  //---ctors---
  EEcalorimeter(CfgManager conf);
  //---dtor---
  ~EEcalorimeter();
  //---utils--
  Float_t  GetPcorrected(const Int_t &i);
  Float_t  GetRegression(const Int_t &i) {return energySCEle[i]/(rawEnergySCEle[i]+esEnergySCEle[i]);}
  Float_t  GetICEnergy(const Int_t &i);
  
  Int_t    GetEERingSeed(const Int_t &i);
  void     GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i);
  int      GetietaSeed(const Int_t &i);
  int      GetiphiSeed(const Int_t &i);
  int      GetixSeed(const Int_t &i) {return xSeed[i];}
  int      GetiySeed(const Int_t &i) {return ySeed[i];}

 private:
  TEndcapRings* eeRing;

};

#endif
