#ifndef CALORIMETER__
#define CALORIMETER__

#include <iostream>
#include <string>
#include <vector>

#include "CfgManager.h"
#include "CfgManagerT.h"
#include "crystal.h"

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
  Float_t  GetESEnergy(const Int_t &i);
  Float_t  GetP(const Int_t &i);
  Float_t  GetPcorrected(const Int_t &i);
  Float_t  GetEtaSC(const Int_t &i);
  Float_t  GetPhi(const Int_t &i);
  Float_t  GetICEnergy(const Int_t &i);
  Float_t  GetIC(const Int_t &index);
  Float_t  GetIC(const Int_t &ieta, const Int_t &iphi);
  void     GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i);
  int      GetietaSeed(const Int_t &i);
  int      GetiphiSeed(const Int_t &i);
  Float_t  GetWeight(const Float_t &Eop,const Float_t &Eta);
  Float_t  GetRegression(const Int_t &i) {return energySCEle[i]/rawEnergySCEle[i];}
  void     GetEtaboundaries(Float_t &ietamin_, Float_t &ietamax_) {ietamin_=ietamin; ietamax_=ietamax;}
  void     GetPhiboundaries(Float_t &iphimin_, Float_t &iphimax_) {iphimin_=iphimin; iphimax_=iphimax;}
  Int_t    GetNeta() {return Neta;}
  Int_t    GetNphi() {return Nphi;}
  std::vector<float>* GetERecHit(const Int_t &i)        {return ERecHit[i];}
  std::vector<float>* GetfracRecHit(const Int_t &i)     {return fracRecHit[i];}
  std::vector<int>*   GetXRecHit(const Int_t &i)        {return XRecHit[i];}
  std::vector<int>*   GetYRecHit(const Int_t &i)        {return YRecHit[i];}
  std::vector<int>*   GetZRecHit(const Int_t &i)        {return ZRecHit[i];}
  std::vector<int>*   GetrecoFlagRecHit(const Int_t &i) {return recoFlagRecHit[i];}
  void LoadMomentumCorrection(std::string filename);
  void LoadEopWeight(const std::vector<std::string> &weightcfg);
  void LoadIC(const std::vector<std::string> &ICcfg);
  void InitializeIC();

 protected:
  void BranchSelected(TChain* chain);
  void BranchExtraCalib(TChain* chain);

  TTreeFormula *selection;
  struct crystal *xtal;
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
  Short_t         xSeed[3];
  Short_t         ySeed[3];
  Float_t         etaEle[3];
  Float_t         phiEle[3];
  Float_t         rawEnergySCEle[3];
  Float_t         energySCEle[3];
  Float_t         etaSCEle[3];
  Float_t         esEnergySCEle[3];
  Float_t         pAtVtxGsfEle[3];
  Float_t         fbremEle[3];

  ///! RecHit variables
  std::vector<float>   *ERecHit[2];
  std::vector<int>     *XRecHit[2];  //iETA
  std::vector<int>     *YRecHit[2];  //iPHI
  std::vector<int>     *ZRecHit[2];
  std::vector<int>     *recoFlagRecHit[2];
  std::vector<float>   *fracRecHit[2];
};


#endif
