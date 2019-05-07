#include "MonitoringManager.h"

using namespace std;

MonitoringManager::MonitoringManager(CfgManager conf):
  ECALELFInterface(conf),
  h_template_(0),
  xmin_template_(-1),
  xmax_template_(-1),
  Nbin_template_(-1)
{
  label_ = conf.GetOpt<string> ("Input.label");  
  scalename_ = conf.GetOpt<string> ("LaserMonitoring.variable");  

}

MonitoringManager::~MonitoringManager()
{
  if(h_template_)
    delete h_template_;
}

void MonitoringManager::SetTemplateModel(int Nbin, float xmin, float xmax)
{
  Nbin_template_ = Nbin;
  xmin_template_ = xmin;
  xmax_template_ = xmax;
  cout<<">> Set template with "<<Nbin_template_<<" bins from "<<xmin<<" to "<<xmax<<endl; 
}
 
TH1F* MonitoringManager::BuildTemplate()
{
  string templatename = "h_template";
  if(Nbin_template_!=-1)
    templatename = Form("h_template(%i,%f,%f)",Nbin_template_,xmin_template_,xmax_template_);
  
  //TDirectory* curr_dir = gDirectory->GetDirectory();
  gDirectory->cd(0);

  cout<<">> Building template \""<<scalename_<<">>"<<templatename<<"\"\n"
      <<"   with selection \""<<selection_str_<<"\"\n"
      <<">> It can take hours"<<endl; 

  chain_->Draw( Form("%s>>%s",scalename_.c_str(),templatename.c_str()) , selection_str_.c_str() , "goff");
  h_template_=(TH1F*)gDirectory->Get("h_template");
  h_template_->SetName(("h_template_"+label_).c_str());
  h_template_->SetTitle(("h_template_"+label_).c_str());
  //curr_dir->cd();
  return h_template_;
}
