#include "aimbot.hpp"

#include "playerInfo.hpp"
#include "../engine/engine.hpp"
#include "../GUI/config.hpp"

#include "../xutil.hpp"
#include "../math.hpp"
#include "../memory.hpp"

void aimbot(pid_t gamePid, Display* aimDisplay) {
  for (;;) {
    for (int i = 0; i < 32; ++i) {
      
      Player player = getPlayerByIndex(i);
      Player p_local = getLocalPlayer();

      if (!config->AIM) continue;

      // if (getPidByWindow(aimDisplay, getFocusedWindow(aimDisplay)) != gamePid) continue;
      
      if (player.isDead == true || player.health <= 0) continue;
      if (player.dormant == true) continue;
      if (player.team == p_local.team) continue;
      float screen_center[2] = {ENGINE::screenX/2.f, ENGINE::screenY/2.f};
    
      float enemy_screen[2];
      WorldToScreen(gamePid, player.boneMatrix[14], enemy_screen);

      // float screenIndex_screen[2];
      // Player closestToScreen = getPlayerByIndex(AIMBOT::closestScreenIndex);
      // WorldToScreen(gamePid, closestToScreen.absLocation, screenIndex_screen);

      // if (distanceFormula2D(enemy_screen, screen_center) < distanceFormula2D(screenIndex_screen, screen_center))
      // 	AIMBOT::closestScreenIndex = i;

      if (distanceFormula3D(player.absLocation, p_local.absLocation) < distanceFormula3D(getPlayerByIndex(AIMBOT::closestDistanceIndex).absLocation, p_local.absLocation))
       	AIMBOT::closestDistanceIndex = i;

      if (getPlayerByIndex(AIMBOT::aimIndex).isDead == true) AIMBOT::aimIndex = -1;
      
      float plocal_v[3];
      Memory::Read(gamePid, ENGINE::pLocalPitch, &plocal_v[0], sizeof(float));
      Memory::Read(gamePid, ENGINE::pLocalYaw, &plocal_v[1], sizeof(float));
      plocal_v[2] = 0;
      float plocal_vo[3] = {*plocal_v};
      
	if (AIMBOT::aimIndex == i) {

	  float deltaLocation[3] = { float(p_local.absLocation[0] - player.boneMatrix[14][0]),
				     float(p_local.absLocation[1] - player.boneMatrix[14][1]),
				     float((p_local.absLocation[2] + p_local.height ) - player.boneMatrix[14][2]) };

	  float hyp = sqrt(deltaLocation[0] * deltaLocation[0] + deltaLocation[1] * deltaLocation[1]);

	  plocal_vo[0] = (float)(atan(deltaLocation[2] / hyp) * radpi);
	  plocal_vo[1] = (float)(atan(deltaLocation[1] / deltaLocation[0]) * radpi);

	  if (deltaLocation[0] >= 0.0f) {
	    plocal_vo[1] += 180.0f;
	  }

	  if (config->AIMsmooth > 0) {
	    float deltaAngle[2] = {plocal_vo[0] - plocal_v[0], plocal_vo[1] - plocal_v[1]};

	    // https://github.com/joaovarelas/h00k-game-hack/blob/master/h00k/aimbot.cpp#L658
	    if (deltaAngle[0] >  180) deltaAngle[0] -= 360;
	    if (deltaAngle[1] >  180) deltaAngle[1] -= 360;
	    if (deltaAngle[0] < -180) deltaAngle[0] += 360;
	    if (deltaAngle[1] < -180) deltaAngle[1] += 360;

	    plocal_v[0] = deltaAngle[0]/config->AIMsmooth + plocal_v[0];
	    plocal_v[1] = deltaAngle[1]/config->AIMsmooth + plocal_v[1];

	    if(plocal_v[0] >  180) plocal_v[0] -= 360; 
	    if(plocal_v[1] >  180) plocal_v[1] -= 360; 
	    if(plocal_v[0] < -180) plocal_v[0] += 360; 
	    if(plocal_v[1] < -180) plocal_v[1] += 360;
	  } else {
	    plocal_v[0] = plocal_vo[0];
	    plocal_v[1] = plocal_vo[1];
	  }

	  while (plocal_v[0] > 89)
	    plocal_v[0] -= 180;

	  while (plocal_v[0] < -89)
	    plocal_v[0] += 180;

	  if (isNaN(plocal_v[0]))
	    plocal_v[0] = 0;

	  while (plocal_v[1] > 180)
	    plocal_v[1] -= 360;

	  while (plocal_v[1] < -180)
	    plocal_v[1] += 360;

	  if (isNaN(plocal_v[1]))
	    plocal_v[1] = 0;
	}

	if (AIMBOT::aimIndex != -1 && isKeyDown(aimDisplay, XK_Alt_L)) {
	  if (config->AIMsmooth > 0) usleep(1000*1000/300);
	  Memory::Write(gamePid, ENGINE::pLocalPitch, &plocal_v[0], sizeof(float));
	  Memory::Write(gamePid, ENGINE::pLocalYaw, &plocal_v[1], sizeof(float));
	} else {
	  if (distanceFormula2D(screen_center, enemy_screen) <= 50)
	    AIMBOT::aimIndex = i;
	}
    }
  }
}
