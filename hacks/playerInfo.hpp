#include <sys/types.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace playerOffset {
  constexpr unsigned int health = 0x34;
  constexpr unsigned int name = 0x10; //first ascii char
  constexpr unsigned int pitch = 0x44;
  constexpr unsigned int yaw = 0x48;
  constexpr unsigned int x = 0x38;
  constexpr unsigned int y = 0x3C;
  constexpr unsigned int z = 0x40;
  constexpr unsigned int team = 0x30;
};

void players(pid_t gamePid, Display* d, Window win, unsigned int playerList, unsigned int viewMatrix);


//playerlist math
//begining offset from playerlist 0x28
//players are 0x140 appart from each other
