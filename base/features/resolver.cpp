#include "resolver.h"

// used: engine, globals, cliententitylist interfaces
#include "../core/interfaces.h"
// used: cheat variables
#include "../core/variables.h"
// used: angle/vector calculations
#include "../utilities/math.h"
#include "lagcompensation.h"
#include "../global.h"
#include "antiaim.h"
#include "ragebot.h"

enum EAnimationLayers : int {
	ANIMATION_LAYER_AIMMATRIX = 0,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
	ANIMATION_LAYER_COUNT,
};

bool CResolver::CheckTarget(CBaseEntity* pEntity, CBaseEntity* pLocal) {
	if (pEntity == nullptr)
		return false;

	if (!pEntity->IsPlayer())
		return false;

	if (pEntity->IsDormant())
		return false;

	if (!pEntity->IsAlive())
		return false;

	if (pEntity->HasImmunity())
		return false;

	if (!pEntity->IsEnemy(pLocal))
		return false;

	if (pEntity == pLocal)
		return false;

	return true;
}

static int iLastResolvedSide[65];
static int iOldTick[65];

float GetNetworkLatency() {
	INetChannelInfo* nci = I::Engine->GetNetChannelInfo();
	if (nci) {
		return nci->GetAvgLatency(FLOW_INCOMING);
	}
	return 0.0f;
}

int CResolver::GetNumberOfTicksChoked(CBaseEntity* pEntity) { //thank you sharklaser, i could kiss you!
	float flSimulationTime = pEntity->GetSimulationTime();
	float flSimDiff = I::Globals->flCurrentTime - flSimulationTime;
	float latency = GetNetworkLatency();
	return TIME_TO_TICKS(std::max(0.0f, flSimDiff - latency));
}


float CResolver::GetMaxDesyncDelta(CCSGOPlayerAnimState* pAnimState) {
	if (!pAnimState)
		return 0.f;

	float flDuckAmount = pAnimState->flDuckAmount;
	float flRunningSpeed = std::clamp<float>(pAnimState->flRunningSpeed, 0.0f, 1.0f);
	float flDuckingSpeed = std::clamp<float>(pAnimState->flDuckingSpeed, 0.0f, 1.0f);
	float flYawModifier = (((pAnimState->flWalkToRunTransition * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;

	if (flDuckAmount > 0.0f)
		flYawModifier += ((flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier));

	float flMaxYawModifier = flYawModifier * pAnimState->flMaxBodyYaw;
	return flMaxYawModifier;
}

void CResolver::Run(CBaseEntity* pEntity) {
	if (!I::Engine->IsInGame() || !G::pLocal || !pEntity)
		return;

	auto anim_state = pEntity->GetAnimationState();
	if (!anim_state)
		return;

	if (GetNumberOfTicksChoked(pEntity) < 2) {
		resolver_stage[pEntity->GetIndex()] = "legit";
		return;
	}

	auto max_desync_delta = CResolver::GetMaxDesyncDelta(anim_state);

	const float playback_rate = pEntity->GetAnimationLayer(ANIMATION_LAYER_MOVEMENT_MOVE)->flPlaybackRate * 1000000.0f;
	const auto animation_lean = pEntity->GetAnimationLayer(ANIMATION_LAYER_LEAN);
	const float velocity = std::max< float >(pEntity->GetVelocity().Length2D(), 1);
	const float ground_speed = 1.f + anim_state->flGroundSpeed();
	const float calculated_playback_rate = playback_rate / velocity * ground_speed;
	const auto side = calculated_playback_rate > 60.f;
	int max_delta = INT_MAX;
	int best_side = -1;

	int left_delta = 0;
	int right_delta = 0;

	if (pEntity->GetVelocity().Length2D() <= 0.1f) {
		resolved_abs_yaw = side ? anim_state->flEyeYaw - max_desync_delta : anim_state->flEyeYaw + max_desync_delta;
		resolver_stage[pEntity->GetIndex()] = "standing";
	}
	else if ((animation_lean->flWeight * 10 >= 0.99f)) {
		resolved_abs_yaw = side ? anim_state->flEyeYaw + max_desync_delta : anim_state->flEyeYaw - max_desync_delta;
		resolver_stage[pEntity->GetIndex()] = "moving";
	}
	else if (!(pEntity->GetFlags() & FL_ONGROUND)) {
		resolved_abs_yaw = side ? anim_state->flEyeYaw + 10.f : anim_state->flEyeYaw - 10.f;
		resolver_stage[pEntity->GetIndex()] = "not on ground";
	}
	else if (G::MissedShots[pEntity->GetIndex()] > 4) {
		resolver_stage[pEntity->GetIndex()] = "bruteforce";
		switch (G::MissedShots[pEntity->GetIndex()] % 4) {
		case 0:
			resolved_abs_yaw =
				calculated_playback_rate > 60.f ? anim_state->flEyeYaw - max_desync_delta : anim_state->flEyeYaw +
				max_desync_delta;
			break;
		case 1:
			resolved_abs_yaw = side ? anim_state->flEyeYaw - 45.f : anim_state->flEyeYaw + 45.f;
			break;
		case 2:
			resolved_abs_yaw = side ? anim_state->flEyeYaw - 25.f : anim_state->flEyeYaw + 25.f;
			break;
		case 3:
			resolved_abs_yaw = side ? anim_state->flEyeYaw - 10.f : anim_state->flEyeYaw + 10.f;
			break;
		}
	}

	pEntity->GetAnimationState()->flGoalFeetYaw = M::NormalizeYaw(resolved_abs_yaw);
	pEntity->SetAbsAngles(QAngle(0.f, resolved_abs_yaw, 0.f));
}