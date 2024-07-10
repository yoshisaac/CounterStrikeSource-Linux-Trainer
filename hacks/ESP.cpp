#include "ESP.hpp"

#include <cmath>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdbe.h>

#include "playerInfo.hpp"
#include "../engine/engine.hpp"

#include "../GUI/config.hpp"

#include "../math.hpp"
#include "../xutil.hpp"

void esp(pid_t gamePid, XdbeBackBuffer back_buffer, Display* espDisplay, Window window) {
  GC gc;

  for (;;) {    
    gc = XCreateGC(espDisplay, window, 0, 0);
    XSetBackground(espDisplay, gc, ESP::white.pixel);

    db_clear(back_buffer, espDisplay, gc);

    
    if (getPidByWindow(espDisplay, getFocusedWindow(espDisplay)) != gamePid || !config->ESP) { //ESP is off
      db_swap_buffers(espDisplay, window);
      XFreeGC(espDisplay, gc);
      usleep(1000*1000/300);
      continue;
    }

    for(int i = 0; i < 32; ++i) {
      Player player = getPlayerByIndex(i);
      Player p_local = getLocalPlayer();

      if (player.isDead == true || player.health <= 0) continue;
      //if (player.dormant == true) continue;
      if (p_local.team == player.team) continue;
      
      float out[2];
      if (WorldToScreen(gamePid, player.absLocation, out)) {      
	
        float distance = distanceFormula3D(p_local.absLocation, player.absLocation);

	if (i == 0) distance == 100.f;
	
	float getFeet[3] = {player.absLocation[0], player.absLocation[1], player.absLocation[2]};
	getFeet[2] += player.height + 10;
	float screenFeet[2];
        WorldToScreen(gamePid, getFeet, screenFeet); //get feet height
	
	float topY = out[1];
	float bottomY = screenFeet[1];

	//Helped with box spacing: https://www.unknowncheats.me/forum/c-and-c-/76713-esp-box-size-calculation.html

	//text screen offset
	player.absLocation[2] += player.height + 14;
	float screenText[2];
	WorldToScreen(gamePid, player.absLocation, screenText);

	if (config->ESPname) {
	  //name shadow
	  XSetFont(espDisplay, gc, ESP::shadowfont->fid);
	  XSetForeground(espDisplay, gc, ESP::black.pixel);
	  XDrawString(espDisplay, back_buffer, gc, out[0] + 1, screenText[1] + 1, player.name.c_str(), strlen(player.name.c_str()));
	  //name
	  XSetFont(espDisplay, gc, ESP::font->fid);
	  XSetForeground(espDisplay, gc, ESP::white.pixel);
	  XDrawString(espDisplay, back_buffer, gc, out[0], screenText[1], player.name.c_str(), strlen(player.name.c_str()));
	}

	
	if (config->ESPbox) {
	  //box shadow
	  XSetForeground(espDisplay, gc, ESP::black.pixel);
	  db_thickline(back_buffer, espDisplay, gc, out[0] - (9800/distance), topY, out[0] - (9800/distance), bottomY, 4, distance);
	  db_thickline(back_buffer, espDisplay, gc, out[0] + (9800/distance), topY, out[0] + (9800/distance), bottomY, 4, distance);
	  db_thickline(back_buffer, espDisplay, gc, out[0] + (9800/distance), topY, out[0] - (9800/distance), topY, 4, distance);
	  db_thickline(back_buffer, espDisplay, gc, out[0] - (9800/distance), bottomY, out[0] + (9800/distance), bottomY, 4, distance);
	  //box
	  /*
	    switch (team) {
	    case 2:
	    XSetForeground(espDisplay, gc, ESP::tColor.pixel);
	    break;
	    case 3:
	    XSetForeground(espDisplay, gc, ESP::ctColor.pixel);
	    break;
	    default:
	    XSetForeground(espDisplay, gc, ESP::white.pixel);
	    break;
	    }
	  */

	  if (player.dormant == true)
	    XSetForeground(espDisplay, gc, ESP::gray.pixel);
	  else
	    XSetForeground(espDisplay, gc, ESP::tColor.pixel);

	  db_thickline(back_buffer, espDisplay, gc, (out[0] - (9800/distance)), topY, (out[0] - (9800/distance)), bottomY, 2, distance);
	  db_thickline(back_buffer, espDisplay, gc, (out[0] + (9800/distance)), topY, (out[0] + (9800/distance)), bottomY, 2, distance);
	  db_thickline(back_buffer, espDisplay, gc, out[0] + (9800/distance), topY, out[0] - (9800/distance), topY, 2, distance);
	  db_thickline(back_buffer, espDisplay, gc, out[0] - (9800/distance), bottomY, out[0] + (9800/distance), bottomY, 2, distance);
	}

	if (config->ESPhealthbar) {
	  //health bar shadow
	  XSetForeground(espDisplay, gc, ESP::black.pixel);
	  db_thickline(back_buffer, espDisplay, gc, out[0] - (11500/distance), topY, out[0] - (11500/distance), bottomY, 4, distance);
	  //health bar color
	  if (player.health >= 90)
	    XSetForeground(espDisplay, gc, ESP::green.pixel);
	  else if (player.health < 90 && player.health > 60)
	    XSetForeground(espDisplay, gc, ESP::yellow.pixel);
	  else if (player.health <= 60 && player.health > 35)
	    XSetForeground(espDisplay, gc, ESP::orange.pixel);
	  else if (player.health <= 35)
	    XSetForeground(espDisplay, gc, ESP::red.pixel);
	  //health bar
	  db_thickline(back_buffer, espDisplay, gc, out[0] - (11500/distance), topY, out[0] - (11500/distance), bottomY, 2, distance);
	}

	if (config->ESPhealthtext) {
	  //health text shadow
	  XSetFont(espDisplay, gc, ESP::shadowfont->fid);
	  XSetForeground(espDisplay, gc, ESP::black.pixel);
	  XDrawString(espDisplay, back_buffer, gc, (out[0] - (11500/distance)) + 1, screenText[1] + 1, std::to_string(player.health).c_str(), strlen(std::to_string(player.health).c_str()));
	  //health text
	  if (player.health >= 90)
	    XSetForeground(espDisplay, gc, ESP::green.pixel);
	  else if (player.health < 90 && player.health > 60)
	    XSetForeground(espDisplay, gc, ESP::yellow.pixel);
	  else if (player.health <= 60 && player.health > 35)
	    XSetForeground(espDisplay, gc, ESP::orange.pixel);
	  else if (player.health <= 35)
	    XSetForeground(espDisplay, gc, ESP::red.pixel);
	  XSetFont(espDisplay, gc, ESP::font->fid);
	  XDrawString(espDisplay, back_buffer, gc, out[0] - (11500/distance), screenText[1], std::to_string(player.health).c_str(), strlen(std::to_string(player.health).c_str()));
	}

      }
      
    }
    
    db_swap_buffers(espDisplay, window);

    XFreeGC(espDisplay, gc);
    
    usleep(1000*1000/300);
  }
}
