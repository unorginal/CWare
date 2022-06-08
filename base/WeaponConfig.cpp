#include "WeaponConfig.h"

int WC::GetCurrentHitchance() {
	CBaseCombatWeapon* pWeapon = G::pLocal->GetWeapon();
	int definitionIndex = pWeapon->GetItemDefinitionIndex();
	int currentHitchanceRange;
	switch (definitionIndex) {
	case WEAPON_SSG08:
		currentHitchanceRange =  C::Get<int>(Vars.iScoutHitchanceRange);
		break;
	case WEAPON_AWP:
		currentHitchanceRange = C::Get<int>(Vars.iAWPHitchanceRange);
		break;
	case WEAPON_DEAGLE:
		currentHitchanceRange = C::Get<int>(Vars.iHVPistolHitchanceRange);
		break;
	case WEAPON_REVOLVER:
		currentHitchanceRange = C::Get<int>(Vars.iHVPistolHitchanceRange);
		break;
	case WEAPON_G3SG1:
		currentHitchanceRange = C::Get<int>(Vars.iAutoHitchanceRange);
		break;
	case WEAPON_SCAR20:
		currentHitchanceRange = C::Get<int>(Vars.iAutoHitchanceRange);
		break;
	default:
		currentHitchanceRange = C::Get<int>(Vars.iHitchanceRange);
		break;
	}
	return currentHitchanceRange;
}

float WC::GetCurrentMinDamage() {
	CBaseCombatWeapon* pWeapon = G::pLocal->GetWeapon();
	int definitionIndex = pWeapon->GetItemDefinitionIndex();
	float currentMinDmg;
	switch (definitionIndex) {
	case WEAPON_SSG08:
		currentMinDmg = C::Get<float>(Vars.fScoutRageMinDamage);
		break;
	case WEAPON_AWP:
		currentMinDmg = C::Get<float>(Vars.fAWPRageMinDamage);
		break;
	case WEAPON_DEAGLE:
		currentMinDmg = C::Get<float>(Vars.fHVPistolRageMinDamage);
		break;
	case WEAPON_REVOLVER:
		currentMinDmg = C::Get<float>(Vars.fHVPistolRageMinDamage);
		break;
	case WEAPON_G3SG1:
		currentMinDmg = C::Get<float>(Vars.fAutoRageMinDamage);
		break;
	case WEAPON_SCAR20:
		currentMinDmg = C::Get<float>(Vars.fAutoRageMinDamage);
		break;
	default:
		currentMinDmg = C::Get<float>(Vars.fRageMinDamage);
		break;
	}
	return currentMinDmg;
}

std::vector<bool> WC::GetCurrentHitboxes() {
	CBaseCombatWeapon* pWeapon = G::pLocal->GetWeapon();
	int definitionIndex = pWeapon->GetItemDefinitionIndex();
	switch (definitionIndex) {
	case WEAPON_SSG08:
		return C::Get<std::vector<bool>>(Vars.vScoutHitboxes);
		break;
	case WEAPON_AWP:
		return C::Get<std::vector<bool>>(Vars.vAWPHitboxes);
		break;
	case WEAPON_DEAGLE:
		return C::Get<std::vector<bool>>(Vars.vHVPistolHitboxes);
		break;
	case WEAPON_REVOLVER:
		return C::Get<std::vector<bool>>(Vars.vHVPistolHitboxes);
		break;
	case WEAPON_G3SG1:
		return C::Get<std::vector<bool>>(Vars.vAutoHitboxes);
		break;
	case WEAPON_SCAR20:
		return C::Get<std::vector<bool>>(Vars.vAutoHitboxes);
		break;
	default:
		return C::Get<std::vector<bool>>(Vars.vHitboxes);
		break;
	}
}

std::vector<int> WC::GetCurrentHitboxesI() {
	std::vector<bool> currentHitboxes = WC::GetCurrentHitboxes();
	std::vector<int> selectedHitboxes;

	if (currentHitboxes.at(EAimbotHitboxes::HEAD)) {
		selectedHitboxes.push_back(HITBOX_HEAD);
	}
	if (currentHitboxes.at(EAimbotHitboxes::NECK)) {
		selectedHitboxes.push_back(HITBOX_NECK);
	}
	if (currentHitboxes.at(EAimbotHitboxes::UPPER_CHEST)) {
		selectedHitboxes.push_back(HITBOX_UPPER_CHEST);
	}
	if (currentHitboxes.at(EAimbotHitboxes::CHEST)) {
		selectedHitboxes.push_back(HITBOX_CHEST);
	}
	if (currentHitboxes.at(EAimbotHitboxes::PELVIS)) {
		selectedHitboxes.push_back(HITBOX_PELVIS);
	}
	if (currentHitboxes.at(EAimbotHitboxes::STOMACH)) {
		selectedHitboxes.push_back(HITBOX_STOMACH);
	}
	if (currentHitboxes.at(EAimbotHitboxes::ARMS)) {
		selectedHitboxes.push_back(HITBOX_LEFT_FOREARM);
		selectedHitboxes.push_back(HITBOX_RIGHT_FOREARM);
		selectedHitboxes.push_back(HITBOX_LEFT_HAND);
		selectedHitboxes.push_back(HITBOX_RIGHT_HAND);
	}
	if (currentHitboxes.at(EAimbotHitboxes::LEGS)) {
		selectedHitboxes.push_back(HITBOX_LEFT_THIGH);
		selectedHitboxes.push_back(HITBOX_RIGHT_THIGH);
		selectedHitboxes.push_back(HITBOX_LEFT_CALF);
		selectedHitboxes.push_back(HITBOX_RIGHT_CALF);
	}
	if (currentHitboxes.at(EAimbotHitboxes::FEET)) {
		selectedHitboxes.push_back(HITBOX_LEFT_FOOT);
		selectedHitboxes.push_back(HITBOX_RIGHT_FOOT);
	}
	return selectedHitboxes;
}

int WC::GetCurrentHitboxCount() {
	std::vector<bool> currentHitboxes = GetCurrentHitboxes();
	int hitboxCount = 0;
	if (currentHitboxes.at(EAimbotHitboxes::HEAD)) {
		hitboxCount += 1;
	}
	if (currentHitboxes.at(EAimbotHitboxes::NECK)) {
		hitboxCount += 1;
	}
	if (currentHitboxes.at(EAimbotHitboxes::UPPER_CHEST)) {
		hitboxCount += 1;
	}
	if (currentHitboxes.at(EAimbotHitboxes::CHEST)) {
		hitboxCount += 1;
	}
	if (currentHitboxes.at(EAimbotHitboxes::PELVIS)) {
		hitboxCount += 1;
	}
	if (currentHitboxes.at(EAimbotHitboxes::STOMACH)) {
		hitboxCount += 1;
	}
	if (currentHitboxes.at(EAimbotHitboxes::ARMS)) {
		hitboxCount += 4;
	}
	if (currentHitboxes.at(EAimbotHitboxes::LEGS)) {
		hitboxCount += 4;
	}
	if (currentHitboxes.at(EAimbotHitboxes::FEET)) {
		hitboxCount += 2;
	}
	return hitboxCount;
}