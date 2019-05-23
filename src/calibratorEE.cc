#include "calibratorEE.h"
//#include "utils.h"

using namespace std;

calibratorEE::calibratorEE(CfgManager conf):
  calibrator(conf)
{
  //-------------------------------------
  //initialize EEring
  string EEringsFileName = conf.GetOpt<string> ("Input.eeringsFileName");
  eeRing_ = new TEndcapRings(EEringsFileName);
}

calibratorEE::~calibratorEE()
{
  if(eeRing_)
    delete eeRing_;
}

Float_t calibratorEE::GetPcorrected(const Int_t &i)
{
#ifdef DEBUG
  if(!electron_momentum_correction_)
    cerr<<"[ERROR]: electron momentum correction not loaded"<<endl;
  if(!positron_momentum_correction_)
    cerr<<"[ERROR]: positron momentum correction not loaded"<<endl;
#endif
  if(chargeEle_[i]==-1)
    return pAtVtxGsfEle_[i]/electron_momentum_correction_->Eval(phiEle_[i]) - esEnergySCEle_[i];
  if(chargeEle_[i]==+1)
    return pAtVtxGsfEle_[i]/positron_momentum_correction_->Eval(phiEle_[i]) - esEnergySCEle_[i];
  return -999.;
}

Float_t calibratorEE::GetICEnergy(const Int_t &i)
{
  float kRegression=1;
  if(useRegression_)
    kRegression=energySCEle_[i]/(rawEnergySCEle_[i]+esEnergySCEle_[i]);
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
    if(ieta<0)     cout<<"ieta<0"<<endl;
    if(iphi<0)     cout<<"iphi<0"<<endl;
#endif
    IC = (xtal_[fromIetaIphito1Dindex(ieta,iphi,Neta_,Nphi_,ietamin_,iphimin_)]).IC;
    E += kRegression * ERecHit_[i]->at(iRecHit) * fracRecHit_[i]->at(iRecHit) * IC;
  }
      
  return E;
}


Int_t calibratorEE::GetEERingSeed(const Int_t &i)
{
  return eeRing_->GetEndcapRing( xSeed_[i], ySeed_[i], (int)(etaSCEle_[i]>0) );
}

void calibratorEE::GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i)
{
  ieta = eeRing_->GetEndcapIeta(xSeed_[i], ySeed_[i], (int)(etaSCEle_[i]>0) );
  iphi = eeRing_->GetEndcapIphi(xSeed_[i], ySeed_[i], (int)(etaSCEle_[i]>0) );
}

int calibratorEE::GetietaSeed(const Int_t &i)
{
  return eeRing_->GetEndcapIeta(xSeed_[i], ySeed_[i], (int)(etaSCEle_[i]>0) );
}

int calibratorEE::GetiphiSeed(const Int_t &i)
{
  return eeRing_->GetEndcapIphi(xSeed_[i], ySeed_[i], (int)(etaSCEle_[i]>0) );
}
