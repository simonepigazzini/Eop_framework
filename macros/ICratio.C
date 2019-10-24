
void ProduceICratio(TH2F* h1,TH2F* h2)
{

  //check if range and binning is equal
  if( h1->GetNbinsX() != h2->GetNbinsX() || h1->GetNbinsY() != h2->GetNbinsY())
  {
    cout<<"ERROR: different #bin"<<endl;
    exit(EXIT_FAILURE);
  }
  if( h1->GetXaxis()->GetXmin() != h2->GetXaxis()->GetXmin() || h1->GetXaxis()->GetXmax() != h2->GetXaxis()->GetXmax() )
  {
    cout<<"WARNING: different x range"<<endl;
    //exit(EXIT_FAILURE);
  }
  if( h1->GetYaxis()->GetXmin() != h2->GetYaxis()->GetXmin() || h1->GetYaxis()->GetXmax() != h2->GetYaxis()->GetXmax() )
  {
    cout<<"WARNING: different y range"<<endl;
    //exit(EXIT_FAILURE);
  }

  TH2F* h_ratio = new TH2F("h1/h2","h1/h2",h1->GetNbinsX(),h1->GetXaxis()->GetXmin(), h1->GetXaxis()->GetXmax(), h1->GetNbinsY(), h1->GetYaxis()->GetXmin(), h1->GetYaxis()->GetXmax());
  TH1F* h_ratio1D = new TH1F("IC1/IC2 histogram","IC1/IC2 histogram",100,0.9,1.1);
  for(int ix=1; ix<h_ratio->GetNbinsX()+1; ++ix)
  {
    for(int iy=1; iy<h_ratio->GetNbinsY()+1; ++iy)
    {
      float v1 = h1->GetBinContent(ix,iy);
      float v2 = h2->GetBinContent(ix,iy);
      if(v2!=0)
      {
	h_ratio->SetBinContent(ix,iy,v1/v2);
	h_ratio1D->Fill(v1/v2);
      }
      else
	h_ratio->SetBinContent(ix,iy,0.);
    }
  }
  TCanvas *c1 = new TCanvas();   
  h_ratio->Draw("COLZ");
  h_ratio->GetZaxis()->SetRangeUser(0.95,1.05);

  TCanvas *c2 = new TCanvas();   
  h_ratio1D->Draw();
}  

void ICratio(TString filename1, TString objname1, TString filename2, TString objname2)
{
  TFile* file1 = new TFile(filename1.Data(),"READ");
  TH2F* h1 = (TH2F*) file1->Get(objname1.Data());
  h1->SetDirectory(0);
  file1->Close();

  TFile* file2 = new TFile(filename2.Data(),"READ");
  TH2F* h2 = (TH2F*) file2->Get(objname2.Data());
  h2->SetDirectory(0);
  file2->Close();

  ProduceICratio(h1,h2);
}

void ICratio(TString txtfilename1, TString txtfilename2, bool isEB=true)
{
  TH2F *h1,*h2,*h1m,*h2m,*h1p,*h2p;
  if(isEB)
  {
    h1 = new TH2F("h1", "h1", 360, 1, 361, 171, -85, 86);    
    h2 = new TH2F("h2", "h2", 360, 1, 361, 171, -85, 86);
  }
  else
  {
    h1m = new TH2F("h1m", "h1m", 100, 1, 101, 100, 1, 101);
    h2m = new TH2F("h2m", "h2m", 100, 1, 101, 100, 1, 101);
    h1p = new TH2F("h1p", "h1p", 100, 1, 101, 100, 1, 101);
    h2p = new TH2F("h2p", "h2p", 100, 1, 101, 100, 1, 101);
  }

  int iPhi, iEta, iz;
  double ic, eic;

  std::cout << " Opening first file ..... reading " << std::endl;
  std::ifstream File1 (txtfilename1.Data());
  while (!File1.eof()) 
  {
    File1 >> iEta >> iPhi >> iz >> ic >> eic ;
    if(isEB)
    {
      if(iz==0)
	h1->Fill(iPhi, iEta, ic);
    }
    else
      if(iz == 1) 
	h1p->Fill(iEta, iPhi, ic);
      else
	if(iz == -1)
	  h1m->Fill(iEta, iPhi, ic);
  }
  std::cout << " End first file " << std::endl;
  File1.close();

  std::cout << " Opening second file ..... reading " << std::endl;
  std::ifstream File2 (txtfilename2.Data());
  while (!File2.eof()) 
  {
    File2 >> iEta >> iPhi >> iz >> ic >> eic ;
    if(isEB)
    {
      if(iz==0)
	h2->Fill(iPhi, iEta, ic);
    }
    else
      if(iz == 1) 
	h2p->Fill(iEta, iPhi, ic);
      else
	if(iz == -1)
	  h2m->Fill(iEta, iPhi, ic);
  }
  std::cout << " End second file " << std::endl;
  File2.close();

  if(isEB)
    ProduceICratio(h1,h2);

  else
  {
    ProduceICratio(h1m,h2m);
    ProduceICratio(h1p,h2p);
  }
}


