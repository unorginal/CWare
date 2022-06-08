#include "legitbot.h"

// used: cheat variables
#include "../core/variables.h"
// used: main window open state
#include "../core/menu.h"
// used: key state
#include "../utilities/inputsystem.h"
// used: globals, cliententitylist interfaces
#include "../core/interfaces.h"
// used: backtracking
#include "../../base/utilities/math.h"
#include "../global.h"
#include "lagcompensation.h"

#include <iostream>

double PIforCalc = 3.14159265358;

void CLegitBot::Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;
	static CConVar* weapon_recoil_scale = I::ConVar->FindVar(XorStr("weapon_recoil_scale"));
	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;
	// disable when in-menu for more legit (lol)
	if (W::bMainOpened)
		return;

	// Beautiful Scope Check
	if (C::Get<bool>(Vars.bScopeCheck)) {
		if (pLocal->IsScoped() != true) {
			return;
		}
	}

	if (C::Get<int>(Vars.iAimbotKey) > 0 && !IPT::IsKeyDown(C::Get<int>(Vars.iAimbotKey)))
	{
		return;
	}
	CBaseEntity* closestEntity = nullptr;
	switch (C::Get<int>(Vars.iAimbotType)) {
	case 0:
		closestEntity = CLegitBot::FindClosestPlayerViaFov(pCmd, pLocal);
		break;
	case 1:
		closestEntity = CLegitBot::FindClosestPlayer(pLocal);
		break;
	}

	if (closestEntity) {
		// Now we gotta aim at them
		// Check if no bonez!!!
		if (closestEntity->GetBonePosition(8) != std::nullopt) {

			// Get Enemy Player Stuff
			std::optional<Vector> enemyHead = closestEntity->GetBonePosition(8);
			// Get Local Player Info
			Vector ourOrigin = pLocal->GetOrigin();
			Vector viewOffset = pLocal->GetViewOffset();
			Vector ourPositionVec = (ourOrigin + viewOffset);
			Vector* ourPosition = &ourPositionVec;

			// Math $$$
			Vector deltaVec = { enemyHead->x - ourPosition->x, enemyHead->y - ourPosition->y, enemyHead->z - ourPosition->z };
			float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

			// Where to aim
			float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PIforCalc);
			float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PIforCalc);

			// Check the values so no auto-vac
			if (pitch >= -89 && pitch <= 89 && yaw >= -189 && yaw <= 189) {
				// do that aim
				QAngle whereToAim = QAngle(pitch, yaw, 0);
				QAngle distance = QAngle(fabs(pitch - pCmd->angViewPoint.x), fabs(yaw - pCmd->angViewPoint.y), 0);
				QAngle curViewAngle = pCmd->angViewPoint;
				float howMuchToSmooth = C::Get<float>(Vars.fSmoothAmount);
				QAngle actualAim = QAngle( (curViewAngle.x + ( ( pitch - pCmd->angViewPoint.x ) * howMuchToSmooth) ), (curViewAngle.y + ((yaw - pCmd->angViewPoint.y) * howMuchToSmooth)), 0);
				if (C::Get<bool>(Vars.bNoRecoilHax)) {
					actualAim.x -= (pLocal->GetPunch().x * ( weapon_recoil_scale->GetFloat() * howMuchToSmooth) );
					actualAim.y -= (pLocal->GetPunch().y * ( weapon_recoil_scale->GetFloat() * howMuchToSmooth) );
				}
				if (C::Get<bool>(Vars.bSilentAim))
				{
					// Silent Aim $$$
					pCmd->angViewPoint = actualAim;
				}
				else 
				{
					// AIMLOCK TYPE BEAT $$$
					I::Engine->SetViewAngles(actualAim);
				}
			}
		}
	}
}

CBaseEntity* CLegitBot::FindClosestPlayer(CBaseEntity* pLocal) {
	float closestPlayerDistance = 1000000000000000.0f;
	CBaseEntity* closestEntity = nullptr;

	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr || !pEntity->IsPlayer() || pEntity->IsDormant() || !pEntity->IsAlive() || pEntity->HasImmunity())
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam()) 
		{
			// Don't aim at your own team?
			continue;
		}

		if (C::Get<bool>(Vars.bVisibleCheck)) {
			auto entityHeadPos = pEntity->GetBonePosition(8);
			if (entityHeadPos.has_value())
				if (pLocal->IsVisible(pEntity, entityHeadPos.value(), Vars.bSmokeCheck))
				{
					//Aimbot
				}
				else continue;
		}

		Vector* localPosition = &pLocal->GetOrigin();
		Vector* enemyPosition = &pEntity->GetOrigin();
		Vector delta = Vector(enemyPosition->x - localPosition->x, enemyPosition->y - localPosition->y, enemyPosition->z - localPosition->z);

		float distanceBetween = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

		if (distanceBetween < closestPlayerDistance) 
		{
			closestPlayerDistance = distanceBetween;
			closestEntity = pEntity;
		}
	}

	return closestEntity;
}
CBaseEntity* CLegitBot::FindClosestPlayerViaFov(CUserCmd* pCmd, CBaseEntity* pLocal) {
	float closestPlayerDistance = C::Get<float>(Vars.fMaxFovDistance);
	CBaseEntity* closestEntity = nullptr;

	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr || !pEntity->IsPlayer() || pEntity->IsDormant() || !pEntity->IsAlive() || pEntity->HasImmunity())
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam())
		{
			// Don't aim at your own team?
			continue;
		}

		if (C::Get<bool>(Vars.bVisibleCheck)) {
			auto entityHeadPos = pEntity->GetBonePosition(8);
			if (entityHeadPos.has_value())
				if (pLocal->IsVisible(pEntity, entityHeadPos.value(), Vars.bSmokeCheck))
				{
					//Aimbot
				}
				else continue;
		}
		std::optional<Vector> enemyHead = pEntity->GetBonePosition(8);
		// Get Local Player Info
		Vector ourOrigin = pLocal->GetOrigin();
		Vector viewOffset = pLocal->GetViewOffset();
		Vector ourPositionVec = (ourOrigin + viewOffset);
		Vector* ourPosition = &ourPositionVec;

		// Math $$$
		Vector deltaVec = { enemyHead->x - ourPosition->x, enemyHead->y - ourPosition->y, enemyHead->z - ourPosition->z };
		float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

		// Where to aim
		float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PIforCalc);
		float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PIforCalc);

		// Calc movement
		QAngle distance = QAngle(fabs(pitch - pCmd->angViewPoint.x), fabs(yaw - pCmd->angViewPoint.y), 0);
		float totalMove = sqrt ( (distance.x * distance.x) + ( distance.y * distance.y ) );

		if ((totalMove < C::Get<float>(Vars.fMaxFovDistance)) && (totalMove < closestPlayerDistance))
		{
			closestPlayerDistance = totalMove;
			closestEntity = pEntity;
		}
	}

	return closestEntity;
}