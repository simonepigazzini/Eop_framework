#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"

#include "interface/CMS_lumi.h"

#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TString.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TChain.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TF1.h"



class Plotter
{
public:
  //---ctor
  Plotter(const CfgManager& opts,
          const std::vector<std::string>& vars,
          const std::string& outputFolder);
  
  //---dtor
  ~Plotter();
  
  //---utils
  void DrawPlots();
  
private:
  CfgManager opts_;
  std::vector<std::string> vars_;
  std::string outputFolder_;
};



class PlotBase
{
public:
  //---ctor
  PlotBase(const std::string& varName,
           const CfgManager& opts);

  //---dtor
  ~PlotBase();

  //---utils
  TCanvas* Plot() { return c_; };
  TCanvas* PlotLog() { return clog_; };

protected:
  std::string varName_;
  CfgManager opts_;
  TCanvas* c_;
  TCanvas* clog_;
  
  std::vector<TObject*> objects_;
};



class PlotTH1F: public PlotBase
{
public:
  //---ctor
  PlotTH1F(const std::string& varName,
           const CfgManager& opts);

  //---dtor
  ~PlotTH1F();

  //---utils
  
private:
};




/* PlotBase::PlotBase(const std::string& varName, */
/*                    const CfgManager& opts): */
/* varName_(varName), */
/*   opts_(opts) */
/* { */
/*   c_ = new TCanvas(); */
  /* h_data = new TH1F(Form("h_data_%s",varName.c_str()),"",200,0.,200.); */
  /* h_mc   = new TH1F(Form("h_mc_%s",varName.c_str()),  "",200,0.,200.); */
  /*                                                                                                                                                                                                                                            \ */

  /* h_data_extra = new TH1F(Form("h_data_extra_%s",varName.c_str()),"",202,-1.,201.); */
  /* h_mc_extra   = new TH1F(Form("h_mc_extra_%s",  varName.c_str()),"",202,-1.,201.); */
  /*                                                                                                                                                                                                                                            \ */

  /* t_data -> Draw( Form("%s >>h_data_%s",varName.c_str(),varName.c_str()), "", "goff" ); */
  /* t_mc   -> Draw( Form("%s >>h_mc_%s",varName.c_str(),varName.c_str()), "", "goff" ); */
  /*                                                                                                                                                                                                                                            \ */

  /* h_data_extra -> Sumw2(); */
  /* h_mc_extra   -> Sumw2(); */
  /* for(int bin = 0; bin <= h_data->GetNbinsX()+1; ++bin) */
  /* { */
  /*   h_data_extra -> SetBinContent(bin+1,h_data->GetBinContent(bin)); */
  /*   h_data_extra -> SetBinError(bin+1,h_data->GetBinError(bin)); */
  /* } */
  /* for(int bin = 0; bin <= h_mc->GetNbinsX()+1; ++bin) */
  /* { */
  /*   h_mc_extra -> SetBinContent(bin+1,h_mc->GetBinContent(bin)); */
  /*   h_mc_extra -> SetBinError(bin+1,h_mc->GetBinError(bin)); */
  /* } */
  /*                                                                                                                                                                                                                                            \ */

  /* h_data_extra -> Scale(1./h_data_extra->Integral()); */
  /* h_mc_extra -> Scale(1./h_mc_extra->Integral()); */
  /* yMin = +999999999.; */
  /* if( h_data_extra->GetBinContent(bin) < yMin && h_data_extra->GetBinContent(bin) != 0. ) yMin = h_data_extra->GetBinContent(bin);                                    x = h_mc_extra->GetBinContent(bin);                                    \ */

  /* if( h_mc_extra->GetBinContent(bin) < yMin && h_mc_extra->GetBinContent(bin) != 0. ) yMin = h_mc_extra->GetBinContent(b */
  /*                                                                                                                      } */
/* } */
