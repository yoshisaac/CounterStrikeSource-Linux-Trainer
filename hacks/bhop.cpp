#include "bhop.hpp"

#include "playerInfo.hpp"

#include <linux/uinput.h>
#include <thread>

#include "../xutil.hpp"
#include "../memory.hpp"
#include "../uinput.hpp"

#include "../client/client.hpp"

#include "../GUI/config.hpp"

void bhop(pid_t gamePid, Display* d, int dev_uinput) {
  for (;;) {

    if (getPidByWindow(d, getFocusedWindow(d)) != gamePid) { continue; } //dont read, write, or get key states if we aren't focused into the game
    
    if (!config->BHOP) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue; 
    }

    Player p_local = getLocalPlayer();

    if (p_local.flags & FL_ONGROUND && isKeyDown(d, XK_space) == true) {
      if (config->ROmode == false)
        Memory::Write(gamePid, CLIENT::dwForceJump, &CLIENT::DO, sizeof(int));
      else {
	/*
	emit(dev_uinput, EV_KEY, KEY_SPACE, 1);
	emit(dev_uinput, EV_SYN, SYN_REPORT, 0);
	emit(dev_uinput, EV_KEY, KEY_SPACE, 0);
	emit(dev_uinput, EV_SYN, SYN_REPORT, 0);
	*/
      }
    }
    else {
      if (config->ROmode == false)
	Memory::Write(gamePid, CLIENT::dwForceJump, &CLIENT::DONT, sizeof(int));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
