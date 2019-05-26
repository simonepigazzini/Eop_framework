#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calibrator.h"

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
  cerr << ">>>>> usage:  NormalizeBuildEopEta --Eopweight <objtype> <objname> <filename>" << endl;
  cerr << "               " <<            " --Eopweight          MANDATORY" <<endl;
}

int main(int argc, char* argv[])
{

  string objtype   = "";
  string objkey    = "";
  string filename  = "";

  for(int iarg=1; iarg<argc; ++iarg)
    if(string(argv[iarg])=="--Eopweight")
    {
      objtype   = argv[iarg+1];
      objkey    = argv[iarg+2];
      filename  = argv[iarg+3];
    }

  if( objtype=="" || objkey=="" || filename=="" )
  {
    PrintUsage();
    return -1;
  }

  if( objtype!="TH2F" )
  {
    cout<<"[ERROR]: unsupported objtype"<<endl;
    return -1;
  }

  TFile* inweightfile = new TFile(filename.c_str(),"UPDATE");
  TH2F* Eop_vs_Eta = (TH2F*) inweightfile->Get(objkey.c_str());

  //loop over bins to normalize to 1 each eta ring
  cout<<"> Normalization"<<endl;
  TH1D* Eop_projection;
  for(int ieta=1 ; ieta<Eop_vs_Eta->GetNbinsX()+1 ; ++ieta)
  {
    Eop_projection=Eop_vs_Eta->ProjectionY("_py",ieta,ieta,"");
    float Nev = Eop_projection->Integral(0,-1);//integral including underflow and overflow
    if(Nev==0)
      continue;
    for(int iEop=0 ; iEop<=Eop_vs_Eta->GetNbinsY()+1 ; ++iEop)
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
  Eop_vs_Eta->Write(Eop_vs_Eta->GetName(),TObject::kOverwrite);
  inweightfile->Close();
  return 0;
}
