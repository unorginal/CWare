#include "ragebot.h"
#include <iostream>
// used: cheat variables
#include "../core/variables.h"
// used: main window open state
#include "../core/menu.h"
// used: key state
#include "../utilities/inputsystem.h"
// used: globals, cliententitylist interfaces
#include "../core/interfaces.h"
// used: backtracking
#include "autowall.h"
#include "../../base/utilities/math.h"
#include "../global.h"
#include "lagcompensation.h"
#include "../../dependencies/imgui/imstb_truetype.h"
#include "../utilities/math.h"
#include "../WeaponConfig.h"
#include "../../base/utilities.h"

double PI = 3.14159265358;

// My Ragebot Math/Code
void CRageBot::Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket)
{
	if (!(C::Get<bool>(Vars.bRage))) {
		return;
	}
	if (!pLocal->IsAlive())
		return;
	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return;

	short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);
	// check is weapon gun
	if (pWeaponData == nullptr || !pWeaponData->IsGun())
		return;
	CBaseEntity* closestEntity = CRageBot::FindBestTarget(pLocal);
	automatic_revolver(pCmd, *pWeapon);
	if (closestEntity) {
		// Now we gotta aim at them
		// Check if no bonez!!!
		if (C::Get<bool>(Vars.bAutoStop) && pWeapon->GetAmmo() > 0) {
			if (pCmd->flForwardMove > 36.0f) {
				pCmd->flForwardMove = 36.0f;
			}
			else if (pCmd->flForwardMove < -36.0f) {
				pCmd->flForwardMove = -36.0f;
			}
			if (pCmd->flSideMove > 36.0f) {
				pCmd->flSideMove = 36.0f;
			}
			else if (pCmd->flSideMove < -36.0f) {
				pCmd->flSideMove = -36.0f;
			}
		}
		if (C::Get<bool>(Vars.bAutoDuck)) {
			pCmd->iButtons |= IN_DUCK;
		}
		Vector hitbox_position;
		matrix3x4_t matrix[128];
		if (!closestEntity->SetupBones(matrix, 128, 256, I::Engine->GetLastTimeStamp()))
			return;
		hitbox_position = CalculateBestPoint(closestEntity, HITBOX_PELVIS, WC::GetCurrentMinDamage(), false, matrix);
		if (!hitbox_position.IsValid())
		{
			return;
		}
		// enemyHead Local Player Info
		Vector ourOrigin = pLocal->GetOrigin();
		Vector viewOffset = pLocal->GetViewOffset();
		Vector ourPositionVec = (ourOrigin + viewOffset);
		Vector* ourPosition = &ourPositionVec;
		// Math $$$
		Vector deltaVec = { hitbox_position.x - ourPosition->x, hitbox_position.y - ourPosition->y, hitbox_position.z - ourPosition->z };
		float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);
		// Where to aim
		float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
		float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PI);

		// Check the values so no auto-vac
		if (pitch >= -89 && pitch <= 89 && yaw >= -189 && yaw <= 189) {
			// do that aim
			QAngle whereToAim = QAngle(pitch, yaw, 0);
			static CConVar* weapon_recoil_scale = I::ConVar->FindVar(XorStr("weapon_recoil_scale"));
			whereToAim.x -= (pLocal->GetPunch().x * weapon_recoil_scale->GetFloat());
			whereToAim.y -= (pLocal->GetPunch().y * weapon_recoil_scale->GetFloat());
			if (C::Get<bool>(Vars.bScopeCheck)) {
				switch (pWeaponData->nWeaponType) {
				case WEAPON_AWP:
					if (pLocal->IsScoped() == false)
						return;
					break;
				case WEAPON_SCAR20:
					if (pLocal->IsScoped() == false)
						return;
					break;
				case WEAPON_SSG08:
					if (pLocal->IsScoped() == false)
						return;
					break;
				case WEAPON_G3SG1:
					if (pLocal->IsScoped() == false)
						return;
					break;
				case WEAPON_SG556:
					if (pLocal->IsScoped() == false)
						return;
					break;
				case WEAPON_AUG:
					if (pLocal->IsScoped() == false)
						return;
					break;
				}
			}
			if (!CRageBot::CheckHitchance(pLocal, closestEntity, whereToAim)) {
				return;
			}
			if (pLocal->CanShoot(static_cast<CWeaponCSBase*>(pWeapon)))
			{
				pCmd->angViewPoint = whereToAim;
				pCmd->iButtons |= IN_ATTACK;
				if (C::Get<bool>(Vars.bSeeAngles)) {
					I::Engine->SetViewAngles(whereToAim);
				}
			}
		}
	}
}
void CRageBot::DoNoRecoil(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	if (pLocal)
	{
		Vector AimPunch = Vector(pLocal->GetViewPunch().x, pLocal->GetViewPunch().y, pLocal->GetViewPunch().z);
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->angViewPoint -= QAngle((AimPunch * 2).x, (AimPunch * 2).y, (AimPunch * 2).z);

			//GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}
void CRageBot::automatic_revolver(CUserCmd* cmd, CBaseCombatWeapon& weapon)
{
	if (weapon.GetItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / I::Globals->flIntervalPerTick); };
	constexpr float revolverPrepareTime{ 0.234375f };

	static float readyTime;
	const CBaseCombatWeapon* activeWeapon = G::pLocal->GetWeapon();
	if (!readyTime) readyTime = serverTime(cmd) + revolverPrepareTime;
	auto ticksToReady = timeToTicks(readyTime - serverTime(cmd) - I::Engine->GetNetChannelInfo()->GetLatency(0));
	if (ticksToReady > 0 && ticksToReady <= timeToTicks(revolverPrepareTime))
		cmd->iButtons |= IN_ATTACK;
	else
		readyTime = 0.0f;
}
float CRageBot::serverTime(CUserCmd* cmd) noexcept
{
	static int tick;
	static CUserCmd* lastCmd;

	if (cmd) {
		if (G::pLocal && (!lastCmd || lastCmd->bHasBeenPredicted))
			tick = G::pLocal->GetTickBase();
		else
			tick++;
		lastCmd = cmd;
	}
	return tick * I::Globals->flIntervalPerTick;
}

CBaseEntity* CRageBot::FindBestTarget(CBaseEntity* pLocal) {
	float closestPlayerDistance = 10000000000.f;
	CBaseEntity* closestEntity = nullptr;
	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr || !pEntity->IsPlayer() || pEntity->IsDormant() || !pEntity->IsAlive() || pEntity->HasImmunity())
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam())
		{
			continue;
		}
		Vector vecTarget;
		matrix3x4_t matrix[128];
		if (!pEntity->SetupBones(matrix, 128, 256, I::Engine->GetLastTimeStamp()))
			continue;

		vecTarget = CalculateBestPoint(pEntity, 0, WC::GetCurrentMinDamage(), false, matrix);
		if (!vecTarget.IsValid())
			continue;
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


