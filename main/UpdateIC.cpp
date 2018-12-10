#include "utils.h"
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

int main(int argc, char* argv[])
{
  if( argc < 2 )
  {
    std::cerr << ">>>>> usage:   " << argv[0] << " configFileName label(loop_number)" << std::endl;
    return -1;
  }

  // parse the config file
  CfgManager config;
  config.ParseConfigFile(argv[1]);

  
  //get the label (usually it is the number of the loop)
  TString label = "";
  if(argc>2)
    label = argv[2];

  
  //define the calorimeter object to easily access to the ntuples data
  calorimeter EB(config);

  //eta, phi boundaries are taken by the calorimeter constructor from configfile 
  float ietamin, ietamax, iphimin, iphimax;
  int Neta, Nphi;
  EB.GetEtaboundaries(ietamin, ietamax);
  EB.GetPhiboundaries(iphimin, iphimax);
  Neta=EB.GetNeta();
  Nphi=EB.GetNphi();

  string outfilename = "IC_"+string(label)+".root";
  if(config.OptExist("Output.UpdateIC_output"))
    outfilename = config.GetOpt<string> ("Output.UpdateIC_output");
  TFile *outFile = new TFile(outfilename.c_str(),"RECREATE");
  TH2D* numerator = new TH2D(("numerator_"+label).Data(),("numerator_"+label).Data(), Nphi, iphimin, iphimax+1, Neta, ietamin, ietamax+1);
  TH2D* denominator = new TH2D(("denominator_"+label).Data(),("denominator_"+label).Data(), Nphi, iphimin, iphimax+1, Neta, ietamin, ietamax+1);
  TH2D* temporaryIC = new TH2D(("temporaryIC_"+label).Data(),("temporaryIC_"+label).Data(), Nphi, iphimin, iphimax+1, Neta, ietamin, ietamax+1);

  double *numerator1D = new double[Neta*Nphi];
  double *denominator1D = new double[Neta*Nphi]; 
  
  //initialize numerator and denominator
  for( int index=0; index<Neta*Nphi; ++index)
  {
    numerator1D[index]=0;
    denominator1D[index]=0;
  }

  //loop over entries to fill the histo  
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;
  float E,p,eta,IC,weight,regression;
  int iEle, index, ieta, iphi;
  vector<float>* ERecHit;
  vector<float>* fracRecHit;
  vector<int>*   XRecHit;
  vector<int>*   YRecHit;
  vector<int>*   ZRecHit;
  vector<int>*   recoFlagRecHit;

  for(Long64_t ientry=0 ; ientry<Nentries ; ++ientry)
  {
    if( ientry%10000==0 )
      std::cout << "Processing entry "<< ientry << "\r" << std::flush;
    EB.GetEntry(ientry);
    for(iEle=0;iEle<2;++iEle)
    {
      if(EB.isSelected(iEle))
      {
	ERecHit=EB.GetERecHit(iEle);
	fracRecHit=EB.GetfracRecHit(iEle);
	XRecHit=EB.GetXRecHit(iEle);
	YRecHit=EB.GetYRecHit(iEle);
	//ZRecHit=EB.GetZRecHit(iEle);
	recoFlagRecHit=EB.GetrecoFlagRecHit(iEle);
	E=EB.GetICEnergy(iEle);
	p=EB.GetPcorrected(iEle);
	eta=EB.GetEtaSC(iEle);
	weight=EB.GetWeight(eta,E/p);
	regression=EB.GetRegression(iEle);
	//cout<<"E="<<E<<"\tp="<<p<<"\teta="<<eta<<"\tweight="<<weight<<"\tregression="<<regression<<endl;
	for(unsigned iRecHit=0; iRecHit<ERecHit->size(); ++iRecHit)
	{
	  if(recoFlagRecHit->at(iRecHit) >= 4)
	    continue;
	  ieta=XRecHit->at(iRecHit);
	  iphi=YRecHit->at(iRecHit);
	  index = fromIetaIphito1Dindex(ieta, iphi, Neta, Nphi, ietamin, iphimin);
	  IC=EB.GetIC(index);
	  //cout<<"ieta="<<ieta<<"\tiphi="<<iphi<<"\tindex="<<index<<"\tIC="<<IC<<endl;
	  //cout<<"ERH="<<ERecHit->at(iRecHit)<<"\tfracRH="<<fracRecHit->at(iRecHit)<<endl;
	  if(E>15. && p>15.)
	  {
	    numerator1D[index]   += ERecHit->at(iRecHit) * fracRecHit->at(iRecHit) * regression * IC / E * p / E * weight;
	    denominator1D[index] += ERecHit->at(iRecHit) * fracRecHit->at(iRecHit) * regression * IC / E * weight;
	    //cout<<"numerator="<<numerator1D[index]<<"\tdenominator="<<denominator1D[index]<<endl;
	  }
	  //getchar();
	  //else
	  //  cout<<"[WARNING]: E="<<E<<" and p="<<p<<" for event "<<ientry<<endl;
	}
      }
    }
  }	  

  //fill numerator and denominator histos
  for(int xbin=1; xbin<numerator->GetNbinsX()+1; ++xbin)
    for(int ybin=1; ybin<numerator->GetNbinsY()+1; ++ybin)
    {
      index = fromTH2indexto1Dindex(xbin, ybin, Nphi, Neta);
      numerator->SetBinContent(xbin,ybin,numerator1D[index]);
      denominator->SetBinContent(xbin,ybin,denominator1D[index]);
      if (denominator1D[index]!=0)
	temporaryIC->SetBinContent(xbin,ybin,numerator1D[index]/denominator1D[index]);	
    }


  //save and close
  outFile->cd();
  numerator->Write();
  denominator->Write();
  temporaryIC->Write();
  outFile->Close();
  return 0;
}
