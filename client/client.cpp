#include "client.hpp"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <thread>

#include "../xutil.hpp"
#include "../memory.hpp"

void client(pid_t gamePid, Display* d, unsigned int dwForceAttack) {
  for (;;) {
    pid_t currentWindowPid = getPidByWindow(d, getFocusedWindow(d)); //get the current processes id of the window we are focused into

    if (currentWindowPid != gamePid) { continue; } //dont read, write, or get key states if we aren't focused into the game
    
    if (isMouseDown(d, Button1Mask))
      Memory::Write(gamePid, dwForceAttack, &CLIENT::DOSHOOT, sizeof(int));
    else 
      Memory::Write(gamePid, dwForceAttack, &CLIENT::DONTSHOOT, sizeof(int));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
