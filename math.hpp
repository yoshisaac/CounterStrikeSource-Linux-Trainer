#include <cmath>

#include "engine/engine.hpp"

#include "memory.hpp"

//180/pi
#define radpi 57.295779513082f

#define pideg 0.017453293f

#define isNaN(n) n != n

inline static float distanceFormula3D(float locationOne[3], float locationTwo[2]) {
  return sqrt(((locationOne[1] - locationTwo[1])*(locationOne[1] - locationTwo[1])) + ((locationOne[0] - locationTwo[0])*(locationOne[0] - locationTwo[0])) + ((locationOne[2] - locationTwo[2])*(locationOne[2] - locationTwo[2])));
}

inline static float distanceFormula2D(float locationOne[2], float locationTwo[2]) {
  return sqrt(((locationOne[1] - locationTwo[1])*(locationOne[1] - locationTwo[1])) + ((locationOne[0] - locationTwo[0])*(locationOne[0] - locationTwo[0])));
}

inline static float distanceFormula(float x1, float x2) {
  return x2 - x1;
}

// https://raw.githubusercontent.com/emsr/cxx_linear/master/lerp.h
namespace std
{

  /**
   * Linearly interpolate from @c a to @c b by fraction @c t.
   * @f[
   *    lerp(t; a, b) = t b + (1 - t) a
   * @f]
   */
  template<typename _Float>
    constexpr std::enable_if_t<std::is_floating_point_v<_Float>, _Float>
    inline lerp(_Float __a, _Float __b, _Float __t)
    {
      if (std::isnan(__a) || std::isnan(__b) || std::isnan(__t))
	return std::numeric_limits<_Float>::quiet_NaN();
      else if ((__a <= _Float{0} && __b >= _Float{0})
	    || (__a >= _Float{0} && __b <= _Float{0}))
	// ab <= 0 but product could overflow.
#ifndef FMA
	return __t * __b + (_Float{1} - __t) * __a;
#else
	return std::fma(__t, __b, (_Float{1} - __t) * __a);
#endif
      else if (__t == _Float{1})
	return __b;
      else
	{ // monotonic near t == 1.
#ifndef FMA
	  const auto __x = __a + __t * (__b - __a);
#else
	  const auto __x = std::fma(__t, __b - __a, __a);
#endif
	  return (__t > _Float{1}) == (__b > __a)
		 ? std::max(__b, __x)
		 : std::min(__b, __x);
	}
    }

} // namespace std



//https://github.com/ALittlePatate/CSS-external/blob/b17e083a4f0d0e4406d49d55c9c761cedab1ad66/ImGuiExternal/Memory.h#L61
inline static float vmatrix[4][4];
inline static bool WorldToScreen(pid_t gamePid, const float vIn[3], float vOut[2]) {
  Memory::Read(gamePid, ENGINE::viewMatrix, &vmatrix, sizeof(vmatrix));

  float w = vmatrix[3][0] * vIn[0] + vmatrix[3][1] * vIn[1] + vmatrix[3][2] * vIn[2] + vmatrix[3][3];

  if (w < 0.01f)
    return false;

  float vOutTmp[2];
  
  vOutTmp[0] = vmatrix[0][0] * vIn[0] + vmatrix[0][1] * vIn[1] + vmatrix[0][2] * vIn[2] + vmatrix[0][3];
  vOutTmp[1] = vmatrix[1][0] * vIn[0] + vmatrix[1][1] * vIn[1] + vmatrix[1][2] * vIn[2] + vmatrix[1][3];
  float invw = 1.0f / w;

  vOutTmp[0] *= invw;
  vOutTmp[1] *= invw;

  int width = ENGINE::screenX;
  int height = ENGINE::screenY;

  float x = width / 2.0f;
  float y = height / 2.0f;

  x += 0.5f * vOutTmp[0] * width + 0.5f;
  y -= 0.5f * vOutTmp[1] * height + 0.5f;
  
  vOut[0] = x * 100.f / 100.f;
  vOut[1] = y * 100.f / 100.f;

  return true;
}

inline bool WorldToScreenTest(pid_t gamePid, const float vIn[3], float vOut[2]) {
  Memory::Read(gamePid, ENGINE::viewMatrix, &vmatrix, sizeof(vmatrix));

  float clip_coords[4];
  clip_coords[0] = vIn[0] * vmatrix[0][0] + vIn[1] * vmatrix[0][1] + vIn[2] * vmatrix[0][2] + vmatrix[0][3];
  clip_coords[1] = vIn[0] * vmatrix[1][0] + vIn[1] * vmatrix[1][1] + vIn[2] * vmatrix[1][2] + vmatrix[1][3];
  clip_coords[2] = vIn[0] * vmatrix[2][0] + vIn[1] * vmatrix[2][1] + vIn[2] * vmatrix[2][2] + vmatrix[2][3];
  clip_coords[3] = vIn[0] * vmatrix[3][0] + vIn[1] * vmatrix[3][1] + vIn[2] * vmatrix[3][2] + vmatrix[3][3];

  if (clip_coords[3] < 0.1f)
    return false;

  float ndc[3];
  ndc[0] = clip_coords[0] / clip_coords[3];
  ndc[1] = clip_coords[1] / clip_coords[3];
  ndc[2] = clip_coords[2] / clip_coords[3];

  vOut[0] = (int)((ENGINE::screenX / 2 * ndc[0]) + (ndc[0] + ENGINE::screenX / 2));
  vOut[1] = (int)(-(ENGINE::screenY / 2 * ndc[1]) + (ndc[1] + ENGINE::screenY / 2));
  return true;
}
