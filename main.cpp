//g++ main.cpp hacks/bhop.cpp hacks/playerInfo.cpp client/client.cpp -o hack -L/usr/X11/lib -lX11 -lXfixes -lGL -lGLU -lstdc++ -Wno-int-to-pointer-cast

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
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "hacks/bhop.hpp"
#include "hacks/playerInfo.hpp"
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
  const unsigned int ClientObject = Memory::getModuleBaseAddress(gamePid, "bin/client.so");
  const unsigned int EngineObject = Memory::getModuleBaseAddress(gamePid, "bin/engine.so");
  const unsigned int hl2_linux = Memory::getModuleBaseAddress(gamePid, "hl2_linux");

  const unsigned int screenXLength = EngineObject + 0xD20014;
  Memory::Read(gamePid, screenXLength, &ENGINE::screenX, sizeof(int));
  
  const unsigned int screenYLength = EngineObject + 0xD20018;
  Memory::Read(gamePid, screenYLength, &ENGINE::screenY, sizeof(int));
  
  unsigned int playerList = -1;
  Memory::Read(gamePid, ClientObject + 0xBE9380, &playerList, sizeof(unsigned int));

  const unsigned int viewMatrix = EngineObject + 0xC7213C;
  
  const unsigned int dwForceJump = ClientObject + 0xBEE4E8;
  const unsigned int dwForceAttack1 = ClientObject + 0xBEE578;
  const unsigned int dwForceAttack2 = ClientObject + 0xBEE4C8;
  
  const unsigned int onGround = ClientObject + 0xB9E650;

  /* beginning of X and OpenGL initiation*/
  Display* d = XOpenDisplay(NULL);
  Display* clientDisplay = XOpenDisplay(NULL);
  Display* bhopDisplay = XOpenDisplay(NULL);

  int screen = DefaultScreen(d);

  Window root = DefaultRootWindow(d);

  XVisualInfo* visual_info;
  static int visual_attribs[] = { GLX_RGBA, None };
  visual_info = glXChooseVisual(d, screen, visual_attribs);
  if (!visual_info) {
    printf("Failed it initiate visual_info\n");
    return 1;
  }

  XVisualInfo vinfo;
  XMatchVisualInfo(d, DefaultScreen(d), 32, TrueColor, &vinfo);

  XSetWindowAttributes attr;
  attr.colormap = XCreateColormap(d, DefaultRootWindow(d), vinfo.visual, AllocNone);
  attr.border_pixel = 0;
  attr.background_pixel = 0;
  attr.override_redirect = True;

  int gameX = 0, gameY = 0;
  Window gameWin = waitUntilPidIsFocus(d, gamePid);
  getWindowPosition(d, gameWin, gameX, gameY); //This will not work on some window managers, including but not limited to Mutter
  
  Window win = XCreateWindow(d, root, gameX, gameY, ENGINE::screenX, ENGINE::screenY, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);
  XSelectInput(d, win, NoEventMask);

  XserverRegion region = XFixesCreateRegion (d, NULL, 0);

  XFixesSetWindowShapeRegion (d, win, ShapeBounding, 0, 0, 0);
  XFixesSetWindowShapeRegion (d, win, ShapeInput, 0, 0, region);

  XFixesDestroyRegion (d, region);

  GC gc = XCreateGC(d, win, 0, 0);

  GLXContext gl_context = glXCreateContext(d, visual_info, NULL, GL_TRUE);
  glXMakeCurrent(d, win, gl_context);


  Atom wm_delete_window = XInternAtom(d, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(d, win, &wm_delete_window, 1);

  Atom always_on_top = XInternAtom(d, "_NET_WM_STATE_ABOVE", False);
  XChangeProperty(d, win, XInternAtom(d, "_NET_WM_STATE", False),
  		  XA_ATOM, 32, PropModeReplace, (unsigned char *)&always_on_top, 1);

  glViewport(0, 0, ENGINE::screenX, ENGINE::screenY); // Set the viewport size
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0f, (GLfloat)ENGINE::screenX / (GLfloat)ENGINE::screenY, 0.1f, 100.0f); // Set the projection matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLXContext context = glXCreateContext(d, visual_info, NULL, GL_TRUE);
  glXMakeCurrent(d, win, context);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  XMapWindow(d, win);

  XGrabPointer(d, win, False, ButtonPressMask | ButtonReleaseMask,
	       GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

  XInitThreads();
  /* End of X and OpenGL initiation */



  
  //Client fixes thread
  std::thread clientThread(client, gamePid, clientDisplay, dwForceAttack1, dwForceAttack2);
  pthread_setname_np(clientThread.native_handle(), "clientThread");

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
  //OpenGL can only have calls from a single thread
  //But we want the iterator and drawer on seperate threads
  //And this will be accomplished soon, but OpenGL will just always sit on the main thread
  for (;;) {
    
    
    players(gamePid, d, win, playerList, viewMatrix); 
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return 0;
}
