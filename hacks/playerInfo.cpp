#include "playerInfo.hpp"
#include "player/player.hpp"

#include <iostream>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../memory.hpp"
#include "../vector.hpp"


//https://github.com/ALittlePatate/CSS-external/blob/master/ImGuiExternal/Memory.h#L61
float vmatrix[4][4];
bool WorldToScreen(pid_t gamePid, const Vector3& vIn, Vector3& vOut, unsigned int viewMatrix)
{
  Memory::Read(gamePid, viewMatrix, &vmatrix, sizeof(vmatrix));

  float w = vmatrix[3][0] * vIn.x + vmatrix[3][1] * vIn.y + vmatrix[3][2] * vIn.z + vmatrix[3][3];

  if (w < 0.01f)
    return false;

  vOut.x = vmatrix[0][0] * vIn.x + vmatrix[0][1] * vIn.y + vmatrix[0][2] * vIn.z + vmatrix[0][3];
  vOut.y = vmatrix[1][0] * vIn.x + vmatrix[1][1] * vIn.y + vmatrix[1][2] * vIn.z + vmatrix[1][3];
  float invw = 1.0f / w;

  vOut.x *= invw;
  vOut.y *= invw;

  int width = 1366;
  int height = 768;

  float x = width / 2.0f;
  float y = height / 2.0f;

  x += 0.5f * vOut.x * width + 0.5f;
  y -= 0.5f * vOut.y * height + 0.5f;

  vOut.x = x;
  vOut.y = y;

  return true;
}

void printPlayers(pid_t gamePid, Display* d, Window win, unsigned int playerList, unsigned int viewMatrix) {

  playerList += 0x28; //I don't know, my playerlist ptr is most likely wrong but it works, for the most part... 
  
  for (int i = 0; i < 32; i++) {
    unsigned int player = playerList + (i * 0x140);
    
    int index = -1;
    Memory::Read(gamePid, player, &index, sizeof(int));
    if (index == -1) { continue; } //read failed
    if (i > 0 && index == 0) { continue; } //index out of bounds of how many players exist

    int health = 0;
    Memory::Read(gamePid, player + playerOffset::health, &health, sizeof(int));

    /*
    float pitch = 0;
    Memory::Read(gamePid, player + playerOffset::pitch, &pitch, sizeof(float));

    float yaw = 0;
    Memory::Read(gamePid, player + playerOffset::yaw, &yaw, sizeof(float));
    */
    
    //This needs to be optimized to a single Read call
    Vector3 location;
    Memory::Read(gamePid, player + playerOffset::x, &location.x, sizeof(float));
    Memory::Read(gamePid, player + playerOffset::y, &location.y, sizeof(float));
    Memory::Read(gamePid, player + playerOffset::z, &location.z, sizeof(float));

    if (location.x == 0 && location.y == 0 && location.z == 0) continue; //here is that "for the most part" kicking in.
                                                                        //This is a check for if the player is even in the server (for now)
    Vector3 out;                                                       //Why tf does this happen? God do I know.

    std::string name = "";
    for (int h = 0; h < 256; h++) {
      char currentCharacter;
      Memory::Read(gamePid, player + playerOffset::name + h, &currentCharacter, sizeof(char));

      if (currentCharacter == '\0') { break; } //strings in c/c++ are terminated via a null character

      name += currentCharacter;
    }

    // Convert screen coordinates to OpenGL coordinates
    float oglX = (2.0 * ((out.x)-20)) / 1366 - 1.0; //top left
    float oglY = 1.0 - (2.0 * ((out.y)+30)) / 768; //top left
  
    float oglX2 = (2.0 * ((out.x)-20)) / 1366 - 1.0; //bottom left
    float oglY2 = 1.0 - (2.0 * ((out.y)-30)) / 768; //bottom left
  
    float oglX3 = (2.0 * ((out.x)+20)) / 1366 - 1.0; //top right
    float oglY3 = 1.0 - (2.0 * ((out.y)+30)) / 768; //top right

    float oglX4 = (2.0 * ((out.x)+20)) / 1366 - 1.0; //bottom right
    float oglY4 = 1.0 - (2.0 * ((out.y)-30)) / 768; //bottom right

    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Draw a square
    glBegin(GL_QUAD_STRIP);
    //glColor3f(1.0f, 0.0f, 0.0f); // Set color to red
    glVertex2f(oglX, oglY);
    //glColor3f(0.0f, 1.0f, 0.0f); // Set color to green
    glVertex2f(oglX2, oglY2);
    //glColor3f(0.0f, 0.0f, 1.0f); // Set color to blue
    glVertex2f(oglX3, oglY3);
    glVertex2f(oglX4, oglY4);
    glEnd();

    glXSwapBuffers(d, win); // Swap buffers
    
    std::cout << "Name: " << name << '\n'
    << "Index: " << index << '\n'
    << "Health: " << health << '\n'
    //std::cout << "Pitch: " << pitch << '\n';
    //std::cout << "Yaw: " << yaw << '\n';
    << "Coords: " << location.x << ' ' << location.y << ' ' << location.z << '\n'
    << "WorldToScreen: " << WorldToScreen(gamePid, location, out, viewMatrix) << "\n\n";
  }
  
}
