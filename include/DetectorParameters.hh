////////////////////////////////////////////////////////
//
// DetectorParameters.hh
//
// linehan3@fnal.gov
// This is a file of detector parameters, those
// that define dimensions of the various geometries
// used in the silicon qubit sims code.
//
////////////////////////////////////////////////////////

#ifndef DetectorParameters_hh
#define DetectorParameters_hh 1

#include "CLHEP/Units/SystemOfUnits.h"
#include <cstdlib>
#include <cmath>

namespace DetectorParameters
{
  //----------------------------------------------------------------
  //Overall World
  // constexpr double WorldL = 0.127 * 2.2 * CLHEP::m;
  constexpr double dp_worldSize = 0.127 * 2.2 * CLHEP::m;  //((0.02+0.01+0.02)*2+0.006);

  //----------------------------------------------------------------
  //Absorption fridge
  constexpr double dp_fridgeInnerRadius = 0.114338 * CLHEP::m;
  constexpr double dp_fridgeOuterRadius = 0.114846 * CLHEP::m;
  constexpr double dp_fridgeInnerRadiusShield = 0.124155 * CLHEP::m;
  constexpr double dp_fridgeOuterRadiusShield = 0.127 * CLHEP::m;
  constexpr double dp_fridgeHeight = 0.127 * CLHEP::m;
  
  double test = 1 * CLHEP::deg;

  //----------------------------------------------------------------
  //Misc
  constexpr double dp_eps = 0.0001 * CLHEP::mm;
  constexpr double pi = 3.141592654;


  //----------------------------------------------------------------
  //Parameters of the copper housing
  constexpr bool dp_useQubitHousing = true;
  constexpr double dp_housing1DimX = 6. * CLHEP::mm;
  constexpr double dp_housing1DimY = 4. * CLHEP::mm;
  constexpr double dp_housing1DimZ = 2. * CLHEP::mm;
  constexpr double dp_housing2DimX = 6. * CLHEP::mm;
  constexpr double dp_housing2DimY = 2. * CLHEP::mm;
  constexpr double dp_housing2DimZ = 6. * CLHEP::mm;

  //----------------------------------------------------------------
  //Si substrate dimensions
  constexpr double dp_SisubstrateDimX = 1. * CLHEP::mm;
  constexpr double dp_SisubstrateDimY = 1. * CLHEP::mm;
  constexpr double dp_SisubstrateDimZ = 0.525 * CLHEP::mm;

  //----------------------------------------------------------------
  //SiO2 substrate dimensions
  constexpr double dp_SiO2substrateDimX = dp_SisubstrateDimX;
  constexpr double dp_SiO2substrateDimY = dp_SisubstrateDimY;
  constexpr double dp_SiO2substrateDimZ = 240. * CLHEP::nm;

  //----------------------------------------------------------------
  //SiO2 toplayer dimensions
  constexpr double dp_SiO2toplayerDimX = dp_SisubstrateDimX;
  constexpr double dp_SiO2toplayerDimY = dp_SisubstrateDimY;
  constexpr double dp_SiO2toplayerDimZ = 40. * CLHEP::nm;

  //----------------------------------------------------------------
  //Sensor dimensions
  constexpr double dp_sensorDimX = dp_SisubstrateDimX;
  constexpr double dp_sensorDimY = dp_SisubstrateDimY;
  constexpr double dp_sensorDimZ = -1. * CLHEP::mm;

  //----------------------------------------------------------------
  //Wire dimensions
	constexpr double dp_stripDimX = dp_sensorDimX * 0.9;
  constexpr double dp_stripDimY = dp_sensorDimY * 0.9;
  constexpr double dp_stripDimZ = 3 * CLHEP::nm;
	constexpr double dp_stripThickness  = 1500 * CLHEP::nm;
	constexpr double dp_stripSpacing = 2250 * CLHEP::nm;
  constexpr double dp_stripWrapInnerRadius = dp_stripSpacing / 2;
  constexpr double dp_stripWrapOuterRadius = (dp_stripSpacing + (2 * dp_stripThickness)) / 2;
  constexpr int dp_numStrips = 240;
}







#endif
