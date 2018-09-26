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
  getchar();
  const vector<float>* rechit1;
  for(Long64_t ientry=0 ; ientry<Nentries ; ++ientry)
  {
    EB.GetEntry(ientry);
    cout<<ientry<<"\t"<<EB.GetEnergy(0)<<endl;
    rechit1=EB.GetRecHit1();
    float somma=0;
    for(auto rechit1E : (*rechit1))
    {
      somma+=rechit1E;
      //cout<<"\t"<<rechit1E<<endl;
    }
    cout<<somma<<endl;
  }

  return 0;
}
