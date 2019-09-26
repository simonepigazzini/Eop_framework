#include "MonitoringManager.h"

using namespace std;

MonitoringManager::MonitoringManager(CfgManager conf):
  ECALELFInterface(conf),
  conf_(conf),
  h_template_(0),
  last_accessed_bin_(timebins.end())
{
  label_ = conf.GetOpt<string> ("Input.label");  
  variablename_ = conf.GetOpt<string> ("LaserMonitoring.variable");
}

MonitoringManager::~MonitoringManager()
{
  if(h_template_)
    delete h_template_;
}
 
TH1F* MonitoringManager::BuildTemplate()
{
  int Nbin   = conf_.GetOpt<int>   ("LaserMonitoring.BuildTemplate.Nbin");
  float xmin = conf_.GetOpt<float> ("LaserMonitoring.BuildTemplate.xmin");
  float xmax = conf_.GetOpt<float> ("LaserMonitoring.BuildTemplate.xmax");

  string templatename = Form("h_template(%i,%f,%f)",Nbin,xmin,xmax);
  
  gDirectory->cd(0);

  cout<<">> Building template \""<<variablename_<<">>"<<templatename<<"\"\n"
      <<"   with selection \""<<selection_str_<<"\"\n"
      <<">> It can take hours.."<<endl; 

  chain_->Draw( Form("%s>>%s",variablename_.c_str(),templatename.c_str()) , selection_str_.c_str() , "goff");
  h_template_=(TH1F*)gDirectory->Get("h_template");
  h_template_->SetName(("h_template_"+label_).c_str());
  h_template_->SetTitle(("h_template_"+label_).c_str());
  //curr_dir->cd();
  return h_template_;
}

void  MonitoringManager::RunDivide()
{
  cout<<">> Running RunDivide"<<endl;
  int    Nevmax_bin     = conf_.GetOpt<int>          ("LaserMonitoring.RunDivide.Nevmax_bin");
  float  maxduration    = 60*60*conf_.GetOpt<float>  ("LaserMonitoring.RunDivide.maxduration");//It is provided in hours

  //Loop on events to create a map with key=(run,LS) and value=(time0,timef,multiplicity)
  //Exploit methods inherited from ECALELFInterface to access the ntuple content
  long Nentries = this->GetEntries();
  cout<<Nentries<<" total entries\n"<<endl;
  map<struct TimeBin::runlumi,struct TimeBin::timeweight> lumisec_list;
  for(long ientry=0; ientry<Nentries; ++ientry)
  {
    this->GetEntry(ientry);
    if(ientry%100000==0)
      cout<<"reading entry "<<ientry<<"\r"<<std::flush;
    int w=0;
    if(this->isSelected(0)) ++w;
    if(this->isSelected(1)) ++w;
    if(w)
    {
      unsigned t = this->GetTime();
      struct TimeBin::runlumi ev{this->GetRunNumber(), this->GetLS()};
      auto itr = lumisec_list.find(ev);
      if( itr == lumisec_list.end()) 
      {
	lumisec_list[ev].weight=w;
	lumisec_list[ev].time0=t;
	lumisec_list[ev].timef=t;
      }
      else
      {
	itr->second.weight+=w;
	if(t<itr->second.time0)
	  itr->second.time0=t;
	else
	  if(t>itr->second.timef)
	    itr->second.timef=t;
      }
      //cout<<"increment "<<w<<endl;
    }
  }
  
  cout<<endl;
  cout<<">> Merging lumisections"<<endl;
  //Merge the lumisections to create TimeBins with about the required number of events
  struct TimeBin::runlumi ev_begin   = (lumisec_list.begin())->first;
  struct TimeBin::runlumi ev_end     = (lumisec_list.begin())->first;
  UInt_t                  time_begin = (lumisec_list.begin())->second.time0;
  UInt_t                  time_end   = (lumisec_list.begin())->second.timef;
  int                     Nev_bin    = 0;
  TimeBin::TimeBin        bin;
  for(auto it=lumisec_list.begin() ; it!=lumisec_list.end() ; ++it )
  {
    if(time_end - time_begin > maxduration)
    {
      //cout<<"exceed max delta t --> close bin"<<endl;
      bin.SetBinRanges(ev_begin.runNumber, ev_end.runNumber, ev_begin.lumiBlock, ev_end.lumiBlock, time_begin, time_end);
      bin.SetNev(Nev_bin);
      timebins.push_back(bin);

      //reset
      ev_begin=it->first;
      ev_end=it->first;
      time_begin=it->second.time0;
      time_end=it->second.timef;
      Nev_bin=0;
    }
    else
      if(Nev_bin>=Nevmax_bin)
      {
	//cout<<"reach "<<Nev_bin<<" events in the bin --> close bin"<<endl;
	bin.SetBinRanges(ev_begin.runNumber, ev_end.runNumber, ev_begin.lumiBlock, ev_end.lumiBlock, time_begin, time_end);
	bin.SetNev(Nev_bin);
	timebins.push_back(bin);

	//reset
	ev_begin=it->first;
	ev_end=it->first;
	time_begin=it->second.time0;
	time_end=it->second.timef;
	Nev_bin=0;
      }	
      else
      {
	ev_end=it->first;
	time_end=it->second.timef;
      }

    ///cout<<(it->first).runNumber<<"\t"<<(it->first).lumiBlock<<"\t"<<(it->second).time0<<"\t"<<(it->second).timef<<"\t"<<(it->second).weight<<"\t+\t"<<Nev_bin<<endl;    
    Nev_bin+=it->second.weight;
  }

  if((ev_begin.lumiBlock!=ev_end.lumiBlock || ev_begin.runNumber!=ev_end.runNumber) && Nev_bin>=Nevmax_bin*0.5)
  {
    //cout<<"last bin with "<<Nev_bin<<" events --> close bin"<<endl;
    bin.SetBinRanges(ev_begin.runNumber, ev_end.runNumber, ev_begin.lumiBlock, ev_end.lumiBlock, time_begin, time_end);
    bin.SetNev(Nev_bin);
    timebins.push_back(bin);
  }
  std::sort(timebins.begin(), timebins.end());

}


