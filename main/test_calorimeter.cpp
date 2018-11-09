#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"
#include "interface/calorimeter.h"
#include "interface/crystal.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

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
    std::cerr << ">>>>> usage:   " << argv[0] << " configFileName" << std::endl;
    return -1;
  }
  
  //----------------------
  // parse the config file
  
  CfgManager config;
  config.ParseConfigFile(argv[1]);

  calorimeter EB(config);
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;
  const vector<float>* rechit1;
  TFile* inICfile = new TFile("/afs/cern.ch/user/f/fmonti/work/Eop_framework/data/test_inputIC.root","READ");
  TH2F* ICmap = (TH2F*) inICfile->Get("IC_eta_phi");
  ICmap ->SetDirectory(0);
  inICfile->Close();

  getchar();
  for(int ieta=-85;ieta<-70;ieta=ieta+1)
    for(int iphi=1;iphi<20;iphi=iphi+1)
    {
      cout<<ieta<<"\t"<<iphi<<"\tmy IC"<<EB.GetIC(iphi,ieta)<<"\tIC map"<<ICmap->GetBinContent( ICmap->FindBin(iphi,ieta) )<<endl;
    }
    
  for(Long64_t ientry=0 ; ientry<Nentries ; ++ientry)
  {
    EB.GetEntry(ientry);
    if(EB.isSelected(0))
    {
      EB.GetICEnergy(0);
      //cout<<"p="<<EB.GetP(0)<<"\tpcorr="<<EB.GetPcorrected(0)<<endl;
      //cout<<"\tE="<<EB.GetEnergy(0)<<"E(ICweighted)="<< EB.GetICEnergy(0) <<endl;
    }
    //cout<<ientry<<"\t"<<EB.GetEnergy(0)<<"\teta1,phi1="<<EB.GetEtaSC(0)<<","<<EB.GetPhi(0)<<endl;
    //if(EB.isSelected(0)) cout<<"selected1"<<endl;
    //cout<<ientry<<"\t"<<EB.GetEnergy(1)<<"\teta2,phi2="<<EB.GetEtaSC(1)<<","<<EB.GetPhi(1)<<endl;
    //if(EB.isSelected(1)) cout<<"selected2"<<endl;
    //rechit1=EB.GetRecHit1();
    //float somma=0;
    //for(auto rechit1E : (*rechit1))
    //{
    //  somma+=rechit1E;
    //  cout<<"\t"<<rechit1E<<endl;
    //}
    //cout<<somma<<endl;
    //cout<<EB.GetICEnergy(0)<<endl;
    //cout<<"diff="<<EB.GetICEnergy(0)-EB.GetEnergy(0)<<endl;
  }


  return 0;
}
