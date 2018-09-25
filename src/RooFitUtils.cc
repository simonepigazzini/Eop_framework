#include "interface/RooFitUtils.h"



float makeFits(RooDataSet* tth, RooDataSet* cs, float mvaMin, string title, int i, bool usePowerLaw, bool useSingleExp)
{
  float significance = -1;
  
  TCut mvaCut = Form("mva_ > %f && mass_ > 110",mvaMin);
  RooDataSet* tth_red = (RooDataSet*)tth->reduce(mvaCut);
  RooDataSet* cs_red  = (RooDataSet*)cs->reduce(mvaCut);
  
  RooWorkspace work("work");
  
  work.factory("mass_[110, 180]");
  work.factory("mean1[125., 123., 127.]");
  work.factory("mean2[125., 123., 127.]");
  work.factory("mean3[125., 123., 127.]");
  work.factory("sigma1[1., 0., 2]");
  work.factory("sigma2[3. , 1., 5.]");
  work.factory("sigma3[5. , 1., 10.]");
  work.factory("tth_ev[0.5, 0., 15.]");
  work.factory("bkg_ev[50. , 0., 1500.]");
  work.factory("Gaussian::gauss1(mass_, mean1, sigma1)");
  work.factory("Gaussian::gauss2(mass_, mean2, sigma2)");
  work.factory("Gaussian::gauss3(mass_, mean3, sigma3)");
  work.factory("SUM::signalz(frac1[0., 1.]*gauss1,gauss2)");
  work.factory("SUM::signal(frac1[0., 1.]*gauss3, signalz)");
  
  if(usePowerLaw)
  {
    RooRealVar x0("x0", "x0", 0, 10000); 
    RooRealVar x1("x1", "x1", -100, -0.001); 
    RooRealVar mass_("mass_", "mass_", 110, 180); 
    RooAbsPdf* bkg = RooClassFactory::makePdfInstance("bkg", "x0*pow(mass_, x1)", RooArgSet(mass_, x0, x1));
    work.import(*bkg);
  }
  
  if(!usePowerLaw && useSingleExp)
  {
    work.factory("lambda1[-0.3, -10., 0.]");
    work.factory("Exponential::bkg(mass_, lambda1)");
  }
  
  else
  {
    work.factory("lambda1[-0.3, -10., 0.]");
    work.factory("lambda2[-0.3, -10., 0.]");
    work.factory("Exponential::exp1(mass_, lambda1)");
    work.factory("Exponential::exp2(mass_, lambda2)");
    work.factory("SUM::bkg(frac[0., 1.]*exp1, exp2)");
  }
  
  work.factory("SUM::model(tth_ev*signal, bkg_ev*bkg)");
  work.var("tth_ev")->setVal(tth_red->sumEntries());
  work.var("bkg_ev")->setVal(cs_red->sumEntries());
  
  work.pdf("signal")->fitTo(*tth_red, Range(110., 140.), SumW2Error(kTRUE), PrintLevel(-1), RooFit::Minimizer("Minuit2","Migrad"));
  work.pdf("bkg")->fitTo(*cs_red, Range(110., 180.), SumW2Error(kTRUE), PrintLevel(-1),  RooFit::Minimizer("Minuit2","Migrad"));
  
  work.defineSet("poi", "tth_ev");
  
  // make the plots                                                                                                                                                                                                    
  RooDataSet data(*cs_red);
  data.append(*tth_red);
  
  TCanvas* c3 = new TCanvas("c3", "c3");
  c3 -> cd();
  
  RooPlot* massbkg_frame = work.var("mass_")->frame();
  cs_red -> plotOn(massbkg_frame, Binning(70));
  work.pdf("bkg")->plotOn(massbkg_frame);
  massbkg_frame -> SetTitle("m_{#gamma#gamma} background");
  massbkg_frame -> Draw();
  std::string title_ = ("Bkg_" + title + std::to_string(i) + "_mvaMin" + std::to_string(mvaMin)).c_str();
  
  c3 -> SaveAs((TString)("c_" +title_ + ".png"));
  c3 -> SaveAs((TString)("c_" +title_ + ".pdf"));
  
  TCanvas* c4 = new TCanvas("c4", "c4");
  c4 -> cd();
  RooPlot* masssignal_frame = work.var("mass_")->frame();
  tth_red -> plotOn(masssignal_frame, Binning(70));
  work.pdf("signal")->plotOn(masssignal_frame);
  masssignal_frame -> SetTitle("m_{#gamma#gamma} signal");
  masssignal_frame -> Draw();
  title_ = ("Signal" + title + std::to_string(i) + "_mvaMin" + std::to_string(mvaMin)).c_str();
  
  c4 -> SaveAs((TString)("c_" +title_ + ".png"));
  c4 -> SaveAs((TString)("c_" +title_ + ".pdf"));
  
  
  TCanvas* c5 = new TCanvas("c5", "c5");
  c5 -> cd();
  
  RooPlot* massfit_frame = work.var("mass_")->frame();
  data.plotOn(massfit_frame, Binning(70));
  work.pdf("model")->plotOn(massfit_frame);
  work.pdf("model")->plotOn(massfit_frame, Components(*work.pdf("bkg")), LineStyle(kDashed));
  massfit_frame -> SetTitle("m_{#gamma#gamma} signal+background");
  massfit_frame -> Draw();
  title_ = ("SignalBkg" + title + std::to_string(i) + "_mvaMin" + std::to_string(mvaMin)).c_str();
  
  c5 -> SaveAs((TString)("c_" +title_ + ".png"));
  c5 -> SaveAs((TString)("c_" +title_ + ".pdf"));
  
  delete c3;
  delete c4;
  delete c5;
  
  // Create (and plot) asimov dataset                                                                                                                                                                                  
  
  RooAbsData* asimov(RooStats::AsymptoticCalculator::GenerateAsimovData(*work.pdf("model"), RooArgSet(*work.var("mass_"))));
  
  TCanvas* c6 = new TCanvas("c6", "c6");
  c6 -> cd();
  
  RooPlot* asimov_frame = work.var("mass_")->frame();
  asimov->plotOn(asimov_frame);
  work.pdf("model")->plotOn(asimov_frame, Name("l1"));
  work.pdf("model")->plotOn(asimov_frame, Components(*work.pdf("bkg")), LineStyle(kDashed), Name("l2"));
  asimov_frame -> Draw();
  TLegend* leg3 = new TLegend(.7, .6, .95, .95, "");
  leg3 -> AddEntry("l1", "Signal + bkg", "l");
  leg3 -> AddEntry("l2", "bkg only", "l");
  leg3 -> Draw();
  title_ = ("Asimov" + title + std::to_string(i) + "_mvaMin" + std::to_string(mvaMin)).c_str();
  
  c6 -> SaveAs((TString)("c_" +title_ + ".png"));
  c6 -> SaveAs((TString)("c_" +title_ + ".pdf"));
  
  delete c6;
  
  //Just a little check!!!                                                                                                                                                                                             
  work.pdf("model")->fitTo(*asimov, Range(110., 180.), SumW2Error(kFALSE), PrintLevel(-1), RooFit::Minimizer("Minuit2","Migrad"));
  // cout << "Signal events from fit: " << work.var("tth_ev")->getVal() << ", true number of events: "<< tth_red->sumEntries() << endl;
  // cout << "Bkg events from fit: " << work.var("bkg_ev")->getVal() << ", true number of events: " << cs_red->sumEntries() << endl;
  
  //Fit the asimov dataset                                                                                                                                                                                    
  
  RooArgSet* snap =(RooArgSet*) work.set("poi")->snapshot();
  snap -> setRealValue("tth_ev", 0.);
  RooStats::ProfileLikelihoodCalculator pl(*asimov, *work.pdf("model"), *work.set("poi"), 0.05, snap);
  RooStats::HypoTestResult* hypo = pl.GetHypoTest();
  
  if( hypo == 0 )
  {
    cout << "Fit failed" << endl;
  }  
  else
  {
    // cout << "Significance = " << hypo->Significance() << endl;
    
    work.var("mass_") -> setRange("Integral", 120., 130. ) ;
    RooAbsReal* intPdf = work.pdf("model")-> createIntegral(*(work.var("mass_")), *(work.var("mass_")), "Integral");
    // cout << "Background in 120-130 GeV: " << intPdf->getVal()*work.var("bkg_ev")->getVal() << " (" << intPdf->getVal()*work.var("bkg_ev")->getVal()/10. << " ev/GeV)" << endl;
    
    significance = hypo->Significance();
  }
  
  return significance;
}