void  MonitoringManager::SaveTimeBins(std::string outfilename, std::string writemethod)
{
  TFile* outfile = new TFile(outfilename.c_str(), writemethod.c_str());
  outfile->cd();
  TTree* outtree = new TTree(label_.c_str(), label_.c_str());
  TimeBin::TimeBin bin( *(timebins.begin()) );
  bin.BranchOutput(outtree);
  
  for(auto bincontent : timebins)
  {
    bin=bincontent;
    outtree->Fill();
  }

  outfile->cd();
  outtree->AutoSave();
  outfile->Close();
}

void  MonitoringManager::LoadTimeBins(std::string option)
{
  cout<<">> Loading timebins"<<endl; 
  if(timebins.size()>0)
    if(option=="RELOAD")
      timebins.clear();
    else
    {
      cout<<"[WARNING]: timebins not loaded because already in memory"<<endl
	  <<"           if you want to overwrite call LoadTimeBins(\"RELOAD\")"<<endl;
      return;
    }

  string inputfilename = conf_.GetOpt<string>  ("LaserMonitoring.scaleMonitor.runranges");
  cout<<">> Reading file "<<inputfilename<<endl;
  TFile* inputfile = new TFile(inputfilename.c_str(),"READ");
  TTree* intree = (TTree*) inputfile->Get(label_.c_str());
  if(!intree)
  {
    cout<<"[ERROR]: can't get tree "<<label_<<" in the file"<<endl;
    return;
  }
  TimeBin::TimeBin bin;
  bin.BranchInput(intree);
  Long64_t Nbins = intree->GetEntries();
  for(Long64_t ibin=0; ibin<Nbins; ++ibin)
  {
    intree->GetEntry(ibin);//tree entry is copied in bin
    TimeBin::TimeBin bincopy(bin);//perhaps not needed, but for security I avoid to make a mess with pointers
    timebins.push_back(bincopy);
  }
  
  std::sort(timebins.begin(), timebins.end());//It should be already ordered, just for security
  cout<<">> Loaded "<<timebins.size()<<" bins"<<endl;
  inputfile->Close();
  last_accessed_bin_=timebins.end();
}

bool MonitoringManager::BookHistos()
{
  int Nbin_histos = conf_.GetOpt<int>      ("LaserMonitoring.scaleMonitor.Nbin_histos");
  float xmin_histos = conf_.GetOpt<float>  ("LaserMonitoring.scaleMonitor.xmin_histos");
  float xmax_histos = conf_.GetOpt<float>  ("LaserMonitoring.scaleMonitor.xmax_histos");
  for(unsigned ibin=0; ibin<timebins.size(); ++ibin)
    if(!timebins.at(ibin).InitHisto( Form("Histo%i",ibin), Form("Histo%i",ibin), Nbin_histos, xmin_histos, xmax_histos))
      return false;

  return true;
}

