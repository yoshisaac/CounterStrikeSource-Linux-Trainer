#include "aimbot.hpp"

#include "playerInfo.hpp"
#include "../engine/engine.hpp"
#include "../GUI/config.hpp"

#include "../xutil.hpp"
#include "../math.hpp"
#include "../memory.hpp"

void aimbot(pid_t gamePid, Display* aimDisplay) {
  for (;;) {
    for (unsigned int i = 0; i < 64; ++i) {
      
      Player player = getPlayerByIndex(i);
      Player p_local = getLocalPlayer();
      p_local.absLocation[2] += p_local.height;
      
      if (!config->AIM) {
	usleep(1000*1000/250); //at most, servers will run at a 128 ticks per second.	
	continue;
      }

      int aimbot_bone = 14; //default option if the switch statement is not hit for any reason
      switch (config->AIMhitbox) {
      case 0: //head
	aimbot_bone = 14;
	break;
      case 1: //upper body
	aimbot_bone = 12;
	break;
      case 2: //lower body
        aimbot_bone = 11;
	break;
      }
      
      // if (getPidByWindow(aimDisplay, getFocusedWindow(aimDisplay)) != gamePid) continue;

      if (getPlayerByIndex(AIMBOT::aimIndex).isDead == true) AIMBOT::aimIndex = -1;
      if (getPlayerByIndex(AIMBOT::aimIndex).dormant == true) AIMBOT::aimIndex = -1;
      
      if (player.isDead == true || player.health <= 0) continue;
      if (player.dormant == true) continue;
      if (player.team == p_local.team) continue;

      float plocal_v[3];
      Memory::Read(gamePid, ENGINE::pLocalPitch, &plocal_v[0], sizeof(float));
      Memory::Read(gamePid, ENGINE::pLocalYaw, &plocal_v[1], sizeof(float));
      plocal_v[2] = 0;
      float plocal_vo[3] = {*plocal_v};

      
      float deltaLocation[3] = { float(p_local.absLocation[0] - player.boneMatrix[aimbot_bone][0]),
				 float(p_local.absLocation[1] - player.boneMatrix[aimbot_bone][1]),
				 float(p_local.absLocation[2] - player.boneMatrix[aimbot_bone][2]) };

      float hyp = sqrt(deltaLocation[0] * deltaLocation[0] + deltaLocation[1] * deltaLocation[1]);

      plocal_vo[0] = (float)(atan(deltaLocation[2] / hyp) * radpi);
      plocal_vo[1] = (float)(atan(deltaLocation[1] / deltaLocation[0]) * radpi);
      
      float distance = distanceFormula3D(p_local.absLocation, player.boneMatrix[aimbot_bone]);
      
      float fov = sqrt(powf(sin((plocal_v[0] - plocal_vo[0] + p_local.aimPunch[0] * 2) * pideg) * distance, 2.0) + powf(sin((plocal_v[1] - plocal_vo[1] + p_local.aimPunch[1] * 2) * pideg) * distance, 2.0));

      float out[2];
      if (AIMBOT::aimIndex == i && WorldToScreenTest(gamePid, player.absLocation, out)) {

	playerInfo::l_players[i].aimbotFov = fov;
	
	if (fov > 90 && !isKeyDown(aimDisplay, XK_Alt_L)) {
	  AIMBOT::aimIndex = -1;
	  continue;
	}
	
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

	// https://github.com/GhostsOfHiroshima/Counter-Strike-Source-Hack/blob/master/Aimbot.cpp#L65
	if (config->AIMrecoilcompensation) {
	  plocal_v[0] -= (p_local.aimPunch[0] * 2.f);
	  plocal_v[1] -= (p_local.aimPunch[1] * 2.f);
	}
      }

      if (AIMBOT::aimIndex != -1 && isKeyDown(aimDisplay, XK_Alt_L)) {
	if (config->AIMsmooth > 0) usleep(1000*1000/300);
	Memory::Write(gamePid, ENGINE::pLocalPitch, &plocal_v[0], sizeof(float));
	Memory::Write(gamePid, ENGINE::pLocalYaw, &plocal_v[1], sizeof(float));
      } else {
	if (fov <= 90 && fov < getPlayerByIndex(AIMBOT::aimIndex).aimbotFov) {
	  AIMBOT::aimIndex = i;
	  playerInfo::l_players[AIMBOT::aimIndex].aimbotFov = fov;
	}
      }
    }
    usleep(1000*1000/250); //at most, servers will run at a 128 ticks per second.
  }                       //even then im unsure of that.
}
