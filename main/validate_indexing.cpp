#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calibrator.h"
#include "utils.h"

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

  calibrator EB(config);
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;

  cout<<"----------------------------------------------------------------------------------------------"<<endl;
  cout<<"test1"<<endl;
  for(int ieta=-85; ieta<86; ++ieta)
    for(int iphi=1; iphi<361; ++iphi)
    {
      int index1 = fromIetaIphito1Dindex(ieta,iphi,171,360,-85,1);
      int index2 = fromTH2indexto1Dindex(iphi-1+1,ieta+85+1,360,171);
      //cout<<"(ieta,iphi)=("<<ieta<<","<<iphi<<")\t"<<index1<<"\t"<<index2<<endl;
      if(index1!=index2)
	getchar();
    }

  cout<<"----------------------------------------------------------------------------------------------"<<endl;
  cout<<"test2"<<endl;
  for(int biny=1; biny<172; ++biny)
    for(int binx=1; binx<361; ++binx)
    {
      int index = fromTH2indexto1Dindex(binx,biny,360,171);
      int binx2,biny2;
      from1DindextoTH2index(index,binx2,biny2,360,171);
      //cout<<"(binx,biny)=("<<binx<<","<<biny<<")\t-->index="<<index<<"\t-->(binx2,biny2)=("<<binx2<<","<<biny2<<")"<<endl;
      if(binx!=binx2 || biny!=biny2)
	getchar();
    }

  cout<<"----------------------------------------------------------------------------------------------"<<endl;
  cout<<"test3"<<endl;
  for(int ieta=-85; ieta<86; ++ieta)
    for(int iphi=1; iphi<361; ++iphi)
    {
      int index = fromIetaIphito1Dindex(ieta,iphi,171,360,-85,1);
      int ieta2,iphi2;
      from1DindextoIetaIphi(index,ieta2,iphi2,171,360,-85,1);
      //cout<<"(ieta,iphi)=("<<ieta<<","<<iphi<<")\t"<<"(ieta2,iphi2)=("<<ieta2<<","<<iphi2<<")"<<endl;
      if(ieta!=ieta2 || iphi!=iphi2)
	getchar();
    }

  cout<<"----------------------------------------------------------------------------------------------"<<endl;
  cout<<"test4"<<endl;
  TFile* inICfile = new TFile("/home/fabio/Eop_framework/data/test_inputIC.root","READ");
  TH2F* ICmap = (TH2F*) inICfile->Get("IC_eta_phi");
  ICmap ->SetDirectory(0);
  inICfile->Close();
  for(int biny=1; biny<ICmap->GetNbinsY()+1; ++biny)
    for(int binx=1; binx<ICmap->GetNbinsX()+1; ++binx)
    {
      int index = fromTH2indexto1Dindex(binx,biny,360,171);
      float IC1 = ICmap->GetBinContent(binx,biny);
      float IC2 = EB.GetIC(index);
      int iphi = (int)(ICmap->ProjectionX()->GetBinLowEdge(binx));
      int ieta = (int)(ICmap->ProjectionY()->GetBinLowEdge(biny));
      float IC3 = EB.GetIC(ieta,iphi);
      cout<<"(binx,biny)=("<<binx<<","<<biny<<")=(ieta,iphi)=("<<ieta<<","<<iphi<<")\t-->index="<<index<<"\tIC1="<<IC1<<"\tIC2="<<IC2<<"\tIC3="<<IC3<<endl;
      if(IC1!=IC2 || IC1!=IC3 || IC2!=IC3)
	getchar();
    }
  
  delete ICmap;
  return 0;
}
