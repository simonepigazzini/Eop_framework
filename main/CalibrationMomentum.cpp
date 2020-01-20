#include "histoFunc.h"
#include "calibrator.h"
#include "CfgManager.h"
#include "CfgManagerT.h"
#include "FitUtils.h"

#include <iostream>
#include <iomanip>

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TMath.h"

#define NEWLINE cout<<endl
using namespace std;

void PrintUsage()
{
  cerr << ">>>>> usage:  CalibrationMomentum --cfg <configFileName>" << endl;
  cerr << "               " <<            " --cfg                MANDATORY"<<endl;
}

//**************  MAIN PROGRAM **************************************************************
int main(int argc, char** argv)
{
  string cfgfilename="";
  // Acquisition from cfg file
  for(int iarg=1; iarg<argc; ++iarg)
  {
    if(string(argv[iarg])=="--cfg")
      cfgfilename=argv[iarg+1];
  }

  if(cfgfilename=="")
  {
    PrintUsage();
    return -1;
  }

  // parse the config file
  CfgManager config;
  config.ParseConfigFile(cfgfilename.c_str());

  int nPhiBins = 1;
  if( config.OptExist("CalibrationMomentum.nPhiBins"))
    nPhiBins =  config.GetOpt<int>("CalibrationMomentum.nPhiBins");

  int rebin = 1;
  if( config.OptExist("CalibrationMomentum.rebin"))
    rebin =  config.GetOpt<int>("CalibrationMomentum.rebin");

  string outputFile = "calibrationmomentum.root";
  if( config.OptExist("Output.CalibrationMomentum_output"))
    outputFile =  config.GetOpt<string>("Output.CalibrationMomentum_output");

  string TemplatePlotsFolder = "";
  if( config.OptExist("Output.CalibrationMomentum_TemplatePlotsFolder"))
    TemplatePlotsFolder =  config.GetOpt<string>("Output.CalibrationMomentum_TemplatePlotsFolder");

  bool doEB = true;
  if( config.OptExist("CalibrationMomentum.doEB"))
    doEB =  config.GetOpt<bool>("CalibrationMomentum.doEB");

  cout << " Basic Configuration " << endl;
  cout << " nPhiBins = " << nPhiBins << endl;
  cout << " rebin = " << rebin << endl;
  cout << "Making calibration plots for Momentum scale studies " << endl;

  //**************************** define histograms
  TFile* o = new TFile(outputFile.c_str(), "RECREATE");
  TH1D* h_Et_Positron = new TH1D("h_Et_Positron", "h_Et_Positron", 100, 0., 100.);
  TH1D* h_Et_Electron = new TH1D("h_Et_Electron", "h_Et_Electron", 100, 0., 100.);
  TH1D* h_phiPositron = new TH1D("h_phiPositron","h_phiPositron", nPhiBins, -TMath::Pi(), TMath::Pi());
  TH1D* h_phiElectron = new TH1D("h_phiElectron","h_phiElectron", nPhiBins, -TMath::Pi(), TMath::Pi()); 
  TH2D* h2_Mee_PPositron_EElectron_vs_phiPositron = new TH2D("Mee_vs_phiPositron", "Mee_vs_phiPositron", nPhiBins, -TMath::Pi(), TMath::Pi(), 2200, 0.2, 1.6);
  TH2D* h2_Mee_PElectron_EPositron_vs_phiElectron = new TH2D("Mee_vs_phiElectron", "Mee_vs_phiElectron", nPhiBins, -TMath::Pi(), TMath::Pi(), 2200, 0.2, 1.6);

  //**************************** loop on events
  calibrator* data = new calibrator(config);
  
  long int Nentries = data->GetEntries();
  std::cout << "Loop in data events " << endl;
  std::cout << "     DATA: " << Nentries << " entries in Data sample" << std::endl;  
  for(long int entry = 0; entry < Nentries; ++entry)
  {
    if( entry % 100000 == 0 )
      std::cout << "reading saved entry " << entry << "\r" << std::flush;

    data->GetEntry(entry);

    if(!(data->isSelected(0) && data->isSelected(1)))
      continue;

    int posElectron, posPositron; //positions of the electron and positron inside the array (0 or 1)
    if(data->GetCharge(0)==-1 && data->GetCharge(1)==+1)
    {
      posElectron=0;
      posPositron=1;
    }
    else
      if(data->GetCharge(0)==+1 && data->GetCharge(1)==-1)
      {
	posElectron=1;
	posPositron=0;
      }
      else
      {
#ifdef DEBUG
	cerr<<"[INFO]: charge values different from (-1,+1) or (+1,-1) --> SKIP EVENT"<<endl;
#endif
	continue;
      }

    float Mee_PPositron_EElectron = data->GetMee() * sqrt( data->GetP(posPositron) / data->GetICEnergy(posPositron) ) / 91.19;
    float Mee_PElectron_EPositron = data->GetMee() * sqrt( data->GetP(posElectron) / data->GetICEnergy(posElectron) ) / 91.19;
    //NOTE: the correction must be linear in P, therefore I will use Mee^2 to extract the scale correction



    if( doEB && data->isEB(posPositron) )
    {
      /*
      std::cout<<"EB";
      if(posElectron==0)
	std::cout<<"\tchargeEle[0]="<<data->GetCharge(0)<<" <- SELECTED"
		 <<"\tchargeEle[1]="<<data->GetCharge(1);
      else
	std::cout<<"\tchargeEle[0]="<<data->GetCharge(0)
		 <<"\tchargeEle[1]="<<data->GetCharge(1)<<" <- SELECTED";

      std::cout<<"\tPhibinEB="<<h2_Mee_PPositron_EElectron_vs_phiPositron -> GetXaxis() -> FindBin(data->GetPhi(posPositron))
	       <<"\tregionId=//"
	       <<"\tvar="<<Mee_PPositron_EElectron
	       <<"\tvar*var="<<Mee_PPositron_EElectron*Mee_PPositron_EElectron
	       <<std::endl;
      getchar();
      */
      h2_Mee_PPositron_EElectron_vs_phiPositron -> Fill( data->GetPhi(posPositron) , Mee_PPositron_EElectron*Mee_PPositron_EElectron );
    }
    else
      if(!doEB && data->isEE(posPositron))
      {
	/*
	std::cout<<"EE";
	if(posElectron==0)
	  std::cout<<"\tchargeEle[0]="<<data->GetCharge(0)<<" <- SELECTED"
		   <<"\tchargeEle[1]="<<data->GetCharge(1);
	else
	  std::cout<<"\tchargeEle[0]="<<data->GetCharge(0)
		   <<"\tchargeEle[1]="<<data->GetCharge(1)<<" <- SELECTED";
	
	std::cout<<"\tPhibinEE="<<h2_Mee_PPositron_EElectron_vs_phiPositron -> GetXaxis() -> FindBin(data->GetPhi(posPositron))
		 <<"\tregionId=//"
		 <<"\tE[positron]="<<data->GetICEnergy(posPositron)
		 <<"\tp[positron]="<<data->GetP(posPositron)
		 <<"\tMee="<<data->GetMee()
		 <<"\tvar="<<Mee_PPositron_EElectron
		 <<"\tvar*var="<<Mee_PPositron_EElectron*Mee_PPositron_EElectron
		 <<std::endl;
	getchar();
	*/
	h2_Mee_PPositron_EElectron_vs_phiPositron -> Fill( data->GetPhi(posPositron) , Mee_PPositron_EElectron*Mee_PPositron_EElectron );
      }

    if(doEB && data->isEB(posElectron) )
      h2_Mee_PElectron_EPositron_vs_phiElectron -> Fill( data->GetPhi(posElectron) , Mee_PElectron_EPositron*Mee_PElectron_EPositron );
    else
      if(!doEB && data->isEE(posElectron) )
	h2_Mee_PElectron_EPositron_vs_phiElectron -> Fill( data->GetPhi(posElectron) , Mee_PElectron_EPositron*Mee_PElectron_EPositron );
  }

  NEWLINE;

  // initialize TGraphs
  TGraphErrors* g_PositronCorrection_vs_phi = new TGraphErrors();
  g_PositronCorrection_vs_phi->SetName("positron_correction");
  g_PositronCorrection_vs_phi->SetTitle("positron_correction");
  TGraphErrors* g_ElectronCorrection_vs_phi = new TGraphErrors();
  g_ElectronCorrection_vs_phi->SetName("electron_correction");
  g_ElectronCorrection_vs_phi->SetTitle("electron_correction");

  //integrating events over phi
  TH1D* h_Mee_PPositron = h2_Mee_PPositron_EElectron_vs_phiPositron -> ProjectionY("Mee_PPositron");
  TH1D* h_Mee_PElectron = h2_Mee_PElectron_EPositron_vs_phiElectron -> ProjectionY("Mee_PElectron");
  h_Mee_PPositron -> Rebin(rebin);
  h_Mee_PElectron -> Rebin(rebin);
  // initialize template functions
  histoFunc* templateHistoFunc_Mee_PPositron = new histoFunc((TH1F*)h_Mee_PPositron);
  histoFunc* templateHistoFunc_Mee_PElectron = new histoFunc((TH1F*)h_Mee_PElectron);
  
  //-------------------
  // Template Fit in EB
  vector<TH1D*> h_Mee_PPositron_phibin;
  vector<TH1D*> h_Mee_PElectron_phibin;
  vector<TF1*>  fitfunc_Mee_PPositron;
  vector<TF1*>  fitfunc_Mee_PElectron;

  int Ngoodfits=0;
  int Nbadfits=0;
  for(int phibin = 1; phibin <= nPhiBins; ++phibin)
  {
    cout<<"Fit progress "<<100*phibin/nPhiBins<<"\%  \r"<<flush;
    float phi = h2_Mee_PPositron_EElectron_vs_phiPositron->GetXaxis()->GetBinCenter(phibin);

    // define the histograms to be fitted
    h_Mee_PPositron_phibin.push_back( h2_Mee_PPositron_EElectron_vs_phiPositron->ProjectionY( Form("h_Mee_PPositron_phibin%i",phibin), phibin, phibin) );
    h_Mee_PPositron_phibin.back() -> Rebin(rebin);
    h_Mee_PElectron_phibin.push_back( h2_Mee_PElectron_EPositron_vs_phiElectron->ProjectionY( Form("h_Mee_PElectron_phibin%i",phibin), phibin, phibin) );
    h_Mee_PElectron_phibin.back() -> Rebin(rebin);

    // define the fitting function
    if(doEB)
      fitfunc_Mee_PPositron.push_back( new TF1(Form("f_Mee_PPositron_phibin%i",phibin), templateHistoFunc_Mee_PPositron, 0.85, 1.1, 3, "histoFunc") );
    else
      fitfunc_Mee_PPositron.push_back( new TF1(Form("f_Mee_PPositron_phibin%i",phibin), templateHistoFunc_Mee_PPositron,  0.7, 1.1, 3, "histoFunc") );
    fitfunc_Mee_PPositron.back() -> SetParName(0, "Norm");
    fitfunc_Mee_PPositron.back() -> SetParName(1, "Scale factor");
    if(doEB)
      fitfunc_Mee_PElectron.push_back( new TF1(Form("f_Mee_PElectron_phibin%i",phibin), templateHistoFunc_Mee_PElectron, 0.85, 1.1, 3, "histoFunc") );
    else
      fitfunc_Mee_PElectron.push_back( new TF1(Form("f_Mee_PElectron_phibin%i",phibin), templateHistoFunc_Mee_PElectron,  0.7, 1.1, 3, "histoFunc") );

    fitfunc_Mee_PElectron.back() -> SetParName(0, "Norm");
    fitfunc_Mee_PElectron.back() -> SetParName(1, "Scale factor");

    //fix the normalization and the shift of the fit functions
    double Norm_Mee_PPositron =
      h_Mee_PPositron_phibin.back()->Integral() / h_Mee_PPositron->Integral() * 
      h_Mee_PPositron_phibin.back()->GetBinWidth(1) / h_Mee_PPositron->GetBinWidth(1);
    fitfunc_Mee_PPositron.back() -> FixParameter(0, Norm_Mee_PPositron);
    fitfunc_Mee_PPositron.back() -> FixParameter(2, 0.);
    double Norm_Mee_PElectron =
      h_Mee_PElectron_phibin.back()->Integral() / h_Mee_PElectron->Integral() * 
      h_Mee_PElectron_phibin.back()->GetBinWidth(1) / h_Mee_PElectron->GetBinWidth(1);
    fitfunc_Mee_PElectron.back() -> FixParameter(0, Norm_Mee_PElectron);
    fitfunc_Mee_PElectron.back() -> FixParameter(2, 0.);

    //fit positron correction
    //cout << "***** Positron ";
    bool goodFit = FitUtils::PerseverantFit( h_Mee_PPositron_phibin.back(), fitfunc_Mee_PPositron.back(), "QRL+", 10, TemplatePlotsFolder);
    if(goodFit)
    {
      ++Ngoodfits;
      //cout << "fit OK    ";
      double k = fitfunc_Mee_PPositron.back()->GetParameter(1);
      double eee = fitfunc_Mee_PPositron.back()->GetParError(1);
      g_PositronCorrection_vs_phi -> SetPoint(phibin, phi, 1./k);
      g_PositronCorrection_vs_phi -> SetPointError(phibin, 0., eee/k/k);
    }
    else
    {
      ++Nbadfits;
      //cout << "fit BAD   ";
      g_PositronCorrection_vs_phi -> SetPoint(phibin, phi, 1.);
      g_PositronCorrection_vs_phi -> SetPointError(phibin, 0., 0.01);
    }
    //NEWLINE;

    //fit electron correction
    //cout << "***** Electron ";
    goodFit = FitUtils::PerseverantFit( h_Mee_PElectron_phibin.back(), fitfunc_Mee_PElectron.back(), "QRL+", 10);
    if(goodFit)
    {
      ++Ngoodfits;
      //cout << "fit OK    ";
      double k = fitfunc_Mee_PElectron.back()->GetParameter(1);
      double eee = fitfunc_Mee_PElectron.back()->GetParError(1);
      g_ElectronCorrection_vs_phi -> SetPoint(phibin, phi, 1./k);
      g_ElectronCorrection_vs_phi -> SetPointError(phibin, 0., eee/k/k);
    }
    else
    {
      ++Nbadfits;
      //cout << "fit BAD   ";
      g_ElectronCorrection_vs_phi -> SetPoint(phibin, phi, 1.);
      g_ElectronCorrection_vs_phi -> SetPointError(phibin, 0., 0.01);
    }
    //NEWLINE;
  }
  NEWLINE;
  cout<<"Fit summary:"<<endl;
  cout<<"\t"<<Ngoodfits<<" GOOD fits"<<endl;
  cout<<"\t"<<Nbadfits<<" BAD fits"<<endl;

  //first point is slightly larger than -pi and last point slightly smaller than +pi 
  // --> use the following trick in order to guarantee full coverage
  double phi_underflow = h2_Mee_PPositron_EElectron_vs_phiPositron->GetXaxis()->GetBinCenter( 0 );
  double phi_overflow  = h2_Mee_PPositron_EElectron_vs_phiPositron->GetXaxis()->GetBinCenter( nPhiBins+1 );
  double phi_lastbin, pcorr_lastbin, err_pcorr_lastbin, phi_firstbin, pcorr_firstbin, err_pcorr_firstbin;

  //copy pcorr(-pi+epsilon) in pcorr(pi+epsilon)
  g_PositronCorrection_vs_phi -> GetPoint( 1 ,        phi_firstbin , pcorr_firstbin );
  err_pcorr_firstbin = g_PositronCorrection_vs_phi -> GetErrorX(1);
  g_PositronCorrection_vs_phi -> SetPoint( nPhiBins+1 , phi_overflow , pcorr_firstbin );
  g_PositronCorrection_vs_phi -> SetPointError(nPhiBins+1 , 0., err_pcorr_firstbin );
  //copy pcorr(pi-epsilon) in pcorr(-pi-epsilon)
  g_PositronCorrection_vs_phi -> GetPoint( nPhiBins , phi_lastbin  , pcorr_lastbin  );
  err_pcorr_lastbin = g_PositronCorrection_vs_phi -> GetErrorX(nPhiBins);
  g_PositronCorrection_vs_phi -> SetPoint( 0 , phi_underflow , pcorr_lastbin );
  g_PositronCorrection_vs_phi -> SetPointError(0 , 0., err_pcorr_lastbin );

  //copy pcorr(-pi+epsilon) in pcorr(pi+epsilon)
  g_ElectronCorrection_vs_phi -> GetPoint( 1 ,        phi_firstbin , pcorr_firstbin );
  err_pcorr_firstbin = g_ElectronCorrection_vs_phi -> GetErrorX(1);
  g_ElectronCorrection_vs_phi -> SetPoint( nPhiBins+1 , phi_overflow , pcorr_firstbin );
  g_ElectronCorrection_vs_phi -> SetPointError(nPhiBins+1 , 0., err_pcorr_firstbin );
  //copy pcorr(pi-epsilon) in pcorr(-pi-epsilon)
  g_ElectronCorrection_vs_phi -> GetPoint( nPhiBins , phi_lastbin  , pcorr_lastbin  );
  err_pcorr_lastbin = g_ElectronCorrection_vs_phi -> GetErrorX(nPhiBins);
  g_ElectronCorrection_vs_phi -> SetPoint( 0 , phi_underflow , pcorr_lastbin );
  g_ElectronCorrection_vs_phi -> SetPointError(0 , 0., err_pcorr_lastbin );

  
  //-------
  // Output
  cout<<">> Saving data in "<<outputFile<<endl;
  o -> cd();
  g_PositronCorrection_vs_phi -> Write();
  g_ElectronCorrection_vs_phi -> Write();
  h_Mee_PPositron -> Write();
  h_Mee_PElectron -> Write();
  h_phiPositron   -> Write();
  h_phiElectron   -> Write();
  h_Et_Positron->Write();
  h_Et_Electron->Write();
  o -> Close();

  cout<<">> Deleting objects"<<endl;
  delete g_PositronCorrection_vs_phi;
  delete g_ElectronCorrection_vs_phi;
  delete templateHistoFunc_Mee_PPositron;
  delete templateHistoFunc_Mee_PElectron;
  for(auto f : fitfunc_Mee_PPositron)
    if(f)
      delete f;
  for(auto f : fitfunc_Mee_PElectron)
    if(f)
      delete f;
  delete data;
  
  return 0;

}
