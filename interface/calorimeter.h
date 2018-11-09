#ifndef CALORIMETER__
#define CALORIMETER__

#include <iostream>
#include <string>
#include <vector>

#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"
#include "interface/crystal.h"

#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TTreeFormula.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TFile.h"
#include "TObject.h"

class calorimeter
{

 public:
  //---ctors---
  calorimeter(CfgManager conf);
  //---dtor---
  ~calorimeter();
  //---utils--
  Long64_t GetEntries() {return chain->GetEntries();}
  Long64_t GetEntry(const Long64_t &i);
  Bool_t   isSelected(const Int_t &i) {return selection->EvalInstance(i);}
  Float_t  GetEnergy(const Int_t &i);
  Float_t  GetP(const Int_t &i);
  Float_t  GetPcorrected(const Int_t &i);
  Float_t  GetEtaSC(const Int_t &i);
  Float_t  GetPhi(const Int_t &i);
  Float_t  GetICEnergy(const Int_t &i);
  Float_t  GetIC(const Int_t &iphi, const Int_t &ieta);
  Float_t  GetWeight(const Float_t &Eop,const Float_t &Eta);
  const std::vector<float>* GetRecHit1() {return energyRecHitSCEle1;}
  const std::vector<float>* GetRecHit2() {return energyRecHitSCEle2;}
  void LoadMomentumCorrection(std::string filename);
  void LoadEopWeight(const std::vector<std::string> &weightcfg);
  void LoadIC(const std::vector<std::string> &ICcfg);
  void InitializeIC();

 private:
  void BranchSelected(TChain* chain);
  void BranchExtraCalib(TChain* chain);

  TTreeFormula *selection;
  struct crystal **xtal;
  int Neta,Nphi,ietamin,ietamax,iphimin,iphimax;
  std::map<std::string,TChain*> ch;
  TChain* chain;
  int Ncurrtree;
  bool useRegression;
  TGraphErrors* electron_momentum_correction;
  TGraphErrors* positron_momentum_correction;
  TH2F* weight;
  ///! Declaration of leaf types
  UInt_t          runNumber;
  Int_t           lumiBlock;
  Int_t           eventNumber;
  Short_t         chargeEle[3];
  Float_t         etaEle[3];
  Float_t         phiEle[3];
  Float_t         rawEnergySCEle[3];
  Float_t         energySCEle[3];
  Float_t         etaSCEle[3];
  Float_t         esEnergySCEle[3];
  //Float_t         e3x3SCEle[3];
  Float_t         pAtVtxGsfEle[3];
  Float_t         fbremEle[3];
  //Float_t         energyMCEle[3];
  //Float_t         etaMCEle[3];
  //Float_t         phiMCEle[3];
  ///! Ele 1 variables
  std::vector<float>   *energyRecHitSCEle1;
  std::vector<int>     *XRecHitSCEle1;  //ETA
  std::vector<int>     *YRecHitSCEle1;  //PHI
  std::vector<int>     *ZRecHitSCEle1;
  std::vector<int>     *recoFlagRecHitSCEle1;
  std::vector<float>   *fracRecHitSCEle1;
  ///! Ele 2 variables
  std::vector<float>   *energyRecHitSCEle2;
  std::vector<int>     *XRecHitSCEle2;  //ETA
  std::vector<int>     *YRecHitSCEle2;  //PHI
  std::vector<int>     *ZRecHitSCEle2;
  std::vector<int>     *recoFlagRecHitSCEle2;
  std::vector<float>   *fracRecHitSCEle2;
};


#endif
