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

  TFile* inweightfile = new TFile(filename.c_str(),"READ");
  weight_ = (TH2F*) inweightfile->Get(objkey.c_str());
  weight_ -> SetDirectory(0);
  inweightfile->Close();

  //loop over bins to normalize to 1 each eta ring
  cout<<"> Normalization"<<endl;
  TH1D* Eop_projection;
  for(int ieta=1 ; ieta<Eop_vs_Eta->GetNbinsX()+1 ; ++ieta)
  {
    Eop_projection=Eop_vs_Eta->ProjectionY("_py",ieta,ieta,"");
    int Nbins=Eop_projection->GetNbinsX();
    int Nev = Eop_projection->/*GetEntries();*/Integral(0,-1);//integral including underflow and overflow
    cout<<"index"<<ieta-1<<endl;
    cout<<"entries="<<Eop_projection->GetEntries()<<endl;
    cout<<"integral="<<Eop_projection->Integral()<<endl;
    cout<<"integral with overunderflow="<<Eop_projection->Integral(0,-1)<<endl;
    cout<<"integral with overunderflowV2="<<Eop_projection->Integral(0,Nbins+1)<<endl;
    if(Nev==0)
      cout<<"[WARNING]: Nev=0 for eta bin "<<ieta<<endl;
    //Eop_projection->Scale(1./Nev);
    //cout<<Nev<<endl;
    for(int iEop=0 ; iEop<=Eop_vs_Eta->GetNbinsY()+1 ; ++iEop)
    {
      float Eop = Eop_vs_Eta->GetBinContent(ieta,iEop);
      Eop_vs_Eta->SetBinContent(ieta,iEop,Eop/Nev);
      //Eop_projection->GetBinContent(iEop);
      //Eop_vs_Eta->SetBinContent(ieta,iEop,Eop_projection->GetBinContent(iEop));
    }
    cout<<"afternorm integral with overunderflow="<<Eop_vs_Eta->ProjectionY("_py",ieta,ieta,"")->Integral(0,-1)<<endl;
  }

  /*
  //set underflow and overflow to 0
  for(int ieta=1 ; ieta<Eop_vs_Eta->GetNbinsX()+1 ; ++ieta)
  {
    Eop_vs_Eta->SetBinContent(ieta,0.,0);//underflow
    Eop_vs_Eta->SetBinContent(ieta, Eop_vs_Eta->GetNbinsY()+1 ,0);//overflow
  }
  */
  //save and close
  Eop_vs_Eta->Write();
  outFile->Close();
  return 0;
}
