#include "interface/ParticleNames.h"



std::string GetParticleName(const int& pdgId)
{
  // quarks
  if( pdgId == +1 ) return "d";
  if( pdgId == -1 ) return "d_bar";
  if( pdgId == +2 ) return "u";
  if( pdgId == -2 ) return "u_bar";
  if( pdgId == +3 ) return "s";
  if( pdgId == -3 ) return "s_bar";
  if( pdgId == +4 ) return "c";
  if( pdgId == -4 ) return "c_bar";
  if( pdgId == +5 ) return "b";
  if( pdgId == -5 ) return "b_bar";
  if( pdgId == +6 ) return "t";
  if( pdgId == -6 ) return "t_bar";
  if( pdgId == +7 ) return "b'";
  if( pdgId == -7 ) return "b'_bar";
  if( pdgId == +8 ) return "t'";
  if( pdgId == -8 ) return "t'_bar";
  
  // di-quarks
  if( pdgId == +1103 ) return "(dd)1";
  if( pdgId == +2101 ) return "(ud)0";
  if( pdgId == +2103 ) return "(ud)1";
  if( pdgId == +2203 ) return "(uu)1";
  if( pdgId == +3101 ) return "(sd)0";
  if( pdgId == +3103 ) return "(sd)1";
  if( pdgId == +3201 ) return "(su)0";
  if( pdgId == +3203 ) return "(su)1";
  if( pdgId == +3303 ) return "(ss)1";
  if( pdgId == +4101 ) return "(cd)0";
  if( pdgId == +4103 ) return "(cd)1";
  if( pdgId == +4201 ) return "(cu)0";
  if( pdgId == +4203 ) return "(cu)1";
  if( pdgId == +4301 ) return "(cs)0";
  if( pdgId == +4303 ) return "(cs)1";
  if( pdgId == +4403 ) return "(cc)1";
  if( pdgId == +5101 ) return "(bd)0";
  if( pdgId == +5103 ) return "(bd)1";
  if( pdgId == +5201 ) return "(bu)0";
  if( pdgId == +5203 ) return "(bu)1";
  if( pdgId == +5301 ) return "(bs)0";
  if( pdgId == +5303 ) return "(bs)1";
  if( pdgId == +5401 ) return "(bc)0";
  if( pdgId == +5403 ) return "(bc)1";
  if( pdgId == +5503 ) return "(bb)1";
  
  // leptons
  if( pdgId == +11 ) return "e-";
  if( pdgId == -11 ) return "e+";
  if( pdgId == +12 ) return "nu_e";
  if( pdgId == -12 ) return "nu_e_bar";
  if( pdgId == +13 ) return "mu-";
  if( pdgId == -13 ) return "mu+";
  if( pdgId == +14 ) return "nu_mu";
  if( pdgId == -14 ) return "nu_mu_bar";
  if( pdgId == +15 ) return "tau-";
  if( pdgId == -15 ) return "tau+";
  if( pdgId == +16 ) return "nu_tau";
  if( pdgId == -16 ) return "nu_tau_bar";
  if( pdgId == +17 ) return "tau'-";
  if( pdgId == -17 ) return "tau'+";
  if( pdgId == +18 ) return "nu_tau'";
  if( pdgId == -18 ) return "nu_tau'_bar";
  
  // gauge and Higgs bosons
  if( pdgId == +21 ) return "g";
  if( pdgId == +22 ) return "gamma";
  if( pdgId == +23 ) return "Z0";
  if( pdgId == +24 ) return "W+";
  if( pdgId == -24 ) return "W-";
  if( pdgId == +25 ) return "h0";
  if( pdgId == +32 ) return "Z'";
  if( pdgId == +33 ) return "Z''";
  if( pdgId == +34 ) return "W'+";
  if( pdgId == -34 ) return "W'-";
  if( pdgId == +35 ) return "H0";
  if( pdgId == +36 ) return "A0";
  if( pdgId == +37 ) return "H+";
  if( pdgId == -37 ) return "H-";
  if( pdgId == +39 ) return "G";
  if( pdgId == +41 ) return "R0";
  if( pdgId == +42 ) return "LQ";
  
  // light I=1 mesons
  if( pdgId == +111 ) return "pi0";
  if( pdgId == +211 ) return "pi+";
  if( pdgId == -211 ) return "pi-";
  if( pdgId == +9000111 ) return "a0(980)0";
  if( pdgId == +9000211 ) return "a0(980)+";
  if( pdgId == -9000211 ) return "a0(980)-";
  if( pdgId == +100111 ) return "pi(1300)0";
  if( pdgId == +100211 ) return "pi(1300)+";
  if( pdgId == -100211 ) return "pi(1300)-";
  if( pdgId == +10111 ) return "a0(1450)0";
  if( pdgId == +10211 ) return "a0(1450)+";
  if( pdgId == -10211 ) return "a0(1450)-";
  if( pdgId == +9010111 ) return "pi(1800)0";
  if( pdgId == +9010211 ) return "pi(1800)+";
  if( pdgId == -9010211 ) return "pi(1800)-";
  if( pdgId == +113 ) return "rho(770)0";
  if( pdgId == +213 ) return "rho(770)+";
  if( pdgId == -213 ) return "rho(770)-";
  if( pdgId == +10113 ) return "b1(1235)0";
  if( pdgId == +10213 ) return "b1(1235)+";
  if( pdgId == -10213 ) return "b1(1235)-";
  if( pdgId == +20113 ) return "a1(1260)0";
  if( pdgId == +20213 ) return "a1(1260)+";
  if( pdgId == -20213 ) return "a1(1260)-";
  
  // light I=0 mesons
  if( pdgId == +221 ) return "eta";
  if( pdgId == +331 ) return "eta'(958)";
  if( pdgId == +223 ) return "omega(782)";
  if( pdgId == +333 ) return "phi(1020)";
  
  // strange mesons
  if( pdgId == +130 ) return "K0_L";
  if( pdgId == -130 ) return "K0_L_bar";
  if( pdgId == +310 ) return "K0_S";
  if( pdgId == -310 ) return "K0_S_bar";
  if( pdgId == +311 ) return "K0";
  if( pdgId == -311 ) return "K0_bar";
  if( pdgId == +321 ) return "K+";
  if( pdgId == -321 ) return "K-";
  if( pdgId == +9000311 ) return "K0*(800)0";
  if( pdgId == -9000311 ) return "K0*(800)0_bar";
  if( pdgId == +9000321 ) return "K0*(800)+";
  if( pdgId == -9000321 ) return "K0*(800)-_bar";
  if( pdgId == +10311 ) return "K0*(1430)0";
  if( pdgId == -10311 ) return "K0*(1430)0_bar";
  if( pdgId == +10321 ) return "K0*(1430)+";
  if( pdgId == -10321 ) return "K0*(1430)-_bar";
  if( pdgId == +100311 ) return "K*(1460)0";
  if( pdgId == -100311 ) return "K*(1460)0_bar";
  if( pdgId == +100321 ) return "K*(1460)+";
  if( pdgId == -100321 ) return "K*(1460)-_bar";
  if( pdgId == +9010311 ) return "K(1830)0";
  if( pdgId == -9010311 ) return "K(1830)0_bar";
  if( pdgId == +9010321 ) return "K(1830)+";
  if( pdgId == -9010321 ) return "K(1830)-_bar";
  if( pdgId == +9020311 ) return "K0*(1950)0";
  if( pdgId == -9020311 ) return "K0*(1950)0_bar";
  if( pdgId == +9020321 ) return "K0*(1950)+";
  if( pdgId == -9020321 ) return "K0*(1950)-_bar";
  if( pdgId == +313 ) return "K*(892)0";
  if( pdgId == -313 ) return "K*(892)0_bar";
  if( pdgId == +323 ) return "K*(892)+";
  if( pdgId == -323 ) return "K*(892)-";
  if( pdgId == +10313 ) return "K1(1270)0";
  if( pdgId == -10313 ) return "K1(1270)0_bar";
  if( pdgId == +10323 ) return "K1(1270)+";
  if( pdgId == -10323 ) return "K1(1270)-";
  if( pdgId == +20313 ) return "K1(1400)0";
  if( pdgId == -20313 ) return "K1(1400)0_bar";
  if( pdgId == +20323 ) return "K1(1400)+";
  if( pdgId == -20323 ) return "K1(1400)-";
  if( pdgId == +100313 ) return "K*(1410)0";
  if( pdgId == -100313 ) return "K*(1410)0_bar";
  if( pdgId == +100323 ) return "K*(1410)+";
  if( pdgId == -100323 ) return "K*(1410)-";
  
  // charmed mesons
  if( pdgId == +411 ) return "D+";
  if( pdgId == -411 ) return "D-";
  if( pdgId == +421 ) return "D0";
  if( pdgId == -421 ) return "D0_bar";
  if( pdgId == +10411 ) return "D0*(2400)+";
  if( pdgId == -10411 ) return "D0*(2400)-";
  if( pdgId == +10421 ) return "D0*(2400)0";
  if( pdgId == -10421 ) return "D0*(2400)0_bar";
  if( pdgId == +413 ) return "D*(2010)+";
  if( pdgId == -413 ) return "D*(2010)-";
  if( pdgId == +423 ) return "D*(2007)0";
  if( pdgId == -423 ) return "D*(2007)0_bar";
  if( pdgId == +431 ) return "D_s+";
  if( pdgId == -431 ) return "D_s-";
  if( pdgId == +433 ) return "D*_s+";
  if( pdgId == -433 ) return "D*_s-";
  
  // bottom mesons
  if( pdgId == +511 ) return "B0";
  if( pdgId == -511 ) return "B0_bar";
  if( pdgId == +521 ) return "B+";
  if( pdgId == -521 ) return "B-";
  if( pdgId == +10511 ) return "B0*0";
  if( pdgId == -10511 ) return "B0*0_bar";
  if( pdgId == +10521 ) return "B0*+";
  if( pdgId == -10521 ) return "B0*-";
  if( pdgId == +513 ) return "B*0";
  if( pdgId == -513 ) return "B*0_bar";
  if( pdgId == +523 ) return "B*+";
  if( pdgId == -523 ) return "B*-";
  
  // cc_bar mesons
  if( pdgId == +441 ) return "eta_c(1S)";
  if( pdgId == +10441 ) return "chi_c0(1P)";
  if( pdgId == +100441 ) return "eta_c(2S)";
  if( pdgId == +443 ) return "J/psi(1S)";
  if( pdgId == +10443 ) return "h_c(1P)";
  if( pdgId == +20443 ) return "chi_c1(1P)";
  
  // light baryons
  if( pdgId == +2212 ) return "p";
  if( pdgId == -2212 ) return "p_bar";
  if( pdgId == +2112 ) return "n";
  if( pdgId == -2112 ) return "n_bar";
  if( pdgId == +2224 ) return "Delta++";
  if( pdgId == -2224 ) return "Delta++_bar";
  if( pdgId == +2214 ) return "Delta+";
  if( pdgId == -2214 ) return "Delta+_bar";
  if( pdgId == +2114 ) return "Delta0";
  if( pdgId == -2114 ) return "Delta0_bar";
  if( pdgId == +1114 ) return "Delta-";
  if( pdgId == -1114 ) return "Delta-_bar";
  
  // strange baryons
  if( pdgId == +3122 ) return "Lambda";
  if( pdgId == -3122 ) return "Lambda_bar";
  if( pdgId == +3222 ) return "Sigma+";
  if( pdgId == -3222 ) return "Sigma*_bar";
  if( pdgId == +3212 ) return "Sigma0";
  if( pdgId == -3212 ) return "Sigma0_bar";
  if( pdgId == +3112 ) return "Sigma-";
  if( pdgId == -3112 ) return "Sigma-_bar";
  if( pdgId == +3224 ) return "Sigma*+";
  if( pdgId == -3224 ) return "Sigma*+_bar";
  if( pdgId == +3214 ) return "Sigma*0";
  if( pdgId == -3214 ) return "Sigma*0_bar";
  if( pdgId == +3114 ) return "Sigma*-";
  if( pdgId == -3114 ) return "Sigma*-_bar";
  if( pdgId == +3322 ) return "Chi0";
  if( pdgId == -3322 ) return "Chi0_bar";
  if( pdgId == +3312 ) return "Chi-";
  if( pdgId == -3312 ) return "Chi-_bar";
  if( pdgId == +3324 ) return "Chi0*";
  if( pdgId == -3324 ) return "Chi0*_bar";
  if( pdgId == +3314 ) return "Chi*-";
  if( pdgId == -3314 ) return "Chi*-_bar";
  if( pdgId == +3334 ) return "Omega-";
  if( pdgId == -3334 ) return "Omega-_bar";
  
  // charmed baryons
  if( pdgId == +4122 ) return "Lambda_c+";
  if( pdgId == -4122 ) return "Lambda_c+_bar";
  if( pdgId == +4222 ) return "Sigma_c++";
  if( pdgId == -4222 ) return "Sigma_c++_bar";
  if( pdgId == +4212 ) return "Sigma_c+";
  if( pdgId == -4212 ) return "Sigma_c+_bar";
  if( pdgId == +4112 ) return "Sigma_c0";
  if( pdgId == -4112 ) return "Sigma_c0_bar";
  if( pdgId == +4224 ) return "Sigma*_c++";
  if( pdgId == -4224 ) return "Sigma*_c++_bar";
  if( pdgId == +4214 ) return "Sigma*_c+";
  if( pdgId == -4214 ) return "Sigma*_c+_bar";
  if( pdgId == +4114 ) return "Sigma*_c0";
  if( pdgId == -4114 ) return "Sigma*_c0_bar";
  if( pdgId == +4232 ) return "Chi_c+";
  if( pdgId == -4232 ) return "Chi_c+_bar";
  if( pdgId == +4132 ) return "Chi_c0";
  if( pdgId == -4132 ) return "Chi_c0_bar";
  if( pdgId == +4322 ) return "Chi'_c+";
  if( pdgId == -4322 ) return "Chi'_c+_bar";
  if( pdgId == +4312 ) return "Chi'_c0";
  if( pdgId == -4312 ) return "Chi'_c0_bar";
  if( pdgId == +4324 ) return "Chi*_c+";
  if( pdgId == -4324 ) return "Chi*_c+_bar";
  if( pdgId == +4314 ) return "Chi*_c0";
  if( pdgId == -4314 ) return "Chi*_c0_bar";
  if( pdgId == +4332 ) return "Omega_c0";
  if( pdgId == -4332 ) return "Omega_c0_bar";
  if( pdgId == +4334 ) return "Omega*_c0";
  if( pdgId == -4334 ) return "Omega*_c0_bar";
  if( pdgId == +4412 ) return "Chi_cc+";
  if( pdgId == -4412 ) return "Chi_cc+_bar";
  if( pdgId == +4422 ) return "Chi_cc++";
  if( pdgId == -4422 ) return "Chi_cc++_bar";
  if( pdgId == +4414 ) return "Chi*_cc+";
  if( pdgId == -4414 ) return "Chi*_cc+_bar";
  if( pdgId == +4424 ) return "Chi*_cc++";
  if( pdgId == -4424 ) return "Chi*_cc++_bar";
  if( pdgId == +4432 ) return "Omega_cc+";
  if( pdgId == -4432 ) return "Omega_cc+_bar";
  if( pdgId == +4434 ) return "Omega*_cc+";
  if( pdgId == -4434 ) return "Omega*_cc+_bar";
  if( pdgId == +4444 ) return "Omega_ccc++";
  if( pdgId == -4444 ) return "Omega_ccc++_bar";
  
  return "n/a";
}
