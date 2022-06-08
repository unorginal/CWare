#pragma once
// used: define to add values to variables vector
#include "config.h"
#include <vector>
//#include "../WeaponConfig.h"
#pragma region variables_array_entries
enum EVisualsInfoFlags : int
{
	INFO_FLAG_HELMET = 0,
	INFO_FLAG_KEVLAR,
	INFO_FLAG_KIT,
	INFO_FLAG_ZOOM,
	INFO_FLAG_MAX
};

enum EVisualsRemovals : int
{
	REMOVAL_POSTPROCESSING = 0,
	REMOVAL_PUNCH,
	REMOVAL_SMOKE,
	REMOVAL_SCOPE,
	REMOVAL_MAX
};

enum EAimbotHitboxes : int
{
	HEAD = 0,
	NECK,
	UPPER_CHEST,
	CHEST,
	ARMS,
	STOMACH,
	PELVIS,
	LEGS,
	FEET,
	HITBOXES_ALL
};
#pragma endregion

#pragma region variables_combo_entries
enum class EAntiAimPitchType : int
{
	NONE = 0,
	CUSTOM,
	JITTER,
	RANDOM,
	ZERO
};

enum class EAntiAimYawType : int
{
	NONE = 0,
	CUSTOM,
	JITTER,
	RANDOM,
	DESYNC,
	DESYNCBACKWARDS
};

enum class EVisualsBoxType : int
{
	NONE = 0,
	FULL,
	CORNERS
};

enum class EVisualsGlowStyle : int
{
	OUTER = 0,
	COVER,
	INNER
};
enum ERageBotFakelag : int {
	FAKELAG_STANDING,
	FAKELAG_MOVING,
	FAKELAG_AIR,
	FAKELAG_DUCKING,
	FAKELAG_NONE,
	FAKELAG_WEAPON_ACTIVITY,
	FAKELAG_ONSHOT,
	FAKELAG_MAX
};
enum class EVisualsPlayersChams : int
{
	COVERED = 0,
	FLAT,
	WIREFRAME,
	REFLECTIVE
};

enum class EVisualsViewModelChams : int
{
	NO_DRAW = 0,
	COVERED,
	FLAT,
	WIREFRAME,
	GLOW,
	SCROLL,
	CHROME
};
enum class EWeaponCategory : int
{
	GENERAL = 0,
	PISTOLS,
	HEAVYPISTOL,
	SCOUT,
	AWP,
	AUTOSNIPER
};
#pragma endregion

struct Variables_t
{
	#pragma region variables_rage
	// aimbot
	C_ADD_VARIABLE(bool, bRage, false);

	// antiaim
	C_ADD_VARIABLE(bool, bAntiAim, false);
	C_ADD_VARIABLE(int, iAntiAimPitch, 0);
	C_ADD_VARIABLE(int, iAntiAimYaw, 0);
	C_ADD_VARIABLE(float, fAntiAimPitchAmount, 89.0f);
	C_ADD_VARIABLE(float, fAntiAimYawAmount, -189.0f);
	C_ADD_VARIABLE(int, iAntiAimDesyncAmount, 0);
	C_ADD_VARIABLE(bool, bAntiAimPitchJitter, false);
	C_ADD_VARIABLE(bool, bAntiAimAtTarget, false);
	C_ADD_VARIABLE(bool, bAntiAimYawJitter, false);
	C_ADD_VARIABLE(int, iFakelagTicks, 4);
	C_ADD_VARIABLE_VECTOR(bool, FAKELAG_MAX, vecMiscFakeLagType, true);
	C_ADD_VARIABLE(int, iAntiAimDesyncKey, VK_XBUTTON1);
	#pragma endregion

	#pragma region variables_legit
	// aimbot
	C_ADD_VARIABLE(bool, bLegit, false);
	C_ADD_VARIABLE(int, iAimbotType, 0);
	C_ADD_VARIABLE(float, fMaxFovDistance, 1.0f);
	C_ADD_VARIABLE(float, fSmoothAmount, 0.99f);
	C_ADD_VARIABLE(bool, bSilentAim, false);
	C_ADD_VARIABLE(int, iAimbotKey, 0);
	C_ADD_VARIABLE(bool, bAlwaysShowFovCircle, false);
	C_ADD_VARIABLE(bool, bVisibleCheck, false);
	C_ADD_VARIABLE(bool, bSmokeCheck, false);
	C_ADD_VARIABLE(bool, bScopeCheck, false);
	C_ADD_VARIABLE(bool, bRCS, false);


