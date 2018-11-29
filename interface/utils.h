#ifndef UTILS__
#define UTILS__

#include <iostream>
#include <string>
#include <vector>


Float_t fromIEtaIPhito1Dindex(const Int_t &ieta, const Int_t &iphi, const Int_t &Nphi, const Int_t &ietamin, const Int_t &iphimin)
{
  return (iphi - iphimin)+Nphi*(ieta - ietamin);
}

Float_t from2Dto1Dindex(const Int_t &ix, const Int_t &iy, const Int_t &Nphi)
{
  return ix+Nphi*iy;
}

void from1DindextoIEtaIPhi(const Int_t &index, Int_t &ieta, Int_t &iphi, const Int_t &Nphi)
{
  iphi = index % Nphi;
  ieta = index / Nphi;
}
#endif
