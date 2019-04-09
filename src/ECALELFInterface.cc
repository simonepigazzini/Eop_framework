#include "ECALELFInterface.h"

using namespace std;

void ECALELFInterface::BranchSelected(TChain* chain)
{
  chain->SetBranchAddress("runNumber",          &runNumber_);
  chain->SetBranchAddress("lumiBlock",          &lumiBlock_);
  chain->SetBranchAddress("eventNumber",        &eventNumber_);
  chain->SetBranchAddress("chargeEle",          chargeEle_);
  chain->SetBranchAddress("etaEle",             etaEle_);
  chain->SetBranchAddress("phiEle",             phiEle_);
  chain->SetBranchAddress("rawEnergySCEle",     rawEnergySCEle_);
  chain->SetBranchAddress("energy_ECAL_ele",    energySCEle_); //OLD ENERGY: energySCEle_must
  chain->SetBranchAddress("etaSCEle",           etaSCEle_);
  chain->SetBranchAddress("esEnergySCEle",      esEnergySCEle_);
  chain->SetBranchAddress("pAtVtxGsfEle",       pAtVtxGsfEle_);
  chain->SetBranchAddress("fbremEle",           fbremEle_);
  chain->SetBranchAddress("xSeedSC",            xSeed_);
  chain->SetBranchAddress("ySeedSC",            ySeed_);
}


void ECALELFInterface::BranchExtraCalib(TChain* chain)
{
  for(int i=0;i<2;++i)
  {
    ERecHit_[i]=0;
    XRecHit_[i]=0;  //ETA
    YRecHit_[i]=0;  //PHI
    ZRecHit_[i]=0;
    recoFlagRecHit_[i]=0;
    fracRecHit_[i]=0;
  }

  // ele1
  chain->SetBranchAddress("energyRecHitSCEle1",   &ERecHit_[0]);
  chain->SetBranchAddress("XRecHitSCEle1",        &XRecHit_[0]);
  chain->SetBranchAddress("YRecHitSCEle1",        &YRecHit_[0]);
  chain->SetBranchAddress("ZRecHitSCEle1",        &ZRecHit_[0]);
  chain->SetBranchAddress("recoFlagRecHitSCEle1", &recoFlagRecHit_[0]);
  chain->SetBranchAddress("fracRecHitSCEle1",     &fracRecHit_[0]);
  // ele2
  chain->SetBranchAddress("energyRecHitSCEle2",   &ERecHit_[1]);
  chain->SetBranchAddress("XRecHitSCEle2",        &XRecHit_[1]);
  chain->SetBranchAddress("YRecHitSCEle2",        &YRecHit_[1]);
  chain->SetBranchAddress("ZRecHitSCEle2",        &ZRecHit_[1]);
  chain->SetBranchAddress("recoFlagRecHitSCEle2", &recoFlagRecHit_[1]);
  chain->SetBranchAddress("fracRecHitSCEle2",     &fracRecHit_[1]);
}


ECALELFInterface::ECALELFInterface(CfgManager conf)
{
  //-------------------------------------
  //initialize chain and branch tree
  std::vector<std::string> treelist = conf.GetOpt<std::vector<std::string> >("Input.treelist");
  for(auto treename : treelist)
  { 
    ch_[treename] = new TChain(treename.c_str(),treename.c_str());
    std::vector<std::string> filelist = conf.GetOpt<std::vector<std::string> >(Form("Input.%s.filelist",treename.c_str()));
    for(auto filename : filelist)
      ch_[treename]->Add(filename.c_str());
    if(treename=="selected")
      BranchSelected(ch_[treename]);
    else
      if(treename=="extraCalibTree")
	BranchExtraCalib(ch_[treename]);
      else
	cerr<<"[WARNING]: unknown tree "<<treename<<endl;
  }

  for(unsigned int nchain = 1; nchain < treelist.size(); ++nchain)
  {
    cout << ">>> Adding chain " << treelist.at(nchain) << " as friend to chain " << treelist.at(0) << endl;
    ch_[treelist.at(0)]->AddFriend(treelist.at(nchain).c_str(),"");
  }
  chain_=ch_[treelist.at(0)];
  Ncurrtree_=1;

  //-------------------------------------
  //load event selection
  selection_str_ = conf.GetOpt<string> ("Input.selection");
  selection_ = new TTreeFormula("selection", selection_str_.c_str(), chain_);
  
}

ECALELFInterface::~ECALELFInterface()
{
  delete selection_;

  for(auto ch_iterator : ch_)
    if(ch_iterator.second)
      (ch_iterator.second)->Delete();
}

Long64_t ECALELFInterface::GetEntry(const Long64_t &entry)
{
  Long64_t i;
  i=chain_->GetEntry(entry);
  if(chain_->GetTreeNumber() != Ncurrtree_)
  {
    Ncurrtree_ = chain_->GetTreeNumber();
    selection_->UpdateFormulaLeaves();
  }
  return i;
}

Float_t ECALELFInterface::GetEnergy(const Int_t &i)
{
  return energySCEle_[i];
}

Float_t ECALELFInterface::GetEnergyRaw(const Int_t &i)
{
  return rawEnergySCEle_[i];
}

Float_t ECALELFInterface::GetESEnergy(const Int_t &i)
{
  return esEnergySCEle_[i];
}

Float_t  ECALELFInterface::GetP(const Int_t &i)
{
  return pAtVtxGsfEle_[i];
}

Float_t ECALELFInterface::GetEtaSC(const Int_t &i)
{
  return etaSCEle_[i];
}

Float_t ECALELFInterface::GetPhi(const Int_t &i)
{
  return phiEle_[i];
}

void ECALELFInterface::GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i)
{
  ieta=xSeed_[i];
  iphi=ySeed_[i];
}

int ECALELFInterface::GetietaSeed(const Int_t &i)
{
  return xSeed_[i];
}

int ECALELFInterface::GetiphiSeed(const Int_t &i)
{
  return ySeed_[i];
}
