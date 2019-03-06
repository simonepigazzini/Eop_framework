#include "EEcalorimeter.h"
#include "utils.h"

using namespace std;

EEcalorimeter::EEcalorimeter(CfgManager conf):
  calorimeter(conf)
{
  //-------------------------------------
  //initialize EEring
  string EEringsFileName = conf.GetOpt<string> ("Input.eeringsFileName");
  eeRing = new TEndcapRings(EEringsFileName);
}

EEcalorimeter::~EEcalorimeter()
{
  if(eeRing)
    delete eeRing;
}

Float_t EEcalorimeter::GetPcorrected(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]: array out of range"<<endl;
  if(!electron_momentum_correction)
    cerr<<"[ERROR]: electron momentum correction not loaded"<<endl;
  if(!positron_momentum_correction)
    cerr<<"[ERROR]: positron momentum correction not loaded"<<endl;
  if(chargeEle[i]==-1)
    return pAtVtxGsfEle[i]/electron_momentum_correction->Eval(phiEle[i]) - esEnergySCEle[i];
  if(chargeEle[i]==+1)
    return pAtVtxGsfEle[i]/positron_momentum_correction->Eval(phiEle[i]) - esEnergySCEle[i];
  return -999.;
}

Float_t EEcalorimeter::GetICEnergy(const Int_t &i)
{
  if(i>1)
    cerr<<"[ERROR]:energy array out of range"<<endl;
  float kRegression=1;
  if(useRegression)
    kRegression=energySCEle[i]/(rawEnergySCEle[i]+esEnergySCEle[i]);
  float E=0;
  float IC = 1.;
  int ieta,iphi;

  for(unsigned int iRecHit = 0; iRecHit < ERecHit[i]->size(); iRecHit++) 
  {
    if(recoFlagRecHit[i]->at(iRecHit) >= 4)
	continue;
    ieta = XRecHit[i]->at(iRecHit);
    iphi = YRecHit[i]->at(iRecHit);
    /*
    if(ieta>=Neta) cout<<"ieta>=Neta"<<endl;
    if(iphi>=Nphi) cout<<"iphi>=Nphi"<<endl;
    if(ieta<0)     cout<<"ieta<0"<<endl;
    if(iphi<0)     cout<<"iphi<0"<<endl;

    if(ieta>=Neta || iphi>=Nphi || ieta<0 || iphi<0)
      continue;
    */
    IC = (xtal[fromIetaIphito1Dindex(ieta,iphi,Neta,Nphi,ietamin,iphimin)]).IC;
    E += kRegression * ERecHit[i]->at(iRecHit) * fracRecHit[i]->at(iRecHit) * IC;
  }
      
  return E;
}


Int_t EEcalorimeter::GetEERingSeed(const Int_t &i)
{
  return eeRing->GetEndcapRing( xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}

void EEcalorimeter::GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i)
{
  ieta = eeRing->GetEndcapIeta(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
  iphi = eeRing->GetEndcapIphi(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}

int EEcalorimeter::GetietaSeed(const Int_t &i)
{
  return eeRing->GetEndcapIeta(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}

int EEcalorimeter::GetiphiSeed(const Int_t &i)
{
  return eeRing->GetEndcapIphi(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}
