#include "calibrator.h"
//#include "utils.h"

using namespace std;

calibrator::calibrator(CfgManager conf):
  ECALELFInterface(conf),
  ICmanager(conf),
  electron_momentum_correction_(0),
  positron_momentum_correction_(0),
  weight_(0)
{

  //-------------------------------------
  //load momentum correction if present in cfg
  if(conf.OptExist("Input.MomentumCorrection"))
    LoadMomentumCorrection(conf.GetOpt<std::string> ("Input.MomentumCorrection"));
  else
    cout<<"[WARNING]: no Input.MomentumCorrection found in cfg file"<<endl;

  //-------------------------------------
  //load E/p event weight if present in cfg
  if(conf.OptExist("Input.Eopweight"))
    LoadEopWeight( conf.GetOpt<std::vector<std::string> > ("Input.Eopweight") );
  else
  {
    cout<<"> Set by default Eopweight to 1"<<endl;
    weight_ = new TH2F("weight","weight",1,0.,5.,1,0.,2.5);
    weight_ -> SetDirectory(0); //in order to avoid contrast with different TFle and directories opened in the main program
    weight_->SetBinContent(1,1,1.);
  }
  
  //-------------------------------------
  //set true or false the usage of regression --> change the energy value 
  useRegression_ = true;
  if(conf.OptExist("Input.useRegression"))
    useRegression_ = conf.GetOpt<bool> ("Input.useRegression");

}


calibrator::~calibrator()
{

  if(electron_momentum_correction_)
    delete electron_momentum_correction_;
  if(positron_momentum_correction_)
    delete positron_momentum_correction_;
  if(weight_)
    delete weight_;
}

void calibrator::LoadMomentumCorrection(string filename)
{
  //delete previous corrections to avoid memory leak
  if(electron_momentum_correction_)
    delete electron_momentum_correction_;
  if(positron_momentum_correction_)
    delete positron_momentum_correction_;

  cout<<"> Loading momentum correction from file "<<filename<<endl;
  TFile* momentumcorrectionfile = new TFile(filename.c_str(),"READ");
  electron_momentum_correction_  = (TGraphErrors*)(momentumcorrectionfile->Get("electron_correction"));
  positron_momentum_correction_ = (TGraphErrors*)(momentumcorrectionfile->Get("positron_correction"));
  momentumcorrectionfile->Close();
}

void calibrator::LoadEopWeight(const vector<string> &weightcfg)
{
  //delete previous weight to avoid memory leak
  if(weight_)
    delete weight_;
  string type = weightcfg.at(0);
  if(type=="TH2F")
  {
    string objkey   = weightcfg.at(1);
    string filename = weightcfg.at(2);
    cout<<"> Loading E/p weight as TH2F from "<<filename<<"/"<<objkey<<endl;
    TFile* inweightfile = new TFile(filename.c_str(),"READ");
    weight_ = (TH2F*) inweightfile->Get(objkey.c_str());
    weight_ -> SetDirectory(0);
    inweightfile->Close();
  }

  else
    cout<<"[ERROR]: unknown type "<<type<<endl;

}

Float_t  calibrator::GetWeight(const Float_t &Eta, const Float_t &Eop)
{
  return weight_->GetBinContent(weight_->FindBin(Eta,Eop));
}
