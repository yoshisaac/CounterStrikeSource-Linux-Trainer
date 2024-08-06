#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdbe.h>

namespace DRAW {
  inline XFontStruct* shadowfont;
  inline XFontStruct* font;

  inline XColor red;
  inline XColor orange;
  inline XColor black;
  inline XColor white;
  inline XColor green;
  inline XColor yellow;
  inline XColor tColor;
  inline XColor ctColor;
  inline XColor cyan;
  inline XColor gray;
}

void draw(pid_t, XdbeBackBuffer, Display*, Window);