// Gladiator Cheats Multipoint
float CRageBot::BestHitPoint(CBaseEntity* player, int prioritized, float minDmg, mstudiohitboxset_t* hitset, matrix3x4_t matrix[], Vector& vecOut)
{
	mstudiobbox_t* hitbox = hitset->GetHitbox(prioritized);
	if (!hitbox)
		return 0.f;

	std::vector<Vector> vecArray;
	float flHigherDamage = 0.f;

	float mod = hitbox->flRadius != -1.f ? hitbox->flRadius : 0.f;

	Vector max;
	Vector min;

	GM::VectorTransform(hitbox->vecBBMax + mod, matrix[hitbox->iBone], max);
	GM::VectorTransform(hitbox->vecBBMin - mod, matrix[hitbox->iBone], min);

	auto center = (min + max) * 0.5f;

	QAngle curAngles = M::CalcAngle(center, G::pLocal->GetEyePosition());

	Vector forward;
	GM::AngleVectors(curAngles, forward);

	Vector right = forward.CrossProduct(Vector(0, 0, 1));
	Vector left = Vector(-right.x, -right.y, right.z);

	Vector top = Vector(0, 0, 1);
	Vector bot = Vector(0, 0, -1);

	const float POINT_SCALE = C::Get<float>(Vars.fMultipointScale);
	if (C::Get<bool>(Vars.bUseMultipoint)) {
		switch (prioritized)
		{
		case HITBOX_HEAD:
			for (auto i = 0; i < 4; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray[1] += top * (hitbox->flRadius * POINT_SCALE);
			vecArray[2] += right * (hitbox->flRadius * POINT_SCALE);
			vecArray[3] += left * (hitbox->flRadius * POINT_SCALE);
			break;

		default:

			for (auto i = 0; i < 2; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray[0] += right * (hitbox->flRadius * POINT_SCALE);
			vecArray[1] += left * (hitbox->flRadius * POINT_SCALE);
			break;
		}
	}
	else
		vecArray.emplace_back(center);

	for (Vector cur : vecArray)
	{
		if (!IPT::IsKeyDown(C::Get<int>(Vars.iRageMinDamageOverrideKey)))
			{
				FireBulletData_t data;
				float flCurDamage = CAutoWall::Get().GetDamage(G::pLocal, cur, &data);

				if (!flCurDamage)
					continue;
				if (flCurDamage >= player->GetHealth()) {
					flHigherDamage = flCurDamage;
					vecOut = cur;
					break;
				}
				if ((flCurDamage > flHigherDamage) && ( (flCurDamage > minDmg) ))
				{
					flHigherDamage = flCurDamage;
					vecOut = cur;
				}
		}
		else {
			FireBulletData_t data;
			float flCurDamage = CAutoWall::Get().GetDamage(G::pLocal, cur, &data);

			if (!flCurDamage)
				continue;
			if (flCurDamage >= player->GetHealth()) {
				flHigherDamage = flCurDamage;
				vecOut = cur;
				break;
			}
			if ((flCurDamage > flHigherDamage) && (flCurDamage > 3))
			{
				flHigherDamage = flCurDamage;
				vecOut = cur;
			}
		}
	}
	return flHigherDamage;
}

Vector CRageBot::CalculateBestPoint(CBaseEntity* player, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[])
{
	studiohdr_t* studioHdr = I::ModelInfo->GetStudioModel(player->GetModel());
	mstudiohitboxset_t* set = studioHdr->GetHitboxSet(player->GetHitboxSet());
	Vector vecOutput;

	if (CRageBot::BestHitPoint(player, prioritized, minDmg, set, matrix, vecOutput) > minDmg && onlyPrioritized)
	{
		return vecOutput;
	}
	else
	{
		float flHigherDamage = 0.f;

		Vector vecCurVec;
		std::vector<int> CurrentHitboxes = WC::GetCurrentHitboxesI();

		for (int i = 0; i < CurrentHitboxes.size(); ++i)
		{

			float flCurDamage = CRageBot::BestHitPoint(player, CurrentHitboxes.at(i), minDmg, set, matrix, vecCurVec);

			if (!flCurDamage)
				continue;

			if (flCurDamage > flHigherDamage)
			{
				player->PriotizedHitbox = CurrentHitboxes.at(i);
				flHigherDamage = flCurDamage;
				vecOutput = vecCurVec;
			}
		}
		return vecOutput;
	}
}


int CRageBot::hitchance(const QAngle& aim_angle, CBaseEntity* finalTarget, int hitbox)
{
	auto final_hitchance = 0;
	CBaseCombatWeapon* pWeapon = G::pLocal->GetWeapon();

	if (pWeapon == nullptr)
		std::cout << "Weapon not found" << std::endl;
		return 0;

	short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
	CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);
	auto weapon_info = pWeaponData;

	if (!weapon_info)
		std::cout << "Weapon info not found" << std::endl;
		return final_hitchance;

	if ((G::pLocal->GetEyePosition() - finalTarget->GetAbsOrigin()).Length() > weapon_info->flRange)
		std::cout << "Range issue" << std::endl;
		return final_hitchance;

	auto forward = Vector(0.0f, 0.0f, 0.0f);
	auto right = Vector(0.0f, 0.0f, 0.0f);
	auto up = Vector(0.0f, 0.0f, 0.0f);

	M::AngleVectors(aim_angle, &forward, &right, &up);

	M::fast_vec_normalize(forward);
	M::fast_vec_normalize(right);
	M::fast_vec_normalize(up);

	auto is_special_weapon = pWeapon->GetItemDefinitionIndex() == WEAPON_AWP || pWeapon->GetItemDefinitionIndex() == WEAPON_G3SG1 || pWeapon->GetItemDefinitionIndex() == WEAPON_SCAR20 ||  pWeapon->GetItemDefinitionIndex() == WEAPON_SSG08;
	auto inaccuracy = pWeapon->GetInaccuracy();

	if (pWeapon->GetInaccuracy() - 0.000001f < inaccuracy)
		final_hitchance = 101;
	else
	{
		static auto setup_spread_values = true;
		static float spread_values[256][6];

		if (setup_spread_values)
		{
			setup_spread_values = false;

			for (auto i = 0; i < 256; ++i)
			{
				M::RandomSeed(i + 1);

				auto a = M::RandomFloatFunc(0.0f, 1.0f);
				auto b = M::RandomFloatFunc(0.0f, DirectX::XM_2PI);
				auto c = M::RandomFloatFunc(0.0f, 1.0f);
				auto d = M::RandomFloatFunc(0.0f, DirectX::XM_2PI);

				spread_values[i][0] = a;
				spread_values[i][1] = c;

				auto sin_b = 0.0f, cos_b = 0.0f;
				DirectX::XMScalarSinCos(&sin_b, &cos_b, b);

				auto sin_d = 0.0f, cos_d = 0.0f;
				DirectX::XMScalarSinCos(&sin_d, &cos_d, d);

				spread_values[i][2] = sin_b;
				spread_values[i][3] = cos_b;
				spread_values[i][4] = sin_d;
				spread_values[i][5] = cos_d;
			}
		}

		auto hits = 0;
		matrix3x4_t matrix_data[128];
		if (!finalTarget->SetupBones(matrix_data, 128, 256, I::Engine->GetLastTimeStamp())) {
			std::cout << "Could not setupbones" << std::endl;
			return 0;
		}
		for (auto i = 0; i < 256; ++i)
		{
			auto inaccuracy = spread_values[i][0] * pWeapon->GetInaccuracy();
			auto spread = spread_values[i][1] * pWeapon->GetSpread();

			auto spread_x = spread_values[i][3] * inaccuracy + spread_values[i][5] * spread;
			auto spread_y = spread_values[i][2] * inaccuracy + spread_values[i][4] * spread;

			auto direction = Vector(0.0f, 0.0f, 0.0f);

			direction.x = forward.x + right.x * spread_x + up.x * spread_y;
			direction.y = forward.y + right.y * spread_x + up.y * spread_y;
			direction.z = forward.z + right.z * spread_x + up.z * spread_y; //-V778

			auto end = G::pLocal->GetEyePosition() + direction * weapon_info->flRange;

			if (CRageBot::Get().hitbox_intersection(finalTarget, matrix_data, finalTarget->PriotizedHitbox, G::pLocal->GetEyePosition(), end))
				++hits;
		}
		std::cout << "Calculating... Hits: "; std::cout << hits << std::endl;
		final_hitchance = (int)((float)hits / 2.56f);
	}

	auto damage = 0;
	auto high_accuracy_weapon = pWeapon->GetItemDefinitionIndex() == WEAPON_AWP || pWeapon->GetItemDefinitionIndex() == WEAPON_SSG08;
	matrix3x4_t matrix_data[128];
	if (!finalTarget->SetupBones(matrix_data, 128, 256, I::Engine->GetLastTimeStamp())) {
		std::cout << "Could not setupbones" << std::endl;
		return 0;
	}
	auto spread = pWeapon->GetSpread() + pWeapon->GetInaccuracy();

	for (auto i = 1; i <= 6; ++i)
	{
		for (auto j = 0; j < 8; ++j)
		{
			auto current_spread = spread * ((float)i / 6.0f);

			auto direction_cos = 0.0f;
			auto direction_sin = 0.0f;

			DirectX::XMScalarSinCos(&direction_cos, &direction_sin, (float)j / 8.0f * DirectX::XM_2PI);

			auto spread_x = direction_cos * current_spread;
			auto spread_y = direction_sin * current_spread;

			auto direction = Vector(0.0f, 0.0f, 0.0f);

			direction.x = forward.x + spread_x * right.x + spread_y * up.x;
			direction.y = forward.y + spread_x * right.y + spread_y * up.y;
			direction.z = forward.z + spread_x * right.z + spread_y * up.z;

			auto end = G::pLocal->GetEyePosition() + direction * weapon_info->flRange;

			if (CRageBot::Get().hitbox_intersection(finalTarget, matrix_data, finalTarget->PriotizedHitbox, G::pLocal->GetEyePosition(), end))
			{
				FireBulletData_t fire_data;
				CBaseEntity* pLocal = G::pLocal;
				int damage = CAutoWall::Get().GetDamage(pLocal, end, &fire_data);
				auto valid_hitbox = true;

				//if (final_target.data.hitbox == HITBOX_HEAD && fire_data.enterTrace != HITBOX_HEAD)
					//valid_hitbox = false;

				if (fire_data.flCurrentDamage >= 1 && valid_hitbox)
					damage += high_accuracy_weapon ? fire_data.flCurrentDamage : 1;
			}
		}
	}

	if (high_accuracy_weapon) {
		std::cout << "Some high accuracy bs" << std::endl;
		return (float)damage / 48.0f >= CRageBot::Get().get_minimum_damage(G::pLocal->IsVisible(finalTarget, finalTarget->GetEyePosition()), finalTarget->GetHealth()) ? final_hitchance : 0;
	}
	std::cout << "Normal" << std::endl;
	return (float)damage / 48.0f >= (float)WC::GetCurrentHitchance() * 0.01f ? final_hitchance : 0;
}

int CRageBot::get_minimum_damage(bool visible, int health)
{
	auto minimum_damage = 1;
	if (WC::GetCurrentMinDamage() > 100)
		minimum_damage = health + WC::GetCurrentMinDamage() - 100;
	else
		minimum_damage = M::clamp(WC::GetCurrentMinDamage(), 1, health);

	return minimum_damage;
}

bool CRageBot::hitbox_intersection(CBaseEntity* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end, float* safe)
{
	auto model = e->GetModel();

	if (!model)
		return false;

	auto studio_model = I::ModelInfo->GetStudioModel(model);

	if (!studio_model)
		return false;

	auto studio_set = studio_model->GetHitboxSet(e->GetHitboxSet());

	if (!studio_set)
		return false;

	auto studio_hitbox = studio_set->GetHitbox(hitbox);

	if (!studio_hitbox)
		return false;

	Trace_t trace;

	Ray_t ray = Ray_t(start, end);

	auto intersected = CRageBot::clip_ray_to_hitbox(ray, studio_hitbox, matrix[studio_hitbox->iBone], trace) >= 0;

	if (!safe)
		return intersected;

	Vector min, max;

	GM::VectorTransform(studio_hitbox->vecBBMin, matrix[studio_hitbox->iBone], min);
	GM::VectorTransform(studio_hitbox->vecBBMax, matrix[studio_hitbox->iBone], max);

	auto center = (min + max) * 0.5f;
	auto distance = center.DistTo(end);

	if (distance > *safe)
		*safe = distance;

	return intersected;
}



// Skeet Dump (Module) / Supremacy Code $$$ (quite)
bool CRageBot::CheckHitchance(CBaseEntity* player, CBaseEntity* enemyPlayer, const QAngle& angle) {
	constexpr float HITCHANCE_MAX = 100.f;
	constexpr int   SEED_MAX = 256;

	Vector     start = player->GetEyePosition(), end, fwd, right, up, dir, wep_spread;
	float      inaccuracy, spread;
	CGameTrace tr;
	size_t     total_hits{ }, needed_hits{ (size_t)std::ceil((WC::GetCurrentHitchance() * SEED_MAX) / HITCHANCE_MAX) };
	// get needed directional vectors.
	M::AngleVectors(angle, &fwd, &right, &up);

	// store off inaccuracy / spread ( these functions are quite intensive and we only need them once )
	inaccuracy = player->GetWeapon()->GetInaccuracy();
	spread = player->GetWeapon()->GetSpread();

	// iterate all possible seeds.
	for (int i{ }; i <= SEED_MAX; ++i) {
		// get spread.
		short nDefinitionIndex = player->GetWeapon()->GetItemDefinitionIndex();
		CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);
		wep_spread = CalculateSpread(pWeaponData, player, i, inaccuracy, spread);

		// get spread direction.
		dir = (fwd + (right * wep_spread.x) + (up * wep_spread.y)).Normalized();
		// get end of trace.
		end = start + (dir * pWeaponData->flRange);

		// setup ray and trace.
		I::EngineTrace->ClipRayToEntity(Ray_t(start, end), MASK_SHOT, enemyPlayer, &tr);

		// check if we hit a valid player / hitgroup on the player and increment total hits.
		if (tr.pHitEntity == enemyPlayer)
			++total_hits;

		// we made it.
		if (total_hits >= needed_hits)
			return true;

		// we cant make it anymore.
		if ((SEED_MAX - i + total_hits) < needed_hits)
			return false;
	}

	return false;
}


