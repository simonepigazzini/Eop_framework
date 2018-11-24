#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calorimeter.h"
#include "crystal.h"

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

  //define the output histo
  TFile *outFile = new TFile(("EopEta_"+label+".root").Data(),"RECREATE");
  TH2F* Eop_vs_Eta = new TH2F(("EopEta_"+label).Data(),("EopEta_"+label).Data(),0,0,0,0,0,0);
  
  
  
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;
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
