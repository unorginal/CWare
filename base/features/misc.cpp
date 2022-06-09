// used: random_device, mt19937, uniform_int_distribution
#include <random>
#include <cmath>
#include "misc.h"
#include "lagcompensation.h"
#include <valarray>
#include <ctime>
#include <cstdlib>


void CMiscellaneous::Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;

	// @credits: a8pure c:
	if (C::Get<bool>(Vars.bMiscNoCrouchCooldown))
		pCmd->iButtons |= IN_BULLRUSH;

	if (C::Get<bool>(Vars.bMiscBunnyHop))
		BunnyHop(pCmd, pLocal);

	if (C::Get<bool>(Vars.bMiscAutoStrafe))
		AutoStrafe(pCmd, pLocal);

	static CConVar* show_impact = I::ConVar->FindVar(XorStr("sv_showimpacts"));
	if (C::Get<bool>(Vars.bShowBulletImpacts)) {
		if (pLocal->IsAlive() && I::Engine->IsInGame()) {
			show_impact->SetValue(1);
		}
	}
	else {
		show_impact->SetValue(0);
	}

	if (C::Get<bool>(Vars.bMiscDoClantag)) {
		static auto lastspammed = 0;
		if (I::Engine->IsInGame() && !I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage()) {
			if (GetTickCount() - lastspammed > 800)
			{
				U::SendClanTag("catware.dev", "catware.dev");
			}
		}
	}

	if (C::Get<bool>(Vars.bMiscDoDomToretto))
		DomToretto();

	if (C::Get<bool>(Vars.bMiscRevealRanks) && pCmd->iButtons & IN_SCORE)
		I::Client->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0U, 0, nullptr);
}

void CMiscellaneous::Event(IGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::Engine->IsInGame())
		return;
}

ERageBotFakelag CMiscellaneous::FakeLagMode(CBaseEntity* pLocal, CUserCmd* pCmd) {
	if (!pLocal || !pLocal->IsAlive())
		return ERageBotFakelag::FAKELAG_NONE;

	CCSGOPlayerAnimState* State = pLocal->GetAnimationState();
	const float flSpeed = State->vecVelocity.Length2D();
	const bool OnGround = pLocal->GetFlags() & FL_ONGROUND;

	const bool bStanding = flSpeed < 4.f && OnGround;
	const bool bMoving = flSpeed > 5.f;
	const bool bInAir = !OnGround;
	const bool bDucking = pLocal->GetFlags() & FL_DUCKING;
	const bool bAttacking = (pCmd->iButtons & IN_ATTACK);

	auto pWeapon = pLocal->GetWeapon();

	//if (!pWeapon)
		//return ERageBotFakelag::FAKELAG_NONE;

	const bool bWeaponActivity = G::pCmd->iButtons & IN_ATTACK || pWeapon->IsReloading();

	const auto vecFakelagToggles = C::Get<std::vector<bool>>(Vars.vecMiscFakeLagType);

	const auto Standing = vecFakelagToggles.at(FAKELAG_STANDING);
	const auto Moving = vecFakelagToggles.at(FAKELAG_MOVING);
	const auto InAir = vecFakelagToggles.at(FAKELAG_AIR);
	const auto Ducking = vecFakelagToggles.at(FAKELAG_DUCKING);
	const auto Weapon = vecFakelagToggles.at(FAKELAG_WEAPON_ACTIVITY);
	const auto OnShot = vecFakelagToggles.at(FAKELAG_ONSHOT);

	if (bAttacking && OnShot) return FAKELAG_ONSHOT;
	if (bWeaponActivity && Weapon) return FAKELAG_WEAPON_ACTIVITY;
	if (bStanding && Standing && !bWeaponActivity) return FAKELAG_STANDING;
	if (bMoving && Moving && !bWeaponActivity) return FAKELAG_MOVING;
	if (bInAir && InAir && !bWeaponActivity) return FAKELAG_AIR;
	if (bDucking && Ducking && !bWeaponActivity) return FAKELAG_DUCKING;

	return FAKELAG_NONE;
}

