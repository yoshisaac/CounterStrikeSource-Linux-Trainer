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

#define FL_ONGROUND (1 << 0)

//https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/const.h#L79
#define ENT_ENTRY_MASK ((1<<12)-1)

#define SP_DEATH 1
#define SP_FIRST 4
#define SP_THIRD 5
#define SP_FREE  7

namespace playerOffset {
  inline constexpr uintptr_t health = 0x84;
  inline constexpr uintptr_t armor = 0x1478;
  inline constexpr uintptr_t viewAngles = 0x25C;
  inline constexpr uintptr_t absLocation = 0x250;
  inline constexpr uintptr_t vecOrigin = 0x328;
  inline constexpr uintptr_t team = 0x8C; //3 is CT, 2 is T
  inline constexpr uintptr_t isDead = 0x83;
  inline constexpr uintptr_t height = 0x1C0;
  inline constexpr uintptr_t dormant = 0x16E;
  inline constexpr uintptr_t flags = 0x340; // https://github.com/rdbo/cssmh-external/blob/master/cssmh/offsets.hpp#L19
  inline constexpr uintptr_t boneMatrixPtr = 0x810;
  inline constexpr uintptr_t aimPunch = 0xE28;
  inline constexpr uintptr_t fov = 0xFB4;
  inline constexpr uintptr_t fov_default = 0xFC0;
  inline constexpr uintptr_t spectatorTarget = 0x1020;
  inline constexpr uintptr_t spectatorMode = 0x101C;
  inline constexpr uintptr_t isDefusing = 0x13EC;
  inline constexpr uintptr_t money = 0x146C;
};

class Player {
public:
  int index;
  int health;
  int armor;
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
  int flags;
  float aimbotFov;
  int fov;
  int fov_default;
  int spectatorTargetIndex;
  int spectatorMode;
  bool isDefusing;
  
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
    this->flags = -1;
    this->aimbotFov = 9999;
    this->armor = 0;
    this->fov = 0;
    this->fov_default = 0;
    this->spectatorTargetIndex = 0;
    this->spectatorMode = -1;
    this->isDefusing = 0;
  }
  
  Player(int index, int health, std::string name,
	 float viewAngles[2], float absLocation[3],
	 int team, bool isDead, float height,
	 bool dormant, float boneMatrix[48][3],
	 int dormant_frames, short dormant_alpha,
	 float aimPunch[3], int flags, float aimbotFov,
	 int armor, int fov, int spectatorTargetIndex,
	 int spectatorMode, bool isDefusing,
	 int fov_default) {

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
    this->flags = flags;
    this->aimbotFov = aimbotFov;
    this->armor = armor;
    this->fov = fov;
    this->spectatorTargetIndex = spectatorTargetIndex;
    this->spectatorMode = spectatorMode;
    this->isDefusing = isDefusing;
    this->fov_default = fov_default;
  }
};

namespace playerInfo {
  inline Player l_players[64];
};

inline Player getPlayerByIndex(unsigned int i) {
  if (i > 64) return Player();
  else return playerInfo::l_players[i];
}

inline Player* getPlayerReferenceByIndex(unsigned int i) {
  if (i > 64) return nullptr;
  else return &playerInfo::l_players[i];
}

inline Player getLocalPlayer(void) {
  return playerInfo::l_players[CLIENT::pLocalIndex];
}

void players(pid_t);

//playerlist math
//the playerlist is a list of pointers
//player pointers are 0x10 appart from each other
