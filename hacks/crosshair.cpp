#include "crosshair.hpp"

#include "draw.hpp"

#include "../engine/engine.hpp"
#include "../xutil.hpp"

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

#include "../GUI/config.hpp"

//https://github.com/8dcc/cs-source-external/blob/main/src/crosshair.c#L27
void crosshair(pid_t gamePid, XdbeBackBuffer back_buffer, Display* drawDisplay, Window window, GC gc) {

  int screen_center[2] = {ENGINE::screenX/2, ENGINE::screenY/2};

  Player p_local = getLocalPlayer();
  
  if (getPidByWindow(drawDisplay, getFocusedWindow(drawDisplay)) != gamePid)
    return;

  if (config->ESPcrosshair) {  
    XSetForeground(drawDisplay, gc, createXColorFromRGB(config->ESPcrosshaircolor[0],
							config->ESPcrosshaircolor[1],
							config->ESPcrosshaircolor[2],
							drawDisplay,
							DefaultScreen(drawDisplay)).pixel);
 
    XDrawLine(drawDisplay, back_buffer, gc, screen_center[0]-13, screen_center[1], screen_center[0]+14, screen_center[1]);
    XDrawLine(drawDisplay, back_buffer, gc, screen_center[0], screen_center[1]-13, screen_center[0], screen_center[1]+14);
  }

  if (config->ESPcrosshairRCS) {    
    XSetForeground(drawDisplay, gc, createXColorFromRGB(config->ESPcrosshairRCScolor[0],
							config->ESPcrosshairRCScolor[1],
							config->ESPcrosshairRCScolor[2],
							drawDisplay,
							DefaultScreen(drawDisplay)).pixel);
  
    XDrawLine(drawDisplay, back_buffer, gc,
	      (screen_center[0] - ((screen_center[0] / 90) * p_local.aimPunch[1])) - 13,
	      (screen_center[1] + ((screen_center[1] / 45) * p_local.aimPunch[0])),
	      (screen_center[0] - ((screen_center[0] / 90) * p_local.aimPunch[1])) + 14,
	      (screen_center[1] + ((screen_center[1] / 45) * p_local.aimPunch[0])));

    XDrawLine(drawDisplay, back_buffer, gc,
	      (screen_center[0] - ((screen_center[0] / 90) * p_local.aimPunch[1])),
	      (screen_center[1] + ((screen_center[1] / 45) * p_local.aimPunch[0])) - 13,
	      (screen_center[0] - ((screen_center[0] / 90) * p_local.aimPunch[1])),
	      (screen_center[1] + ((screen_center[1] / 45) * p_local.aimPunch[0])) + 14);
  }
  
  return;
}
