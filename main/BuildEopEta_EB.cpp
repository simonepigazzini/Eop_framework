#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calorimeter.h"
#include "crystal.h"

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
  cerr << ">>>>> usage:  BuildEopEta_EB --cfg <configFileName> --inputIC <objname> <filename> --Eopweight <objtype> <objname> <filename> --BuildEopEta_output <outputFileName> --odd[or --even]" << endl;
  cerr << "               " <<            " --cfg                MANDATORY"<<endl;
  cerr << "               " <<            " --inputIC            OPTIONAL, can be also provided in the cfg"<<endl;
  cerr << "               " <<            " --Eopweight          OPTIONAL, can be also provided in the cfg" <<endl;
  cerr << "               " <<            " --BuildEopEta_output OPTIONAL, can be also provided in the cfg" <<endl;
  cerr << "               " <<            " --odd[or --even]     OPTIONAL" <<endl;
}

int main(int argc, char* argv[])
{
  string cfgfilename="";
  vector<string> ICcfg;
  vector<string> weightcfg;
  string outfilename="";
  string splitstat="";

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

  //define the calorimeter object to easily access to the ntuples data
  calorimeter EB(config);

  //set the options directly given as input to the executable, overwriting, in case, the corresponding ones contained in the cfg
  if(weightcfg.size()>0)
    EB.LoadEopWeight(weightcfg);
  if(ICcfg.size()>0)
    EB.LoadIC(ICcfg);

  //define the output histo
  if(outfilename == "")
    if(config.OptExist("Output.BuildEopEta_output"))
      outfilename = config.GetOpt<string> ("Output.BuildEopEta_output");
    else
      outfilename = "EopEta.root";
  TFile *outFile = new TFile(outfilename.c_str(),"RECREATE");
  float Eopweightrange = 0.8;
  if(config.OptExist("Input.Eopweightrange"))
    Eopweightrange = config.GetOpt<float> ("Input.Eopweightrange");
  cout<<"> Set Eop range from "<<1.-Eopweightrange<<" to "<<1.+Eopweightrange<<endl;

  /////////////////////////////////////////////////////
  //for debug set the binwidth equal to the previous calibration code
  float eop_binwidth = (1.9-0.2)/100;
  float Neop_bins = (int)(2*Eopweightrange / eop_binwidth);
  /////////////////////////////////////////////////////
  TH2F* Eop_vs_Eta = new TH2F("EopEta","EopEta", 171, -85.5, +85.5, Neop_bins, 1.-Eopweightrange, 1.+Eopweightrange);

  //loop over entries to fill the histo  
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;
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
    if( ientry%10000==0 || (ientry-1)%10000==0)
      std::cout << "Processing entry "<< ientry << "\r" << std::flush;
    EB.GetEntry(ientry);
    for(iEle=0;iEle<2;++iEle)
    {
      if(EB.isSelected(iEle))
      {
	E=EB.GetICEnergy(iEle);
	p=EB.GetPcorrected(iEle);
	//eta=EB.GetEtaSC(iEle);
	ietaSeed=EB.GetietaSeed(iEle);

	if(p!=0)
	{
	  Eop_vs_Eta->Fill(ietaSeed,E/p);
	  //	  if(Eop_vs_Eta->GetXaxis()->FindBin(eta) == 80)
	  //{
	  // cout<<"E="<<E<<"\tp="<<p<<"\teta_seed="<<eta/1.477*85<<"\teta="<<eta<<endl;
	  // getchar();
	  //}
	}
	else
	  cout<<"[WARNING]: p=0 for entry "<<ientry<<endl;
      }
    }
  }
  
  /////////////////////////////////////////////////////
  //template to be removed
  TH1D* Eop_projection_test=Eop_vs_Eta->ProjectionY("_py",1,1,"");
  for(int i=0; i<Eop_projection_test->GetNbinsX()+2; ++i)
  {
    cout<<i<<"\t"<<Eop_projection_test->GetBinContent(i)<<endl;
  }
  Eop_projection_test=Eop_vs_Eta->ProjectionY("_py",55,55,"");
  for(int i=0; i<Eop_projection_test->GetNbinsX()+2; ++i)
  {
    cout<<i<<"\t"<<Eop_projection_test->GetBinContent(i)<<endl;
  }
  /////////////////////////////////////////////////////
  

  //loop over bins to normalize to 1 each eta ring
  cout<<"> Normalization"<<endl;
  TH1D* Eop_projection;
  for(int ieta=1 ; ieta<Eop_vs_Eta->GetNbinsX()+1 ; ++ieta)
  {
    Eop_projection=Eop_vs_Eta->ProjectionY("_py",ieta,ieta,"");
    int Nev = Eop_projection->/*GetEntries();*/Integral(0,-1);//integral including underflow and overflow
    cout<<"index"<<ieta-1<<endl;
    cout<<"entries="<<Eop_projection->GetEntries()<<endl;
    cout<<"integral="<<Eop_projection->Integral()<<endl;
    cout<<"integral with overunderflow="<<Eop_projection->Integral(0,-1)<<endl;
    if(Nev==0)
      cout<<"[WARNING]: Nev=0 for eta bin "<<ieta<<endl;
    //Eop_projection->Scale(1./Nev);
    //cout<<Nev<<endl;
    for(int iEop=1 ; iEop<Eop_vs_Eta->GetNbinsY()+1 ; ++iEop)
    {
      float Eop = Eop_vs_Eta->GetBinContent(ieta,iEop);
      Eop_vs_Eta->SetBinContent(ieta,iEop,Eop/Nev);
      //Eop_projection->GetBinContent(iEop);
      //Eop_vs_Eta->SetBinContent(ieta,iEop,Eop_projection->GetBinContent(iEop));
    }
  }

  //set underflow and overflow to 0
  for(int ieta=1 ; ieta<Eop_vs_Eta->GetNbinsX()+1 ; ++ieta)
  {
    Eop_vs_Eta->SetBinContent(ieta,0.,0);//underflow
    Eop_vs_Eta->SetBinContent(ieta, Eop_vs_Eta->GetNbinsY()+1 ,0);//overflow
  }
  
  //save and close
  Eop_vs_Eta->Write();
  outFile->Close();
  return 0;
}
