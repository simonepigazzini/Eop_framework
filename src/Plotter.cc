#include "interface/Plotter.h"



Plotter::Plotter(const CfgManager& opts,
                 const std::vector<std::string>& vars,
                 const std::string& outputFolder):
  opts_(opts),
  vars_(vars),
  outputFolder_(outputFolder)
{}



Plotter::~Plotter()
{}



void Plotter::DrawPlots()
{
  
  system(Form("mkdir -p %s",outputFolder_.c_str()));
  system(Form("cp /afs/cern.ch/user/a/abenagli/public/index.php %s",outputFolder_.c_str()));
  
  for(unsigned int ii = 0; ii < vars_.size(); ++ii)
  {
    std::string varName = vars_.at(ii);
    std::string type = opts_.GetOpt<std::string>(Form("%s.type",varName.c_str()));
    
    if( type == "TH1F" )
    {
      PlotTH1F myplot(varName,opts_);
      
      TCanvas* c = myplot.Plot();
      c -> Print(Form("%s/c_%s.png",outputFolder_.c_str(),varName.c_str()));
      c -> Print(Form("%s/c_%s.pdf",outputFolder_.c_str(),varName.c_str()));
      c -> SaveAs(Form("%s/c_%s.root",outputFolder_.c_str(),varName.c_str()));
      TCanvas* clog = myplot.PlotLog();
      clog -> Print(Form("%s/clog_%s.png",outputFolder_.c_str(),varName.c_str()));
      clog -> Print(Form("%s/clog_%s.pdf",outputFolder_.c_str(),varName.c_str()));
      c -> SaveAs(Form("%s/clog_%s.root",outputFolder_.c_str(),varName.c_str()));
    }
  }
}



PlotBase::PlotBase(const std::string& varName,
                   const CfgManager& opts):
  varName_(varName),
  opts_(opts)
{
  c_ = new TCanvas();
  clog_ = new TCanvas();
}



PlotBase::~PlotBase()
{
  delete c_;
  for(unsigned int ii = 0; ii < objects_.size(); ++ii)
    delete objects_.at(ii);
}



