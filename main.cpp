
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

#include "hacks/bhop.hpp"
#include "hacks/playerInfo.hpp"
#include "hacks/ESP.hpp"
#include "client/client.hpp"
#include "engine/engine.hpp"

#include "memory.hpp"
#include "xutil.hpp"

//Reads the data of a file
std::string ReadFile(std::string filePath, bool firstLine=true) {
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
    std::string strComm{ ReadFile("/proc/" + strProcID + "/comm") };


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
    printf("Please run as root\nExample: \"sudo ./hack\"\n");
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

  const uintptr_t screenXLength = EngineObject + 0xD20014;
  Memory::Read(gamePid, screenXLength, &ENGINE::screenX, sizeof(int));

  const uintptr_t screenYLength = EngineObject + 0xD20018;
  Memory::Read(gamePid, screenYLength, &ENGINE::screenY, sizeof(int));

  uintptr_t playerList = -1;
  Memory::Read(gamePid, ClientObject + 0xBE9380, &playerList, sizeof(uintptr_t));

  const uintptr_t viewMatrix = EngineObject + 0xC7213C;

  const uintptr_t dwForceJump = ClientObject + 0xBEE4E8;
  const uintptr_t dwForceAttack1 = ClientObject + 0xBEE578;
  const uintptr_t dwForceAttack2 = ClientObject + 0xBEE4C8;

  const uintptr_t onGround = ClientObject + 0xB9E650;

  std::cout << "Client.so: " << std::hex << ClientObject << '\n';
  std::cout << "Engine.so: " << std::hex << EngineObject << '\n';
  std::cout << "hl2_linux: " << std::hex << hl2_linux << '\n';
  std::cout << "viewMatrix: " << std::hex << viewMatrix << '\n';
  std::cout << "playerList: " << std::hex << playerList << '\n';

  //Dumb bodge I put in for myself. Can't find the local player pointer.
  std::string tmp = "";
  std::cout << "Please input your steam name: ";
  std::cin >> tmp;

  if (tmp == ",")
    ENGINE::pLocalName = "DoctorC";
  else
    ENGINE::pLocalName = tmp;

  //https://gist.github.com/ericek111/774a1661be69387de846f5f5a5977a46 great piece of source code. 
  /* beginning of X initiation*/
  Display* d = XOpenDisplay(NULL);
  Display* clientDisplay = XOpenDisplay(NULL);
  Display* bhopDisplay = XOpenDisplay(NULL);

  if (!d || !clientDisplay || !bhopDisplay) {
    printf("Please run startx/xinit\n");
    return 1;
  }

  int screen = DefaultScreen(d);

  int shape_event_base;
  int shape_error_base;

  if (!XShapeQueryExtension (d, &shape_event_base, &shape_error_base)) {
    printf("NO shape extension in your system !\n");
    return 1;
  }

  XSetWindowAttributes wattr;
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

  XShapeCombineMask(d, window, ShapeInput, 0, 0, None, ShapeSet );

#define SHAPE_MASK ShapeNotifyMask
  XShapeSelectInput (d, window, SHAPE_MASK );

  //wattr.override_redirect = 1;
  //XChangeWindowAttributes(d, window, CWOverrideRedirect, &wattr);

  XserverRegion region = XFixesCreateRegion (d, NULL, 0);
  XFixesSetWindowShapeRegion (d, window, ShapeInput, 0, 0, region);
  XFixesDestroyRegion (d, region);

  XdbeBackBuffer back_buffer = XdbeAllocateBackBufferName(d, window, 0);

  XMapWindow(d, window);

  ESP::shadowfont = XLoadQueryFont(d, "6x13bold");
  ESP::font = XLoadQueryFont(d, "6x13");

  ESP::red = createXColorFromRGB(255, 0, 0, d, DefaultScreen(d));
  ESP::black = createXColorFromRGB(0, 0, 0, d, DefaultScreen(d));
  ESP::white = createXColorFromRGB(255, 255, 255, d, DefaultScreen(d));
  ESP::green = createXColorFromRGB(0, 255, 0, d, DefaultScreen(d));
  ESP::yellow = createXColorFromRGB(255, 255, 0, d, DefaultScreen(d));
  ESP::tColor = createXColorFromRGB(230, 35, 35, d, DefaultScreen(d));
  ESP::ctColor = createXColorFromRGB(148, 196, 248, d, DefaultScreen(d));
  ESP::cyan = createXColorFromRGB(11, 192, 212, d, DefaultScreen(d));
  /* end of X initiation */




  //Is not needed anymore until something like triggerbot or aimbot is added
  //Client fixes thread
  //std::thread clientThread(client, gamePid, clientDisplay, dwForceAttack1, dwForceAttack2);
  //pthread_setname_np(clientThread.native_handle(), "clientThread");

  //bhop thread
  std::thread bhopThread(bhop, gamePid, bhopDisplay, onGround, dwForceJump);
  pthread_setname_np(bhopThread.native_handle(), "bhopThread");

  printf("Ready\n");
  printf("The Free and Open Source no-name GNU CS:S cheat, made with GNU Emacs, for your GNU operating system.\n");
  printf("    ,           ,   \n");
  printf("   /             \\ \n");
  printf("  ((__-^^-,-^^-__)) \n");
  printf("   `-_---\' `---_-\' \n");
  printf("    `--|o` \'o|--\'   \n"); //GNU ascii :D
  printf("       \\  `  /        \n");
  printf("        ): :(         \n");
  printf("        :o_o:         \n");
  printf("         \"-\"         \n");
  //Drawer/player iterator thread
  for (;;) {
    if (isKeyDown(d, XK_Delete)) { XCloseDisplay(d); } //close the program with a segfault!!!!!!!!1
    players(gamePid, back_buffer, d, window, playerList, viewMatrix);
    usleep(100*100/300); //my game runs at 300fps. Need to make this dynamic so it updates properly for slower screens
  }

  return 0;
}
