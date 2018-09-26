#include "interface/calorimeter.h"

using namespace std;

void calorimeter::BranchSelected(TChain* chain)
{
  chain->SetBranchAddress("runNumber",          &runNumber);
  chain->SetBranchAddress("lumiBlock",          &lumiBlock);
  chain->SetBranchAddress("eventNumber",        &eventNumber);
  chain->SetBranchAddress("chargeEle",          chargeEle);
  chain->SetBranchAddress("etaEle",             etaEle);
  chain->SetBranchAddress("phiEle",             phiEle);
  chain->SetBranchAddress("rawEnergySCEle",     rawEnergySCEle);
  chain->SetBranchAddress("energy_ECAL_ele",    energySCEle); //OLD ENERGY: energySCEle_must
  chain->SetBranchAddress("etaSCEle",           etaSCEle);
  chain->SetBranchAddress("esEnergySCEle",      esEnergySCEle);
  chain->SetBranchAddress("pAtVtxGsfEle",       pAtVtxGsfEle);
  chain->SetBranchAddress("fbremEle",           fbremEle);
  //chain->SetBranchAddress("energyMCEle",      energyMCEle);
  //chain->SetBranchAddress("etaMCEle",         etaMCEle);
  //chain->SetBranchAddress("phiMCEle",         phiMCEle);
}


void calorimeter::BranchExtraCalib(TChain* chain)
{
  // ele1
  energyRecHitSCEle1=0;
  XRecHitSCEle1=0;
  YRecHitSCEle1=0;
  ZRecHitSCEle1=0;
  recoFlagRecHitSCEle1=0;
  chain->SetBranchAddress("energyRecHitSCEle1",   &energyRecHitSCEle1);
  chain->SetBranchAddress("XRecHitSCEle1",        &XRecHitSCEle1);
  chain->SetBranchAddress("YRecHitSCEle1",        &YRecHitSCEle1);
  chain->SetBranchAddress("ZRecHitSCEle1",        &ZRecHitSCEle1);
  chain->SetBranchAddress("recoFlagRecHitSCEle1", &recoFlagRecHitSCEle1);
  // ele2
  energyRecHitSCEle2=0;
  XRecHitSCEle2=0;
  YRecHitSCEle2=0;
  ZRecHitSCEle2=0;
  recoFlagRecHitSCEle2=0;
  chain->SetBranchAddress("energyRecHitSCEle2",   &energyRecHitSCEle2);
  chain->SetBranchAddress("XRecHitSCEle2",        &XRecHitSCEle2);
  chain->SetBranchAddress("YRecHitSCEle2",        &YRecHitSCEle2);
  chain->SetBranchAddress("ZRecHitSCEle2",        &ZRecHitSCEle2);
  chain->SetBranchAddress("recoFlagRecHitSCEle2", &recoFlagRecHitSCEle2);
}


calorimeter::calorimeter(CfgManager conf)
{
  std::vector<std::string> treelist = conf.GetOpt<std::vector<std::string> >("Input.treelist");
  for(auto treename : treelist)
  { 
    ch[treename] = new TChain(treename.c_str(),treename.c_str());
    std::vector<std::string> filelist = conf.GetOpt<std::vector<std::string> >(Form("Input.%s.filelist",treename.c_str()));
    for(auto filename : filelist)
      ch[treename]->Add(filename.c_str());
    if(treename=="selected")
      BranchSelected(ch[treename]);
    else
      if(treename=="extraCalibTree")
	BranchExtraCalib(ch[treename]);
      else
	cerr<<"[WARNING]: unknown tree "<<treename<<endl;
  }

  for(unsigned int nchain = 1; nchain < treelist.size(); ++nchain)
  {
    std::cout << ">>> Adding chain " << treelist.at(nchain) << " as friend to chain " << treelist.at(0) << std::endl;
    ch[treelist.at(0)]->AddFriend(treelist.at(nchain).c_str(),"");
  }
  chain=ch[treelist.at(0)];

  if(conf.OptExist("Input.InitializeFromFile"))
  {
    std::string rootfile = conf.GetOpt<std::string> ("Input.InitializeFromFile");
    Initialize(rootfile);
  }
  else
  {
    Neta = conf.GetOpt<int> ("Input.Neta");
    Nphi = conf.GetOpt<int> ("Input.Nphi");
    Initialize();
  }

  useRegression = true;
  if(conf.OptExist("Input.useRegression"))
    bool useRegression = conf.GetOpt<bool> ("Input.useRegression");

}


calorimeter::~calorimeter()
{
  for(auto ch_iterator : ch)
    if(ch_iterator.second)
      (ch_iterator.second)->Delete();

  for(int iphi = 0; iphi < Nphi; ++iphi)
    delete [] xtal[iphi];
  delete [] xtal;

}


void calorimeter::Initialize()
{
  cout<<"Initialize calorimeter with Neta="<<Neta<<" and Nphi="<<Nphi<<endl;
  xtal = new crystal*[Nphi];
  for(int iphi = 0; iphi < Nphi; ++iphi)
    xtal[iphi] = new crystal[Neta];

  for(int ieta=0;ieta<Neta;++ieta)
    for(int iphi=0;iphi<Nphi;++iphi)
    {
      (xtal[iphi][ieta]).IC = 1.;
      (xtal[iphi][ieta]).status = 1.;
    }
}

void calorimeter::Initialize(const std::string &rootfile)
{
  cout<<"Initialize from file "<<rootfile<<endl;



}

Long64_t calorimeter::GetEntry(const Long64_t &entry)
{
  Long64_t i;
  i=chain->GetEntry(entry);
  if(chain->GetTreeNumber() != Ncurrtree)
  {
    Ncurrtree = chain->GetTreeNumber();
    //Update TTreeFormula
    //...
  }
  return i;
}

Float_t calorimeter::GetEnergy(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]:energy array out of range"<<endl;
  return energySCEle[i];
}

Float_t calorimeter::GetICEnergy(const Int_t &i)
{
  if(i>2)
    cerr<<"[ERROR]:energy array out of range"<<endl;
  kRegression=energySCEle[i]/rawEnergySCEle[i];
  float E=0;
  float IC = 1.;

  if(i==0)
  {
    for(unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle1->size(); iRecHit++) 
    {
      if(recoFlagRecHitSCEle1->at(iRecHit) >= 4)
	continue;
      int ieta = XRecHitSCEle1->at(iRecHit);
      int iphi = YRecHitSCEle1->at(iRecHit);
      if(ieta>Neta || iphi>Nphi)
	continue;
      IC = (xtal[iphi][ieta]).IC;
      E += kRegression * energyRecHitSCEle1 -> at(iRecHit) * IC;
    }
  }
  else
    if(i==1)
    {
      for(unsigned int iRecHit = 0; iRecHit < energyRecHitSCEle2->size(); iRecHit++) 
      {
	if(recoFlagRecHitSCEle2->at(iRecHit) >= 4)
	  continue;
	int ieta = XRecHitSCEle2->at(iRecHit);
	int iphi = YRecHitSCEle2->at(iRecHit);
	if(ieta>Neta || iphi>Nphi)
	  continue;
	IC = (xtal[iphi][ieta]).IC;
	E += kRegression * energyRecHitSCEle2 -> at(iRecHit) * IC;
      }
    }
      
  return E;
}


const std::vector<float>* calorimeter::GetRecHit1()
{
  return energyRecHitSCEle1;
}


/*
calorimeter::Initialize(const std::string &rootfile)
{


}

calorimeter::Initialize();
*/
