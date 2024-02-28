#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace BHOP {
  static int DOJUMP = 5;
  static int DONTJUMP = 4;
};

void bhop(pid_t gamePid, Display* d, unsigned int onGround, unsigned int dwForceJump);
