#ifndef __CMS_LUMI_H__
#define __CMS_LUMI_H__

#include <iostream>

#include "TString.h"
#include "TPad.h"
#include "TLatex.h"

using namespace std;



//-----------------
// Global variables

extern TString cmsText;
extern float cmsTextFont;

extern bool writeExtraText;
extern TString extraText;
extern float extraTextFont;

// text sizes and text offsets with respect to the top frame
// in unit of the top margin size
extern float lumiTextSize;
extern float lumiTextOffset;
extern float cmsTextSize;
extern float cmsTextOffset;

extern float relPosX;
extern float relPosY;
extern float relExtraDY;

// ratio of "CMS" and extra text size
extern float extraOverCmsTextSize;

extern TString lumi_13TeV;
extern TString lumi_8TeV;
extern TString lumi_7TeV;
extern TString lumi_sqrtS;

extern bool drawLogo;

void CMS_lumi(TPad* pad, int iPeriod=3, int iPosX=10);



#endif
