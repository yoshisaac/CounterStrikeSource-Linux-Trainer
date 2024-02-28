#include <sys/types.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace playerOffset {
  const unsigned int health = 0x34;
  const unsigned int name = 0x10;
  const unsigned int pitch = 0x44;
  const unsigned int yaw = 0x48;
  const unsigned int x = 0x38;
  const unsigned int y = 0x3C;
  const unsigned int z = 0x40;
  const unsigned int team = 0x30;
};

void players(pid_t gamePid, Display* d, Window win, unsigned int playerList, unsigned int viewMatrix);


//playerlist math
//begining offset from playerlist 0x28
//players are 0x140 appart from each other
