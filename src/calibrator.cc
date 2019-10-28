#include "calibrator.h"
//#include "utils.h"

using namespace std;

calibrator::calibrator(CfgManager conf):
  ECALELFInterface(conf),
  ICmanager(conf),
  electron_momentum_correction_(0),
  positron_momentum_correction_(0),
  weight_(0),
  EScorrection_(false)
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

  if(conf.OptExist("Input.EScorrection"))
    EScorrection_ = conf.GetOpt<bool>("Input.EScorrection");
  else
    cout<<"[WARNING]: option Input.EScorrection NOT found --> set "<<EScorrection_ <<" by default"<<endl;


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

void calibrator::PrintSettings()
{
  ECALELFInterface::PrintSettings();
  ICmanager::PrintSettings();
  cout<<"----------------------------------------------------------------------------------"<<endl;
  cout<<"> calibrator settings:"<<endl;
  if(useRegression_)
    cout<<">>> USE REGRESSION: true"<<endl;
  else
    cout<<">>> USE REGRESSION: false"<<endl;

  if(electron_momentum_correction_)
    cout<<">>> Electron momentum correction \'"<<electron_momentum_correction_->GetName()<<"\' LOADED"<<endl;
  else
    cout<<">>> Electron momentum correction NOT LOADED"<<endl;

  if(positron_momentum_correction_)
    cout<<">>> Positron momentum correction \'"<<positron_momentum_correction_->GetName()<<"\' LOADED"<<endl;
  else
    cout<<">>> Positron momentum correction NOT LOADED"<<endl;

  if(weight_)
    cout<<">>> Eop weight \'"<<weight_->GetName()<<"\' LOADED"<<endl;
  else
    cout<<">>> Eop weight NOT LOADED"<<endl;
  cout<<"----------------------------------------------------------------------------------"<<endl;
}

Float_t calibrator::GetPcorrected(const Int_t &i)
{
  if(isEB(i))
    return GetPcorrectedEB(i);
  else
    if(isEE(i))
      return GetPcorrectedEE(i);
    else
      return -999;
}

Float_t calibrator::GetICEnergy(const Int_t &i)
{
  if(isEB(i))
    return GetICEnergyEB(i);
  else
    if(isEE(i))
      return GetICEnergyEE(i);
    else
      return -999;
  
}

Float_t calibrator::GetRegression(const Int_t &i)
{
  if(isEB(i))
    return GetRegressionEB(i);
  else
    if(isEE(i))
      return GetRegressionEE(i);
    else
      return -999;

}


//sometimes ES energy > tracker momentum, in those cases return 0
Float_t calibrator::GetPcorrectedEE(const Int_t &i)
{
#ifdef DEBUG
  if(!electron_momentum_correction_)
    cerr<<"[ERROR]: electron momentum correction not loaded"<<endl;
  if(!positron_momentum_correction_)
    cerr<<"[ERROR]: positron momentum correction not loaded"<<endl;
#endif
  if(chargeEle_[i]==-1)
  {
    float pCORR = pAtVtxGsfEle_[i]/electron_momentum_correction_->Eval(phiEle_[i]) - EScorrection_*esEnergySCEle_[i];
    if(pCORR>0)
      return pCORR;
    else
      return 0;
  }
  if(chargeEle_[i]==+1)
  {
    float pCORR= pAtVtxGsfEle_[i]/positron_momentum_correction_->Eval(phiEle_[i]) - EScorrection_*esEnergySCEle_[i];
    if(pCORR>0)
      return pCORR;
    else
      return 0;
  }
  return -999.;
}

