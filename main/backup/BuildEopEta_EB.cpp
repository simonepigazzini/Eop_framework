#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calibratorEB.h"

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
  cerr << ">>>>> usage:  BuildEopEta_EB --cfg <configFileName> --inputIC <objname> <filename> --Eopweightrange <weightrangemin> <weightrangemax> --Eopweightbins <Nbins> --BuildEopEta_output <outputFileName> --odd[or --even]" << endl;
  cerr << "               " <<            " --cfg                MANDATORY"<<endl;
  cerr << "               " <<            " --inputIC            OPTIONAL, can be also provided in the cfg"<<endl;
  cerr << "               " <<            " --Eopweightrange     OPTIONAL, can be also provided in the cfg" <<endl; 
  cerr << "               " <<            " --Eopweightbins      OPTIONAL, can be also provided in the cfg" <<endl; 
  cerr << "               " <<            " --BuildEopEta_output OPTIONAL, can be also provided in the cfg" <<endl;
  cerr << "               " <<            " --odd[or --even]     OPTIONAL" <<endl;
}

int main(int argc, char* argv[])
{
  string cfgfilename="";
  vector<string> ICcfg;
  float Eopweightmin=-1;
  float Eopweightmax=-1;
  int   Eopweightbins=-1;
  string outfilename="";
  string splitstat="";

  //Parse the input options
  for(int iarg=1; iarg<argc; ++iarg)
  {
    if(string(argv[iarg])=="--cfg")
      cfgfilename=argv[iarg+1];
    if(string(argv[iarg])=="--inputIC")
    {
      ICcfg.push_back(argv[iarg+1]);
      ICcfg.push_back(argv[iarg+2]);
    }

    if(string(argv[iarg])=="--Eopweightrange")
    {
      Eopweightmin=atof(argv[iarg+1]);
      Eopweightmax=atof(argv[iarg+2]);
    }
    if(string(argv[iarg])=="--Eopweightbins")
      Eopweightbins=atoi(argv[iarg+1]);

    if(string(argv[iarg])=="--BuildEopEta_output")
      outfilename=argv[iarg+1];
    if(string(argv[iarg])=="--odd")
      splitstat="odd";
    if(string(argv[iarg])=="--even")
      splitstat="even";
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
  calibratorEB EB(config);

  //set the options directly given as input to the executable, overwriting, in case, the corresponding ones contained in the cfg
  if(ICcfg.size()>0)
    EB.LoadIC(ICcfg);

  //define the output histo
  if(outfilename == "")
    if(config.OptExist("Output.BuildEopEta_output"))
      outfilename = config.GetOpt<string> ("Output.BuildEopEta_output");
    else
      outfilename = "EopEta.root";
  TFile *outFile = new TFile(outfilename.c_str(),"RECREATE");


  //define the range for the E/p weight histogram 
  if(Eopweightmin==-1 || Eopweightmax==-1)
    if(config.OptExist("Input.Eopweightrange"))
    {
      vector<float> Eopweightrange = config.GetOpt<vector<float> >("Input.Eopweightrange");
      Eopweightmin=Eopweightrange.at(0);
      Eopweightmax=Eopweightrange.at(1);
    }
    else
    {
      cout<<"[WARNING]: no Eopweightrange setting provided --> use default value"<<endl; 
      Eopweightmin = 0.2;
      Eopweightmax = 1.9;
    }

  if(Eopweightbins==-1)
    if(config.OptExist("Input.Eopweightbins"))
      Eopweightbins=config.GetOpt<int>("Input.Eopweightbins");
    else
    {
      cout<<"[WARNING]: no Eopweightbins setting provided --> use default value"<<endl; 
      Eopweightbins=100;
    }
  
  cout<<"> Set Eop range from "<<Eopweightmin<<" to "<<Eopweightmax<<" in "<<Eopweightbins<<" bins"<<endl;

  TH2F* Eop_vs_Eta = new TH2F("EopEta","EopEta", 171, -85.5, +85.5, Eopweightbins, Eopweightmin, Eopweightmax);

  //loop over entries to fill the histo  
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;
  if(Nentries==0)
    return -1;
  float E,p,eta;
  int iEle;
  int ietaSeed;

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
    EB.GetEntry(ientry);
    for(iEle=0;iEle<2;++iEle)
    {
      if(EB.isSelected(iEle))
      {
	E=EB.GetICEnergy(iEle);
	p=EB.GetPcorrected(iEle);
	ietaSeed=EB.GetietaSeed(iEle);

	if(p!=0)
	{
	  Eop_vs_Eta->Fill(ietaSeed,E/p);
	  //EB.PrintEleSummary(iEle);
	  //EB.PrintEleSummaryRH(iEle);
	  //getchar();
	}
	//else
	//  cout<<"[WARNING]: p=0 for entry "<<ientry<<endl;
      }
    }
  }

  //save and close
  Eop_vs_Eta->Write();
  outFile->Close();
  return 0;
}
