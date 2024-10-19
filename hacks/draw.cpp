#include "draw.hpp"

#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdbe.h>

#include "ESP.hpp"
#include "crosshair.hpp"
#include "fovCircle.hpp"

#include "../xutil.hpp"

#include "../engine/engine.hpp"

void draw(pid_t gamePid, XdbeBackBuffer back_buffer, Display* drawDisplay, Window window) {
  GC gc;
  for (;;) {
    gc = XCreateGC(drawDisplay, window, 0, 0); //allocate memory for the graphics context
    db_clear(back_buffer, drawDisplay, gc); //clear the current back buffer

    //hack features
    esp(gamePid, back_buffer, drawDisplay, window, gc);
    crosshair(gamePid, back_buffer, drawDisplay, window, gc);
    fovCircle(gamePid, back_buffer, drawDisplay, window, gc);

    //move drawing window if needed
    XWindowAttributes gameAttr = getWindowAttributesFromPid(drawDisplay, gamePid);
    if (ENGINE::screenXpos != gameAttr.x || ENGINE::screenYpos != gameAttr.y) {
      XMoveWindow(drawDisplay, window, gameAttr.x, gameAttr.y);
      ENGINE::screenXpos = gameAttr.x;
      ENGINE::screenYpos = gameAttr.y;
    }
    
    db_swap_buffers(drawDisplay, window); //swap the back buffer
    XFreeGC(drawDisplay, gc); //free the memory we allocated for the graphics context
    usleep(1000*1000/300); //frame rate
  }
}
