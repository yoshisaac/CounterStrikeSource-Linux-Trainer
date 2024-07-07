#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/types.h>
#include <cstdint>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace CLIENT {
  inline static int DO = 5;
  inline static int DONT = 4;

  inline uintptr_t playerList;

  inline uintptr_t onGround;

  inline uintptr_t localPlayer;
  inline unsigned int pLocalIndex = 0;
  
  //networked variables
  inline uintptr_t dwForceJump;
  inline uintptr_t dwForceAttack1;
  inline uintptr_t dwForceAttack2;
};

#endif
