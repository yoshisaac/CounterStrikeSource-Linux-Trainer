#include "playerInfo.hpp"

#include <iostream>
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
#include <stdlib.h>
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
bool WorldToScreen(pid_t gamePid, const float vIn[3], float vOut[2], unsigned int viewMatrix)
{
  Memory::Read(gamePid, viewMatrix, &vmatrix, sizeof(vmatrix));

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
  
  vOut[0] = x;
  vOut[1] = y;

  return true;
}

float pLocalLocation[3] = {0, 0, 0};
int pLocalTeam = 0;
void players(pid_t gamePid, XdbeBackBuffer back_buffer, Display* d, Window win, unsigned int playerList, unsigned int viewMatrix) {
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

      std::string name = "";
      for (int h = 0; h < 256; h++) {
	char currentCharacter;
	Memory::Read(gamePid, player + playerOffset::name + h, &currentCharacter, sizeof(char));

	if (currentCharacter == '\0') { break; } //strings in c/c++ are terminated via a null ascii character

	name += currentCharacter;
      }
      
      if (name == ENGINE::pLocalName)
	Memory::Read(gamePid, player + playerOffset::x, &pLocalLocation, sizeof(float[3]));
      if (pLocalLocation[0] == 0 && pLocalLocation[1] == 0 && pLocalLocation[2] == 0) continue;

      
      if (name == ENGINE::pLocalName)
	Memory::Read(gamePid, player + playerOffset::team, &pLocalTeam, sizeof(int));

      float color[4];
      int team = -1;
      Memory::Read(gamePid, player + playerOffset::team, &team, sizeof(int));
      if (team == pLocalTeam) continue; //dont render team mates

      
      int index = -1;
      Memory::Read(gamePid, player, &index, sizeof(int));
      if (index == -1) continue; //read failed
      if (i > 0 && index == 0) continue; //index out of bounds of how many players exist
      
      int health = 0;
      Memory::Read(gamePid, player + playerOffset::health, &health, sizeof(int));
      if (health <= 0) continue; //if they are dead

      /*
	float pitch = 0;
	Memory::Read(gamePid, player + playerOffset::pitch, &pitch, sizeof(float));

	float yaw = 0;
	Memory::Read(gamePid, player + playerOffset::yaw, &yaw, sizeof(float));
      */
      
      float location[3];
      Memory::Read(gamePid, player + playerOffset::x, &location, sizeof(float[3]));
      if (location[0] == 0 && location[1] == 0 && location[2] == 0) continue; //here is that "for the most part" kicking in.
                                                                            //This is a check for if the player is even in the server, because players that leave still stay in the list.
      float out[2];                                                       //Why tf does this happen? God do I know.

      location[2] += 8;

      if (WorldToScreen(gamePid, location, out, viewMatrix) && name != ENGINE::pLocalName /*local player check*/) {      
	
        float distance = distanceFormula3D(pLocalLocation, location);

	float getFeet[3] = {location[0], location[1], location[2]};
	getFeet[2] -= 78;
	float screenFeet[2];
	if (!WorldToScreen(gamePid, getFeet, screenFeet, viewMatrix)) continue; //get feet height
	
	float topY = out[1];
	float bottomY = screenFeet[1];
      
	float leftSideOffset[3] = {location[0], location[1], location[2]};
	leftSideOffset[2] += 30;
	float screenLeftOffset[2];
	if (!WorldToScreen(gamePid, leftSideOffset, screenLeftOffset, viewMatrix)) continue;

	//Helped with box spacing: https://www.unknowncheats.me/forum/c-and-c-/76713-esp-box-size-calculation.html

	//text screen offset
	location[2] += 3;
	float screenText[2];
	WorldToScreen(gamePid, location, screenText, viewMatrix);

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
	if (health >= 85)
	  XSetForeground(d, gc, ESP::green.pixel);
	else if (health < 85 && health > 35)
	  XSetForeground(d, gc, ESP::yellow.pixel);
	else if (health <= 35 )
	  XSetForeground(d, gc, ESP::red.pixel);

	db_thickline(back_buffer, d, gc, out[0] - (11500/distance), topY, out[0] - (11500/distance), bottomY, 2, distance);

	XDrawString(d, back_buffer, gc, out[0] - (11500/distance), screenText[1], std::to_string(health).c_str(), strlen(std::to_string(health).c_str()));

	//head indicator
	XSetForeground(d, gc, ESP::cyan.pixel);
	XFillArc(d, back_buffer, gc, out[0] - ((9800/distance)/2), out[1], (9500/distance), (9500/distance), 0, 360*64);
      }
      
    }

    db_swap_buffers(d, win);

    XFreeGC(d, gc);
}
