
// macro to build a TGraph of integrated luminosity vs time 
// NOTE!! 
// Before running remember to export the brilcalc path: 
// export PATH=$HOME/.local/bin:/afs/cern.ch/cms/lumi/brilconda-1.1.7/bin:$PATH

void ParseCSVline(ifstream &brilfile, vector<string> &parsedline)
{
  parsedline.clear();
  std::string                line;
  std::getline(brilfile,line);
  if(line[0]=='#')
    return;
  std::stringstream          lineStream(line);
  std::string                cell;

  while(std::getline(lineStream,cell, ','))
    parsedline.push_back(cell);

  // This checks for a trailing comma with no data after it.
  if (!lineStream && cell.empty())
  {
    // If there was a trailing comma then add an empty element.
    parsedline.push_back("");
  }
}

void BuildLumiGraph(string begin="03/20/18 00:00:00", string end="10/20/19 00:00:00", TString outfilename="./test.root")
{
  //produce the brilcalc file and put it in /tmp/fmonti/lumi.dat  
  string makedir_command = "mkdir /tmp/fmonti";
  //string brilcalc_command = "brilcalc lumi --begin \"03/20/18 00:00:00\" --output-style tab -o /tmp/fmonti/lumi.dat --tssec  --byls -b \'STABLE BEAMS\' ";
  string brilcalc_command = "brilcalc lumi --begin \""+begin+"\" --end \""+end+"\" --output-style tab -o /tmp/fmonti/lumi.dat --tssec  --byls -b \'STABLE BEAMS\' ";
  cout<<makedir_command<<endl;
  system(makedir_command.c_str());
  cout<<brilcalc_command<<endl;
  system(brilcalc_command.c_str());

  //read the brilcalc file and fill the corresponding TGraph
  TGraph* g_intlumi_time = new TGraph();
  g_intlumi_time->SetName ("g_intlumi_time");
  g_intlumi_time->SetTitle("g_intlumi_time;time (s);integrated luminosity(fb^{-1})");
  TGraph* g_time_intlumi = new TGraph();
  g_time_intlumi->SetName ("g_time_intlumi");
  g_time_intlumi->SetTitle("g_time_intlumi;integrated luminosity(fb^{-1});time (s)");
  TGraph* g_run_intlumi = new TGraph();
  g_run_intlumi->SetName ("g_run_intlumi");
  g_run_intlumi->SetTitle("g_run_intlumi;integrated luminosity(fb^{-1});run number");
  TGraph* g_ls_intlumi = new TGraph();
  g_ls_intlumi->SetName ("g_ls_intlumi");
  g_ls_intlumi->SetTitle("g_ls_intlumi;integrated luminosity(fb^{-1});lumisection number");

  ifstream brilfile("/tmp/fmonti/lumi.dat");
  if(!brilfile.is_open())
  {
    cout<<"[ERROR]: file not opened"<<endl;
    return;
  }
  double intlumi=0.;
  long time;
  vector<string> splittedline;
  while(!brilfile.eof())
  {
    ParseCSVline(brilfile, splittedline);
    if(splittedline.size()==9)//good lines have 9 entries, could be different in future!!!
    {
      TString runstr = splittedline.at(0);
      runstr.Remove(runstr.First(":"),runstr.Length()-runstr.First(":"));
      double run = runstr.Atof();

      TString lsstr = splittedline.at(1);
      lsstr.Remove(lsstr.First(":"),lsstr.Length()-lsstr.First(":"));
      double ls = lsstr.Atof();

      time = stol(splittedline.at(2));
      intlumi += stod(splittedline.at(5));

      g_intlumi_time -> SetPoint(g_intlumi_time->GetN(),1.*time,intlumi*1.e-9);//I want the lumi in fb-1
      g_time_intlumi -> SetPoint(g_time_intlumi->GetN(),intlumi*1.e-9,1.*time);
      g_run_intlumi -> SetPoint(g_run_intlumi->GetN(),intlumi*1.e-9,run);
      g_ls_intlumi -> SetPoint(g_ls_intlumi->GetN(),intlumi*1.e-9,ls);
    }
  }
  brilfile.close();

  TFile* outfile = new TFile(outfilename.Data(),"RECREATE");
  outfile->cd();
  g_intlumi_time->Write();
  g_time_intlumi->Write();
  g_run_intlumi->Write();
  g_ls_intlumi->Write();

  outfile->Close();

}
