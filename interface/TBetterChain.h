#ifndef ROOT_TBetterChain
#define ROOT_TBetterChain

#include "TFile.h"
#include "TTreeCache.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TFriendElement.h"
#include "TVirtualIndex.h"

class TBetterChain: public TChain
{
 public:
  TBetterChain();
  TBetterChain(const char* name, const char* title = "");
  virtual ~TBetterChain();
  Int_t GetEntry(Long64_t entry=0, Int_t getall=0);
  Long64_t  LoadTree(Long64_t entry);
};
#endif
