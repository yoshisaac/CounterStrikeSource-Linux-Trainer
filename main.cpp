#include <stdio.h>
#include <string>
#include <thread>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <climits>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <vector>
#include <cstring>
#include <cstddef>
#include <algorithm>
#include <cctype>
#include <linux/uinput.h>

#include "hacks/bhop.hpp"
#include "hacks/aimbot.hpp"
#include "hacks/playerInfo.hpp"
#include "hacks/ESP.hpp"
#include "hacks/draw.hpp"
#include "client/client.hpp"
#include "engine/engine.hpp"
#include "GUI/GUI.hpp"

#include "memory.hpp"
#include "xutil.hpp"

//Reads the data of a file
std::string readFile(std::string filePath, bool firstLine=true) {
  std::ifstream file;

  file.open(filePath, std::ifstream::in);

  char currentChar;
  std::string concat = "";

  while (file.get(currentChar)) {
    if (firstLine == true && currentChar == '\n') {break;}
    concat += currentChar;
  }

  file.close();

  return concat;
}

//https://www.unknowncheats.me/forum/general-programming-and-reversing/147140-linux-accessing-process-memory.html
pid_t getProcessByName(std::string strName) {
  if (strName.length() == 0)
    return -1;

  //Try to open the /proc/ directory
  DIR *pDir = opendir("/proc/");

  if (pDir == nullptr) { return -1; } // if that fails then exit

  dirent* pDirent = nullptr;

  //basically loop through all the processes
  while (pDirent = readdir(pDir), pDirent != nullptr) {

    //Skip non processes
    if (pDirent->d_type != DT_DIR) {
      continue;
    }

    //Basically contains the process ID
    std::string strProcID(pDirent->d_name);

    //Get the name of the processes
    std::string strComm{ readFile("/proc/" + strProcID + "/comm") };


    //Check if the currently iterated processes is the one we want
    if (strComm == strName) {
      pid_t pid = (pid_t) atoi(strProcID.c_str());
      return pid;
    }
  }

  return -1;
}

//Returns Address of the Signature
//Unfinished, doesn't work, would like some help.
/*
unsigned int findPattern(pid_t procId, std::string moduleName, std::string Pattern) {
  const unsigned int start = Memory::getModuleBaseAddress(procId, moduleName);
  const unsigned int end = (start + Memory::getModuleSize(procId, moduleName));

  int howManyBytes = 0;
  std::string concatTemp = "";
  for (int i = 0; i < Pattern.length(); i++) {
    if (Pattern[i] == ' ') {
      howManyBytes++;
      continue;
    }
    concatTemp += Pattern[i];
  }
  Pattern = concatTemp;


  bool found = true;
  unsigned int patternIter = 0;
  std::byte currentByte;
  std::byte currentByteFromPattern;
  for (unsigned int i = start; i <= end; i++) {
    found = true;
    patternIter = 0;
    for (unsigned int h = 0; h <= howManyBytes; h++) {
      Memory::Read(procId, i, &currentByte, sizeof(currentByte));

      std::string byte = "";
      byte += Pattern[patternIter];
      byte += Pattern[patternIter+1];

      if (byte == "??") {
	patternIter++;
	continue;
      }

      currentByteFromPattern = (std::byte) Memory::hexToInt( byte );

      std::cout << "currentByte: ";
      std::cout << std::hex << (int) currentByte << '\n';
      std::cout << "currentByteFromPattern: ";
      std::cout << std::hex << (int) currentByteFromPattern << '\n';
      std::cout << "patternIterator: " << patternIter << '\n';

      if (currentByte != currentByteFromPattern) {
	found = false;
	break;
      }
      patternIter++;
    }

    if (found == true) {
      return i;
    }

  }

  return 0; //failed, cant return negative numbers due to signed-ness
}
*/