Vector CRageBot::CalculateSpread(CCSWeaponData* data, CBaseEntity* player, int seed, float inaccuracy, float spread, bool revolver2) {
	int        item_def_index;
	float      recoil_index, r1, r2, r3, r4, s1, c1, s2, c2;

	CCSWeaponData* wep_info = data;
	if (!wep_info || !wep_info->iBullets)
		return {};

	item_def_index = player->GetWeapon()->GetItemDefinitionIndex();
	recoil_index = 0.f;//I::ConVar->FindVar(XorStr("weapon_recoil_scale"))->GetFloat();

	RandomSeedFn((seed & 0xff) + 1);

	r1 = M::RandomFloatFunc(0.f, 1.f);
	r2 = M::RandomFloatFunc(0.f, PI);
	r3 = M::RandomFloatFunc(0.f, 1.f);
	r4 = M::RandomFloatFunc(0.f, PI);

	if (item_def_index == WEAPON_REVOLVER && revolver2) {
		r1 = 1.f - (r1 * r1);
		r3 = 1.f - (r3 * r3);
	}

	else if (item_def_index == WEAPON_NEGEV && recoil_index < 3.f) {
		for (int i{ 3 }; i > recoil_index; --i) {
			r1 *= r1;
			r3 *= r3;
		}

		r1 = 1.f - r1;
		r3 = 1.f - r3;
	}

	c1 = std::cos(r2);
	c2 = std::cos(r4);
	s1 = std::sin(r2);
	s2 = std::sin(r4);

	return {
		(c1 * (r1 * inaccuracy)) + (c2 * (r3 * spread)),
		(s1 * (r1 * inaccuracy)) + (s2 * (r3 * spread)),
		0.f
	};
}