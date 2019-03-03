#include "calorimeter.h"
#include "utils.h"

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
  chain->SetBranchAddress("xSeedSC",            xSeed);
  chain->SetBranchAddress("ySeedSC",            ySeed);
  //chain->SetBranchAddress("energyMCEle",      energyMCEle);
  //chain->SetBranchAddress("etaMCEle",         etaMCEle);
  //chain->SetBranchAddress("phiMCEle",         phiMCEle);
}


void calorimeter::BranchExtraCalib(TChain* chain)
{
  for(int i=0;i<2;++i)
  {
    ERecHit[i]=0;
    XRecHit[i]=0;  //ETA
    YRecHit[i]=0;  //PHI
    ZRecHit[i]=0;
    recoFlagRecHit[i]=0;
    fracRecHit[i]=0;
  }

  // ele1
  /*  energyRecHitSCEle1=0;
  XRecHitSCEle1=0;
  YRecHitSCEle1=0;
  ZRecHitSCEle1=0;
  recoFlagRecHitSCEle1=0;
  fracRecHitSCEle1=0;
  */
  chain->SetBranchAddress("energyRecHitSCEle1",   &ERecHit[0]);
  chain->SetBranchAddress("XRecHitSCEle1",        &XRecHit[0]);
  chain->SetBranchAddress("YRecHitSCEle1",        &YRecHit[0]);
  chain->SetBranchAddress("ZRecHitSCEle1",        &ZRecHit[0]);
  chain->SetBranchAddress("recoFlagRecHitSCEle1", &recoFlagRecHit[0]);
  chain->SetBranchAddress("fracRecHitSCEle1",     &fracRecHit[0]);
  // ele2
  /*  energyRecHitSCEle2=0;
  XRecHitSCEle2=0;
  YRecHitSCEle2=0;
  ZRecHitSCEle2=0;
  recoFlagRecHitSCEle2=0;
  fracRecHitSCEle2=0;
  */
  chain->SetBranchAddress("energyRecHitSCEle2",   &ERecHit[1]);
  chain->SetBranchAddress("XRecHitSCEle2",        &XRecHit[1]);
  chain->SetBranchAddress("YRecHitSCEle2",        &YRecHit[1]);
  chain->SetBranchAddress("ZRecHitSCEle2",        &ZRecHit[1]);
  chain->SetBranchAddress("recoFlagRecHitSCEle2", &recoFlagRecHit[1]);
  chain->SetBranchAddress("fracRecHitSCEle2",     &fracRecHit[1]);
}


calorimeter::calorimeter(CfgManager conf):
  electron_momentum_correction(0),
  positron_momentum_correction(0),
  weight(0),
  xtal(0)
{
  //-------------------------------------
  //initialize chain and branch tree
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
  Ncurrtree=1;

  //-------------------------------------
  //load event selection
  string selection_str = conf.GetOpt<string> ("Input.selection");
  selection = new TTreeFormula("selection", selection_str.c_str(), chain);

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
    weight = new TH2F("weight","weight",1,0.,5.,1,0.,2.5);
    weight->SetBinContent(1,1,1.);
  }
  
  //-------------------------------------
  //load input IC
  if(conf.OptExist("Input.inputIC"))
    LoadIC( conf.GetOpt<std::vector<std::string> > ("Input.inputIC") );
  else
    if(conf.OptExist("Input.ietamin") && conf.OptExist("Input.ietamax") && conf.OptExist("Input.iphimin") && conf.OptExist("Input.iphimax"))
    {
      ietamin = conf.GetOpt<int> ("Input.ietamin");
      ietamax = conf.GetOpt<int> ("Input.ietamax");
      iphimin = conf.GetOpt<int> ("Input.iphimin");
      iphimax = conf.GetOpt<int> ("Input.iphimax");
      InitializeIC();
    }
    else
      cout<<"[WARNING]: no inputIC, nor ietamin&&ietamax&&iphimin&&iphimax found in Input in cfg"<<endl;

  //-------------------------------------
  //set true or false the usage of regression --> change the energy value 
  useRegression = true;
  if(conf.OptExist("Input.useRegression"))
    useRegression = conf.GetOpt<bool> ("Input.useRegression");

}


calorimeter::~calorimeter()
{
  delete selection;

  for(auto ch_iterator : ch)
    if(ch_iterator.second)
      (ch_iterator.second)->Delete();

  delete [] xtal;

  if(electron_momentum_correction)
    delete electron_momentum_correction;

  if(positron_momentum_correction)
    delete positron_momentum_correction;

  if(weight)
    delete weight;
}

void calorimeter::LoadMomentumCorrection(string filename)
{
  //delete previous corrections to avoid memory leak
  if(electron_momentum_correction)
    delete electron_momentum_correction;
  if(positron_momentum_correction)
    delete positron_momentum_correction;

  cout<<"> Loading momentum correction from file "<<filename<<endl;
  TFile* momentumcorrectionfile = new TFile(filename.c_str(),"READ");
  electron_momentum_correction  = (TGraphErrors*)(momentumcorrectionfile->Get("electron_correction"));
  positron_momentum_correction = (TGraphErrors*)(momentumcorrectionfile->Get("positron_correction"));
  momentumcorrectionfile->Close();
}

