#ifndef ICMANAGER__
#define ICMANAGER__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "CfgManager.h"
#include "CfgManagerT.h"

#include "TString.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TFile.h"
#include "TObject.h"
#include "TF1.h"

struct crystal
{
  double IC;
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
  Float_t  GetIC(const Int_t &ieta, const Int_t &iphi);
  void     GetEtaboundaries(Float_t &ietamin, Float_t &ietamax) {ietamin=ietamin_; ietamax=ietamax_;}
  void     GetPhiboundaries(Float_t &iphimin, Float_t &iphimax) {iphimin=iphimin_; iphimax=iphimax_;}
  Int_t    GetNeta() {return Neta_;}
  Int_t    GetNphi() {return Nphi_;}
  TH2D*    GetHisto(const char* name="IC", const char* title="IC");
  void     LoadIC(TH2D* IC);
  void     LoadIC(const std::vector<std::string> &ICcfg);
  void     LoadIC(std::ifstream &infile);
  void     InitIC(Int_t ICvalue);
  double&  operator()(const Int_t &ieta, const Int_t &iphi);
  TH2D*    GetPulledIC(TH2D* h2_ICpull);
  TH2D*    PullIC(TH2D* h2_ICpull);
  void     EtaringNormalizationEB();
  void     PrintSettings();
  //TH2D* EtaringNormalizationEE(); //TBD
  TH1D*    GetICspread(int nBins_spread = 2000, float spreadMin = 0., float spreadMax = 2.);
  TGraphErrors* GetICspreadvsEtaEB(int nBins_spread = 2000, float spreadMin = 0., float spreadMax = 2.);
  //TGraphErrors* GetICspreadvsEtaEE(); //TBD
  TGraphErrors* GetAvgICvsEtaEB();
  //TGraphErrors* GetAvgICvsEtaEE();
  TGraphErrors* GetICspreadvsPhiEB(int nBins_spread = 2000, float spreadMin = 0., float spreadMax = 2.);
  //TGraphErrors* GetICspreadvsPhiEE(); //TBD
  TGraphErrors* GetAvgICvsPhiEB();
  //TGraphErrors* GetAvgICvsPhiEE();
  TGraphErrors* GetPhiFoldProfileEB(int ietamin, int ietamax, int PhiPeriod );
  void SupermoduleGapCorrectionEB(int ietamin, int ietamax, int PhiPeriod );
  TH2D* GetStatPrec(ICmanager* IC2);
  //  TGraphErrors* GetStatPrecvsEtaringEB(ICmanager* IC2);
  //TGraphErrors* GetStatPrecvsEtaringEE(ICmanager* IC2);//TBD
  //TGraphErrors* GetResidualSpreadvsEtaringEB(ICmanager* IC2);//TBD
  //TGraphErrors* GetResidualSpreadvsEtaringEE(ICmanager* IC2);//TBD
  //void SaveICAs(const char *output);
  bool EB;
  
 protected:
  struct crystal *xtal_;
  int Neta_,Nphi_,ietamin_,ietamax_,iphimin_,iphimax_;

 private:
  Float_t  GetIC(const Int_t &index);
  void     CreateIC();

};

TH2D* GetICpull(TH2D* h2_numerator,TH2D* h2_denominator);



int  fromIetaIphito1Dindex(const int &ieta,  const int &iphi, const int &Neta,  const int &Nphi,  const int &ietamin, const int &iphimin);
int  fromTH2indexto1Dindex(const int &binx,  const int &biny, const int &Nbinx, const int &Nbiny);
void from1DindextoIetaIphi(const int &index,       int &ieta,       int &iphi,  const int &Neta,  const int &Nphi,    const int &ietamin, const int &iphimin);
void from1DindextoTH2index(const int &index,       int &binx,       int &biny,  const int &Nbinx, const int &Nbiny);

#endif
