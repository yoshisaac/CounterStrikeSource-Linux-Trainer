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

#include "../GUI/config.hpp"

void players(pid_t gamePid) {

  for (int i = 0; i < 64; i++) {
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

    
    std::string name = "";
    for (int h = 0; h < 256; h++) {
      char currentCharacter;
      Memory::Read(gamePid, (CLIENT::radarList + 0x28 + (i * 0x140)) + 0x10 + h, &currentCharacter, sizeof(char));

      if (currentCharacter == '\0') { break; } //strings in c/c++ are terminated via a null ascii character

      name += currentCharacter;
    }

    int team = -1;
    Memory::Read(gamePid, player + playerOffset::team, &team, sizeof(int));

    int health = 0;
    Memory::Read(gamePid, player + playerOffset::health, &health, sizeof(int));

    int armor = 0;
    Memory::Read(gamePid, player + playerOffset::armor, &armor, sizeof(int));
    
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

    uintptr_t boneMatrixPtr; 
    Memory::Read(gamePid, player + playerOffset::boneMatrixPtr, &boneMatrixPtr, sizeof(uintptr_t));

    float boneMatrix[48][3];
    for (int i = 0; i < 48 && boneMatrixPtr != NULL; ++i) {
      uintptr_t bone = 0xC + boneMatrixPtr + (0x30 * i); // NO iterating past 48
      Memory::Read(gamePid, bone, &boneMatrix[i][0], sizeof(float));
      Memory::Read(gamePid, bone + 0x10, &boneMatrix[i][1], sizeof(float));
      Memory::Read(gamePid, bone + 0x10*2, &boneMatrix[i][2], sizeof(float));
    }

    float aimPunch[3];
    Memory::Read(gamePid, player + playerOffset::aimPunch, &aimPunch, sizeof(float[3]));

    int flags;
    Memory::Read(gamePid, player + playerOffset::flags, &flags, sizeof(int));    

    int fov;
    Memory::Read(gamePid, player + playerOffset::fov, &fov, sizeof(int));


    //https://www.unknowncheats.me/forum/counterstrike-global-offensive/207551-external-spectators-list.html
    uintptr_t spectatorTargetHandle;
    Memory::Read(gamePid, player + playerOffset::spectatorTarget, &spectatorTargetHandle, sizeof(uintptr_t));
    //https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/const.h#L79
    int spectatorTargetIndex = (spectatorTargetHandle & ENT_ENTRY_MASK) - 1; //convert ehandle to player list index with black magic

    int spectatorMode;
    Memory::Read(gamePid, player + playerOffset::spectatorMode, &spectatorMode, sizeof(int));
    
    playerInfo::l_players[i] = Player(i, health, name, viewAngle,
				      location, team, isDead, height,
				      dormant, boneMatrix, playerInfo::l_players[i].dormant_frames,
				      playerInfo::l_players[i].dormant_alpha,
				      aimPunch, flags, playerInfo::l_players[i].aimbotFov,
				      armor, fov, spectatorTargetIndex,
				      spectatorMode);

  }
  usleep(1000*1000/250);
}