	/*
	
	RAGE AIMBOT TAB
	
	*/

	C_ADD_VARIABLE(int, iRageWeaponType, 0);
	C_ADD_VARIABLE(int, iRageMinDamageOverrideKey, 0);
	C_ADD_VARIABLE(int, iRageDoubleTapKey, 0);

	// Rage Aimbot (General)
	C_ADD_VARIABLE(float, fRageMinDamage, 100.0f);
	C_ADD_VARIABLE(bool, bUseMultipoint, false);
	C_ADD_VARIABLE(float, fMultipointScale, 1.0f);
	C_ADD_VARIABLE(int, iHitchanceRange, 80);
	C_ADD_VARIABLE(bool, bSeeAngles, false);
	C_ADD_VARIABLE(bool, bResolver, true);
	C_ADD_VARIABLE(bool, bAutoDuck, false);
	C_ADD_VARIABLE(bool, bAutoStop, false);
	C_ADD_VARIABLE_VECTOR(bool, HITBOXES_ALL, vHitboxes, false);

	// Pistols
	C_ADD_VARIABLE(float, fPistolRageMinDamage, 100.0f);
	C_ADD_VARIABLE(bool, bPistolUseMultipoint, false);
	C_ADD_VARIABLE(float, fPistolMultipointScale, 1.0f);
	C_ADD_VARIABLE(int, iPistolHitchanceRange, 80);
	C_ADD_VARIABLE(bool, bPistolSeeAngles, false);
	C_ADD_VARIABLE(bool, bPistolResolver, true);
	C_ADD_VARIABLE(bool, bPistolAutoDuck, false);
	C_ADD_VARIABLE(bool, bPistolAutoStop, false);
	C_ADD_VARIABLE_VECTOR(bool, HITBOXES_ALL, vPistolHitboxes, false);

	// Heavy Pistols
	C_ADD_VARIABLE(float, fHVPistolRageMinDamage, 100.0f);
	C_ADD_VARIABLE(bool, bHVPistolUseMultipoint, false);
	C_ADD_VARIABLE(float, fHVPistolMultipointScale, 1.0f);
	C_ADD_VARIABLE(int, iHVPistolHitchanceRange, 80);
	C_ADD_VARIABLE(bool, bHVPistolSeeAngles, false);
	C_ADD_VARIABLE(bool, bHVPistolResolver, true);
	C_ADD_VARIABLE(bool, bHVPistolAutoDuck, false);
	C_ADD_VARIABLE(bool, bHVPistolAutoStop, false);
	C_ADD_VARIABLE_VECTOR(bool, HITBOXES_ALL, vHVPistolHitboxes, false);

	// Scout
	C_ADD_VARIABLE(float, fScoutRageMinDamage, 100.0f);
	C_ADD_VARIABLE(bool, bScoutUseMultipoint, false);
	C_ADD_VARIABLE(float, fScoutMultipointScale, 1.0f);
	C_ADD_VARIABLE(int, iScoutHitchanceRange, 80);
	C_ADD_VARIABLE(bool, bScoutSeeAngles, false);
	C_ADD_VARIABLE(bool, bScoutResolver, true);
	C_ADD_VARIABLE(bool, bScoutAutoDuck, false);
	C_ADD_VARIABLE(bool, bScoutAutoStop, false);
	C_ADD_VARIABLE_VECTOR(bool, HITBOXES_ALL, vScoutHitboxes, false);

	// AWP
	C_ADD_VARIABLE(float, fAWPRageMinDamage, 100.0f);
	C_ADD_VARIABLE(bool, bAWPUseMultipoint, false);
	C_ADD_VARIABLE(float, fAWPMultipointScale, 1.0f);
	C_ADD_VARIABLE(int, iAWPHitchanceRange, 80);
	C_ADD_VARIABLE(bool, bAWPSeeAngles, false);
	C_ADD_VARIABLE(bool, bAWPResolver, true);
	C_ADD_VARIABLE(bool, bAWPAutoDuck, false);
	C_ADD_VARIABLE(bool, bAWPAutoStop, false);
	C_ADD_VARIABLE_VECTOR(bool, HITBOXES_ALL, vAWPHitboxes, false);

