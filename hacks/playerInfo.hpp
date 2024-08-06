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
  inline uintptr_t viewAngles = 0x25C;
  inline uintptr_t absLocation = 0x250;
  inline uintptr_t team = 0x8C; //3 is CT, 2 is T
  inline uintptr_t isDead = 0x83;
  inline uintptr_t height = 0x1C0;
  inline uintptr_t dormant = 0x16E;
  inline uintptr_t boneMatrixPtr = 0x810;
  inline uintptr_t aimPunch = 0xE28;
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
  int dormant_frames;
  short dormant_alpha;
  float boneMatrix[48][3];
  float aimPunch[3];

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
    this->boneMatrix;
    this->dormant_frames = 0;
    this->aimPunch[0] = 0;
    this->aimPunch[1] = 0;
    this->aimPunch[2] = 0;
    
  }
  
  Player(int index, int health, std::string name,
	 float viewAngles[2], float absLocation[3],
	 int team, bool isDead, float height,
	 bool dormant, float boneMatrix[48][3],
	 int dormant_frames, short dormant_alpha,
	 float aimPunch[3]) {

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

    for (int i = 0; i < 48; ++i)
      for (int h = 0; h < 3; ++h)
      this->boneMatrix[i][h] = boneMatrix[i][h];

    this->dormant_frames = dormant_frames;
    this->dormant_alpha = dormant_alpha;
    this->aimPunch[0] = aimPunch[0];
    this->aimPunch[1] = aimPunch[1];
    this->aimPunch[2] = aimPunch[2];
  }
};

namespace playerInfo {
  inline Player l_players[64];
};

inline Player getPlayerByIndex(unsigned int i) {
  if (i > 32) return Player();
  else return playerInfo::l_players[i];
}

inline Player* getPlayerReferenceByIndex(unsigned int i) {
  if (i > 32) return nullptr;
  else return &playerInfo::l_players[i];
}

inline Player getLocalPlayer(void) {
  return playerInfo::l_players[CLIENT::pLocalIndex];
}

void players(pid_t);

//playerlist math
//the playerlist is a list of pointers
//player pointers are 0x10 appart from each other
