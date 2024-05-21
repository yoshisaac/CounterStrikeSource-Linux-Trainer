#include <sys/types.h>
#include <cstdint>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdbe.h>

namespace playerOffset {
  constexpr uintptr_t health = 0x34;
  constexpr uintptr_t name = 0x10; //first ascii char
  constexpr uintptr_t pitch = 0x44;
  constexpr uintptr_t yaw = 0x48;
  constexpr uintptr_t x = 0x38;
  constexpr uintptr_t y = 0x3C;
  constexpr uintptr_t z = 0x40;
  constexpr uintptr_t team = 0x30;
};

void players(pid_t gamePid, XdbeBackBuffer back_buffer, Display* d, Window win, unsigned int playerList);


//playerlist math
//begining offset from playerlist 0x28
//players are 0x140 appart from each other
