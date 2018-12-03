#ifndef UTILS__
#define UTILS__

#include <iostream>
#include <string>
#include <vector>

//there are the 4 different reference frames in usage, taking the barrel as example

//0 CMS reference system eta, phi

//1  ECALELF reference
//   XSeedSCEle=ieta in [-85,85] with 0 excluded for a total of 171 bins
//   YSeedSCEle=iphi in [1,360] for a total of 360 values

//2  TH2 bin numbering: for hystorical reasons phi is on the x-axis 
//   binx in [1,360] for a total of 360 bins --> actually the range is [1,361] because one has to account for the width of the last bin  
//   biny in [-85,85] for a total of 171 bins --> actually the range is [-85,86] because one has to account for the width of the last bin

//3  ix, iy auxiliary reference system 
//   ix in [0,359] for a total of 360 bins
//   iy in [0,170] for a total of 171 bins

//4  1-D index
//   360*171 bins





inline int fromIetaIphito1Dindex(const int &ieta, const int &iphi, const int &Neta, const int &Nphi, const int &ietamin, const int &iphimin)
{
  return (iphi - iphimin)+Nphi*(ieta - ietamin);
}

inline int fromTH2indexto1Dindex(const int &binx, const int &biny, const int &Nbinx, const int &Nbiny)
{
  return (binx - 1)+Nbinx*(biny - 1);
}

inline void from1DindextoIetaIphi(const int &index, int &ieta, int &iphi, const int &Neta, const int &Nphi, const int &ietamin, const int &iphimin)
{
  iphi = index % Nphi+iphimin;
  ieta = index / Nphi+ietamin;
}

inline void from1DindextoTH2index(const int &index, int &binx, int &biny, const int &Nbinx, const int &Nbiny)
{
  binx = index % Nbinx+1;
  biny = index / Nbinx+1;
}

#endif
