#include "playerInfo.hpp"

#include <iostream>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cmath>
#include <stdlib.h>
#include <cfenv>
#include <thread>

#include "../memory.hpp"
#include "../vector.hpp"
#include "../math.hpp"

#include "../engine/engine.hpp"

//https://github.com/ALittlePatate/CSS-external/blob/b17e083a4f0d0e4406d49d55c9c761cedab1ad66/ImGuiExternal/Memory.h#L61
float vmatrix[4][4];
bool WorldToScreen(pid_t gamePid, const float vIn[3], float vOut[2], unsigned int viewMatrix)
{
  Memory::Read(gamePid, viewMatrix, &vmatrix, sizeof(vmatrix));

  float w = vmatrix[3][0] * vIn[0] + vmatrix[3][1] * vIn[1] + vmatrix[3][2] * vIn[2] + vmatrix[3][3];

  if (w < 0.01f)
    return false;

  float vOutTmp[2];
  
  vOutTmp[0] = vmatrix[0][0] * vIn[0] + vmatrix[0][1] * vIn[1] + vmatrix[0][2] * vIn[2] + vmatrix[0][3];
  vOutTmp[1] = vmatrix[1][0] * vIn[0] + vmatrix[1][1] * vIn[1] + vmatrix[1][2] * vIn[2] + vmatrix[1][3];
  float invw = 1.0f / w;

  vOutTmp[0] *= invw;
  vOutTmp[1] *= invw;

  int width = ENGINE::screenX;
  int height = ENGINE::screenY;

  float x = width / 2.0f;
  float y = height / 2.0f;

  x += 0.5f * vOutTmp[0] * width + 0.5f;
  y -= 0.5f * vOutTmp[1] * height + 0.5f;
  
  vOut[0] = x;
  vOut[1] = y;

  return true;
}

void players(pid_t gamePid, Display* d, Window win, unsigned int playerList, unsigned int viewMatrix) {

  playerList += 0x28; //I don't know, my playerlist ptr is most likely wrong, but it works. For the most part...

    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    int teamOfLocalPlayer = 0;
    Memory::Read(gamePid, playerList + playerOffset::team, &teamOfLocalPlayer, sizeof(int)); //Remember now, the first index of the playerlist is the local player
    for (int i = 1; i < 32; i++) {
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
      
      int health = 0;
      Memory::Read(gamePid, player + playerOffset::health, &health, sizeof(int));
      if (health <= 0) continue; //if they are dead

      /*
	float pitch = 0;
	Memory::Read(gamePid, player + playerOffset::pitch, &pitch, sizeof(float));

	float yaw = 0;
	Memory::Read(gamePid, player + playerOffset::yaw, &yaw, sizeof(float));
      */

      float localpLocation[3];
      Memory::Read(gamePid, playerList + playerOffset::x, &localpLocation, sizeof(float[3]));
      if (localpLocation[0] == 0 && localpLocation[1] == 0 && localpLocation[2] == 0) continue;
    
      //This needs to be optimized to a single Read call
      float location[3];
      Memory::Read(gamePid, player + playerOffset::x, &location, sizeof(float[3]));
      if (location[0] == 0 && location[1] == 0 && location[2] == 0) continue; //here is that "for the most part" kicking in.
      //This is a check for if the player is even in the server, because players that leave still stay in the list.
      float out[2];                                                       //Why tf does this happen? God do I know.
    
      std::string name = "";
      for (int h = 0; h < 256; h++) {
	char currentCharacter;
	Memory::Read(gamePid, player + playerOffset::name + h, &currentCharacter, sizeof(char));

	if (currentCharacter == '\0') { break; } //strings in c/c++ are terminated via a null character

	name += currentCharacter;
      }

      location[2] += 8;

      if (WorldToScreen(gamePid, location, out, viewMatrix)) {      
	
        float distance = distanceFormula(localpLocation, location);

	float getFeet[3] = {location[0], location[1], location[2]};
	getFeet[2] -= 73;
	float screenFeet[2];
	WorldToScreen(gamePid, getFeet, screenFeet, viewMatrix); //get feet height

	// Convert screen coordinates to OpenGL coordinates
	float topY = 1.0 - (2.0 * ((out[1] + (1500/distance)))) / ENGINE::screenY;
	float bottomY = 1.0 - (2.0 * ((screenFeet[1] + (1500/distance)))) / ENGINE::screenY;
      
	float leftSideOffset[3] = {location[0], location[1], location[2]};
	leftSideOffset[2] += 30;
	float screenLeftOffset[2];
	WorldToScreen(gamePid, leftSideOffset, screenLeftOffset, viewMatrix);

	//Helped with box spacing: https://www.unknowncheats.me/forum/c-and-c-/76713-esp-box-size-calculation.html
	
	float rightX = ((2.0 * ((screenLeftOffset[0] + (9900/distance)))) / ENGINE::screenX - 1.0);
	float leftX = ((2.0 * ((screenLeftOffset[0] - (9900/distance)))) / ENGINE::screenX - 1.0);


	//float healthY = bottomY + 
	float leftHealthX = ((2.0 * ((screenLeftOffset[0] - (12000/distance)))) / ENGINE::screenX - 1.0);

	
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	if (health >= 95)
	  glColor4f(0, 255, 0, 1);
	else if (health <= 94 && health >= 51)
	  glColor4f(50, 190, 0 , 1);
	else if (health <= 50 && health >= 26)
	  glColor4f(190, 90, 0, 1);
	else if (health <= 25)
	  glColor4f(255, 0, 0, 1);
	glVertex2f(leftHealthX, topY);
	glVertex2f(leftHealthX, bottomY);
	glEnd();
	

	/*
	glBegin(GL_POLYGON);
	glVertex3f(leftX, topY, 0.0);
	glVertex3f(leftX, bottomY, 0.0);
	glVertex3f(rightX, topY, 0.0);
	glVertex3f(rightX, bottomY, 0.0);
	glEnd();
	*/

	
	glLineWidth(1.5);
	glBegin(GL_LINE_LOOP);
	glColor3f(color[0], color[1], color[2]);
	glVertex2f(rightX, topY);
	glVertex2f(rightX, bottomY);
	glEnd();
	
	glLineWidth(1.5);
	glBegin(GL_LINE_LOOP);
	glColor3f(color[0], color[1], color[2]);
	glVertex2f(leftX, topY);
	glVertex2f(leftX, bottomY);
	glEnd();

	glLineWidth(1.5);
	glBegin(GL_LINE_LOOP);
	glColor3f(color[0], color[1], color[2]);
	glVertex2f(rightX, topY);
	glVertex2f(leftX, topY);
	glEnd();
	
      	glLineWidth(1.5);
	glBegin(GL_LINE_LOOP);
	glColor3f(color[0], color[1], color[2]);
	glVertex2f(rightX, bottomY);
	glVertex2f(leftX, bottomY);
	glEnd();
      }
      
    }

  
    glXSwapBuffers(d, win); // Swap buffers
  
}
