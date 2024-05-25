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
#include "../vector.hpp"
#include "../math.hpp"
#include "../xutil.hpp"

#include "../engine/engine.hpp"
#include "../client/client.hpp"

#include "ESP.hpp"

//https://github.com/ALittlePatate/CSS-external/blob/b17e083a4f0d0e4406d49d55c9c761cedab1ad66/ImGuiExternal/Memory.h#L61
float vmatrix[4][4];
bool WorldToScreen(pid_t gamePid, const float vIn[3], float vOut[2])
{
  Memory::Read(gamePid, ENGINE::viewMatrix, &vmatrix, sizeof(vmatrix));

  float w = vmatrix[3][0] * vIn[0] + vmatrix[3][1] * vIn[1] + vmatrix[3][2] * vIn[2] + vmatrix[3][3];

  if (w < 0.01f)
    return false;

  float vOutTmp[2];
  
  vOutTmp[0] = vmatrix[0][0] * vIn[0] + vmatrix[0][1] * vIn[1] + vmatrix[0][2] * vIn[2] + vmatrix[0][3];
  vOutTmp[1] = vmatrix[1][0] * vIn[0] + vmatrix[1][1] * vIn[1] + vmatrix[1][2] * vIn[2] + vmatrix[1][3];
  float invw = 1.0f / w;

  vOutTmp[0] *= invw;
  vOutTmp[1] *= invw;

  int width = ENGINE::screenX;
  int height = ENGINE::screenY;

  float x = width / 2.0f;
  float y = height / 2.0f;

  x += 0.5f * vOutTmp[0] * width + 0.5f;
  y -= 0.5f * vOutTmp[1] * height + 0.5f;
  
  vOut[0] = std::floor(x * 100.f) / 100.f;
  vOut[1] = std::floor(y * 100.f) / 100.f;

  return true;
}

