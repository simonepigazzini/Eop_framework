#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calibratorEB.h"
#include "calibratorEE.h"
#include "ICmanager.h"

#include <iostream>
#include <string>

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TLorentzVector.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TMath.h"

using namespace std;

void PrintUsage()
{
  cerr << ">>>>> usage:  ComputeIC_EB --cfg <configFileName> --inputIC <objname> <filename> --Eopweight <objtype> <objname> <filename> --ComputeIC_output <outputFileName> --odd[or --even]" << endl;
  cerr << "               " <<            " --cfg                MANDATORY"<<endl;
  cerr << "               " <<            " --inputIC            OPTIONAL, can be also provided in the cfg"<<endl;
  cerr << "               " <<            " --Eopweight          OPTIONAL, can be also provided in the cfg" <<endl;
  cerr << "               " <<            " --ComputeIC_output    OPTIONAL, can be also provided in the cfg" <<endl;
  cerr << "               " <<            " --odd[or --even]     OPTIONAL" <<endl;
  cerr << "               " <<            " --EE                 OPTIONAL, default false" <<endl;
}

int main(int argc, char* argv[])
{
  string cfgfilename="";
  vector<string> ICcfg;
  vector<string> weightcfg;
  string outfilename="";
  string splitstat="";
  bool EE=false;

  for(int iarg=1; iarg<argc; ++iarg)
  {
    if(string(argv[iarg])=="--cfg")
      cfgfilename=argv[iarg+1];
    if(string(argv[iarg])=="--inputIC")
    {
      ICcfg.push_back(argv[iarg+1]);
      ICcfg.push_back(argv[iarg+2]);
    }
    if(string(argv[iarg])=="--Eopweight")
    {
      weightcfg.push_back(argv[iarg+1]);
      weightcfg.push_back(argv[iarg+2]);
      weightcfg.push_back(argv[iarg+3]);
    }
    if(string(argv[iarg])=="--ComputeIC_output")
      outfilename=argv[iarg+1];
    if(string(argv[iarg])=="--odd")
      splitstat="odd";
    if(string(argv[iarg])=="--even")
      splitstat="even";
    if(string(argv[iarg])=="--EE")
      EE=true;
  }

  if(cfgfilename=="")
  {
    PrintUsage();
    return -1;
  }

  // parse the config file
  CfgManager config;
  config.ParseConfigFile(cfgfilename.c_str());
  
  //define the calibrator object to easily access to the ntuples data 
  calibrator* calorimeter;
  if(!EE)
    calorimeter = new calibratorEB(config);
  else
    calorimeter = new calibratorEE(config);

  //set the options directly given as input to the executable, overwriting, in case, the corresponding ones contained in the cfg
  if(weightcfg.size()>0)
    calorimeter->LoadEopWeight(weightcfg);
  if(ICcfg.size()>0)
    calorimeter->LoadIC(ICcfg);

  //eta, phi boundaries are taken by the calibrator constructor from configfile 
  float ietamin, ietamax, iphimin, iphimax;
  int Neta, Nphi;
  calorimeter->GetEtaboundaries(ietamin, ietamax);
  calorimeter->GetPhiboundaries(iphimin, iphimax);
  Neta=calorimeter->GetNeta();
  Nphi=calorimeter->GetNphi();

  //define the output 
  if(outfilename == "")
    if(config.OptExist("Output.ComputeIC_output"))
      outfilename = config.GetOpt<string> ("Output.ComputeIC_output");
    else
      outfilename = "IC.root";

  TFile *outFile = new TFile(outfilename.c_str(),"RECREATE");
  ICmanager numerator(ietamin,ietamax,iphimin,iphimax);
  ICmanager denominator(ietamin,ietamax,iphimin,iphimax);

  //Initialize numerator and denominator
  numerator.InitIC(0.);
  denominator.InitIC(0.);

  //loop over entries to fill the histo  
  Long64_t Nentries=calorimeter->GetEntries();
  cout<<Nentries<<" entries"<<endl;
  if(Nentries==0)
    return -1;
  float E,p,eta,IC,weight,regression;
  int iEle, index, ix, iy, ietaSeed;
  vector<float>* ERecHit;
  vector<float>* fracRecHit;
  vector<int>*   XRecHit;
  vector<int>*   YRecHit;
  vector<int>*   ZRecHit;
  vector<int>*   recoFlagRecHit;

  //set the iteration start and the increment accordingly to splitstat
  int ientry0=0;
  int ientry_increment=1;
  if(splitstat=="odd")
  {
    ientry0=1;
    ientry_increment=2;
  }
  else
    if(splitstat=="even")
    {
      ientry0=0;
      ientry_increment=2;
    }
  
  for(Long64_t ientry=ientry0 ; ientry<Nentries ; ientry+=ientry_increment)
  {
    if( ientry%100000==0 || (ientry-1)%100000==0)
      std::cout << "Processing entry "<< ientry << "\r" << std::flush;
    calorimeter->GetEntry(ientry);
    for(iEle=0;iEle<2;++iEle)
    {
      if(calorimeter->isSelected(iEle))
      {
	ERecHit=         calorimeter->GetERecHit(iEle);
	fracRecHit=      calorimeter->GetfracRecHit(iEle);
	XRecHit=         calorimeter->GetXRecHit(iEle);
	YRecHit=         calorimeter->GetYRecHit(iEle);
	//ZRecHit=         calorimeter->GetZRecHit(iEle);
	recoFlagRecHit=  calorimeter->GetrecoFlagRecHit(iEle);
	E=               calorimeter->GetICEnergy(iEle);
	p=               calorimeter->GetPcorrected(iEle);
	if(p==0)
	  continue;
        //eta=             calorimeter->GetEtaSC(iEle);
	ietaSeed=        calorimeter->GetietaSeed(iEle);
	weight=          calorimeter->GetWeight(ietaSeed,E/p);
	regression=      calorimeter->GetRegression(iEle);
	//cout<<"E="<<E<<"\tp="<<p<<"\teta="<<eta<<"\tweight="<<weight<<"\tregression="<<regression<<endl;
	for(unsigned iRecHit=0; iRecHit<ERecHit->size(); ++iRecHit)
	{
	  if(recoFlagRecHit->at(iRecHit) >= 4)
	    continue;
	  ix=XRecHit->at(iRecHit);
	  iy=YRecHit->at(iRecHit);
	  IC=calorimeter->GetIC(ix,iy);
	  //cout<<"entry="<<ientry<<endl;
	  //cout<<"ix="<<ix<<"\tiy="<<iy<<"\tindex="<<index<<"\tIC="<<IC<<endl;
	  //cout<<"ERH="<<ERecHit->at(iRecHit)<<"\tfracRH="<<fracRecHit->at(iRecHit)<<endl;
	  //cout<<"regression="<<regression<<"\tE="<<E<<"\tp="<<p<<"\tweight="<<weight<<endl;
	  numerator(ix,iy)   += ERecHit->at(iRecHit) * fracRecHit->at(iRecHit) * regression * IC / E * p / E * weight;
	  denominator(ix,iy) += ERecHit->at(iRecHit) * fracRecHit->at(iRecHit) * regression * IC / E         * weight;
	}
      }
    }
  }	  

  //get numerator and denominator histos
  TH2D* h2_numerator = numerator.GetHisto("numerator","numerator");
  TH2D* h2_denominator = denominator.GetHisto("denominator","denominator");

  //compute temporary IC-pull and IC-values 
  TH2D* h2_ICpull = GetICpull(h2_numerator,h2_denominator);
  TH2D* h2_temporaryIC = calorimeter->GetPulledIC(h2_ICpull);

  h2_temporaryIC->SetName("temporaryIC");
  h2_temporaryIC->SetTitle("temporaryIC");

  //save and close
  outFile->cd();
  h2_numerator->Write();
  h2_denominator->Write();
  h2_ICpull->Write();
  h2_temporaryIC->Write();
  outFile->Close();

  delete calorimeter;
  return 0;
}
