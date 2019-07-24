void CompareBuildEopEta(TString NewCodeFilename, TString OldCodeFilename)
{

  //load new histo
  cout<<"load new histo"<<endl;
  TFile* NewCodeFile = new TFile(NewCodeFilename,"READ");
  TH2D* h2_NewCode = (TH2D*) NewCodeFile->Get("EopEta");
  h2_NewCode->SetDirectory(0);
  NewCodeFile->Close();

  //Create TH2D for OldCode with correct binning
  cout<<"Create TH2D for OldCode with correct binning"<<endl;
  TFile* OldCodeFile = new TFile(OldCodeFilename,"READ");
  
  TH1F* h_OldCode = (TH1F*) OldCodeFile->Get("h_0_hC_EoP_eta_1");
  TH2D* h2_OldCode = new TH2D("h2_OldCode","h2_OldCode",
			      171,                    -85.5,                            85.5,
			      h_OldCode->GetNbinsX(), h_OldCode->GetXaxis()->GetXmin(), h_OldCode->GetXaxis()->GetXmax());
  h2_OldCode->SetDirectory(0);
 
  //Fill TH2D for OldCode from the TH1F histos in file
  cout<<"Fill TH2D for OldCode from the TH1F histos in file"<<endl;
  for(int iHisto=0; iHisto<171; ++iHisto)
  {
    h_OldCode = (TH1F*) OldCodeFile->Get(Form("h_%i_hC_EoP_eta_1",iHisto));
    for(int iBin=1; iBin<=h_OldCode->GetNbinsX(); ++iBin)
    {
      float bincontent = h_OldCode->GetBinContent(iBin);
      h2_OldCode->SetBinContent(iHisto+1,iBin,bincontent);
    }
  }
  OldCodeFile->Close();

  //Fill ratio histogram
  cout<<"Fill ratio histogram"<<endl;
  TH2D* h2_ratio = new TH2D("ratio old/new","ratio old/new",171,-85.5,85.5,1000,0.1,3.);
  for(int ibinx=1; ibinx<=h2_ratio->GetNbinsX(); ++ibinx)
    for(int ibiny=1; ibiny<=h2_ratio->GetNbinsY(); ++ibiny)
    {
      double x    = h2_ratio->GetXaxis()->GetBinCenter(ibinx);
      double y    = h2_ratio->GetYaxis()->GetBinCenter(ibiny);
      double zOld = h2_OldCode->GetBinContent( h2_OldCode->FindBin(x,y) );
      double zNew = h2_NewCode->GetBinContent( h2_NewCode->FindBin(x,y) );
      //cout<<"x,y,zOld,zNew "<<x<<","<<y<<","<<zOld<<","<<zNew<<endl;
      if(zNew!=0)
	h2_ratio->SetBinContent(ibinx,ibiny,zOld/zNew);
      else
	h2_ratio->SetBinContent(ibinx,ibiny,0.);
    }

  //Draw ratio histogram
  TCanvas *c1 = new TCanvas();
  h2_ratio->Draw("COLZ");
  TCanvas *c2 = new TCanvas();
  h2_OldCode->Draw("COLZ");
  TCanvas *c3 = new TCanvas();
  h2_NewCode->Draw("COLZ");

}


void CompareBuildEopEtaEE(TString NewCodeFilename, TString OldCodeFilename)
{

  //load new histo
  cout<<"load new histo"<<endl;
  TFile* NewCodeFile = new TFile(NewCodeFilename,"READ");
  TH2D* h2_NewCode = (TH2D*) NewCodeFile->Get("EopEta");
  h2_NewCode->SetDirectory(0);
  NewCodeFile->Close();

  //Create TH2D for OldCode with correct binning
  cout<<"Create TH2D for OldCode with correct binning"<<endl;
  TFile* OldCodeFile = new TFile(OldCodeFilename,"READ");
  
  TH1F* h_OldCode = (TH1F*) OldCodeFile->Get("h_0_hC_EoP_eta_1");
  TH2D* h2_OldCode = new TH2D("h2_OldCode","h2_OldCode",
			      41,                     -0.5,                             40.5,
			      h_OldCode->GetNbinsX(), h_OldCode->GetXaxis()->GetXmin(), h_OldCode->GetXaxis()->GetXmax());
  h2_OldCode->SetDirectory(0);
 
  //Fill TH2D for OldCode from the TH1F histos in file
  cout<<"Fill TH2D for OldCode from the TH1F histos in file"<<endl;
  for(int iHisto=0; iHisto<41; ++iHisto)
  {
    h_OldCode = (TH1F*) OldCodeFile->Get(Form("h_%i_hC_EoP_eta_1",iHisto));
    for(int iBin=1; iBin<=h_OldCode->GetNbinsX(); ++iBin)
    {
      float bincontent = h_OldCode->GetBinContent(iBin);
      h2_OldCode->SetBinContent(iHisto+1,iBin,bincontent);
    }
  }
  OldCodeFile->Close();

  //Fill ratio histogram
  cout<<"Fill ratio histogram"<<endl;
  TH2D* h2_ratio = new TH2D("ratio old/new","ratio old/new",41,-0.5,40.5,250,0.1,3.);
  for(int ibinx=1; ibinx<=h2_ratio->GetNbinsX(); ++ibinx)
    for(int ibiny=1; ibiny<=h2_ratio->GetNbinsY(); ++ibiny)
    {
      double x    = h2_ratio->GetXaxis()->GetBinCenter(ibinx);
      double y    = h2_ratio->GetYaxis()->GetBinCenter(ibiny);
      double zOld = h2_OldCode->GetBinContent( h2_OldCode->FindBin(x,y) );
      double zNew = h2_NewCode->GetBinContent( h2_NewCode->FindBin(x,y) );
      //cout<<"x,y,zOld,zNew "<<x<<","<<y<<","<<zOld<<","<<zNew<<endl;
      if(zNew!=0)
	h2_ratio->SetBinContent(ibinx,ibiny,zOld/zNew);
      else
	h2_ratio->SetBinContent(ibinx,ibiny,0.);
    }

  //Draw ratio histogram
  TCanvas *c1 = new TCanvas();
  h2_ratio->Draw("COLZ");
  TCanvas *c2 = new TCanvas();
  h2_OldCode->Draw("COLZ");
  TCanvas *c3 = new TCanvas();
  h2_NewCode->Draw("COLZ");

}