void CMiscellaneous::MovementCorrection(CUserCmd* pCmd, const QAngle& angOldViewPoint) const
{
	static CConVar* cl_forwardspeed = I::ConVar->FindVar(XorStr("cl_forwardspeed"));

	if (cl_forwardspeed == nullptr)
		return;

	static CConVar* cl_sidespeed = I::ConVar->FindVar(XorStr("cl_sidespeed"));

	if (cl_sidespeed == nullptr)
		return;

	static CConVar* cl_upspeed = I::ConVar->FindVar(XorStr("cl_upspeed"));

	if (cl_upspeed == nullptr)
		return;

	// get max speed limits by convars
	const float flMaxForwardSpeed = cl_forwardspeed->GetFloat();
	const float flMaxSideSpeed = cl_sidespeed->GetFloat();
	const float flMaxUpSpeed = cl_upspeed->GetFloat();

	Vector vecForward = { }, vecRight = { }, vecUp = { };
	M::AngleVectors(angOldViewPoint, &vecForward, &vecRight, &vecUp);

	// we don't attempt on forward/right roll, and on up pitch/yaw
	vecForward.z = vecRight.z = vecUp.x = vecUp.y = 0.f;

	vecForward.NormalizeInPlace();
	vecRight.NormalizeInPlace();
	vecUp.NormalizeInPlace();

	Vector vecOldForward = { }, vecOldRight = { }, vecOldUp = { };
	M::AngleVectors(pCmd->angViewPoint, &vecOldForward, &vecOldRight, &vecOldUp);

	// we don't attempt on forward/right roll, and on up pitch/yaw
	vecOldForward.z = vecOldRight.z = vecOldUp.x = vecOldUp.y = 0.f;

	vecOldForward.NormalizeInPlace();
	vecOldRight.NormalizeInPlace();
	vecOldUp.NormalizeInPlace();

	const float flPitchForward = vecForward.x * pCmd->flForwardMove;
	const float flYawForward = vecForward.y * pCmd->flForwardMove;
	const float flPitchSide = vecRight.x * pCmd->flSideMove;
	const float flYawSide = vecRight.y * pCmd->flSideMove;
	const float flRollUp = vecUp.z * pCmd->flUpMove;

	// solve corrected movement
	const float x = vecOldForward.x * flPitchSide + vecOldForward.y * flYawSide + vecOldForward.x * flPitchForward + vecOldForward.y * flYawForward + vecOldForward.z * flRollUp;
	const float y = vecOldRight.x * flPitchSide + vecOldRight.y * flYawSide + vecOldRight.x * flPitchForward + vecOldRight.y * flYawForward + vecOldRight.z * flRollUp;
	const float z = vecOldUp.x * flYawSide + vecOldUp.y * flPitchSide + vecOldUp.x * flYawForward + vecOldUp.y * flPitchForward + vecOldUp.z * flRollUp;

	// clamp and apply corrected movement
	pCmd->flForwardMove = std::clamp(x, -flMaxForwardSpeed, flMaxForwardSpeed);
	pCmd->flSideMove = std::clamp(y, -flMaxSideSpeed, flMaxSideSpeed);
	pCmd->flUpMove = std::clamp(z, -flMaxUpSpeed, flMaxUpSpeed);
}

void CMiscellaneous::DomToretto() {
	static auto lastspammed = 0;
	if (I::Engine->IsInGame() && !I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage()) {
		if (GetTickCount() - lastspammed > 800)
		{
			lastspammed = GetTickCount();
			U::SendName("Dom Toretto");
			U::SendClanTag("Family", "Family");
			int num = (rand() % 10);
			switch (num) {
			case 1:
				I::Engine->ExecuteClientCmd("say I don't got friends, I got family.");
				break;
			case 2:
				I::Engine->ExecuteClientCmd("say How can we not talk about family when family is all we got?");
				break;
			case 3:
				I::Engine->ExecuteClientCmd("say I do all of this for family.");
				break;
			case 4:
				I::Engine->ExecuteClientCmd("say There's always room for family.");
				break;
			case 5:
				I::Engine->ExecuteClientCmd("say You don't turn your back on family. Even when they do.");
				break;
			case 6:
				I::Engine->ExecuteClientCmd("say Everyone's looking for the thrill, but what's real is family. Your family. Hold on to that, Brian.");
				break;
			case 7:
				I::Engine->ExecuteClientCmd("say You've heard me say, that you never turn your back on family. And I wanna thank you all for never turning your back on me..");
				break;
			case 8:
				I::Engine->ExecuteClientCmd("say You never turn your back on family.");
				break;
			case 9:
				I::Engine->ExecuteClientCmd("say You don't mess with family.");
				break;
			case 10:
				I::Engine->ExecuteClientCmd("say Family is by our side.");
				break;
			}
		}
	}
}

void CMiscellaneous::AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!pLocal->IsAlive())
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;

	const short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
	const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);

	// check for pistol and attack
	if (pWeaponData == nullptr || pWeaponData->bFullAuto || pWeaponData->nWeaponType != WEAPONTYPE_PISTOL || !(pCmd->iButtons & IN_ATTACK))
		return;

	if (pLocal->CanShoot(static_cast<CWeaponCSBase*>(pWeapon)))
		pCmd->iButtons |= IN_ATTACK;
	else
		pCmd->iButtons &= ~IN_ATTACK;
}

