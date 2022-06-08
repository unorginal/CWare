#pragma once
// used: winapi includes, singleton
#include "../common.h"
// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: angle
#include "../sdk/datatypes/qangle.h"
// used: baseentity
#include "../sdk/entity.h"
// @note: FYI - https://www.unknowncheats.me/forum/general-programming-and-reversing/173986-math-hack-2-predicting-future-3d-kinematics.html

class CRageBot : public CSingleton<CRageBot>
{
public:
	// Get
	void Run(CUserCmd* pCmd, CBaseEntity* pLocal, bool& bSendPacket);
	CBaseEntity* FindBestTarget(CBaseEntity* pLocal);
	bool CheckHitchance(CBaseEntity* player, CBaseEntity* enemyPlayer, const QAngle& angle);
	void DoNoRecoil(CBaseEntity* pLocal, CUserCmd* pCmd);
	Vector CalculateSpread(CCSWeaponData* data, CBaseEntity* player, int seed, float inaccuracy, float spread, bool revolver2 = false);
	float BestHitPoint(CBaseEntity* player, int prioritized, float minDmg, mstudiohitboxset_t* hitset, matrix3x4_t matrix[], Vector& vecOut);
	Vector CalculateBestPoint(CBaseEntity* player, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[]);
	// Global Values
	void automatic_revolver(CUserCmd* cmd, CBaseCombatWeapon& weapon);
	bool hitbox_intersection(CBaseEntity* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end, float* safe = nullptr);
	float serverTime(CUserCmd* cmd) noexcept;
	CBaseEntity* pBestEntity = nullptr;
	int get_minimum_damage(bool visible, int health);
	int hitchance(const QAngle& aim_angle, CBaseEntity* finalTarget, int hitbox);
	static int clip_ray_to_hitbox(const Ray_t& ray, mstudiobbox_t* hitbox, matrix3x4_t& matrix, Trace_t& trace)
	{
		static auto fn = (MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 F3 0F 10 42")));

		trace.flFraction = 1.0f;
		trace.bStartSolid = false;

		return reinterpret_cast <int(__fastcall*)(const Ray_t&, mstudiobbox_t*, matrix3x4_t&, Trace_t&)> (fn)(ray, hitbox, matrix, trace);
	}
private:
	// Main
	/* get entities, choose best target, aim */

	// Other
	/* 3-rd party functions */

	// Check
	/* is entity valid */

	// Values
	/* hitbox position other info */
};
