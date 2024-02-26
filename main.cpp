//g++ main.cpp hacks/bhop.cpp hacks/playerInfo.cpp -o hack -L/usr/X11/lib -lX11 -lXfixes -lGL -lGLU -lstdc++ -Wno-int-to-pointer-cast

#include <stdio.h>
#include <string>
#include <thread>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <ctype.h>
#include <climits>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <vector>
#include <cstring>
#include <cstddef>
#include <algorithm>
#include <cctype>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "hacks/bhop.hpp"
#include "hacks/playerInfo.hpp"

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

pid_t GetProcessByName(std::string strName) {
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
  
  return 0;
}

int main() {
  if (getuid()) { //check if we are root or not
    printf("Please run as root\nExample: \"sudo ./hack\"\n");
    return 0;
  }
  
  pid_t gamePid = GetProcessByName("hl2_linux");
  if (gamePid == -1) { // check if we successfully got the game processes id
    printf("Please open the game\n");
    return 0;
  }

  Display* d = XOpenDisplay(NULL);

  int screen = DefaultScreen(d);

  XVisualInfo* visual_info;
  static int visual_attribs[] = { GLX_RGBA, None };
  visual_info = glXChooseVisual(d, screen, visual_attribs);
  if (!visual_info) {
    // Handle error
  }

  XVisualInfo vinfo;
  XMatchVisualInfo(d, DefaultScreen(d), 32, TrueColor, &vinfo);

  XSetWindowAttributes attr;
  attr.colormap = XCreateColormap(d, DefaultRootWindow(d), vinfo.visual, AllocNone);
  attr.border_pixel = 0;
  attr.background_pixel = 0;
  attr.override_redirect = True;

  int gameX = 0, gameY = 0;
  //getWindowPosition(d, getWindowByPid(d, gamePid), gameX, gameY);

  Window win = XCreateWindow(d, DefaultRootWindow(d), gameX, gameY, 1366, 768, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);
  XSelectInput(d, win, ButtonPressMask | ButtonReleaseMask);

  GC gc = XCreateGC(d, win, 0, 0);

  GLXContext gl_context = glXCreateContext(d, visual_info, NULL, GL_TRUE);
  glXMakeCurrent(d, win, gl_context);


  Atom wm_delete_window = XInternAtom(d, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(d, win, &wm_delete_window, 1);

  Atom always_on_top = XInternAtom(d, "_NET_WM_STATE_ABOVE", False);
  XChangeProperty(d, win, XInternAtom(d, "_NET_WM_STATE", False),
  		  XA_ATOM, 32, PropModeReplace, (unsigned char *)&always_on_top, 1);

  glViewport(0, 0, 1366, 768); // Set the viewport size
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0f, (GLfloat)1366 / (GLfloat)768, 0.1f, 100.0f); // Set the projection matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLXContext context = glXCreateContext(d, visual_info, NULL, GL_TRUE);
  glXMakeCurrent(d, win, context);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  XMapWindow(d, win);

  XGrabPointer(d, win, False, ButtonPressMask | ButtonReleaseMask,
	       GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

  unsigned int ClientObject = Memory::getModuleBaseAddress(gamePid, "bin/client.so");
  unsigned int EngineObject = Memory::getModuleBaseAddress(gamePid, "bin/engine.so");
  
  unsigned int hl2_linux = Memory::getModuleBaseAddress(gamePid, "hl2_linux");

  unsigned int playerListPtr = ClientObject + 0xBE9380;
  unsigned int playerList = -1;

  unsigned int viewMatrix = EngineObject + 0xC7213C;
  
  unsigned int dwForceJump = ClientObject + 0xBEE4E8;
  unsigned int isOnGround = ClientObject + 0xB9E650;

  /*
  unsigned int test = FindPatternEx(gamePid, "bin/client.so", "00 00 00 00 00 20 41 0a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ??");
  
  if (test == 0) {
    printf("failed\n");
  }
  else {
    std::cout << std::hex << test << '\n';
  }
  */

  //coords for testing
  float oglX = (2.0 * ((1366/2)-20)) / 1366 - 1.0; //top left
  float oglY = 1.0 - (2.0 * ((768/2)+30)) / 768; //top left
  
  float oglX2 = (2.0 * ((1366/2)-20)) / 1366 - 1.0; //bottom left
  float oglY2 = 1.0 - (2.0 * ((768/2)-30)) / 768; //bottom left
  
  float oglX3 = (2.0 * ((1366/2)+20)) / 1366 - 1.0; //top right
  float oglY3 = 1.0 - (2.0 * ((768/2)+30)) / 768; //top right

  float oglX4 = (2.0 * ((1366/2)+20)) / 1366 - 1.0; //bottom right
  float oglY4 = 1.0 - (2.0 * ((768/2)-30)) / 768; //bottom right

  printf("Ready\n");
  for (;;) {
    Memory::Read(gamePid, playerListPtr, &playerList, sizeof(unsigned int));
    //bhop(gamePid, d, isOnGround, dwForceJump);
    printPlayers(gamePid, d, win, playerList, viewMatrix);
  }
  
  return 0;
}
