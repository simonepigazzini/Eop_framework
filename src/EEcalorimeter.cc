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

Float_t  EEcalorimeter::GetPcorrected(const Int_t &i)
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

Int_t EEcalorimeter::GetEERingSeed(const Int_t &i)
{
  return eeRing->GetEndcapRing( xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}

void EEcalorimeter::GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i)
{
  ieta = eeRing->GetEndcapIeta(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
  iphi = eeRing->GetEndcapIphi(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}

int      EEcalorimeter::GetietaSeed(const Int_t &i)
{
  return eeRing->GetEndcapIeta(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}

int      EEcalorimeter::GetiphiSeed(const Int_t &i)
{
  return eeRing->GetEndcapIphi(xSeed[i], ySeed[i], (int)(etaSCEle[i]>0) );
}