/*
void CMiscellaneous::FakeLag(CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!pLocal->IsAlive())
		return;

	INetChannel* pNetChannel = I::ClientState->pNetChannel;

	if (pNetChannel == nullptr)
		return;

	static CConVar* sv_maxusrcmdprocessticks = I::ConVar->FindVar(XorStr("sv_maxusrcmdprocessticks"));

	if (sv_maxusrcmdprocessticks == nullptr)
		return;

	/*
	 * @note: get max available ticks to choke
	 * 2 ticks reserved for server info else player can be stacked
	 * while antiaiming and fakelag is disabled choke only 1 tick
	 */
/*
	const int iMaxCmdProcessTicks = C::Get<bool>(Vars.bMiscFakeLag) ? sv_maxusrcmdprocessticks->GetInt() - 2 :
		C::Get<bool>(Vars.bAntiAim) ? 1 : 0;

	// choke
	bSendPacket = I::ClientState->nChokedCommands >= iMaxCmdProcessTicks;
}
*/

void CMiscellaneous::ShowSpread(CUserCmd* pCmd, bool& bSendPacket) {

}

void CMiscellaneous::FakeLag(CUserCmd * pCmd, CBaseEntity * pLocal, bool& bSendPacket) {
	if (!C::Get<bool>(Vars.bMiscFakeLag)) {
		bSendPacket = true;
		return;
	}

	if (!pLocal->IsAlive()) {
		bSendPacket = true;
		return;
	}

	const auto Mode = FakeLagMode(pLocal, pCmd);

	static int iFactor;

	static int UsedByTickBaseShift = 1;
	iFactor = C::Get<int>(Vars.iFakelagTicks);
	switch (Mode) {
	default:
	case FAKELAG_NONE:
		bSendPacket = true;
		break;
	case FAKELAG_ONSHOT:
		bSendPacket = I::ClientState->nChokedCommands >= iFactor;
		break;
	case FAKELAG_AIR:
		bSendPacket = I::ClientState->nChokedCommands >= iFactor;
		break;
	case FAKELAG_DUCKING:
		bSendPacket = I::ClientState->nChokedCommands >= iFactor;
		break;
	case FAKELAG_MOVING:
		bSendPacket = I::ClientState->nChokedCommands >= iFactor;
		break;
	case FAKELAG_STANDING:
		bSendPacket = I::ClientState->nChokedCommands >= iFactor;
		break;
	case FAKELAG_WEAPON_ACTIVITY:
		bSendPacket = I::ClientState->nChokedCommands >= iFactor;
		break;
	}
}

void CMiscellaneous::BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocal) const
{
	static CConVar* sv_autobunnyhopping = I::ConVar->FindVar(XorStr("sv_autobunnyhopping"));

	if (sv_autobunnyhopping->GetBool())
		return;

	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_OBSERVER)
		return;

	std::random_device randomDevice;
	std::mt19937 generate(randomDevice());
	const std::uniform_int_distribution<> chance(0, 100);

	if (chance(generate) > C::Get<int>(Vars.iMiscBunnyHopChance))
		return;

	static bool bLastJumped = false, bShouldFake = false;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		pCmd->iButtons |= IN_JUMP;
	}
	else if (pCmd->iButtons & IN_JUMP)
	{
		if (pLocal->GetFlags() & FL_ONGROUND || pLocal->GetFlags() & FL_PARTIALGROUND)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			pCmd->iButtons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

void CMiscellaneous::AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	// pasted from legendware DO NOT LOOK - unix
	if (pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;
	if (pLocal->GetFlags() & FL_ONGROUND)
		return;
	static auto cl_sidespeed = I::ConVar->FindVar("cl_sidespeed");
	auto side_speed = cl_sidespeed->GetFloat();

	QAngle engine_angles;
	I::Engine->GetViewAngles(engine_angles);

	auto velocity = pLocal->GetVelocity();

	pCmd->flForwardMove = std::min(5850.0f / velocity.Length2D(), side_speed);
	pCmd->flSideMove = pCmd->iCommandNumber % 2 ? side_speed : -side_speed;

	auto yaw_velocity = M::CalcAngle(Vector(0.0f, 0.0f, 0.0f), velocity).y;
	auto ideal_rotation = M::clamp(M_RAD2DEG(atan2(15.0f, velocity.Length2D())), 0.0f, 45.0f);

	auto yaw_rotation = fabs(yaw_velocity - engine_angles.y) + (pCmd->iCommandNumber % 2 ? ideal_rotation : -ideal_rotation);
	auto ideal_yaw_rotation = yaw_rotation < 5.0f ? yaw_velocity : engine_angles.y;
}
