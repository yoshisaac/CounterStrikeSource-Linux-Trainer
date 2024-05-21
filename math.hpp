#include <cmath>

//180/pi
#define radpi 57.295779513082f

inline float distanceFormula3D(float locationOne[3], float locationTwo[2]) {
  return sqrt(((locationOne[1] - locationTwo[1])*(locationOne[1] - locationTwo[1])) + ((locationOne[0] - locationTwo[0])*(locationOne[0] - locationTwo[0])) + ((locationOne[2] - locationTwo[2])*(locationOne[2] - locationTwo[2])));
}

inline float distanceFormula2D(float locationOne[2], float locationTwo[2]) {
  return sqrt(((locationOne[1] - locationTwo[1])*(locationOne[1] - locationTwo[1])) + ((locationOne[0] - locationTwo[0])*(locationOne[0] - locationTwo[0])));
}

inline float distanceFormula(float x1, float x2) {
  return x2 - x1;
}
