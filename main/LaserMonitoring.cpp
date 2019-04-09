#include "CfgManager.h"
#include "CfgManagerT.h"
#include "MonitoringManager.h"

#include <iostream>
#include <string>

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TLorentzVector.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TMath.h"

using namespace std;

int main(int argc, char* argv[])
{
  string cfgfilename="";

  //Parse the input options
  for(int iarg=1; iarg<argc; ++iarg)
  {
    if(string(argv[iarg])=="--cfg")
      cfgfilename=argv[iarg+1];
  }
      
  // parse the config file
  CfgManager config;
  config.ParseConfigFile(cfgfilename.c_str());

  //define the monitoring manager object
  MonitoringManager monitor(config);

  //define the things to do
  vector<string> ToDoList = config.GetOpt<vector<string> > ("LaserMonitoring.tasklist");
  for(auto ToDo : ToDoList)
  {
    cout<<ToDo<<endl;
    if(ToDo=="BuildTemplate")
    {
      if( config.OptExist("LaserMonitoring.BuildTemplate.Nbin") && config.OptExist("LaserMonitoring.BuildTemplate.xmin") && config.OptExist("LaserMonitoring.BuildTemplate.xmax") ) 
      {
	int Nbin   = config.GetOpt<int>   ("LaserMonitoring.BuildTemplate.Nbin");
	float xmin = config.GetOpt<float> ("LaserMonitoring.BuildTemplate.xmin");
	float xmax = config.GetOpt<float> ("LaserMonitoring.BuildTemplate.xmax");
	monitor.SetTemplateModel(Nbin,xmin,xmax);
      }
      else
	cout<<"[WARNING]: option LaserMonitoring.BuildTemplate.Nbin or LaserMonitoring.BuildTemplate.xmin or LaserMonitoring.BuildTemplate.xmax is missing --> template histo binning will be automatic"<<endl;
      TH1F* h_template = monitor.BuildTemplate();
      string outfilename = config.GetOpt<string> ("LaserMonitoring.BuildTemplate.output");
      TFile* outfile = new TFile(outfilename.c_str(),"RECREATE");
      outfile->cd();
      cout<<">> Saving template to "<<outfilename<<"/"<<h_template->GetName()<<endl;
      h_template->Write();
      outfile->Close();
    }
    if(ToDo=="RunRanges")
    {
      monitor.BuildRunRanges();
    }
    if(ToDo=="RunMonitoring")
    {
      monitor.RunMonitoring();
    }
  }
  return 0;
}
