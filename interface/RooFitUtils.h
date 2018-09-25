#ifndef ROOFIT_UTILS_H
#define ROOFIT_UTILS_H

#include "TCanvas.h"
#include "TLegend.h"
#include "TCut.h"

#include "RooPlot.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooAbsData.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooExtendPdf.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooMinuit.h"
#include "RooMinimizer.h"
#include <RooMCStudy.h>
#include "RooMsgService.h"
#include "RooCustomizer.h"
#include "RooBinning.h"
#include "RooWorkspace.h"
#include "RooClassFactory.h"

#include "RooStats/AsymptoticCalculator.h"
#include "RooStats/ProfileLikelihoodCalculator.h"
#include "RooStats/HypoTestResult.h"

using namespace std;
using namespace RooFit;



float makeFits(RooDataSet* tth, RooDataSet* cs, float mvaMin, string title, int i, bool usePowerLaw=0, bool useSingleExp=0);

float makeFitSimulataneous(RooDataSet* tth1, RooDataSet* ds1, RooDataSet* tth2, RooDataSet* ds2, string title, bool usePowerLaw=0, bool useSingleExp=0);

#endif
