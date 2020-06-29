// Given the graphs produced by BuildLumiGraph.C, this macro creates 
// timebins (saved in a output tree) corresponding to a given integrated luminosity value 

void SplitEventsByLumi(TString inputfilename="/afs/cern.ch/user/f/fmonti/work/Eop_framework/test.root", double intlumistep=2., TString outputfilename="./dummyrunrange.root", TString outputtreename="ciao")
{
  TFile* inputfile = new TFile(inputfilename,"READ");
  TGraph* g_time_intlumi = (TGraph*)inputfile->Get("g_time_intlumi"); 
  TGraph* g_run_intlumi = (TGraph*)inputfile->Get("g_run_intlumi"); 
  TGraph* g_ls_intlumi = (TGraph*)inputfile->Get("g_ls_intlumi"); 
  assert(g_time_intlumi->GetN()==g_run_intlumi->GetN() && (g_time_intlumi->GetN()==g_ls_intlumi->GetN()));
  double* intlumi_values = g_time_intlumi->GetX();
  double* time_values = g_time_intlumi->GetY();
  double* run_values = g_run_intlumi->GetY();
  double* ls_values = g_ls_intlumi->GetY();
  int Npoints = g_time_intlumi->GetN();

  TFile* outputfile = new TFile(outputfilename,"RECREATE");
  TTree* outtree = new TTree(outputtreename,outputtreename);    
  unsigned timemin = 0; 
  unsigned timemax = 0;  
  unsigned runmin = 0; 
  unsigned runmax = 0; 
  unsigned short lsmin = 0; 
  unsigned short lsmax = 0;
  int Nev = 0; 
  outtree->Branch("runmin", &runmin, "runmin/i");
  outtree->Branch("runmax", &runmax, "runmax/i");
  outtree->Branch("lsmin", &lsmin, "lsmin/s");
  outtree->Branch("lsmax", &lsmax, "lsmax/s");
  outtree->Branch("timemin", &timemin, "timemin/i");
  outtree->Branch("timemax", &timemax, "timemax/i");
  outtree->Branch("Nev", &Nev, "Nev/I"); 
  
  double intlumimin=0;
  double intlumimax=0;
  timemin=timemax=time_values[0];
  lsmin=lsmax=ls_values[0];
  runmin=runmax=run_values[0];
  for(int ipoint=1;ipoint<Npoints-1;++ipoint)
  {
    intlumimax = intlumi_values[ipoint];
    //cout<<"\n\nipoint "<<ipoint<<endl;
    //cout<<"intlumimin "<<intlumimin<<endl;
    //cout<<"timemin "<<timemin<<endl;
    //cout<<"lsmin "<<lsmin<<endl;
    //cout<<"runmin "<<runmin<<endl;
    //cout<<"intlumimax "<<intlumimax<<endl;
    //cout<<"timemax "<<timemax<<endl;
    //cout<<"lsmax "<<lsmax<<endl;
    //cout<<"runmax "<<runmax<<endl;

    if(intlumimax-intlumimin > intlumistep && run_values[ipoint]!=run_values[ipoint+1])
    {
      //cout<<"closing!"<<endl;
      //getchar();
      timemax=time_values[ipoint];
      runmax=run_values[ipoint];
      lsmax=ls_values[ipoint];
      outtree->Fill();

      intlumimax=intlumimin=intlumi_values[ipoint+1];
      timemax=timemin=time_values[ipoint+1];
      runmax=runmin=run_values[ipoint+1];
      lsmax=lsmin=ls_values[ipoint+1];
    }
  }

  inputfile->Close();
  outputfile->cd();
  outtree->AutoSave(); 
  outputfile->Close();
}