	// Auto Sniper
	C_ADD_VARIABLE(float, fAutoRageMinDamage, 100.0f);
	C_ADD_VARIABLE(bool, bAutoUseMultipoint, false);
	C_ADD_VARIABLE(float, fAutoMultipointScale, 1.0f);
	C_ADD_VARIABLE(int, iAutoHitchanceRange, 80);
	C_ADD_VARIABLE(bool, bAutoSeeAngles, false);
	C_ADD_VARIABLE(bool, bAutoResolver, true);
	C_ADD_VARIABLE(bool, bAutoSniperAutoDuck, false);
	C_ADD_VARIABLE(bool, bAutoSniperAutoStop, false);
	C_ADD_VARIABLE_VECTOR(bool, HITBOXES_ALL, vAutoHitboxes, false);


	//C_ADD_VARIABLE(WeaponConfig[], weaponConfigs, {});
	// trigger
	C_ADD_VARIABLE(bool, bTrigger, false);
	C_ADD_VARIABLE(int, iTriggerKey, 0);
	C_ADD_VARIABLE(int, iTriggerDelay, 0);
	C_ADD_VARIABLE(bool, bTriggerAutoWall, false);
	C_ADD_VARIABLE(int, iTriggerMinimalDamage, 70);

	C_ADD_VARIABLE(bool, bTriggerHead, true);
	C_ADD_VARIABLE(bool, bTriggerChest, true);
	C_ADD_VARIABLE(bool, bTriggerStomach, true);
	C_ADD_VARIABLE(bool, bTriggerArms, false);
	C_ADD_VARIABLE(bool, bTriggerLegs, false);
	#pragma endregion

	#pragma region variables_visuals
	C_ADD_VARIABLE(bool, bEsp, true);

	// main
	C_ADD_VARIABLE(bool, bEspMain, false);
	C_ADD_VARIABLE(bool, bEspMainEnemies, false);
	C_ADD_VARIABLE(bool, bEspMainAllies, false);
	C_ADD_VARIABLE(bool, bEspMainWeapons, false);
	C_ADD_VARIABLE(bool, bEspMainGrenades, false);
	C_ADD_VARIABLE(bool, bEspMainBomb, false);

	// players
	C_ADD_VARIABLE(int, iEspMainPlayerBox, static_cast<int>(EVisualsBoxType::FULL));
	C_ADD_VARIABLE(Color, colEspMainBoxEnemies, Color(160, 60, 60, 255));
	C_ADD_VARIABLE(Color, colEspMainBoxEnemiesWall, Color(200, 185, 110, 255));
	C_ADD_VARIABLE(Color, colEspMainBoxAllies, Color(0, 200, 170, 255));
	C_ADD_VARIABLE(Color, colEspMainBoxAlliesWall, Color(170, 110, 200, 255));

	C_ADD_VARIABLE(bool, bEspMainPlayerFarRadar, false);
	C_ADD_VARIABLE(bool, bEspMainPlayerInfo, false);

	/* left */
	C_ADD_VARIABLE(bool, bEspMainPlayerHealth, true);
	C_ADD_VARIABLE(bool, bEspMainPlayerMoney, false);

	/* top */
	C_ADD_VARIABLE(bool, bEspMainPlayerName, true);
	C_ADD_VARIABLE(bool, bEspMainPlayerFlash, false);

	/* right */
	C_ADD_VARIABLE_VECTOR(bool, INFO_FLAG_MAX, vecEspMainPlayerFlags, true);

	/* bottom */
	C_ADD_VARIABLE(bool, bEspMainPlayerWeapons, true);
	C_ADD_VARIABLE(bool, bEspMainPlayerAmmo, true);
	C_ADD_VARIABLE(bool, bEspMainPlayerDistance, false);

