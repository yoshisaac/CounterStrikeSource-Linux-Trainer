#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdbe.h>

namespace AIMBOT {
  inline int closestScreenIndex = 1;
  inline int closestDistanceIndex = 1;
  
  inline int aimIndex = -1;
};

void aimbot(pid_t, Display*);
