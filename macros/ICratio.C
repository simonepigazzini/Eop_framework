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
