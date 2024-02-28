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

#include "../engine/engine.hpp"


//https://github.com/ALittlePatate/CSS-external/blob/b17e083a4f0d0e4406d49d55c9c761cedab1ad66/ImGuiExternal/Memory.h#L61
float vmatrix[4][4];
bool WorldToScreen(pid_t gamePid, const Vector3& vIn, Vector2& vOut, unsigned int viewMatrix)
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

  int width = ENGINE::screenX;
  int height = ENGINE::screenY;

  float x = width / 2.0f;
  float y = height / 2.0f;

  x += 0.5f * vOut.x * width + 0.5f;
  y -= 0.5f * vOut.y * height + 0.5f;

  vOut.x = x;
  vOut.y = y;

  return true;
}

void players(pid_t gamePid, Display* d, Window win, unsigned int playerList, unsigned int viewMatrix) {

  playerList += 0x28; //I don't know, my playerlist ptr is most likely wrong but it works, for the most part...

    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    int teamOfLocalPlayer = 0;
    Memory::Read(gamePid, playerList + playerOffset::team, &teamOfLocalPlayer, sizeof(int)); //Remember now, the first index of the playerlist is the local player
    for (int i = 0; i < 32; i++) {
      unsigned int player = playerList + (i * 0x140);

      float color[4];
      int team = -1;
      Memory::Read(gamePid, player + playerOffset::team, &team, sizeof(int));
      //if (team == teamOfLocalPlayer) continue; //dont render team mates
      switch (team) {
      case 2:
	color[0] = 0.506f;
	color[1] = 0.243f;
	color[2] = 0.259f;
	break;
      case 3:
	color[0] = 0.333f;
	color[1] = 0.431f;
	color[2] = 0.506f;
	break;
      }
      
      int index = -1;
      Memory::Read(gamePid, player, &index, sizeof(int));
      if (index == -1) continue; //read failed
      if (i > 0 && index == 0) continue; //index out of bounds of how many players exist

      /*
      float dormant;
      Memory::Read(gamePid, player + playerOffset::dormant, &dormant, sizeof(float));
      if (dormant == -4.0f) continue;
      */
      
      int health = 0;
      Memory::Read(gamePid, player + playerOffset::health, &health, sizeof(int));
      if (health <= 0) continue; //if they are dead

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
      //This is a check for if the player is even in the server, because players that leave still stay in the list.
      Vector2 out;                                                       //Why tf does this happen? God do I know.
    
      std::string name = "";
      for (int h = 0; h < 256; h++) {
	char currentCharacter;
	Memory::Read(gamePid, player + playerOffset::name + h, &currentCharacter, sizeof(char));

	if (currentCharacter == '\0') { break; } //strings in c/c++ are terminated via a null character

	name += currentCharacter;
      }

      if (WorldToScreen(gamePid, location, out, viewMatrix)) {      
	// Convert screen coordinates to OpenGL coordinates

	Vector3 getFeet = location;
	getFeet.z -= 73;
	Vector2 screenFeet;
	WorldToScreen(gamePid, getFeet, screenFeet, viewMatrix); //get feet

	Vector3 rightSideOffset = location;
	rightSideOffset.z += 30;
	Vector2 screenRightOffset;
	WorldToScreen(gamePid, rightSideOffset, screenRightOffset, viewMatrix);
      
	//line
	float rightX = (2.0 * ((screenRightOffset.x))) / ENGINE::screenX - 1.0; //head
	float topY = 1.0 - (2.0 * ((out.y))) / ENGINE::screenY; //head
      
	Vector3 leftSideOffset = location;
	leftSideOffset.z += 30;
	Vector2 screenLeftOffset;
	WorldToScreen(gamePid, leftSideOffset, screenLeftOffset, viewMatrix);
      
	float leftX = (2.0 * ((screenLeftOffset.x))) / ENGINE::screenX - 1.0;
	float bottomY = 1.0 - (2.0 * ((screenFeet.y))) / ENGINE::screenY;

    
	//std::cout << "Name: " << name << '\n'
	//<< "Index: " << index << '\n'
	//<< "Health: " << health << '\n'
	//std::cout << "Pitch: " << pitch << '\n';
	//std::cout << "Yaw: " << yaw << '\n';
	//<< "Coords: " << location.x << ' ' << location.y << ' ' << location.z << '\n'
	//<< "WorldToScreen: " << WorldToScreen(gamePid, location, out, viewMatrix) << "\n\n";

	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glColor4f(color[0], color[1], color[2], 1);
	glVertex2f(leftX, topY);
	glVertex2f(leftX, bottomY);
	glEnd();

	/*
	  glLineWidth(3);
	  glBegin(GL_LINE_LOOP);
	  glColor4f(1, 1, 1, 1);
	  glVertex2f(rightX, topY);
	  glVertex2f(rightX, bottomY);
	  glEnd();
	*/
      
      }
    }

  
    glXSwapBuffers(d, win); // Swap buffers
  
}
