#include "ESP.hpp"

#include "draw.hpp"

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

void esp(pid_t gamePid, XdbeBackBuffer back_buffer, Display* drawDisplay, Window window, GC gc) {

  XSetBackground(drawDisplay, gc, DRAW::white.pixel);

    
  if (getPidByWindow(drawDisplay, getFocusedWindow(drawDisplay)) != gamePid || !config->ESP) //ESP is off/disable ESP/stop rendering
    return;

  for(int i = 0; i < 64; ++i) {
    Player player = getPlayerByIndex(i);
    Player p_local = getLocalPlayer();

    if (player.isDead == true || player.health <= 0) continue;
    if (p_local.team == player.team) continue;
      
    float out[2];
    if (WorldToScreen(gamePid, player.absLocation, out)) {      
	
      float distance = distanceFormula3D(p_local.absLocation, player.absLocation);
	
      float getHead[3] = {player.absLocation[0], player.absLocation[1], player.absLocation[2]};
      getHead[2] += player.height + 10;
      float screenHead[2];
      WorldToScreen(gamePid, getHead, screenHead); //get feet height
	
      float bottomY = out[1];
      float topY = screenHead[1];

      //Helped with box spacing: https://www.unknowncheats.me/forum/c-and-c-/76713-esp-box-size-calculation.html

      //text screen offset
      player.absLocation[2] += player.height + 14;
      float screenText[2];
      WorldToScreen(gamePid, player.absLocation, screenText);

      XColor dormant_color;
      if (player.dormant == true) {
	playerInfo::l_players[i].dormant_frames += 1;

	if (player.dormant_alpha-5 > 0 && player.dormant_frames%15 == 0)
	  playerInfo::l_players[i].dormant_alpha -= 5;

	if (player.dormant_alpha-5 <= 55)
	  continue;
	      
	dormant_color = createXColorFromRGB(player.dormant_alpha,
					    player.dormant_alpha,
					    player.dormant_alpha,
					    drawDisplay, DefaultScreen(drawDisplay));
      }	    
      else {
	playerInfo::l_players[i].dormant_frames = 0;
	playerInfo::l_players[i].dormant_alpha = 220;
      }
	
      if (config->ESPbox) {
	//box shadow
	if (player.dormant == false) {
	  XSetForeground(drawDisplay, gc, DRAW::black.pixel);
	  db_thickline(back_buffer, drawDisplay, gc, out[0] - (9800/distance), topY, out[0] - (9800/distance), bottomY, 4, distance, 0.66f);
	  db_thickline(back_buffer, drawDisplay, gc, out[0] + (9800/distance), topY, out[0] + (9800/distance), bottomY, 4, distance, 0.66f);
	  db_thickline(back_buffer, drawDisplay, gc, out[0] + (9800/distance) - 2, topY, out[0] - (9800/distance) + 2, topY, 4, distance, 0.66f);
	  db_thickline(back_buffer, drawDisplay, gc, out[0] - (9800/distance) - 2, bottomY, out[0] + (9800/distance) + 2, bottomY, 4, distance, 0.66f);
	}
	//box
	/*
	  switch (team) {
	  case 2:
	  XSetForeground(drawDisplay, gc, DRAW::tColor.pixel);
	  break;
	  case 3:
	  XSetForeground(drawDisplay, gc, DRAW::ctColor.pixel);
	  break;
	  default:
	  XSetForeground(drawDisplay, gc, DRAW::white.pixel);
	  break;
	  }
	*/

	if (player.dormant == true)
	  XSetForeground(drawDisplay, gc, dormant_color.pixel);
	else
	  XSetForeground(drawDisplay, gc, createXColorFromRGB(config->ESPboxcolor[0],
							     config->ESPboxcolor[1],
							     config->ESPboxcolor[2],
							     drawDisplay, DefaultScreen(drawDisplay)).pixel);
	  
	db_thickline(back_buffer, drawDisplay, gc, (out[0] - (9800/distance)), topY, (out[0] - (9800/distance)), bottomY, 2, distance, 0.30f);
	db_thickline(back_buffer, drawDisplay, gc, (out[0] + (9800/distance)), topY, (out[0] + (9800/distance)), bottomY, 2, distance, 0.30f);
	db_thickline(back_buffer, drawDisplay, gc, out[0] + (9800/distance), topY, out[0] - (9800/distance), topY, 2, distance, 0.30f);
	db_thickline(back_buffer, drawDisplay, gc, out[0] - (9800/distance), bottomY, out[0] + (9800/distance), bottomY, 2, distance, 0.30f);
      }
	
      if (config->ESPname) {
	//name shadow
	XSetFont(drawDisplay, gc, DRAW::shadowfont->fid);
	XSetForeground(drawDisplay, gc, DRAW::black.pixel);
	XDrawString(drawDisplay, back_buffer, gc, out[0] - (11500/distance) + 20 + 1, screenText[1] + 1, player.name.c_str(), strlen(player.name.c_str()));
	//name
	XSetFont(drawDisplay, gc, DRAW::font->fid);
	XSetForeground(drawDisplay, gc, DRAW::white.pixel);
	XDrawString(drawDisplay, back_buffer, gc, out[0] - (11500/distance) + 20, screenText[1], player.name.c_str(), strlen(player.name.c_str()));
      }	


      if (config->ESPhealthbar) {
	//health bar shadow
	XSetForeground(drawDisplay, gc, DRAW::black.pixel);
	db_thickline(back_buffer, drawDisplay, gc, out[0] - (11500/distance), topY-2, out[0] - (11500/distance), bottomY+2, 4, distance, 0.650f);

	int ydelta = (topY - bottomY) * (1 - (player.health / 100.f));
	
	//health bar color
	if (player.health > 100) {
	  XSetForeground(drawDisplay, gc, DRAW::cyan.pixel);
	  ydelta = 0;
	}
	else if (player.health <= 100 && player.health >= 90)
	  XSetForeground(drawDisplay, gc, DRAW::green.pixel);
	else if (player.health < 90 && player.health > 60)
	  XSetForeground(drawDisplay, gc, DRAW::yellow.pixel);
	else if (player.health <= 60 && player.health > 35)
	  XSetForeground(drawDisplay, gc, DRAW::orange.pixel);
	else if (player.health <= 35)
	  XSetForeground(drawDisplay, gc, DRAW::red.pixel);

	  
	//health bar
	db_thickline(back_buffer, drawDisplay, gc, out[0] - (11500/distance), bottomY, out[0] - (11500/distance), topY - ydelta, 2, distance, 0.20f);
      }

      if (config->ESPhealthtext) {
	//health text shadow
	XSetFont(drawDisplay, gc, DRAW::shadowfont->fid);
	XSetForeground(drawDisplay, gc, DRAW::black.pixel);
	XDrawString(drawDisplay, back_buffer, gc, (out[0] - (11500/distance)) + 1, screenText[1] + 1, std::to_string(player.health).c_str(), strlen(std::to_string(player.health).c_str()));
	//health text
	if (player.health >= 90)
	  XSetForeground(drawDisplay, gc, DRAW::green.pixel);
	else if (player.health < 90 && player.health > 60)
	  XSetForeground(drawDisplay, gc, DRAW::yellow.pixel);
	else if (player.health <= 60 && player.health > 35)
	  XSetForeground(drawDisplay, gc, DRAW::orange.pixel);
	else if (player.health <= 35)
	  XSetForeground(drawDisplay, gc, DRAW::red.pixel);
	XSetFont(drawDisplay, gc, DRAW::font->fid);
	XDrawString(drawDisplay, back_buffer, gc, out[0] - (11500/distance), screenText[1], std::to_string(player.health).c_str(), strlen(std::to_string(player.health).c_str()));
      }

      //skeleton debug
      // XSetForeground(drawDisplay, gc, DRAW::white.pixel);
      // float last_bone[3];
      // for (int i = 0; i < 45; ++i) {
      //   //float bone[3] = {*player.boneMatrix[i]};

      //   float bone_screen[2];
      //   if (!WorldToScreen(gamePid, player.boneMatrix[i], bone_screen)) continue;
	  
      //   XDrawString(drawDisplay, back_buffer, gc, bone_screen[0], bone_screen[1], std::to_string(i).c_str(), strlen(std::to_string(i).c_str()));
	  
      //   //last_bone = {*bone};
      // }


      if (player.dormant == true)
	XSetForeground(drawDisplay, gc, dormant_color.pixel);
      else
	XSetForeground(drawDisplay, gc, createXColorFromRGB(config->ESPskeletoncolor[0],
							   config->ESPskeletoncolor[1],
							   config->ESPskeletoncolor[2],
							   drawDisplay, DefaultScreen(drawDisplay)).pixel);
	
      //ideally, there would be a loop for this. Works, but awful
      float bone_screen0[2], bone_screen1[2], bone_screen2[2], bone_screen3[2], bone_screen4[2];
      float bone_screen5[2], bone_screen6[2], bone_screen7[2], bone_screen8[2], bone_screen9[2];
      float bone_screen10[2], bone_screen11[2], bone_screen12[2], bone_screen13[2], bone_screen14[2];
      float bone_screen15[2], bone_screen16[2], bone_screen17[2], bone_screen43[2], bone_screen18[2];
      float bone_screen28[2], bone_screen29[2], bone_screen30[2], bone_screen44[2], bone_screen31[2];
      if (config->ESPskeleton) {
	if (WorldToScreen(gamePid, player.boneMatrix[0], bone_screen0) && WorldToScreen(gamePid, player.boneMatrix[1], bone_screen1) &&
	    WorldToScreen(gamePid, player.boneMatrix[2], bone_screen2) && WorldToScreen(gamePid, player.boneMatrix[3], bone_screen3) &&
	    WorldToScreen(gamePid, player.boneMatrix[4], bone_screen4) && WorldToScreen(gamePid, player.boneMatrix[5], bone_screen5) &&
	    WorldToScreen(gamePid, player.boneMatrix[6], bone_screen6) && WorldToScreen(gamePid, player.boneMatrix[7], bone_screen7) &&
	    WorldToScreen(gamePid, player.boneMatrix[8], bone_screen8) && WorldToScreen(gamePid, player.boneMatrix[9], bone_screen9) &&
	    WorldToScreen(gamePid, player.boneMatrix[10], bone_screen10) && WorldToScreen(gamePid, player.boneMatrix[11], bone_screen11) &&
	    WorldToScreen(gamePid, player.boneMatrix[12], bone_screen12) && WorldToScreen(gamePid, player.boneMatrix[13], bone_screen13) &&
	    WorldToScreen(gamePid, player.boneMatrix[14], bone_screen14) && WorldToScreen(gamePid, player.boneMatrix[15], bone_screen15) &&
	    WorldToScreen(gamePid, player.boneMatrix[16], bone_screen16) && WorldToScreen(gamePid, player.boneMatrix[17], bone_screen17) &&
	    WorldToScreen(gamePid, player.boneMatrix[43], bone_screen43) && WorldToScreen(gamePid, player.boneMatrix[18], bone_screen18) &&
	    WorldToScreen(gamePid, player.boneMatrix[28], bone_screen28) && WorldToScreen(gamePid, player.boneMatrix[29], bone_screen29) &&
	    WorldToScreen(gamePid, player.boneMatrix[30], bone_screen30) && WorldToScreen(gamePid, player.boneMatrix[44], bone_screen44) &&
	    WorldToScreen(gamePid, player.boneMatrix[31], bone_screen31) && (player.boneMatrix[0][0] != 0 && player.boneMatrix[0][1] != 0 && player.boneMatrix[0][2] != 0)) {
	  //left leg
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen0[0], bone_screen0[1], bone_screen1[0], bone_screen1[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen1[0], bone_screen1[1], bone_screen2[0], bone_screen2[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen2[0], bone_screen2[1], bone_screen3[0], bone_screen3[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen3[0], bone_screen3[1], bone_screen4[0], bone_screen4[1]);
	  
	  //right leg
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen0[0], bone_screen0[1], bone_screen5[0], bone_screen5[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen5[0], bone_screen5[1], bone_screen6[0], bone_screen6[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen6[0], bone_screen6[1], bone_screen7[0], bone_screen7[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen7[0], bone_screen7[1], bone_screen8[0], bone_screen8[1]);
	  
	  //back
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen0[0], bone_screen0[1], bone_screen9[0], bone_screen9[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen9[0], bone_screen9[1], bone_screen10[0], bone_screen10[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen10[0], bone_screen10[1], bone_screen11[0], bone_screen11[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen11[0], bone_screen11[1], bone_screen12[0], bone_screen12[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen12[0], bone_screen12[1], bone_screen13[0], bone_screen13[1]);
	  
	  //head/neck
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen13[0], bone_screen13[1], bone_screen14[0], bone_screen14[1]);
	  
	  //left arm
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen12[0], bone_screen12[1], bone_screen15[0], bone_screen15[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen15[0], bone_screen15[1], bone_screen16[0], bone_screen16[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen16[0], bone_screen16[1], bone_screen17[0], bone_screen17[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen17[0], bone_screen17[1], bone_screen43[0], bone_screen43[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen43[0], bone_screen43[1], bone_screen18[0], bone_screen18[1]);
	  
	  //rightarm
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen12[0], bone_screen12[1], bone_screen28[0], bone_screen28[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen28[0], bone_screen28[1], bone_screen29[0], bone_screen29[1]);
	  XDrawLine(drawDisplay, back_buffer, gc, bone_screen29[0], bone_screen29[1], bone_screen30[0], bone_screen30[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen30[0], bone_screen30[1], bone_screen44[0], bone_screen44[1]);
	  //XDrawLine(drawDisplay, back_buffer, gc, bone_screen44[0], bone_screen44[1], bone_screen31[0], bone_screen31[1]);
	}
      }
      player.boneMatrix[14][2] += 9;
      if (config->ESPdot && WorldToScreen(gamePid, player.boneMatrix[14], bone_screen14))
	XFillArc(drawDisplay, back_buffer, gc, bone_screen14[0] - ((9800/distance)/2), bone_screen14[1], (9500/distance), (9500/distance), 0, 360*64);
	
    }

  }
}
