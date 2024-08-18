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
    if (getPidByWindow(d, getFocusedWindow(d)) != gamePid || !config->BHOP) {
      usleep(1000*1000/250); //at most, servers will run at a 128 ticks per second.
      continue; 
    }

    Player p_local = getLocalPlayer();
    int i = 6;
    if (p_local.flags & FL_ONGROUND && isKeyDown(d, XK_space) == true) {
      if (config->ROmode == false) {
        Memory::Write(gamePid, CLIENT::dwForceJump, &CLIENT::DO, sizeof(int));
      } else {
	/*
	emit(dev_uinput, EV_KEY, KEY_SPACE, 1);
	emit(dev_uinput, EV_SYN, SYN_REPORT, 0);
	emit(dev_uinput, EV_KEY, KEY_SPACE, 0);
	emit(dev_uinput, EV_SYN, SYN_REPORT, 0);
	*/
      }
    } else {
      Memory::Write(gamePid, CLIENT::dwForceJump, &CLIENT::DONT, sizeof(int));
    }
    
    usleep(1000*1000/250);
  }
}