void calorimeter::LoadEopWeight(const vector<string> &weightcfg)
{
  //delete previous weight to avoid memory leak
  if(weight)
    delete weight;
  string type = weightcfg.at(0);
  if(type=="TH2F")
  {
    string objkey   = weightcfg.at(1);
    string filename = weightcfg.at(2);
    cout<<"> Loading E/p weight as TH2F from "<<filename<<"/"<<objkey<<endl;
    TFile* inweightfile = new TFile(filename.c_str(),"READ");
    weight = (TH2F*) inweightfile->Get(objkey.c_str());
    weight -> SetDirectory(0);
    inweightfile->Close();
  }

  else
    cout<<"[ERROR]: unknown type "<<type<<endl;

}

void calorimeter::LoadIC(const std::vector<std::string> &ICcfg)
{
  if(xtal)
    delete[] xtal;
  string objkey   = ICcfg[0];
  string filename = ICcfg[1];
  cout<<"> Loading IC from "<<filename<<"/"<<objkey<<endl;
  TFile* inICfile = new TFile(filename.c_str(),"READ");
  TH2F* ICmap = (TH2F*) inICfile->Get(objkey.c_str());

  Neta=ICmap->GetNbinsY();
  ietamin=ICmap->GetYaxis()->GetXmin();
  ietamax=ICmap->GetYaxis()->GetXmax()-1;//i want the left limit of last bin, not the right one
  Nphi=ICmap->GetNbinsX();
  iphimin=ICmap->GetXaxis()->GetXmin();
  iphimax=ICmap->GetXaxis()->GetXmax()-1;//i want the left limit of last bin, not the right one
  //cout<<">>> Neta="<<Neta<<" in ["<<ietamin<<","<<ietamax<<"] and Nphi="<<Nphi<<" in ["<<iphimin<<","<<iphimax<<"]"<<endl;

  xtal = new crystal[Neta*Nphi];
  for(int xbin=1; xbin<Nphi+1; ++xbin)
    for(int ybin=1; ybin<Neta+1; ++ybin)
    {
      int index = fromTH2indexto1Dindex(xbin, ybin, Nphi, Neta);
      xtal[index].IC = ICmap->GetBinContent(xbin,ybin); 
      xtal[index].status = 1;
    }

  inICfile->Close();
}

void calorimeter::InitializeIC()
{
  Neta=ietamax-ietamin+1;
  Nphi=iphimax-iphimin+1;
  cout<<"> Initialize calorimeter with Neta="<<Neta<<" in ["<<ietamin<<","<<ietamax<<"] and Nphi="<<Nphi<<" in ["<<iphimin<<","<<iphimax<<"]"<<endl;
  xtal = new crystal[Neta*Nphi];
  for(int xbin=1; xbin<Nphi+1; ++xbin)
    for(int ybin=1; ybin<Neta+1; ++ybin)
    {
      int index = fromTH2indexto1Dindex(xbin, ybin, Nphi, Neta);
      xtal[index].IC = 1;
      xtal[index].status = 1;
    }
}

Long64_t calorimeter::GetEntry(const Long64_t &entry)
{
  Long64_t i;
  i=chain->GetEntry(entry);
  if(chain->GetTreeNumber() != Ncurrtree)
  {
    Ncurrtree = chain->GetTreeNumber();
    selection->UpdateFormulaLeaves();
  }
  return i;
}

Float_t calorimeter::GetEnergy(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]:array out of range"<<endl;
  return energySCEle[i];
}

Float_t calorimeter::GetESEnergy(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]:array out of range"<<endl;
  return esEnergySCEle[i];
}

Float_t  calorimeter::GetP(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]:array out of range"<<endl;
  return pAtVtxGsfEle[i];
}

Float_t  calorimeter::GetPcorrected(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]: array out of range"<<endl;
  if(!electron_momentum_correction)
    cerr<<"[ERROR]: electron momentum correction not loaded"<<endl;
  if(!positron_momentum_correction)
    cerr<<"[ERROR]: positron momentum correction not loaded"<<endl;
  if(chargeEle[i]==-1)
    return pAtVtxGsfEle[i]/electron_momentum_correction->Eval(phiEle[i]);
  if(chargeEle[i]==+1)
    return pAtVtxGsfEle[i]/positron_momentum_correction->Eval(phiEle[i]);
  return -999.;
}

Float_t calorimeter::GetEtaSC(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]:array out of range"<<endl;
  return etaSCEle[i];
}

Float_t calorimeter::GetPhi(const Int_t &i)
{
  if(i>3)
    cerr<<"[ERROR]:array out of range"<<endl;
  return phiEle[i];
}

Float_t  calorimeter::GetIC(const Int_t &index)
{
  return (xtal[index]).IC;
}


Float_t calorimeter::GetIC(const Int_t &ieta, const Int_t &iphi)
{
  return ( xtal[ fromIetaIphito1Dindex(ieta,iphi,Neta,Nphi,ietamin,iphimin) ] ).IC;
}

void calorimeter::GetSeed(Int_t &ieta, Int_t &iphi, const Int_t &i)
{
  ieta=xSeed[i];
  iphi=ySeed[i];
}

int calorimeter::GetietaSeed(const Int_t &i)
{
  return xSeed[i];
}

int calorimeter::GetiphiSeed(const Int_t &i)
{
  return ySeed[i];
}

Float_t calorimeter::GetICEnergy(const Int_t &i)
{
  if(i>1)
    cerr<<"[ERROR]:energy array out of range"<<endl;
  float kRegression=1;
  if(useRegression)
    kRegression=energySCEle[i]/rawEnergySCEle[i];
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

Float_t  calorimeter::GetWeight(const Float_t &Eta, const Float_t &Eop)
{
  return weight->GetBinContent(weight->FindBin(Eta,Eop));
}
