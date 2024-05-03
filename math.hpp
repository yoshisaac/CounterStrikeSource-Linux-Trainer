#include <cmath>

inline float distanceFormula(float locationOne[2], float locationTwo[2]) {
  return sqrt(((locationOne[1] - locationTwo[1])*(locationOne[1] - locationTwo[1])) + ((locationOne[0] - locationTwo[0])*(locationOne[0] - locationTwo[0])));
}
