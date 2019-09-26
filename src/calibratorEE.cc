#include "calibratorEE.h"

using namespace std;

calibratorEE::calibratorEE(CfgManager conf):
  calibrator(conf),
  EScorrection_(false)
{
  //-------------------------------------
  //initialize EEring
  string EEringsFileName = conf.GetOpt<string> ("Input.eeringsFileName");
  eeRing_ = new TEndcapRings(EEringsFileName);
  if(conf.OptExist("Input.EScorrection"))
    EScorrection_ = conf.GetOpt<bool>("Input.EScorrection");
  else
    cout<<"[WARNING]: option Input.EScorrection NOT found --> set "<<EScorrection_ <<" by default"<<endl;

}

calibratorEE::~calibratorEE()
{
  if(eeRing_)
    delete eeRing_;
}

//sometimes ES energy > tracker momentum, in those cases return 0
Float_t calibratorEE::GetPcorrected(const Int_t &i)
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

Float_t calibratorEE::GetICEnergy(const Int_t &i)
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

Float_t calibratorEE::GetRegression(const Int_t &i) 
{
  return energySCEle_[i]/(rawEnergySCEle_[i]+EScorrection_*esEnergySCEle_[i]);
}
