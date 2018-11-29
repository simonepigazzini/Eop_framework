#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calorimeter.h"
#include "crystal.h"
#include "utils.h"

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

  TFile *outFile = new TFile(("IC_"+label+".root").Data(),"RECREATE");
  TH2D* numerator = new TH2D(("numerator_"+label).Data(),("numerator_"+label).Data(), Neta, ietamin, ietamax+1, Nphi, iphimin, iphimax+1);
  TH2D* denominator = new TH2D(("denominator_"+label).Data(),("denominator_"+label).Data(), Neta, ietamin, ietamax+1, Nphi, iphimin, iphimax+1);

  double *numerator1D = new double[Neta*Nphi];
  double *denominator1D = new double[Neta*Nphi]; 

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
	weight=EB.GetWeight(E/p,eta);
	regression=EB.GetRegression(iEle);
	for(unsigned iRecHit=0; iRecHit<ERecHit->size(); ++iRecHit)
	{
	  if(recoFlagRecHit->at(iRecHit) >= 4)
	    continue;
	  ieta=XRecHit->at(iRecHit);
	  iphi=YRecHit->at(iRecHit);
	  IC=EB.GetIC(iphi,ieta);
	  index = fromIEtaIPhito1Dindex(ieta,iphi,Nphi,ietamin,iphimin);
	  numerator1D[index] = ERecHit->at(iRecHit) * regression * IC / E * p / E * weight;
	  denominator1D[index] = ERecHit->at(iRecHit) * regression * IC / E * weight;
	}
      }
    }
  }	  

  //fill numerator and denominator histos
  for(int ix=1 ; ix<numerator->GetNbinsX()+1 ; ++ix)
  {
    for(int iy=1 ; iy<numerator->GetNbinsY()+1 ; ++iy)
    {
      index=from2Dto1Dindex(ix-1,iy-1,Nphi);
      numerator->SetBinContent(ix,iy,numerator1D[index]);
      denominator->SetBinContent(ix,iy,denominator1D[index]);
    }
  }

  //save and close
  outFile->cd();
  numerator->Write();
  denominator->Write();
  outFile->Close();
  return 0;
}
