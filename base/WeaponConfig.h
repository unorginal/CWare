#pragma once
#include "./global.h"
#include "./common.h"
#include "./core/variables.h"
#include "./core/interfaces.h"

namespace WC {
	int GetCurrentHitchance();
	float GetCurrentMinDamage();
	std::vector<bool> GetCurrentHitboxes();
	int GetCurrentHitboxCount();
	std::vector<int> GetCurrentHitboxesI();
}