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

typedef  std::map<int,std::map<int,std::map<int,double> > > IC; // ICvalue = IC [ix] [iy] [iz]

struct IOV
{
  UInt_t runmin;
  UShort_t lsmin;
  UInt_t runmax;
  UShort_t lsmax;
  bool Contains(const UInt_t &run, const UShort_t &ls) const
  {
    if(run < runmin)
      return false;
    if(run > runmax)
      return false;
    if( run==runmin && ls<lsmin )
      return false;
    if( run==runmax && ls>lsmax )
      return false;
 
    return true;
  }
  bool GreaterThan(const UInt_t &run, const UShort_t &ls) const
  {
    if(run > runmin)
      return false;
    if(run==runmin && ls > lsmin)
      return false;
    return true;
  }
  bool SmallerThan(const UInt_t &run, const UShort_t &ls) const
  {
    if(run < runmax)
      return false;
    if(run==runmax && ls < lsmax)
      return false;
    return true;
  }
    
};

class ICmanager
{

 public:
  //---ctors---
  ICmanager(CfgManager conf);
  ICmanager(const std::vector<std::string> &ICcfg);
  ICmanager();
  //---dtor---
  ~ICmanager();
  //---utils--
  Float_t  GetIC(const Int_t &ix, const Int_t &iy, const Int_t &iz);
  Float_t  GetIC(const Int_t &ix, const Int_t &iy, const Int_t &iz, const Int_t &iIOV);
  int      FindIOVNumber(const UInt_t &run, const UShort_t &ls);
  int      FindCloserIOVNumber(const UInt_t &run, const UShort_t &ls);
  void     GetXboundaries(const Int_t &iz, Float_t &ixmin, Float_t &ixmax) {ixmin=ixmin_.at(iz); ixmax=ixmax_.at(iz);}
  void     GetYboundaries(const Int_t &iz, Float_t &iymin, Float_t &iymax) {iymin=iymin_.at(iz); iymax=iymax_.at(iz);}
  Int_t    GetNx(const Int_t &iz) {return Nx_.at(iz);}
  Int_t    GetNy(const Int_t &iz) {return Ny_.at(iz);}
  TH2D*    GetHisto(const int &iz, const char* name="IC", const char* title="IC");
  void     LoadIC(TH2D* IC, const int &iz);
  void     LoadIC(const std::vector<std::string> &ICcfg);
  IC       GetICFromtxt(const std::string &txtfilename);
  IC       GetICFromTH2D(TH2D* ICmap, const int &iz);
  void     InitIC(Int_t ICvalue);
  double&  operator()(const Int_t &ix, const Int_t &iy, const Int_t &iz);
  TH2D*    GetPulledIC(TH2D* h2_ICpull, const int &iz);
  TH2D*    PullIC(TH2D* h2_ICpull, const int &iz);
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
  std::vector <IC> timedependent_ICvalues_; //ICvalue = timedependent_ICvalues_[iIOV][ix][iy][iz]
  std::vector <struct IOV> IOVlist_;
  const int izmin_=-1;
  const int izmax_=+1;
  const std::map<int,int> Nx_ =    {{-1,100}, {0,171}, {1,100} };
  const std::map<int,int> ixmin_ = {{-1,1},   {0,-85}, {1,1}   };
  const std::map<int,int> ixmax_ = {{-1,100}, {0,85},  {1,100} };
  const std::map<int,int> Ny_ =    {{-1,100}, {0,360}, {1,100} };
  const std::map<int,int> iymin_ = {{-1,1},   {0,1},   {1,1}   };
  const std::map<int,int> iymax_ = {{-1,100}, {0,360}, {1,100} };

};

TH2D* GetICpull(TH2D* h2_numerator,TH2D* h2_denominator);



int  fromIetaIphito1Dindex(const int &ieta,  const int &iphi, const int &Neta,  const int &Nphi,  const int &ietamin, const int &iphimin);
int  fromTH2indexto1Dindex(const int &binx,  const int &biny, const int &Nbinx, const int &Nbiny);
void from1DindextoIetaIphi(const int &index,       int &ieta,       int &iphi,  const int &Neta,  const int &Nphi,    const int &ietamin, const int &iphimin);
void from1DindextoTH2index(const int &index,       int &binx,       int &biny,  const int &Nbinx, const int &Nbiny);

#endif