float makeFitSimulataneous(RooDataSet* tth1, RooDataSet* ds1, RooDataSet* tth2, RooDataSet* ds2, string title, bool usePowerLaw, bool useSingleExp)
{
  float significance = -1;
  RooWorkspace work("work");
  
  // Create discrete observable to label channels
  
  work.factory("mass_[110, 180]");
  work.factory("mean1_a[125., 123., 127.]");
  work.factory("mean2_a[125., 123., 127.]");
  work.factory("mean3_a[125., 123., 127.]");
  work.factory("sigma1_a[1., 0., 2]");
  work.factory("sigma2_a[3. , 1., 5.]");
  work.factory("sigma3_a[5. , 1., 10.]");
  work.factory("tth_ev_a[0.5, 0., 15.]");
  work.factory("bkg_ev_a[50. , 0., 1500.]");
  work.factory("mu[1. , 0., 10.]");
  work.factory("Gaussian::gauss1_a(mass_, mean1_a, sigma1_a)");
  work.factory("Gaussian::gauss2_a(mass_, mean2_a, sigma2_a)");
  work.factory("Gaussian::gauss3_a(mass_, mean3_a, sigma3_a)");
  work.factory("SUM::signalz_a(frac1_a[0., 1.]*gauss1_a,gauss2_a)");
  work.factory("SUM::signal_a(frac1_a[0., 1.]*gauss3_a, signalz_a)");
  work.factory("mean1_b[125., 123., 127.]");
  work.factory("mean2_b[125., 123., 127.]");
  work.factory("mean3_b[125., 123., 127.]");
  work.factory("sigma1_b[1., 0., 2]");
  work.factory("sigma2_b[3. , 1., 5.]");
  work.factory("sigma3_b[5. , 1., 10.]");
  work.factory("tth_ev_b[0.5, 0., 15.]");
  work.factory("bkg_ev_b[50. , 0., 1500.]");
  work.factory("Gaussian::gauss1_b(mass_, mean1_b, sigma1_b)");
  work.factory("Gaussian::gauss2_b(mass_, mean2_b, sigma2_b)");
  work.factory("Gaussian::gauss3_b(mass_, mean3_b, sigma3_b)");
  work.factory("SUM::signalz_b(frac1_b[0., 1.]*gauss1_b,gauss2_b)");
  work.factory("SUM::signal_b(frac1_b[0., 1.]*gauss3_b, signalz_b)");
  
  if(usePowerLaw)
  {
    RooRealVar x0("x0_a", "x0_a", 0, 10000); 
    RooRealVar x1("x1_a", "x1_a", -100, -0.001); 
    RooRealVar mass_("mass_", "mass_", 110, 180); 
    RooAbsPdf* bkg = RooClassFactory::makePdfInstance("bkg_a", "x0_a*pow(mass_, x1_a)", RooArgSet(mass_, x0, x1));
    work.import(*bkg);
  }
  
  if(!usePowerLaw && useSingleExp)
  {
    work.factory("lambda1_a[-0.3, -10., 0.]");
    work.factory("Exponential::bkg(mass_, lambda1_a)");
  }
  
  else
  {
    work.factory("lambda1_a[-0.3, -10., 0.]");
    work.factory("lambda2_a[-0.3, -10., 0.]");
    work.factory("Exponential::exp1_a(mass_, lambda1_a)");
    work.factory("Exponential::exp2_a(mass_, lambda2_a)");
    work.factory("SUM::bkg_a(frac_a[0., 1.]*exp1_a, exp2_a)");
    work.factory("lambda1_b[-0.3, -10., 0.]");
    work.factory("lambda2_b[-0.3, -10., 0.]");
    work.factory("Exponential::exp1_b(mass_, lambda1_b)");
    work.factory("Exponential::exp2_b(mass_, lambda2_b)");
    work.factory("SUM::bkg_b(frac_b[0., 1.]*exp1_b, exp2_b)");
  }
  
  
  work.factory("prod::nsig_a(mu, tth_ev_a)");
  work.factory("prod::nsig_b(mu, tth_ev_b)");
  work.factory("SUM::model_a(nsig_a*signal_a,bkg_ev_a*bkg_a)");
  work.var("tth_ev_a")->setVal(tth1->sumEntries());
  work.var("bkg_ev_a")->setVal(ds1->sumEntries());
  work.factory("SUM::model_b(nsig_b*signal_b,bkg_ev_b*bkg_b)");
  work.var("tth_ev_b")->setVal(tth2->sumEntries());
  work.var("bkg_ev_b")->setVal(ds2->sumEntries());
  
  work.var("mu")->setVal(1.);
  
  work.pdf("signal_a")->fitTo(*tth1, Range(110., 140.), SumW2Error(kTRUE), PrintLevel(-1), RooFit::Minimizer("Minuit","minimize"));
  work.pdf("bkg_a")->fitTo(*ds1, Range(110., 180.), SumW2Error(kTRUE), PrintLevel(-1),  RooFit::Minimizer("Minuit","minimize"));
  work.pdf("signal_b")->fitTo(*tth2, Range(110., 140.), SumW2Error(kTRUE), PrintLevel(-1), RooFit::Minimizer("Minuit","minimize"));
  work.pdf("bkg_b")->fitTo(*ds2, Range(110., 180.), SumW2Error(kTRUE), PrintLevel(-1),  RooFit::Minimizer("Minuit","minimize"));
  
  // Create discrete observable to label channels
  work.factory("index[channel1, channel2]");
  
  // Create joint pdf (RooSimultaneous)
  work.factory("SIMUL:jointModel(index,channel1=model_a,channel2=model_b)");
  
  work.defineSet("poi", "mu");
  
  // work.Print();	       
  // Create the asimov dataset                                                                                                                                                                                  
  
  RooAbsData* asimov(RooStats::AsymptoticCalculator::GenerateAsimovData(*work.pdf("jointModel"), RooArgSet(*work.var("mass_"), work.var("index"))));
  
  //Fit the asimov dataset                                                                                                                                                                                    
  
  RooArgSet* snap =(RooArgSet*) work.set("poi")->snapshot();
  snap -> setRealValue("mu", 0.);
  RooStats::ProfileLikelihoodCalculator pl(*asimov, *work.pdf("jointModel"), *work.set("poi"), 0.05, snap);
  RooStats::HypoTestResult* hypo = pl.GetHypoTest();
  
  if( hypo == 0 )
  {
    cout << "Fit failed" << endl;
  }
  else
  {
    cout << "Significance = " << hypo->Significance() << endl;
    
    // work.var("mass_") -> setRange("Integral", 120., 130. ) ;
    // RooAbsReal* intPdf = work.pdf("model")-> createIntegral(*(work.var("mass_")), *(work.var("mass_")), "Integral");
    // cout << "Background in 120-130 GeV: " << intPdf->getVal()*work.var("bkg_ev")->getVal() << " (" << intPdf->getVal()*work.var("bkg_ev")->getVal()/10. << " ev/GeV)" << endl;
    
    significance = hypo->Significance();
  }
  
  return significance;
}
