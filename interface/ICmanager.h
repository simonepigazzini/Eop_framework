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
  TH2D*    GetHisto(const char* name="IC", const char* title="IC");
  void     LoadIC(TH2D* IC);
  void     LoadIC(const std::vector<std::string> &ICcfg);
  void     InitializeIC();
  //TH2D* EtaringNormalizationEB(); //TBD
  //TH2D* EtaringNormalizationEE(); //TBD
  //TH1D* GetICspread(); //TBD
  //TGraphErrors* GetICspreadvsEtaEB(); //TBD
  //TGraphErrors* GetICspreadvsEtaEE(); //TBD
  //TGraphErrors* GetAvgICvsEtaEB();
  //TGraphErrors* GetAvgICvsEtaEE();
  //TGraphErrors* GetICspreadvsPhiEB(); //TBD
  //TGraphErrors* GetICspreadvsPhiEE(); //TBD
  //TGraphErrors* GetAvgICvsPhiEB();
  //TGraphErrors* GetAvgICvsPhiEE();
  //TGraphErrors* GetPhiFoldProfileEB(int ietamin, int ietamax, int PhiPeriod );
  //TGraphErrors* SupermoduleGapCorrectionEB(int ietamin, int ietamax, int PhiPeriod );
  //void SaveICAs(const char *output);
  bool EB;
  
 protected:
  struct crystal *xtal_;
  int Neta_,Nphi_,ietamin_,ietamax_,iphimin_,iphimax_;

};

#endif
