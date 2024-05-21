#include <sys/types.h>
#include <cstdint>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace CLIENT {
  inline static int DO = 5;
  inline static int DONT = 4;

  inline uintptr_t onGround;

  //networked variables
  inline uintptr_t dwForceJump;
  inline uintptr_t dwForceAttack1;
  inline uintptr_t dwForceAttack2;
};
