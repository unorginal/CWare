#pragma once
#include "../common.h"
#include "../utilities.h"
#include "../global.h"
// used: engine, globals, cliententitylist interfaces
#include "../core/interfaces.h"
#include "../utilities/math.h"
#include "../features/lagcompensation.h"
#include <deque>


class CResolver : public CSingleton<CResolver>
{
public:
	// Get
	void Run(CBaseEntity* pEntity);
	/* check for hit */
	void Log();
	void Event(IGameEvent* pEvent, const FNV1A_t uNameHash);
	void Resolve();
	void Override();
	int GetNumberOfTicksChoked(CBaseEntity* pEntity);
	bool CheckTarget(CBaseEntity* pEntity, CBaseEntity* pLocal);
	float GetMaxDesyncDelta(CCSGOPlayerAnimState* pAnimState);
	std::string resolver_stage[65];
	float resolved_abs_yaw;
	/* angles, lby states, shots counters etc*/
private:
	// Main
	/* ur resolver */

};