	// weapons
	C_ADD_VARIABLE(int, iEspMainWeaponBox, static_cast<int>(EVisualsBoxType::NONE));
	C_ADD_VARIABLE(Color, colEspMainBoxWeapons, Color(255, 255, 255, 220));

	C_ADD_VARIABLE(bool, bEspMainWeaponInfo, false);

	C_ADD_VARIABLE(bool, bEspMainWeaponIcon, true);
	C_ADD_VARIABLE(bool, bEspMainWeaponAmmo, true);
	C_ADD_VARIABLE(bool, bEspMainWeaponDistance, false);

	// glow
	C_ADD_VARIABLE(bool, bEspGlow, false);
	C_ADD_VARIABLE(bool, bEspGlowEnemies, false);
	C_ADD_VARIABLE(bool, bEspGlowAllies, false);
	C_ADD_VARIABLE(bool, bEspGlowWeapons, false);
	C_ADD_VARIABLE(bool, bEspGlowGrenades, false);
	C_ADD_VARIABLE(bool, bEspGlowBomb, false);

	C_ADD_VARIABLE(bool, bEspGlowBloom, false);

	C_ADD_VARIABLE(Color, colEspGlowEnemies, Color(230, 20, 60, 128));
	C_ADD_VARIABLE(Color, colEspGlowEnemiesWall, Color(255, 235, 240, 128));
	C_ADD_VARIABLE(Color, colEspGlowAllies, Color(85, 140, 255, 128));
	C_ADD_VARIABLE(Color, colEspGlowAlliesWall, Color(240, 235, 255, 128));
	C_ADD_VARIABLE(Color, colEspGlowWeapons, Color(80, 0, 225, 128));
	C_ADD_VARIABLE(Color, colEspGlowGrenades, Color(180, 130, 30, 128));
	C_ADD_VARIABLE(Color, colEspGlowBomb, Color(140, 220, 80, 128));
	C_ADD_VARIABLE(Color, colEspGlowBombPlanted, Color(200, 210, 80, 128));

	// chams
	C_ADD_VARIABLE(bool, bEspChams, false);
	C_ADD_VARIABLE(bool, bEspChamsEnemies, false);
	C_ADD_VARIABLE(bool, bEspChamsAllies, false);
	C_ADD_VARIABLE(bool, bEspChamsViewModel, false);

	C_ADD_VARIABLE(bool, bEspChamsXQZ, false);
	C_ADD_VARIABLE(bool, bEspChamsDesync, false);
	C_ADD_VARIABLE(bool, bEspChamsDisableOcclusion, false);
	C_ADD_VARIABLE(int, iEspChamsPlayer, static_cast<int>(EVisualsPlayersChams::COVERED));
	C_ADD_VARIABLE(int, iEspChamsViewModel, static_cast<int>(EVisualsViewModelChams::WIREFRAME));
	C_ADD_VARIABLE(Color, colEspChamsEnemies, Color(180, 65, 65, 255));
	C_ADD_VARIABLE(Color, colEspChamsEnemiesWall, Color(180, 180, 40, 255));
	C_ADD_VARIABLE(Color, colEspChamsAllies, Color(70, 40, 190, 255));
	C_ADD_VARIABLE(Color, colEspChamsAlliesWall, Color(50, 150, 150, 255));
	C_ADD_VARIABLE(Color, colEspChamsViewModel, Color(80, 255, 45, 255));
	C_ADD_VARIABLE(Color, colEspChamsViewModelAdditional, Color(0, 0, 255, 255));

	// world
	C_ADD_VARIABLE(bool, bWorld, true);
	C_ADD_VARIABLE(bool, bWorldNightMode, false);
	C_ADD_VARIABLE(bool, bWorldChangeSkybox, false);
	C_ADD_VARIABLE(int, iWorldMaxFlash, 100);
	C_ADD_VARIABLE(int, iWorldThirdPersonKey, 0);
	C_ADD_VARIABLE(int, iWorldSkyType, 0);
	C_ADD_VARIABLE(float, flWorldThirdPersonOffset, 150.f);
	C_ADD_VARIABLE(bool, bWorldFullbright, false)
	C_ADD_VARIABLE_VECTOR(bool, REMOVAL_MAX, vecWorldRemovals, false);

