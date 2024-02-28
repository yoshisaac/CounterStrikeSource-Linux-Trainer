#include "bhop.hpp"

#include <thread>

#include "../xutil.hpp"
#include "../memory.hpp"

void bhop(pid_t gamePid, Display* d, unsigned int onGround, unsigned int dwForceJump) {
  for (;;) {
    pid_t currentWindowPid = getPidByWindow(d, getFocusedWindow(d)); //get the current processes id of the window we are focused into

    if (currentWindowPid != gamePid) { continue; } //dont read, write, or get key states if we aren't focused into the game

    bool isOnGround = 0;
    Memory::Read(gamePid, onGround, &isOnGround, sizeof(bool));
  
    if (isOnGround == true && isKeyDown(d, XK_space) == true) {
      Memory::Write(gamePid, dwForceJump, &BHOP::DOJUMP, sizeof(int));
    }
    else {
      Memory::Write(gamePid, dwForceJump, &BHOP::DONTJUMP, sizeof(int));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
