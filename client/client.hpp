#include <sys/types.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace CLIENT {
  static int DOSHOOT = 5;
  static int DONTSHOOT = 4;
};

void client(pid_t gamePid, Display* d, unsigned int dwForceAttack);