//Loop over ECALELF tree to fill the timebins with the corresponding values
void  MonitoringManager::FillTimeBins()
{
  cout<<">> Filling timebin histos with variable "<<variablename_<<endl;
  if(!BookHistos())
  {
    cout<<">> Cannot book the histos... Maybe you have already filled them " << endl;
									  return;
  }
      
  TTreeFormula* variable = new TTreeFormula("variable", variablename_.c_str(), chain_);
  long Nentries = this->GetEntries();
  cout<<Nentries<<" total entries\n"<<endl;
  for(long ientry=0; ientry<Nentries; ++ientry)
  {
    this->GetEntry(ientry);
    if(ientry%100000==0)
      cout<<"reading entry "<<ientry<<"\r"<<std::flush;

    for(int iEle=0; iEle<2; ++iEle)
    {
      if(this->isSelected(iEle))
      {
	//cout<<"selected - "<<"run="<<this->GetRunNumber()<<"\tLS="<<this->GetLS()<<"\tT="<<this->GetTime()<<endl;
	
	auto bin_iterator = FindBin(this->GetRunNumber(),this->GetLS(),this->GetTime());
	if(bin_iterator!=timebins.end())
	  bin_iterator->FillHisto( variable->EvalInstance(iEle) );
      }
    }
  }

  delete variable;
  cout<<">> Histos filled"<<endl;

}

std::vector<TimeBin::TimeBin>::iterator MonitoringManager::FindBin(const UInt_t &run, const UShort_t &ls, const UInt_t &time)
{
  //cout<<"finding bin"<<endl;
  //usually events are in the same time bin of the previous iteration or in adjacent timebins so i start to look for them from there
  std::vector<TimeBin::TimeBin>::iterator it_end   = timebins.end();
  std::vector<TimeBin::TimeBin>::iterator it_begin = timebins.begin();

  if(last_accessed_bin_!=it_end)
  {
    //cout<<"last_accessed_bin_!=it_end"<<endl;
    if(last_accessed_bin_ -> Match(run,ls,time))
      return last_accessed_bin_;

    if(last_accessed_bin_>it_begin)
      if((last_accessed_bin_-1) -> Match(run,ls,time))
      {
	last_accessed_bin_--;
	return (last_accessed_bin_);
      }

    if(last_accessed_bin_<it_end-1)
       if((last_accessed_bin_+1) -> Match(run,ls,time))
       {
	 last_accessed_bin_++;
	 return (last_accessed_bin_);
       }
  }    
  //if I am here, unfortunately I have to perform a search through the entire set
  //cout<<"last_accessed_bin_=it_end"<<endl;
  for(std::vector<TimeBin::TimeBin>::iterator it_bin = it_begin; it_bin<it_end; ++it_bin)//not the smarter way considering that the bins are ordered --> can be improved
    if(it_bin -> Match(run,ls,time))
    {
      //cout<<"match with bin "<<it_bin-it_begin<<endl;
      last_accessed_bin_=it_bin;
      return (last_accessed_bin_);
    }

  //if I am here, I didn't found any match
  return it_end;
}
	     
	     
		 
	  
void  MonitoringManager::RunComputeMean(string scale)
{
  cout<<">> RunComputeMean in function"<<endl;
  for(std::vector<TimeBin::TimeBin>::iterator it_bin = timebins.begin(); it_bin<timebins.end(); ++it_bin)
  {
    //cout<<"reading bin "<<it_bin-timebins.begin()<<endl;
    it_bin->SetVariable("scale_"+scale, it_bin->GetMean());
  }
}

	  
void  MonitoringManager::RunComputeMedian(string scale)
{
  cout<<">> RunComputeMedian in function"<<endl;
  for(std::vector<TimeBin::TimeBin>::iterator it_bin = timebins.begin(); it_bin<timebins.end(); ++it_bin)
  {
    //cout<<"reading bin "<<it_bin-timebins.begin()<<endl;
    it_bin->SetVariable("scale_"+scale, it_bin->GetMedian());
  }
}

void  MonitoringManager::PrintScales()
{
  auto firstbin=timebins.begin();
  
}


