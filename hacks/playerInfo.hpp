#include <sys/types.h>
#include <string>
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

#include "../client/client.hpp"

namespace playerOffset {
  inline uintptr_t health = 0x84;
  //inline uintptr_t name = 0x10; //first ascii char
  inline uintptr_t viewAngles = 0x25C;
  inline uintptr_t absLocation = 0x250;
  inline uintptr_t team = 0x8C; //3 is CT, 2 is T
  inline uintptr_t isDead = 0x83;
  inline uintptr_t height = 0x1C0;
  inline uintptr_t dormant = 0x16E;
};

class Player {
public:
  int index;
  int health;
  std::string name;
  float viewAngles[2];
  float absLocation[3];
  int team;
  bool isDead;
  float height;
  bool dormant;

  Player() {
    this->index = -1;
    this->health = 0;
    this->name = "";
    this->viewAngles[0] = 0;
    this->viewAngles[1] = 0;
    this->absLocation[0] = 0;
    this->absLocation[1] = 0;
    this->absLocation[2] = 0;
    this->team = -1;
    this->isDead = true;
    this->height = 0;
    this->dormant = true;
  }
  
  Player(int index, int health, std::string name,
	 float viewAngles[2], float absLocation[3],
	 int team, bool isDead, float height,
	 bool dormant) {

    this->index = index;
    this->health = health;
    this->name = name;
    this->viewAngles[0] = viewAngles[0];
    this->viewAngles[1] = viewAngles[1];
    this->absLocation[0] = absLocation[0];
    this->absLocation[1] = absLocation[1];
    this->absLocation[2] = absLocation[2];
    this->team = team;
    this->isDead = isDead;
    this->height = height;
    this->dormant = dormant;
  }
};

namespace playerInfo {
  inline Player l_players[32];
};

inline Player getPlayerByIndex(unsigned int i) {
  if (i > 32) return Player();
  else return playerInfo::l_players[i];
}

inline Player getLocalPlayer(void) {
  return playerInfo::l_players[CLIENT::pLocalIndex];
}

void players(pid_t);

//playerlist math
//the playerlist is a list of pointers
//player pointers are 0x10 appart from each other