int main() {
  if (getuid()) { //check if we are root or not
    printf("Please run as root\nExample: \"sudo ./cs-source-hack\"\n");
    return 1;
  }

  pid_t gamePid = getProcessByName("hl2_linux");
  if (gamePid == -1) { // check if we successfully got the game processes id
    printf("Please open the game\n");
    return 1;
  }

  //Get memory addresses/offsets
  const uintptr_t ClientObject = Memory::getModuleBaseAddress(gamePid, "bin/client.so");
  const uintptr_t EngineObject = Memory::getModuleBaseAddress(gamePid, "bin/engine.so");
  const uintptr_t hl2_linux = Memory::getModuleBaseAddress(gamePid, "hl2_linux");

  Memory::Read(gamePid, EngineObject + 0xD20014, &ENGINE::screenX, sizeof(int));
  Memory::Read(gamePid, EngineObject + 0xD20018, &ENGINE::screenY, sizeof(int));

  Memory::Read(gamePid, ClientObject + 0xBE9380, &CLIENT::radarList, sizeof(uintptr_t));
  
  CLIENT::playerList = ClientObject + 0xBA5FB4;
  CLIENT::localPlayer = ClientObject + 0xBD0750;
  
  ENGINE::viewMatrix = EngineObject + 0xC9B160;

  CLIENT::dwForceJump = ClientObject + 0xBEE4E8;
  CLIENT::dwForceAttack1 = ClientObject + 0xBEE578;
  CLIENT::dwForceAttack2 = ClientObject + 0xBEE4C8;

  CLIENT::onGround = ClientObject + 0xB9E650;

  CLIENT::cameraLocation = ClientObject + 0xBC3E08;
  
  ENGINE::pLocalYaw = EngineObject + 0xB3349C;
  ENGINE::pLocalPitch = EngineObject + 0xB33498;

  std::cout << "Client.so: " << std::hex << ClientObject << '\n';
  std::cout << "Engine.so: " << std::hex << EngineObject << '\n';
  std::cout << "hl2_linux: " << std::hex << hl2_linux << '\n';
  std::cout << "viewMatrix: " << std::hex << ENGINE::viewMatrix << '\n';
  std::cout << "playerList: " << std::hex << CLIENT::playerList << '\n';
  std::cout << "radarList: " << std::hex << CLIENT::radarList << '\n';
  std::cout << "cameraLocation: " << std::hex << CLIENT::cameraLocation << '\n';
  std::cout << "pLocalYaw: " << std::hex << ENGINE::pLocalYaw << '\n';
  std::cout << "pLocalPitch: " << std::hex << ENGINE::pLocalPitch << '\n';
  std::cout << "localPlayer: " << std::hex << CLIENT::localPlayer << '\n';
  printf("screenX: %d\n", ENGINE::screenX);
  printf("screenY: %d\n", ENGINE::screenY);

  //https://gist.github.com/ericek111/774a1661be69387de846f5f5a5977a46 great piece of black magic.
  /* beginning of X initiation*/
  Display* d = XOpenDisplay(NULL);
  Display* bhopDisplay = XOpenDisplay(NULL);
  Display* aimDisplay = XOpenDisplay(NULL);
  Display* drawDisplay = XOpenDisplay(NULL);

  Screen* s = DefaultScreenOfDisplay(d);

  printf("s->width %d\n", s->width);
  printf("s->height %d\n", s->height);

  if (s->width < ENGINE::screenX && s->height < ENGINE::screenY) { //bleh, in a nutshell: If you use over scanning on nvidia, then the game's resolution lies
    ENGINE::screenX = s->width;                                                                                            // and will be larger than the screen
    ENGINE::screenY = s->height;                                                                                            
  }

  if (!d) {
    printf("Please run startx/xinit\nIf you are running this program from SSH, it won't work.\n");
    return 1;
  }

  int screen = DefaultScreen(d);

  int shape_event_base;
  int shape_error_base;

  if (!XShapeQueryExtension (d, &shape_event_base, &shape_error_base)) {
    printf("NO shape extension in your system !\n");
    return 1;
  }

  XColor bgcolor = createXColorFromRGBA(0, 0, 0, 0, d, screen);

  Window root = DefaultRootWindow(d);

  Visual* visual = DefaultVisual(d, screen);

  XVisualInfo vinfo;
  XMatchVisualInfo(d, DefaultScreen(d), 32, TrueColor, &vinfo);
  Colormap colormap = XCreateColormap(d, DefaultRootWindow(d), vinfo.visual, AllocNone);

  XSetWindowAttributes attr;
  attr.background_pixmap = None;
  attr.background_pixel = bgcolor.pixel;
  attr.border_pixel=0;
  attr.win_gravity=NorthWestGravity;
  attr.bit_gravity=ForgetGravity;
  attr.save_under=1;
  attr.event_mask=BASIC_EVENT_MASK;
  attr.do_not_propagate_mask=NOT_PROPAGATE_MASK;
  attr.override_redirect=1; // OpenGL > 0
  attr.colormap = colormap;

  unsigned long mask = CWColormap | CWBorderPixel | CWBackPixel | CWEventMask | CWWinGravity|CWBitGravity | CWSaveUnder | CWDontPropagate | CWOverrideRedirect;

  Window window = XCreateWindow(d, root, 0, 0, ENGINE::screenX, ENGINE::screenY, 0, vinfo.depth, InputOutput, vinfo.visual, mask, &attr);

  XShapeCombineMask(d, window, ShapeInput, 0, 0, None, ShapeSet);

  XShapeSelectInput(d, window, ShapeNotifyMask);

  //wattr.override_redirect = 1;
  //XChangeWindowAttributes(d, window, CWOverrideRedirect, &wattr);

  XserverRegion region = XFixesCreateRegion (d, NULL, 0);
  XFixesSetWindowShapeRegion (d, window, ShapeInput, 0, 0, region);
  XFixesDestroyRegion (d, region);

  XdbeBackBuffer back_buffer = XdbeAllocateBackBufferName(d, window, 0);

  XMapWindow(d, window);

  DRAW::shadowfont = XLoadQueryFont(d, "6x13bold");
  DRAW::font = XLoadQueryFont(d, "6x13");

  if (!DRAW::font || !DRAW::shadowfont) {
    std::cout << "fonts not found, aborting." << std::endl;
    return 1;
  }

  //some color constants
  DRAW::red = createXColorFromRGB(255, 0, 0, d, DefaultScreen(d));
  DRAW::orange = createXColorFromRGB(255, 170, 0, d, DefaultScreen(d)); 
  DRAW::black = createXColorFromRGB(0, 0, 0, d, DefaultScreen(d));
  DRAW::white = createXColorFromRGB(255, 255, 255, d, DefaultScreen(d));
  DRAW::green = createXColorFromRGB(0, 255, 0, d, DefaultScreen(d));
  DRAW::yellow = createXColorFromRGB(255, 255, 0, d, DefaultScreen(d));
  DRAW::tColor = createXColorFromRGB(230, 35, 35, d, DefaultScreen(d));
  DRAW::ctColor = createXColorFromRGB(148, 196, 248, d, DefaultScreen(d));
  DRAW::cyan = createXColorFromRGB(0, 255, 240, d, DefaultScreen(d));
  DRAW::gray = createXColorFromRGBA(90, 90, 90, -1, d, DefaultScreen(d));
  DRAW::blue = createXColorFromRGB(38, 113, 252, d, DefaultScreen(d));
  /* end of X initiation */

  /* uinput initiation */
  //file handle
  int dev_uinput = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  //make a pretend USB input device
  struct uinput_setup usetup;
  ioctl(dev_uinput, UI_SET_EVBIT, EV_KEY);
  ioctl(dev_uinput, UI_SET_KEYBIT, KEY_SPACE);
  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x1234; /* sample vendor */
  usetup.id.product = 0x5678; /* sample product */
  strcpy(usetup.name, "input device CSS");
  ioctl(dev_uinput, UI_DEV_SETUP, &usetup);
  ioctl(dev_uinput, UI_DEV_CREATE);
  

  //configuration GUI thread
  pthread_t pGUI;
  pthread_create(&pGUI, NULL, guiThread, NULL);
  pthread_setname_np(pGUI, "guiThread");

  //bhop thread
  std::thread bhopThread(bhop, gamePid, bhopDisplay, dev_uinput);
  pthread_setname_np(bhopThread.native_handle(), "bhopThread");

  //aimbot thread
  std::thread aimbotThread(aimbot, gamePid, aimDisplay);
  pthread_setname_np(aimbotThread.native_handle(), "aimbotThread");

  //draw thread for esp and such
  std::thread drawThread(draw, gamePid, back_buffer, drawDisplay, window);
  pthread_setname_np(drawThread.native_handle(), "drawThread");
  
  printf("Ready\n");
  printf("The Free and Open Source no-name GNU CS:S cheat, made with GNU Emacs, for your GNU operating system.\n");
  printf("    ,           ,    \n");
  printf("   /             \\  \n");
  printf("  ((__-^^-,-^^-__))  \n");
  printf("   `-_---\' `---_-\' \n");
  printf("    `--|o` \'o|--\'  \n"); //GNU ascii :D
  printf("       \\  `  /      \n");
  printf("        ): :(        \n");
  printf("        :o_o:        \n");
  printf("         \"-\"       \n");
  //player iterator thread
  for (;;) {
    if (isKeyDown(d, XK_Delete)) {
      printf("Closing now");
      ioctl(dev_uinput, UI_DEV_DESTROY);
      close(dev_uinput);
      exit(0);
    }
    players(gamePid);
  }

  return 0;
}
