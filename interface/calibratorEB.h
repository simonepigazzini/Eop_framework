#ifndef calibratorEB__
#define calibratorEB__

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

class calibratorEB : public calibrator
{

 public:
  //---ctors---
  calibratorEB(CfgManager conf);
  //---dtor---
  ~calibratorEB();
  //---utils--
  Float_t  GetPcorrected(const Int_t &i);
  Float_t  GetRegression(const Int_t &i);
  Float_t  GetICEnergy(const Int_t &i);

};

#endif
