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

  //define the output histo
  string outfilename = "EopEta_"+string(label)+".root";
  if(config.OptExist("Input.inputIC"))
    outfilename = config.GetOpt<string> ("Output.BuildEopEta_output");
  TFile *outFile = new TFile(outfilename.c_str(),"RECREATE");
  TH2F* Eop_vs_Eta = new TH2F(("EopEta_"+label).Data(),("EopEta_"+label).Data(), 171, -1.4775, +1.4775, 100, 0.2, 1.9);

  //loop over entries to fill the histo  
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;
  float E,p,eta;
  int iEle;
  for(Long64_t ientry=0 ; ientry<Nentries ; ++ientry)
  {
    if( ientry%10000==0 )
      std::cout << "Processing entry "<< ientry << "\r" << std::flush;
    EB.GetEntry(ientry);
    for(iEle=0;iEle<2;++iEle)
    {
      if(EB.isSelected(iEle))
      {
	E=EB.GetICEnergy(iEle);
	p=EB.GetPcorrected(iEle);
	eta=EB.GetEtaSC(iEle);
      }
      if(p!=0)
	Eop_vs_Eta->Fill(eta,E/p);
      else
	cout<<"[WARNING]: p=0 for entry "<<ientry<<endl;
    }
  }

  //loop over bins to normalize to 1 each eta ring
  cout<<"> Normalization"<<endl;
  TH1D* Eop_projection;
  for(int ieta=1 ; ieta<Eop_vs_Eta->GetNbinsX()+1 ; ++ieta)
  {
    Eop_projection=Eop_vs_Eta->ProjectionY("_py",ieta,ieta,"");
    int Nev = Eop_projection->Integral();
    if(Nev==0)
      cout<<"[WARNING]: Nev=0 for eta bin "<<ieta<<endl;
    //cout<<Nev<<endl;
    for(int iEop=1 ; iEop<Eop_vs_Eta->GetNbinsY()+1 ; ++iEop)
    {
      float Eop = Eop_vs_Eta->GetBinContent(ieta,iEop);
      Eop_vs_Eta->SetBinContent(ieta,iEop,Eop/Nev);
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