	// on-screen
	C_ADD_VARIABLE(bool, bScreen, false);
	C_ADD_VARIABLE(float, flScreenCameraFOV, 0.f);
	C_ADD_VARIABLE(float, flScreenViewModelFOV, 0.f);
	C_ADD_VARIABLE(float, flScopedFov, 90.f);
	C_ADD_VARIABLE(bool, bScreenOverwriteZoom, false);
	C_ADD_VARIABLE(bool, bScreenHitMarker, false);
	C_ADD_VARIABLE(bool, bCrosshairAlwaysOn, false);
	C_ADD_VARIABLE(bool, bScreenHitMarkerDamage, false);
	C_ADD_VARIABLE(bool, bScreenHitMarkerSound, false);
	C_ADD_VARIABLE(float, flScreenHitMarkerTime, 1.0f);
	C_ADD_VARIABLE(int, iScreenHitMarkerGap, 5);
	C_ADD_VARIABLE(int, iScreenHitMarkerLenght, 10);
	C_ADD_VARIABLE(Color, colScreenHitMarker, Color(255, 255, 255, 255));
	C_ADD_VARIABLE(Color, colScreenHitMarkerDamage, Color(200, 55, 20, 255));
	#pragma endregion

	#pragma region variables_misc
	// movement
	C_ADD_VARIABLE(bool, bMiscBunnyHop, false);
	C_ADD_VARIABLE(int, iMiscBunnyHopChance, 100);
	C_ADD_VARIABLE(bool, bMiscAutoStrafe, false);

	C_ADD_VARIABLE(bool, bMiscFakeLag, false);
	C_ADD_VARIABLE(bool, bMiscAutoAccept, false);
	C_ADD_VARIABLE(bool, bMiscAutoPistol, false);
	C_ADD_VARIABLE(bool, bMiscNoCrouchCooldown, false);

	// exploits
	C_ADD_VARIABLE(bool, bMiscPingSpike, false);
	C_ADD_VARIABLE(bool, bMiscSvCheat, false);
	C_ADD_VARIABLE(bool, bMiscAntiCrash, false);
	C_ADD_VARIABLE(float, flMiscLatencyFactor, 0.4f);
	C_ADD_VARIABLE(bool, bMiscRevealRanks, false);
	C_ADD_VARIABLE(bool, bMiscUnlockInventory, false);
	C_ADD_VARIABLE(bool, bNoRecoilHax, false);
	C_ADD_VARIABLE(bool, bMiscHiddenCVars, false);
	C_ADD_VARIABLE(bool, bMiscBacktrack, false);
	C_ADD_VARIABLE(bool, bMiscAntiUntrusted, true);
	// Supa Misc
	C_ADD_VARIABLE(std::string, sClanTagText, "catware.dev");
	C_ADD_VARIABLE(bool, bMiscSpectatorList, false);
	C_ADD_VARIABLE(bool, bShowBulletImpacts, false);
	C_ADD_VARIABLE(bool, bShowSpread, false);
	C_ADD_VARIABLE(bool, bMiscDoClantag, false);
	C_ADD_VARIABLE(bool, bMiscDoDomToretto, false);
	#pragma endregion

	#pragma region variables_menu
	C_ADD_VARIABLE(int, iMenuKey, VK_INSERT);
	C_ADD_VARIABLE(int, iPanicKey, VK_END);
	C_ADD_VARIABLE(int, iOpenTab, 0);
	C_ADD_VARIABLE(int, iOpenEspTab, 0);
	C_ADD_VARIABLE(int, iWidthSize, 580);
	C_ADD_VARIABLE(int, iHeightSize, 630);
	//C_ADD_VARIABLE(ImVec2, ivSpectatorPos, ImVec2(C::GetScreenSize().x / 2, 200));
	C_ADD_VARIABLE(bool, bShowWatermark, true);
	C_ADD_VARIABLE(bool, bShowSpectators, false);
	C_ADD_VARIABLE(Color, colMenuColor, Color(180, 197, 213, 255));
	C_ADD_VARIABLE(Color, colFovCircle, Color(255, 255, 255, 80));
	#pragma endregion
};

inline Variables_t Vars;