float pLocalLocation[3] = {0, 0, 0};
int pLocalTeam = 0;
int aimIndex = 0;
void players(pid_t gamePid, XdbeBackBuffer back_buffer, Display* d, Window win, unsigned int playerList) {
  GC gc;
  XGCValues gcv;

  if (!ESP::font || !ESP::shadowfont) {
    std::cout << "font not found, aborting." << std::endl;
    return;
  }

  gc = XCreateGC(d, win, 0, 0);
  XSetBackground(d, gc, ESP::white.pixel);

  db_clear(back_buffer, d, win, gc);
  
  playerList += 0x28; //I don't know, my playerlist ptr is most likely wrong, but it works. For the most part...
    for (int i = 0; i < 32; i++) {
      unsigned int player = playerList + (i * 0x140);

      int index = -1;
      Memory::Read(gamePid, player, &index, sizeof(int));
      if (index == -1) continue; //read failed
      if (index == 0 && i > 0) continue; //index out of bounds of how many players exist

      std::string name = "";
      for (int h = 0; h < 256; h++) {
	char currentCharacter;
	Memory::Read(gamePid, player + playerOffset::name + h, &currentCharacter, sizeof(char));

	if (currentCharacter == '\0') { break; } //strings in c/c++ are terminated via a null ascii character

	name += currentCharacter;
      }
      
      if (name == ENGINE::pLocalName) {
	Memory::Read(gamePid, player + playerOffset::x, &pLocalLocation, sizeof(float[3]));
	Memory::Read(gamePid, player + playerOffset::team, &pLocalTeam, sizeof(int));
	ENGINE::pLocalIndex = index;
      }
      if (pLocalLocation[0] == 0 && pLocalLocation[1] == 0 && pLocalLocation[2] == 0) continue;

      float color[4];
      int team = -1;
      Memory::Read(gamePid, player + playerOffset::team, &team, sizeof(int));
      if (team == pLocalTeam && index != ENGINE::pLocalIndex) continue; //dont render team mates
      
      int health = 0;
      Memory::Read(gamePid, player + playerOffset::health, &health, sizeof(int));
      if (health <= 0) continue; //if they are dead


      float pitch = 0;
      Memory::Read(gamePid, player + playerOffset::pitch, &pitch, sizeof(float));

      float yaw = 0;
      Memory::Read(gamePid, player + playerOffset::yaw, &yaw, sizeof(float));

      float roll = 0; //we might have to worry about this later
      
      float viewAngle[3] = {pitch, yaw, roll};
      
      float location[3];
      Memory::Read(gamePid, player + playerOffset::x, &location, sizeof(float[3]));
      if (location[0] == 0 && location[1] == 0 && location[2] == 0) continue; //here is that "for the most part" kicking in.
                                                                            //This is a check for if the player is even in the server, because players that leave still stay in the list.
      float out[2];                                                       //Why tf does this happen? God do I know.

      location[2] += 8;

      if (WorldToScreen(gamePid, location, out) && name != ENGINE::pLocalName /*local player check*/) {      
	
        float distance = distanceFormula3D(pLocalLocation, location);

	if (name == ENGINE::pLocalName) distance = 100;

	
	float getFeet[3] = {location[0], location[1], location[2]};
	getFeet[2] -= 78;
	float screenFeet[2];
	if (!WorldToScreen(gamePid, getFeet, screenFeet)) continue; //get feet height
	
	float topY = out[1];
	float bottomY = screenFeet[1];

	//Helped with box spacing: https://www.unknowncheats.me/forum/c-and-c-/76713-esp-box-size-calculation.html

	//text screen offset
	location[2] += 3;
	float screenText[2];
	WorldToScreen(gamePid, location, screenText);

	//background shadow for text and ESP
	//text shadow
	XSetFont(d, gc, ESP::shadowfont->fid);
	XSetForeground(d, gc, ESP::black.pixel);
	//name shadow
	XDrawString(d, back_buffer, gc, out[0] + 1, screenText[1] + 1, name.c_str(), strlen(name.c_str()));
	//health number shadow
	XDrawString(d, back_buffer, gc, (out[0] - (11500/distance)) + 1, screenText[1] + 1, std::to_string(health).c_str(), strlen(std::to_string(health).c_str()));
	//health bar shadow
	db_thickline(back_buffer, d, gc, out[0] - (11500/distance), topY, out[0] - (11500/distance), bottomY, 4, distance, true);
	//box shadow
	db_thickline(back_buffer, d, gc, out[0] - (9800/distance), topY, out[0] - (9800/distance), bottomY, 4, distance, true);
	db_thickline(back_buffer, d, gc, out[0] + (9800/distance), topY, out[0] + (9800/distance), bottomY, 4, distance, true);
	db_thickline(back_buffer, d, gc, out[0] + (9800/distance), topY, out[0] - (9800/distance), topY, 4, distance, true);
	db_thickline(back_buffer, d, gc, out[0] - (9800/distance), bottomY, out[0] + (9800/distance), bottomY, 4, distance, true);

	XSetFont(d, gc, ESP::font->fid);
	
	//Name
	XSetForeground(d, gc, ESP::white.pixel);
	XDrawString(d, back_buffer, gc, out[0], screenText[1], name.c_str(), strlen(name.c_str()));

	//BOX
	/*
	switch (team) {
	case 2:
	  XSetForeground(d, gc, tColor.pixel);
	  break;
	case 3:
	  XSetForeground(d, gc, ctColor.pixel);
	  break;
	default:
	  XSetForeground(d, gc, white.pixel);
	  break;
	}
	*/
	XSetForeground(d, gc, ESP::tColor.pixel);
	db_thickline(back_buffer, d, gc, (out[0] - (9800/distance)), topY, (out[0] - (9800/distance)), bottomY, 2, distance);
	db_thickline(back_buffer, d, gc, (out[0] + (9800/distance)), topY, (out[0] + (9800/distance)), bottomY, 2, distance);
	db_thickline(back_buffer, d, gc, out[0] + (9800/distance), topY, out[0] - (9800/distance), topY, 2, distance);
	db_thickline(back_buffer, d, gc, out[0] - (9800/distance), bottomY, out[0] + (9800/distance), bottomY, 2, distance);

	//Health indicators
	if (health >= 90)
	  XSetForeground(d, gc, ESP::green.pixel);
	else if (health < 90 && health > 60)
	  XSetForeground(d, gc, ESP::yellow.pixel);
	else if (health < 60 && health > 35)
	  XSetForeground(d, gc, ESP::orange.pixel);
	else if (health <= 35)
	  XSetForeground(d, gc, ESP::red.pixel);

	db_thickline(back_buffer, d, gc, out[0] - (11500/distance), topY, out[0] - (11500/distance), bottomY, 2, distance);

	XDrawString(d, back_buffer, gc, out[0] - (11500/distance), screenText[1], std::to_string(health).c_str(), strlen(std::to_string(health).c_str()));
	
	//aim
	//some awful code. Proof of concept. Need the bone matrix to actually aim at their head.
	float plocal_v[3];
	Memory::Read(gamePid, ENGINE::pLocalPitch, &plocal_v[0], sizeof(float));
	Memory::Read(gamePid, ENGINE::pLocalYaw, &plocal_v[1], sizeof(float));
	plocal_v[2] = 0;

	if (aimIndex == index) {
	  if (isKeyDown(d, XK_Alt_L)) {
	    location[2] -= 10; //aim head area
	  }

	  float deltaLocation[3] = { float(pLocalLocation[0] - location[0]),
				     float(pLocalLocation[1] - location[1]),
				     float(pLocalLocation[2] - location[2]) };

	  float hyp = sqrt(deltaLocation[0] * deltaLocation[0] + deltaLocation[1] * deltaLocation[1]);

	  plocal_v[0] = (float)(atan(deltaLocation[2] / hyp) * radpi);
	  plocal_v[1] = (float)(atan(deltaLocation[1] / deltaLocation[0]) * radpi);
	
	  if (deltaLocation[0] >= 0.0f) {
	    plocal_v[1] += 180.0f;
	  }

	  while (plocal_v[0] > 89)
	    plocal_v[0] -= 180;

	  while (plocal_v[0] < -89)
	    plocal_v[0] += 180;

	  while (plocal_v[1] > 180)
	    plocal_v[1] -= 360;

	  while (plocal_v[1] < -180)
	    plocal_v[1] += 360;
	}

	
	float screencenter[2] = {ENGINE::screenX/2, ENGINE::screenY/2};

        if (isKeyDown(d, XK_Alt_L)) {
	  Memory::Write(gamePid, ENGINE::pLocalPitch, &plocal_v[0], sizeof(float));
	  Memory::Write(gamePid, ENGINE::pLocalYaw, &plocal_v[1], sizeof(float));   
	} else {
	  if (distanceFormula2D(screencenter, out) <= 50)
	    aimIndex = index;
	}

      }
	
      
    }

    db_swap_buffers(d, win);

    XFreeGC(d, gc);
}