PlotTH1F::PlotTH1F(const std::string& varName,
                   const CfgManager& opts):
  PlotBase(varName,opts)
{
  THStack* hs = new THStack("hs","stack");
  TH1F* h_data = NULL;
  TH1F* h_data_ratio = NULL;
  
  std::string formula = opts_.GetOpt<std::string>(Form("%s.formula",varName_.c_str()));
  
  int nBinsX = int((opts_.GetOpt<std::vector<float> >(Form("%s.xAxis",varName_.c_str()))).at(0));
  float xMin = (opts_.GetOpt<std::vector<float> >(Form("%s.xAxis",varName_.c_str()))).at(1);
  float xMax = (opts_.GetOpt<std::vector<float> >(Form("%s.xAxis",varName_.c_str()))).at(2);
  
  float yMin = +999999999.;
  float yMax = -999999999.;
  
  std::string title = opts_.GetOpt<std::string>(Form("%s.title",varName_.c_str()));
  std::string unit = opts_.GetOpt<std::string>(Form("%s.unit",varName_.c_str()));
  std::string unitString = "";
  if( unit != " ") unitString = "(" + unit + ")";
  std::string weight = opts_.GetOpt<std::string>(Form("%s.weight",varName_.c_str()));
  
  std::string normalization = opts_.GetOpt<std::string>(Form("%s.normalization",varName_.c_str()));
  float lumi = opts_.GetOpt<float>(Form("%s.lumi",varName_.c_str()));
  
  std::vector<std::string> objectList = opts_.GetOpt<std::vector<std::string> >(Form("%s.objectList",varName_.c_str()));
  
  for(unsigned int ii = 0; ii < objectList.size(); ++ii)
  {
    std::string objectName = objectList.at(ii);
    
    std::vector<std::string> input = opts_.GetOpt<std::vector<std::string> >(Form("%s.%s.input",varName_.c_str(),objectName.c_str()));
    TChain* t = new TChain("chain","the chain");
    for(unsigned int ii = 0; ii < input.size()/2; ++ii)
    {
      std::string inFileName = input.at(0+ii*2);
      std::string treeName = input.at(1+ii*2);
      t -> Add((inFileName+"/"+treeName).c_str());
    }
    
    objects_.push_back( new TH1F(Form("%s_%s",objectName.c_str(),varName_.c_str()),"",nBinsX,xMin,xMax));
    TH1F* object = static_cast<TH1F*>(objects_.at(ii));
    object -> Sumw2();
    
    int isData = opts_.GetOpt<int>(Form("%s.%s.isData",varName_.c_str(),objectName.c_str()));
    
    std::string scale = opts_.GetOpt<std::string>(Form("%s.%s.scale",varName_.c_str(),objectName.c_str()));
    int stack = opts_.GetOpt<int>(Form("%s.%s.stack",varName_.c_str(),objectName.c_str()));
    
    std::cout << ">>> drawing variable" << formula << " for object " << objectName << std::endl;
    t -> Draw( Form("%s >>%s_%s",formula.c_str(),objectName.c_str(),varName_.c_str()), (weight+"*"+scale).c_str(), "goff" );
    
    if( normalization == "area" && stack != 1 ) object -> Scale(1./object->Integral());
    if( (normalization == "lumi" || normalization == "lumiScaled") && isData != 1 ) object -> Scale(1.*lumi);
    
    if( !(normalization == "area" && stack == 1) )
    {
      for(int bin = 0; bin <= nBinsX+1; ++bin)
      {
        float binContent = object->GetBinContent(bin);
        if( binContent > yMax ) yMax = binContent;
        // if( binContent < yMin && binContent > 0 ) yMin = binContent;
      }
    }
    
    object -> SetMarkerColor( opts_.GetOpt<int>(Form("%s.%s.color",varName_.c_str(),objectName.c_str())) );
    object -> SetMarkerStyle( opts_.GetOpt<int>(Form("%s.%s.markerStyle",varName_.c_str(),objectName.c_str())) );
    object -> SetMarkerSize( opts_.GetOpt<float>(Form("%s.%s.markerSize",varName_.c_str(),objectName.c_str())) );
    object -> SetLineColor( opts_.GetOpt<int>(Form("%s.%s.lineColor",varName_.c_str(),objectName.c_str())) );
    object -> SetLineWidth( opts_.GetOpt<int>(Form("%s.%s.lineWidth",varName_.c_str(),objectName.c_str())) );
    object -> SetLineStyle( opts_.GetOpt<int>(Form("%s.%s.lineStyle",varName_.c_str(),objectName.c_str())) );
    object -> SetFillColor( opts_.GetOpt<int>(Form("%s.%s.color",varName_.c_str(),objectName.c_str())) );
    object -> SetFillStyle( opts_.GetOpt<int>(Form("%s.%s.fillStyle",varName_.c_str(),objectName.c_str())) );
    
    object -> GetXaxis() -> SetRange(0,nBinsX+1);
    
    if( stack == 1 )
      hs -> Add(object);
    
    
    if( isData == 1 && h_data == NULL )
    {
      h_data = (TH1F*)( object->Clone("h_data") );
      h_data -> SetDirectory(0);
      
      h_data_ratio = (TH1F*)( object->Clone("h_data_ratio") );
      h_data_ratio -> SetDirectory(0); 
    }
    else if( isData == 1 && h_data != NULL )
    {
      h_data -> Add(object);
      h_data_ratio -> Add(object);
    }
  }
  
  TH1F* h_sum = NULL;
  TList* hs_histos = hs->GetHists();
  TIter next(hs_histos);
  TH1F* hist;
  while( (hist =(TH1F*)next()) )
  {
    if( h_sum == NULL ) h_sum = (TH1F*)( hist->Clone("h_sum") );
    else h_sum->Add(hist);
  }
  
  float scaleFactor = 1.;
  if( h_data ) scaleFactor = h_data->Integral() / h_sum->Integral();
  if( normalization == "lumiScaled" && h_sum != NULL )
  {
    hs_histos = hs->GetHists();
    next = hs_histos;
    while( (hist =(TH1F*)next()) )
      hist -> Scale(scaleFactor);
    h_sum -> Scale(scaleFactor);
  }
  
  if( normalization == "area" && h_sum != NULL )
  {
    hs_histos = hs->GetHists();
    next = hs_histos;
    while( (hist =(TH1F*)next()) )
      hist -> Scale(1./h_sum->Integral());
    h_sum -> Scale(1./h_sum->Integral());
  }
  
  if( hs->GetMaximum() > yMax ) yMax = hs->GetMaximum();
  hs_histos = hs->GetHists();
  next = hs_histos;
  while( (hist =(TH1F*)next()) )  
  {
    for(int bin = 0; bin <= nBinsX+1; ++bin)
    {
      float binContent = hist->GetBinContent(bin);
      if( binContent < yMin && binContent > 0 ) yMin = binContent;
    }
    
    break;
  }
  
  
  //-------
  // legend
  
  TLegend* legend = new TLegend(0.70,0.90-0.04*objectList.size(),0.93,0.90);
  legend -> SetFillColor(0);
  legend -> SetFillStyle(1000);  
  legend -> SetTextFont(42);  
  legend -> SetTextSize(0.03);
  
  for(unsigned int ii = 0; ii < objectList.size(); ++ii)
  {
    TH1F* object = static_cast<TH1F*>(objects_.at(ii));
    
    std::string objectName = objectList.at(ii);
    int isData = opts_.GetOpt<int>(Form("%s.%s.isData",varName_.c_str(),objectName.c_str()));
    std::string scale = opts_.GetOpt<std::string>(Form("%s.%s.scale",varName_.c_str(),objectName.c_str()));    
    std::vector<std::string> legendSettings = opts_.GetOpt<std::vector<std::string> >(Form("%s.%s.legend",varName_.c_str(),objectName.c_str()));
    
    std::string legendExtra = "";
    if( atof(scale.c_str()) != 1. ) legendExtra = "#times" + scale;
    if( normalization == "lumiScaled" && h_sum != NULL && !isData ) legendExtra = Form("#times%.2f",atof(scale.c_str())*scaleFactor);
    legend -> AddEntry(object,(legendSettings.at(0)+legendExtra).c_str(),legendSettings.at(1).c_str());
  }
  
  
  //------------
  // draw canvas
  
  c_ -> cd();
  
  TPad* pad1 = NULL;
  if( opts_.GetOpt<int>(Form("%s.drawRatioPlot",varName_.c_str())) == 1 )
  {
    pad1 = new TPad("pad1","pad1",0,0.3,1,1.0);
    pad1->SetBottomMargin(0.03); // Upper and lower plot are joined
    pad1->SetGridx();           // Vertical grid
    pad1->Draw();               // Draw the upper pad: pad1
    pad1->cd(); 
  }
  
  TH1F* hPad = (TH1F*)( gPad->DrawFrame(xMin-(xMax-xMin)/nBinsX,0.,xMax+(xMax-xMin)/nBinsX,1.25*yMax) );
  if( normalization == "area" ) hPad -> SetTitle(Form(";%s %s;event fraction / %.1e %s",title.c_str(),unitString.c_str(),(xMax-xMin)/nBinsX,unitString.c_str()));
  if( normalization == "lumi" || normalization == "lumiScaled") hPad -> SetTitle(Form(";%s %s;events / %.1e %s",title.c_str(),unitString.c_str(),(xMax-xMin)/nBinsX,unitString.c_str()));
  hPad->GetXaxis()->SetTitleFont(43);
  hPad->GetYaxis()->SetTitleFont(43);
  hPad->GetXaxis()->SetTitleSize(25);
  hPad->GetYaxis()->SetTitleSize(22);
  hPad->GetXaxis()->SetTitleOffset(0.9);
  hPad->GetYaxis()->SetTitleOffset(1.85);
  hPad->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  hPad->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  hPad->GetXaxis()->SetLabelSize(17);
  hPad->GetYaxis()->SetLabelSize(17);
  hPad -> Draw();
  
  hs -> Draw("hist,same");
  if( h_sum != NULL )
  {
    h_sum -> SetMarkerSize(0.);
    h_sum -> SetLineColor(kBlack);
    h_sum -> SetFillColor(kBlack);
    h_sum -> SetFillStyle(3004);
    h_sum -> Draw("E2,same");  
  }
  
  for(unsigned int ii = 0; ii < objectList.size(); ++ii)
  {
    TH1F* object = static_cast<TH1F*>(objects_.at(ii));
    std::string objectName = objectList.at(ii);
    
    if( opts_.GetOpt<int>(Form("%s.%s.stack",varName_.c_str(),objectName.c_str())) == 0 )
      object -> Draw(Form("%s,same",(opts_.GetOpt<std::string>(Form("%s.%s.drawStyle",varName_.c_str(),objectName.c_str()))).c_str()));
  }
  
  gPad -> RedrawAxis("");
  gPad -> RedrawAxis("G");
  
  TPad* pad2 = NULL;
  if( opts_.GetOpt<int>(Form("%s.drawRatioPlot",varName_.c_str())) == 1 )
  {
    c_->cd(); // Go back to the main canvas before defining pad2
    
    // Do not draw the X axis label on the upper plot and redraw a small
    hPad->GetXaxis()->SetLabelSize(0.);
    hPad->GetXaxis()->SetTitleSize(0.);
    
    pad2 = new TPad("pad2","pad2", 0,0.,1,0.3);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.4);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();
    
    h_data_ratio -> Divide(h_sum);
    
    h_data_ratio -> SetTitle(Form(";%s;data / bkg",hPad->GetXaxis()->GetTitle()));
    
    h_data_ratio->GetXaxis()->SetRangeUser(xMin-(xMax-xMin)/nBinsX,xMax+(xMax-xMin)/nBinsX);
    h_data_ratio->GetYaxis()->SetRangeUser(0.,2.);
    h_data_ratio->GetYaxis()->SetNdivisions(505);
    
    h_data_ratio->GetXaxis()->SetTitleFont(43);
    h_data_ratio->GetYaxis()->SetTitleFont(43);
    h_data_ratio->GetXaxis()->SetTitleSize(25);
    h_data_ratio->GetYaxis()->SetTitleSize(22);
    h_data_ratio->GetXaxis()->SetTitleOffset(4.);
    h_data_ratio->GetYaxis()->SetTitleOffset(1.85);
    h_data_ratio->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    h_data_ratio->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    h_data_ratio->GetXaxis()->SetLabelSize(17);
    h_data_ratio->GetYaxis()->SetLabelSize(17);
    h_data_ratio -> Draw("P");
    
    // TH1F* h_ratioErr = (TH1F*)( h_data_ratio->Clone("h_ratioErr") );
    // h_data_ratio -> SetLineColor(kBlack);
    // h_data_ratio -> SetFillColor(kBlack);
    // h_data_ratio -> SetFillStyle(3004);
    // h_data_ratio -> Draw("PE3,same");
    
    TF1* f_line1 = new TF1("f_line1","1.",-999999.,999999.);
    f_line1 -> SetLineColor(kBlack);
    f_line1 -> SetLineStyle(2);
    f_line1 -> Draw("same");
  }
  
  if( opts_.GetOpt<int>(Form("%s.drawRatioPlot",varName_.c_str())) == 1 )
  {
    pad1 -> cd();
    if( normalization == "lumi" || normalization == "lumiScaled" )
      CMS_lumi(pad1,5,10);
    legend -> Draw("same");
  }
  else
  {
    c_ -> cd();
    if( normalization == "lumi" || normalization == "lumiScaled" )
      CMS_lumi(c_,5,10);
    legend -> Draw("same");
  }
  
  
  clog_ -> cd();
  
  if( opts_.GetOpt<int>(Form("%s.drawRatioPlot",varName_.c_str())) == 1 )
  {
    pad1 = new TPad("pad1","pad1",0,0.3,1,1.0);
    pad1->SetBottomMargin(0.03); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();
  }
  
  TH1F* hPadLog = (TH1F*)( gPad->DrawFrame(xMin-(xMax-xMin)/nBinsX,yMin/5.,xMax+(xMax-xMin)/nBinsX,5.*yMax) );
  if( normalization == "area" ) hPadLog -> SetTitle(Form(";%s %s;event fraction / %.1e %s",title.c_str(),unitString.c_str(),(xMax-xMin)/nBinsX,unitString.c_str()));
  if( normalization == "lumi" || normalization == "lumiScaled" ) hPadLog -> SetTitle(Form(";%s %s;events / %.1e %s",title.c_str(),unitString.c_str(),(xMax-xMin)/nBinsX,unitString.c_str()));
  hPadLog->GetXaxis()->SetTitleFont(43);
  hPadLog->GetYaxis()->SetTitleFont(43);
  hPadLog->GetXaxis()->SetTitleSize(25);
  hPadLog->GetYaxis()->SetTitleSize(22);
  hPadLog->GetXaxis()->SetTitleOffset(0.9);
  hPadLog->GetYaxis()->SetTitleOffset(1.85);
  hPadLog->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  hPadLog->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  hPadLog->GetXaxis()->SetLabelSize(17);
  hPadLog->GetYaxis()->SetLabelSize(17);
  hPadLog -> Draw();
  gPad -> SetLogy();
  
  hs -> Draw("hist,same");
  
  for(unsigned int ii = 0; ii < objectList.size(); ++ii)
  {
    TH1F* object = static_cast<TH1F*>(objects_.at(ii));
    std::string objectName = objectList.at(ii);
    if( opts_.GetOpt<int>(Form("%s.%s.stack",varName_.c_str(),objectName.c_str())) == 0 )    
      object -> Draw(Form("%s,same",(opts_.GetOpt<std::string>(Form("%s.%s.drawStyle",varName_.c_str(),objectName.c_str()))).c_str()));
  }
  
  gPad -> RedrawAxis("");
  gPad -> RedrawAxis("G");
  
  if( opts_.GetOpt<int>(Form("%s.drawRatioPlot",varName_.c_str())) == 1 )
  {
    clog_->cd(); // Go back to the main canvas before defining pad2
    // Do not draw the X axis label on the upper plot and redraw a small
    hPadLog->GetXaxis()->SetLabelSize(0.);
    hPadLog->GetXaxis()->SetTitleSize(0.);
    
    pad2 = new TPad("pad2","pad2", 0,0.,1,0.3);
    pad2->SetTopMargin(0.03);
    pad2->SetBottomMargin(0.4);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();
    
    h_data_ratio -> Draw("P");
    
    TF1* f_line1 = new TF1("f_line1","1.",-999999.,999999.);
    f_line1 -> SetLineColor(kBlack);
    f_line1 -> SetLineStyle(2);
    f_line1 -> Draw("same");
  }
  
  if( opts_.GetOpt<int>(Form("%s.drawRatioPlot",varName_.c_str())) == 1 )
  {
    pad1 -> cd();
    if( normalization == "lumi" || normalization == "lumiScaled" )
      CMS_lumi(pad1,5,10);
    legend -> Draw("same");
  }
  else
  {
    clog_ -> cd();
    if( normalization == "lumi" || normalization == "lumiScaled" )
      CMS_lumi(clog_,5,10);
    legend -> Draw("same");
  }
}

PlotTH1F::~PlotTH1F()
{}
