#ifndef ICMANAGER__
#define ICMANAGER__

#include <iostream>
#include <string>
#include <vector>

#include "CfgManager.h"
#include "CfgManagerT.h"

#include "TString.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TFile.h"
#include "TObject.h"

struct crystal
{
  float IC;
  int status;
};

class ICmanager
{

 public:
  //---ctors---
  ICmanager(CfgManager conf);
  ICmanager(const std::vector<std::string> &ICcfg);
  ICmanager(int ietamin, int ietamax, int iphimin, int iphimax);
  //---dtor---
  ~ICmanager();
  //---utils--
  Float_t  GetIC(const Int_t &index);
  Float_t  GetIC(const Int_t &ieta, const Int_t &iphi);
  void     GetEtaboundaries(Float_t &ietamin, Float_t &ietamax) {ietamin=ietamin_; ietamax_=ietamax_;}
  void     GetPhiboundaries(Float_t &iphimin, Float_t &iphimax) {iphimin=iphimin_; iphimax=iphimax_;}
  Int_t    GetNeta() {return Neta_;}
  Int_t    GetNphi() {return Nphi_;}
  void     LoadIC(const std::vector<std::string> &ICcfg);
  void     InitializeIC();

 protected:
  struct crystal *xtal_;
  int Neta_,Nphi_,ietamin_,ietamax_,iphimin_,iphimax_;
};

#endif
