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
  Float_t  GetEnergy(const Int_t i);
  const std::vector<float>* GetRecHit1();
  void Initialize(const std::string &rootfile);
  void Initialize();

 private:
  void BranchSelected(TChain* chain);
  void BranchExtraCalib(TChain* chain);

  struct crystal **xtal;
  int Neta;
  int Nphi;
  std::map<std::string,TChain*> ch;
  TChain* chain;
  int Ncurrtree;
  bool useRegression;
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
  ///! Ele 2 variables
  std::vector<float>   *energyRecHitSCEle2;
  std::vector<int>     *XRecHitSCEle2;  //ETA
  std::vector<int>     *YRecHitSCEle2;  //PHI
  std::vector<int>     *ZRecHitSCEle2;
  std::vector<int>     *recoFlagRecHitSCEle2;
  
};


#endif
