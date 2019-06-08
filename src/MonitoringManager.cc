#include "MonitoringManager.h"

using namespace std;

MonitoringManager::MonitoringManager(CfgManager conf):
  ECALELFInterface(conf),
  conf_(conf),
  h_template_(0)
{
  label_ = conf.GetOpt<string> ("Input.label");  
  variable_ = conf.GetOpt<string> ("LaserMonitoring.variable");
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

  cout<<">> Building template \""<<variable_<<">>"<<templatename<<"\"\n"
      <<"   with selection \""<<selection_str_<<"\"\n"
      <<">> It can take hours.."<<endl; 

  chain_->Draw( Form("%s>>%s",variable_.c_str(),templatename.c_str()) , selection_str_.c_str() , "goff");
  h_template_=(TH1F*)gDirectory->Get("h_template");
  h_template_->SetName(("h_template_"+label_).c_str());
  h_template_->SetTitle(("h_template_"+label_).c_str());
  //curr_dir->cd();
  return h_template_;
}