Float_t calibrator::GetICEnergyEE(const Int_t &i)
{
  float kRegression=1;
  if(useRegression_)
    kRegression=energySCEle_[i]/(rawEnergySCEle_[i]+EScorrection_*esEnergySCEle_[i]);
  float E=0;
  float IC = 1.;
  int ix,iy;

  for(unsigned int iRecHit = 0; iRecHit < ERecHit_[i]->size(); iRecHit++) 
  {
    if(recoFlagRecHit_[i]->at(iRecHit) >= 4)
	continue;
    ix = XRecHit_[i]->at(iRecHit);
    iy = YRecHit_[i]->at(iRecHit);
#ifdef DEBUG
    if(ix>=Neta_) cout<<"ieta>=Neta"<<endl;
    if(iy>=Nphi_) cout<<"iphi>=Nphi"<<endl;
    if(ix<0)     cout<<"ieta<0"<<endl;
    if(iy<0)     cout<<"iphi<0"<<endl;
#endif
    if(ZRecHit_[i]->at(iRecHit)==0)
      IC=1.;
    else
      //the function fromIetaIphito1Dindex is tuned for EB --> ix and iy must be inverted
      IC = (xtal_[fromIetaIphito1Dindex(iy,ix,Neta_,Nphi_,ietamin_,iphimin_)]).IC;
    E += kRegression * ERecHit_[i]->at(iRecHit) * fracRecHit_[i]->at(iRecHit) * IC;
    //cout<<"GetICEnergy\tiRECHIT="<<iRecHit<<"\tix="<<XRecHit_[i]->at(iRecHit)<<"\tiy="<<YRecHit_[i]->at(iRecHit)<<"\tiz="<<ZRecHit_[i]->at(iRecHit)<<"\tIC="<<IC<<endl;
  }
      
  return E;
}

Float_t calibrator::GetRegressionEE(const Int_t &i) 
{
  return energySCEle_[i]/(rawEnergySCEle_[i]+EScorrection_*esEnergySCEle_[i]);
}



Float_t  calibrator::GetPcorrectedEB(const Int_t &i)
{
#ifdef DEBUG
  if(!electron_momentum_correction_)
    cerr<<"[ERROR]: electron momentum correction not loaded"<<endl;
  if(!positron_momentum_correction_)
    cerr<<"[ERROR]: positron momentum correction not loaded"<<endl;
#endif
  if(chargeEle_[i]==-1)
    return pAtVtxGsfEle_[i]/electron_momentum_correction_->Eval(phiEle_[i]);
  if(chargeEle_[i]==+1)
    return pAtVtxGsfEle_[i]/positron_momentum_correction_->Eval(phiEle_[i]);
  return -999.;
}

Float_t calibrator::GetICEnergyEB(const Int_t &i)
{
  if(i>1)
    cerr<<"[ERROR]:energy array out of range"<<endl;
  float kRegression=1;
  if(useRegression_)
    kRegression=energySCEle_[i]/rawEnergySCEle_[i];
  float E=0;
  float IC = 1.;
  int ieta,iphi;

  for(unsigned int iRecHit = 0; iRecHit < ERecHit_[i]->size(); iRecHit++) 
  {
    if(recoFlagRecHit_[i]->at(iRecHit) >= 4)
	continue;
    ieta = XRecHit_[i]->at(iRecHit);
    iphi = YRecHit_[i]->at(iRecHit);
#ifdef DEBUG
    if(ieta>=Neta_) cout<<"ieta>=Neta"<<endl;
    if(iphi>=Nphi_) cout<<"iphi>=Nphi"<<endl;
    if(ieta<0)      cout<<"ieta<0"<<endl;
    if(iphi<0)      cout<<"iphi<0"<<endl;
#endif
    if(ZRecHit_[i]->at(iRecHit)!=0)
      IC=1.;
    else
      IC = (xtal_[fromIetaIphito1Dindex(ieta,iphi,Neta_,Nphi_,ietamin_,iphimin_)]).IC;
    E += kRegression * ERecHit_[i]->at(iRecHit) * fracRecHit_[i]->at(iRecHit) * IC;
  }
      
  return E;
}

Float_t  calibrator::GetRegressionEB(const Int_t &i)
{
  return energySCEle_[i]/rawEnergySCEle_[i];
}
