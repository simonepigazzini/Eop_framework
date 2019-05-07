#include "ICmanager.h"
#include "utils.h"

using namespace std;

ICmanager::ICmanager(CfgManager conf):
  xtal_(0)
{
  //-------------------------------------
  //load input IC
  if(conf.OptExist("Input.inputIC"))
    LoadIC( conf.GetOpt<std::vector<std::string> > ("Input.inputIC") );
  else
    if(conf.OptExist("Input.ietamin") && conf.OptExist("Input.ietamax") && conf.OptExist("Input.iphimin") && conf.OptExist("Input.iphimax"))
    {
      ietamin_ = conf.GetOpt<int> ("Input.ietamin");
      ietamax_ = conf.GetOpt<int> ("Input.ietamax");
      iphimin_ = conf.GetOpt<int> ("Input.iphimin");
      iphimax_ = conf.GetOpt<int> ("Input.iphimax");
      InitializeIC();
    }
    else
      cout<<"[WARNING]: no inputIC, nor ietamin&&ietamax&&iphimin&&iphimax found in Input in cfg"<<endl;
  
}

ICmanager::ICmanager(const std::vector<std::string> &ICcfg):
  xtal_(0)
{
  LoadIC( ICcfg );
}

ICmanager::ICmanager(int ietamin, int ietamax, int iphimin, int iphimax):
  xtal_(0),
  ietamin_(ietamin),
  ietamax_(ietamax),
  iphimin_(iphimin),
  iphimax_(iphimax)
{  
  InitializeIC();
}  

ICmanager::~ICmanager()
{
  delete [] xtal_;
}

void ICmanager::LoadIC(TH2D* ICmap)
{
  Neta_=ICmap->GetNbinsY();
  ietamin_=ICmap->GetYaxis()->GetXmin();
  ietamax_=ICmap->GetYaxis()->GetXmax()-1;//i want the left limit of last bin, not the right one
  Nphi_=ICmap->GetNbinsX();
  iphimin_=ICmap->GetXaxis()->GetXmin();
  iphimax_=ICmap->GetXaxis()->GetXmax()-1;//i want the left limit of last bin, not the right one
  //cout<<">>> Neta="<<Neta_<<" in ["<<ietamin_<<","<<ietamax<<"] and Nphi="<<Nphi<<" in ["<<iphimin_<<","<<iphimax<<"]"<<endl;
  
  xtal_ = new crystal[Neta_*Nphi_];
  for(int xbin=1; xbin<Nphi_+1; ++xbin)
    for(int ybin=1; ybin<Neta_+1; ++ybin)
    {
      int index = fromTH2indexto1Dindex(xbin, ybin, Nphi_, Neta_);
      xtal_[index].IC = ICmap->GetBinContent(xbin,ybin); 
      xtal_[index].status = 1;
    }

}

void ICmanager::LoadIC(const std::vector<std::string> &ICcfg)
{
  if(xtal_)
    delete[] xtal_;
  string objkey   = ICcfg[0];
  string filename = ICcfg[1];
  cout<<"> Loading IC from "<<filename<<"/"<<objkey<<endl;
  TFile* inICfile = new TFile(filename.c_str(),"READ");
  TH2D* ICmap = (TH2D*) inICfile->Get(objkey.c_str());
  this->LoadIC(ICmap);
  inICfile->Close();
}

void ICmanager::InitializeIC()
{
  Neta_=ietamax_-ietamin_+1;
  Nphi_=iphimax_-iphimin_+1;
  cout<<"> Initialize ICmanager with Neta="<<Neta_<<" in ["<<ietamin_<<","<<ietamax_<<"] and Nphi="<<Nphi_<<" in ["<<iphimin_<<","<<iphimax_<<"]"<<endl;
  xtal_ = new crystal[Neta_*Nphi_];
  for(int xbin=1; xbin<Nphi_+1; ++xbin)
    for(int ybin=1; ybin<Neta_+1; ++ybin)
    {
      int index = fromTH2indexto1Dindex(xbin, ybin, Nphi_, Neta_);
      xtal_[index].IC = 1;
      xtal_[index].status = 1;
    }
}

Float_t  ICmanager::GetIC(const Int_t &index)
{
  return (xtal_[index]).IC;
}

Float_t ICmanager::GetIC(const Int_t &ieta, const Int_t &iphi)
{
  return ( xtal_[ fromIetaIphito1Dindex(ieta,iphi,Neta_,Nphi_,ietamin_,iphimin_) ] ).IC;
}

TH2D* ICmanager::GetHisto(const char* name, const char* title)
{
  TH2D* ICmap = new TH2D(name,title,Nphi_,iphimin_,iphimax_+1,Neta_,ietamin_,ietamax_+1);
  for(int xbin=1; xbin<ICmap->GetNbinsX()+1; ++xbin)
    for(int ybin=1; ybin<ICmap->GetNbinsY()+1; ++ybin)
    {
      int index = fromTH2indexto1Dindex(xbin, ybin, Nphi_, Neta_);
      ICmap->SetBinContent(xbin,ybin,this->GetIC(index));
    }
  return ICmap;
}
