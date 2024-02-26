#include "bhop.hpp"

#include "../xutil.hpp"
#include "../memory.hpp"

void bhop(pid_t gamePid, Display* d, unsigned int isOnGround, unsigned int dwForceJump) {
  pid_t currentWindowPid = getPidByWindow(d, getFocusedWindow(d)); //get the current processes id of the window we are focused into

  if (currentWindowPid != gamePid) { return; } //dont read, write, or get the space key if we aren't focused into the game

  bool onGround = 0;
  Memory::Read(gamePid, isOnGround, &onGround, sizeof(bool));
  
  if (onGround == true && isKeyDown(d, XK_space) == true) {
    Memory::Write(gamePid, dwForceJump, &BHOP::DOJUMP, sizeof(int));
  }
  else {
    Memory::Write(gamePid, dwForceJump, &BHOP::DONTJUMP, sizeof(int));
  }

  return;
}
