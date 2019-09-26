#ifndef TIMEBIN__
#define TIMEBIN__

#include <iostream>
#include <string>
#include <set>
#include <map>

#include "TH1F.h"
#include "TTree.h"

namespace TimeBin
{
  struct runlumi
  {
    UInt_t runNumber;
    UShort_t lumiBlock;
    bool operator<(const runlumi &other) const
    {
      if(runNumber < other.runNumber)
	return true;
      else
	if(runNumber > other.runNumber)
	  return false;
	else//runNumber = other.runNumber                                                                                                             
	  if(lumiBlock < other.lumiBlock)
	    return true;
	  else
	    return false;
    }    
  };

  struct timeweight
  {
    UInt_t time0;
    UInt_t timef;
    int weight=0;
  };


class TimeBin
{
  
 public:

  //---ctors---
  TimeBin();
  //TimeBin(const UInt_t &runNumber, UShort_t &lumiBlock, UInt_t &time0, UInt_t &timef, int &weight);
  TimeBin(const TimeBin &bincopy);

  //---dtor---
  ~TimeBin();

  //---utils--
  void     SetBinRanges(const UInt_t &runmin, const UInt_t &runmax, const UShort_t &lsmin, const UShort_t &lsmax, const UInt_t &timemin, const UInt_t &timemax);
  void     SetNev(const int &Nev_bin);
  bool     operator<(const TimeBin& other) const;
  void     BranchOutput(TTree* outtree);
  void     BranchInput(TTree* intree);
  TimeBin& operator=(const TimeBin& other);
  bool     Match(const UInt_t &run, const UShort_t &ls, const UInt_t &time) const;
  void     FillHisto(double x) const {h_scale_->Fill(x);} ;
  bool     InitHisto( char* name, char* title, const int &Nbin, const double &xmin, const double &xmax);
  double   GetMean();
  //double GetMean(double xmin, double xmax);
  //double GetMean(double evfraction);
  double   GetMedian();
  //void   SaveAs(std::string outputfilename);  
  void     SetVariable(const std::string &variablename, const float &variablevalue);
  float    GetVariable(const std::string &variablename){return variablelist_[variablename];};
  void     PrintVariables();  
 protected:
  UInt_t runmin_;
  UInt_t runmax_;
  UShort_t lsmin_;
  UShort_t lsmax_;
  UInt_t timemin_;
  UInt_t timemax_;
  int Nev_;
  TH1F* h_scale_;//in this way if I define a std::set<TimeBin>, I can modify the histo content 
  std::map<std::string,float> variablelist_;//in this way if I define a std::set<TimeBins>, I can modify the map content

  //private:
  //void BranchInputTree();

};
}
#endif
