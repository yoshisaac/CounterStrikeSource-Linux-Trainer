#include "playerInfo.hpp"

#include <iostream>
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
#include <cmath>
#include <cfenv>
#include <thread>
#include <stdlib.h>

#include "../memory.hpp"
#include "../math.hpp"
#include "../xutil.hpp"

#include "../engine/engine.hpp"
#include "../client/client.hpp"

void players(pid_t gamePid) {

  for (int i = 0; i < 32; i++) {
    uintptr_t player;
    Memory::Read(gamePid, CLIENT::playerList + (i * 0x10), &player, sizeof(uintptr_t));
    uintptr_t localPlayerAddr;
    Memory::Read(gamePid, CLIENT::localPlayer, &localPlayerAddr, sizeof(uintptr_t));
    if (player == NULL) {
      playerInfo::l_players[i] = Player();
      continue;
    }

    if (player == localPlayerAddr)
      CLIENT::pLocalIndex = i;

    int team = -1;
    Memory::Read(gamePid, player + playerOffset::team, &team, sizeof(int));

    int health = 0;
    Memory::Read(gamePid, player + playerOffset::health, &health, sizeof(int));

    float viewAngle[3];
    Memory::Read(gamePid, player + playerOffset::viewAngles, &viewAngle, sizeof(float[3]));

    float location[3];
    Memory::Read(gamePid, player + playerOffset::absLocation, &location, sizeof(float[3]));

    bool isDead = false;
    Memory::Read(gamePid, player + playerOffset::isDead, &isDead, sizeof(bool));

    float height = 0;
    Memory::Read(gamePid, player + playerOffset::height, &height, sizeof(float));

    bool dormant = false;
    Memory::Read(gamePid, player + playerOffset::dormant, &dormant, sizeof(bool));
 
    playerInfo::l_players[i] = Player(i, health, "", viewAngle, location, team, isDead, height, dormant);

   }
}
