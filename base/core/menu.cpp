#include "menu.h"

// used: global variables
#include "../global.h"
// used: config variables
#include "../core/variables.h"
// used: actions with config
#include "../core/config.h"
// used: configurations error logging
#include "../utilities/logging.h"
// used: render setup, etc
#include "../utilities/draw.h"
// used: engine, inputsystem, convar interfaces
#include "../core/interfaces.h"
// used: render visuals
#include "../features/visuals.h"
// used: skinchanger tab items map
#include "../features/skinchanger.h"
#include "../common.h"
// used: inputtext() wrappers for c++ standard library (stl) type: std::string
#include "../../dependencies/imgui/cpp/imgui_stdlib.h"
#include "../common.h"
#include "../utilities.h"
#include "../features/misc.h"
#include "../LuaEngine.h"

const std::pair<const char*, std::uint32_t> arrColors[] =
{
	{ "[esp] box - enemies", Vars.colEspMainBoxEnemies },
	{ "[esp] box - enemies wall", Vars.colEspMainBoxEnemiesWall },
	{ "[esp] box - allies", Vars.colEspMainBoxAllies },
	{ "[esp] box - allies wall", Vars.colEspMainBoxAlliesWall },
	{ "[glow] enemies", Vars.colEspGlowEnemies },
	{ "[glow] enemies wall", Vars.colEspGlowEnemiesWall },
	{ "[glow] allies", Vars.colEspGlowAllies },
	{ "[glow] allies wall", Vars.colEspGlowAlliesWall },
	{ "[glow] weapons", Vars.colEspGlowWeapons },
	{ "[glow] grenades", Vars.colEspGlowGrenades },
	{ "[glow] bomb", Vars.colEspGlowBomb },
	{ "[glow] planted bomb", Vars.colEspGlowBombPlanted },
	{ "[chams] enemies", Vars.colEspChamsEnemies },
	{ "[chams] enemies wall", Vars.colEspChamsEnemiesWall },
	{ "[chams] allies", Vars.colEspChamsAllies },
	{ "[chams] allies wall", Vars.colEspChamsAlliesWall },
	{ "[chams] viewmodel", Vars.colEspChamsViewModel },
	{ "[chams] viewmodel additional", Vars.colEspChamsViewModelAdditional },
	{ "[screen] hitmarker - lines", Vars.colScreenHitMarker },
	{ "[screen] hitmarker - damage", Vars.colScreenHitMarkerDamage }
};

#pragma region menu_array_entries
static constexpr std::array<std::string_view, 4U> arrVisualsFlags =
{
	"helmet",
	"kevlar",
	"defuse kit",
	"zoom"
};

static constexpr std::array<std::string_view, 4U> arrVisualsRemovals =
{
	"post-processing",
	"punch",
	"smoke",
	"scope"
};
static constexpr std::array<std::string_view, 9U> arrHitboxes =
{
	"head",
	"neck",
	"upper-chest",
	"chest",
	"stomach",
	"pelvis",
	"arms",
	"legs",
	"feet"
};
static constexpr std::array<std::string_view, 6U> fakeLagModes =
{
	"Standing",
	"Moving",
	"Air",
	"Ducking",
	"Weapon-Activity",
	"Onshot"
};
#pragma endregion

// spectator list, radar, other stuff here
#pragma region menu_windows
void W::MainWindow(IDirect3DDevice9* pDevice)
{
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 vecScreenSize = io.DisplaySize;
	ImU32 coolRGBGamerThingColorThing = (C::Get<Color>(Vars.colMenuColor).GetU32());
	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* pForegroundDrawList = ImGui::GetForegroundDrawList();
#pragma region main_visuals
	ImGuiStyle* styleEditor = &ImGui::GetStyle();
	styleEditor->WindowPadding = ImVec2(15, 15);
	styleEditor->WindowRounding = 0;
	styleEditor->FramePadding = ImVec2(5, 5);
	styleEditor->FrameRounding = 0;
	styleEditor->ItemSpacing = ImVec2(12, 8);
	styleEditor->ItemInnerSpacing = ImVec2(8, 6);
	styleEditor->IndentSpacing = 25.0f;
	styleEditor->ScrollbarSize = 15.0f;
	styleEditor->ScrollbarRounding = 9.0f;
	styleEditor->GrabMinSize = 5.0f;
	styleEditor->GrabRounding = 3.0f;
	styleEditor->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	styleEditor->Colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	styleEditor->Colors[ImGuiCol_WindowBg] = ImVec4(0.0470588f, 0.0470588f, 0.0470588f, 1.f);
	styleEditor->Colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_PopupBg] = ImVec4(0.2f, 0.2117f, 0.247f, 1.00f);
	styleEditor->Colors[ImGuiCol_FrameBg] = ImVec4(0.1564f, 0.1782f, 0.1956f, 1.0f);
	styleEditor->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	styleEditor->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	styleEditor->Colors[ImGuiCol_TitleBg] = ImVec4(0.176f, 0.176f, 0.176f, 1.00f);
	styleEditor->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	styleEditor->Colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_MenuBarBg] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_ScrollbarBg] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	styleEditor->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	styleEditor->Colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_Button] = ImVec4(0.1764f, 0.1882f, 0.2156f, 1.0f);
	styleEditor->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.2317f, 0.267f, 1.00f);
	styleEditor->Colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2117f, 0.247f, 1.00f);
	styleEditor->Colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	styleEditor->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	styleEditor->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	styleEditor->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	styleEditor->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	styleEditor->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	styleEditor->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	styleEditor->Colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing);
	styleEditor->WindowBorderSize = 0.0f;
	styleEditor->ChildRounding = 0.5f;
	// Watermark
	if (!I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage() && C::Get<bool>(Vars.bShowWatermark))
	{

		// Nice Watermark
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
		int x, y;
		I::InputSystem->GetCursorPosition(&x, &y);

		// set imgui mouse position
		io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(y));

		ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x - 290, 30), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(250, 35), ImGuiCond_Always);

		/*ImGui::Begin(XorStr("catware watermark"), &C::Get<bool>(Vars.bShowWatermark), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoScrollbar);

		pForegroundDrawList->AddRectFilled(ImVec2(189, 6), ImVec2(305.5, 11), coolRGBGamerThingColorThing, 3.0f, ImDrawCornerFlags_TopRight);
		ImGui::PushFont(F::Verdana);
		ImGui::Text(XorStr("catware.dev | Build Date:")); ImGui::SameLine(); ImGui::Text(__DATE__);
		ImGui::PopFont();
		ImGui::End();*/

		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.1f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(33.0f, 1.0f));
		// hmm, another one watermark
		bool will_aways_be_open = true;
		if (ImGui::Begin("gay", &will_aways_be_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			//pForegroundDrawList->AddRectFilled(ImVec2(vecScreenSize.x - 290 - (250/2), 12), ImVec2((vecScreenSize.x - 290 + (250 / 2)), 14), coolRGBGamerThingColorThing, 3.0f, ImDrawCornerFlags_TopLeft);
			//pForegroundDrawList->AddRectFilled(ImVec2(vecScreenSize.x - 290 - (250 / 2), 12), ImVec2((vecScreenSize.x - 290 + (250 / 2)), 14), coolRGBGamerThingColorThing, 3.0f, ImDrawCornerFlags_TopLeft);
			pForegroundDrawList->AddRect(ImVec2(vecScreenSize.x - 290 - (250 / 2), 14), ImVec2((vecScreenSize.x - 290 + (250 / 2)), 45), coolRGBGamerThingColorThing, 3.0f, ImDrawCornerFlags_None, 2.5f);
			ImGui::PushFont(F::Verdana);
			ImGui::Dummy(ImVec2(1, 1));
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), XorStr("catware.dev | " __DATE__));

			ImGui::PopFont();
			ImGui::End();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);


		//ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);
	}

	ImDrawList* pBackgroundDrawList = ImGui::GetBackgroundDrawList();
	D::RenderDrawData(pBackgroundDrawList);
	if (I::Engine->IsInGame()) {
		if (bMainOpened || C::Get<bool>(Vars.bAlwaysShowFovCircle)) {
			if (C::Get<int>(Vars.iAimbotType) == 0) {
				ImGuiCol circleColor = C::Get<Color>(Vars.colFovCircle).GetU32();
				pBackgroundDrawList->AddCircleFilled(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), 10.0f * C::Get<float>(Vars.fMaxFovDistance), circleColor, 100);
			}
		}
	}

	// Always On Crosshair
	if (!I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage() && I::Engine->IsInGame() && G::pLocal != nullptr && G::pLocal->IsAlive() && C::Get<bool>(Vars.bCrosshairAlwaysOn)) {
		CBaseCombatWeapon* pWeapon = G::pLocal->GetWeapon();

		if (pWeapon == nullptr) {

		}
		else {
			short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();
			CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex);
			// check is weapon gun
			if (pWeaponData == nullptr || !pWeaponData->IsGun()){}
			else {
				ImDrawList* pBackgroundDrawList = ImGui::GetBackgroundDrawList();
				ImGuiCol circleColor = C::Get<Color>(Vars.colMenuColor).GetU32();
				switch (pWeapon->GetItemDefinitionIndex()) {
				case WEAPON_SSG08:
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f) - 10.f), ImVec2( (vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f)+10.f ), circleColor, 3.0f);
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f)+10.f, (vecScreenSize.y * 0.5f)), ImVec2((vecScreenSize.x * 0.5f)-10.f, (vecScreenSize.y * 0.5f)), circleColor, 3.0f);
					break;
				case WEAPON_AWP:
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f) - 10.f), ImVec2((vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f) + 10.f), circleColor, 3.0f);
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f) + 10.f, (vecScreenSize.y * 0.5f)), ImVec2((vecScreenSize.x * 0.5f) - 10.f, (vecScreenSize.y * 0.5f)), circleColor, 3.0f);
					break;
				case WEAPON_G3SG1:
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f) - 10.f), ImVec2((vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f) + 10.f), circleColor, 3.0f);
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f) + 10.f, (vecScreenSize.y * 0.5f)), ImVec2((vecScreenSize.x * 0.5f) - 10.f, (vecScreenSize.y * 0.5f)), circleColor, 3.0f);
					break;
				case WEAPON_SCAR20:
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f) - 10.f), ImVec2((vecScreenSize.x * 0.5f), (vecScreenSize.y * 0.5f) + 10.f), circleColor, 3.0f);
					pBackgroundDrawList->AddLine(ImVec2((vecScreenSize.x * 0.5f) + 10.f, (vecScreenSize.y * 0.5f)), ImVec2((vecScreenSize.x * 0.5f) - 10.f, (vecScreenSize.y * 0.5f)), circleColor, 3.0f);
					break;
				}
			}
		}
	}

	// Spectators List

	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, 200), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(170, 200), ImGuiCond_Always);
	if ( ( (!I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage()) && C::Get<bool>(Vars.bShowSpectators))) {
		
		if (ImGui::Begin(XorStr("spectators list"), &C::Get<bool>(Vars.bShowSpectators), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImU32 coolRGBGamerThingColorThing = (C::Get<Color>(Vars.colMenuColor).GetU32());
			pForegroundDrawList->AddRectFilled(
				ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y-4),
				ImVec2(ImGui::GetWindowPos().x+(ImGui::GetWindowSize().x/2), ImGui::GetWindowPos().y+1),
				coolRGBGamerThingColorThing, 
				3.0f, 
				ImDrawCornerFlags_TopLeft
			);
			pForegroundDrawList->AddRectFilled(
				ImVec2(ImGui::GetWindowPos().x + (ImGui::GetWindowSize().x / 2), ImGui::GetWindowPos().y - 4),
				ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y+1),
				coolRGBGamerThingColorThing, 
				3.0f, 
				ImDrawCornerFlags_TopRight
			);
			std::string text = XorStr("spectators");
			float font_size = ImGui::GetFontSize() * text.size() / 2;
			ImGui::SameLine(
				ImGui::GetWindowSize().x / 2 -
				font_size + (font_size / 2)
			);
			ImGui::PushFont(F::Verdana);
			ImGui::Text(text.c_str());
			ImGui::PopFont();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			ImGui::Spacing();
			if (bMainOpened && !I::Engine->IsInGame()) {
				// tu madre
			}
			else if (C::Get<bool>(Vars.bShowSpectators) && G::pLocal->IsAlive() && I::Engine->IsInGame()) {
				for (int i = 1; i <= I::Globals->nMaxClients; i++)
				{
					CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);
					if (pEntity) {
						if (!pEntity->IsAlive() && !pEntity->IsDormant()) {
							if (pEntity->GetObserverTargetHandle() && I::ClientEntityList->Get<CBaseEntity>(pEntity->GetObserverTargetHandle()) && G::pLocal == I::ClientEntityList->Get<CBaseEntity>(pEntity->GetObserverTargetHandle())) {
								PlayerInfo_t pInfo = {};
								I::Engine->GetPlayerInfo(pEntity->GetIndex(), &pInfo);
								ImGui::PushFont(F::Verdana);
								ImGui::Text(pInfo.szName);
								ImGui::PopFont();
							}
						}
					}
				}
			}
			ImGui::Spacing();
			ImGui::End();
		}
	}
	if (!I::Engine->IsTakingScreenshot() && !I::Engine->IsDrawingLoadingImage() && I::Engine->IsInGame() && G::pLocal->IsAlive() &&  C::Get<bool>(Vars.bShowSpread)) {
		ImGuiCol circleColor = C::Get<Color>(Vars.colFovCircle).GetU32();
		CBaseCombatWeapon* pWeapon = G::pLocal->GetWeapon();
		if (pWeapon && pWeapon != nullptr) {
			pBackgroundDrawList->AddCircleFilled(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), pWeapon->GetInaccuracy() * 500.0f, circleColor, 100);
		}
	}
#pragma endregion

#pragma region main_window
	io.MouseDrawCursor = bMainOpened;

	if (bMainOpened)
	{
		int x, y;
		I::InputSystem->GetCursorPosition(&x, &y);

		// set imgui mouse position
		io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(y));

		ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y / 2), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(580, 630), ImGuiCond_Always);//C::Get<int>(Vars.iWidthSize), C::Get<int>(Vars.iHeightSize)), ImGuiCond_Always);
		styleEditor->WindowBorderSize = 0.0f;
		ImGui::Begin(XorStr("catware"), &bMainOpened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		{

			const ImVec2 vecPosition = ImGui::GetCursorScreenPos();
			const float flWindowWidth = ImGui::GetWindowWidth();
			const ImVec2 winPos = ImGui::GetWindowPos();
			ImDrawList* pWindowDrawList = ImGui::GetWindowDrawList();
			ImU32 coolRGBGamerThingColorThing = (C::Get<Color>(Vars.colMenuColor).GetU32());
			ImDrawList* pForgegroundDrawList = ImGui::GetForegroundDrawList();
			// Tab System
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0470588f, 0.0470588f, 0.0470588f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			ImGui::PushFont(F::ArialLarge);
			ImGui::Text(XorStr("Cat")); 
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing), XorStr("Ware"));
			ImGui::PopFont();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::PushFont(F::ArialBold);
			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(coolRGBGamerThingColorThing));
			ImGui::Spacing();
			// LegitBot
			if (ImGui::ButtonUnderline(XorStr("Legitbot"), ImVec2(92, 25), coolRGBGamerThingColorThing)) {
				C::Get<int>(Vars.iOpenTab) = 1;
			}
			ImGui::SameLine();

			//Ragebot
			if (ImGui::ButtonUnderline(XorStr("Ragebot"), ImVec2(92, 25), coolRGBGamerThingColorThing)) {
				C::Get<int>(Vars.iOpenTab) = 2;
			}
			ImGui::SameLine();

			// Anti-Aim
			if (ImGui::ButtonUnderline(XorStr("Anti-Aim"), ImVec2(92, 25), coolRGBGamerThingColorThing)) {
				C::Get<int>(Vars.iOpenTab) = 5;
			}
			ImGui::SameLine();

			//Visuals
			if (ImGui::ButtonUnderline(XorStr("Visuals"), ImVec2(92, 25), coolRGBGamerThingColorThing)) {
				C::Get<int>(Vars.iOpenTab) = 3;
			}
			ImGui::SameLine();

			// Miscellaneous
			if (ImGui::ButtonUnderline(XorStr("Misc"), ImVec2(92, 25), coolRGBGamerThingColorThing)) {
				C::Get<int>(Vars.iOpenTab) = 4;
			}
			ImGui::SameLine();
			// Miscellaneous
			if (ImGui::ButtonUnderline(XorStr("Settings"), ImVec2(92, 25), coolRGBGamerThingColorThing)) {
				C::Get<int>(Vars.iOpenTab) = 6;
			}
			// Pop dem ppl
			ImGui::PopStyleVar(3);
			ImGui::PopStyleColor(2);
			//ImGui::PopStyleColor(3);
			ImGui::PopFont();
			ImGui::PushFont(F::Tahoma);
			ImGui::Spacing();
			switch (C::Get<int>(Vars.iOpenTab)) {
			case 0:
				T::Welcome();
				break;
			case 1:
				T::LegitBot();
				break;
			case 2:
				T::RageBot();
				break;
			case 3:
				T::Visuals();
				break;
			case 4:
				T::Miscellaneous();
				break;
			case 5:
				T::AntiAim();
				break;
			case 6:
				T::MenuSettings();
				break;
			}
			ImGui::PopFont();
			styleEditor->WindowBorderSize = 0.0f;
			ImGui::End();
		}
	}
#pragma endregion
}
#pragma endregion

void T::Welcome() {
	ImGui::Spacing();
	ImGui::PushFont(F::Verdana);
	ImGui::Text(XorStr("Welcome to Catware"));
	ImGui::Text(XorStr("Please Select one of the tabs above to begin"));
	ImGui::PopFont();
}
#pragma region menu_tabs_main
void T::RageBot()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChildWithColor(XorStr("ragebot.aimbot"), "Ragebot", ImVec2(), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("Ragebot"), &C::Get<bool>(Vars.bRage));
			ImGui::Combo(XorStr("Weapon"), &C::Get<int>(Vars.iRageWeaponType), XorStr("General\0Pistol\0Heavy Pistols\0Scout\0AWP\0Auto Sniper\0\0"));
			switch (C::Get<int>(Vars.iRageWeaponType)) {
			case 0:
				ImGui::Text(XorStr("Ragebot Settings"));
				ImGui::Checkbox(XorStr("Resolver"), &C::Get<bool>(Vars.bResolver));
				ImGui::Checkbox(XorStr("Visualize Angles/Aim"), &C::Get<bool>(Vars.bSeeAngles));
				ImGui::SliderFloat(XorStr("Min Damage"), &C::Get<float>(Vars.fRageMinDamage), 0.0f, 150.0f, " %.0fHP");
				ImGui::Checkbox(XorStr("Multipoint"), &C::Get<bool>(Vars.bUseMultipoint));
				if (C::Get<bool>(Vars.bUseMultipoint))
					ImGui::SliderFloat(XorStr("MP Scale"), &C::Get<float>(Vars.fMultipointScale), 0.0f, 3.0f, "   %.2f");
				ImGui::SliderInt(XorStr("Hitchance"), &C::Get<int>(Vars.iHitchanceRange), 0, 100, "%dhc");
				ImGui::Text(XorStr("Ragebot Options"));
				ImGui::MultiCombo(XorStr("Hitboxes"), C::Get<std::vector<bool>>(Vars.vHitboxes), arrHitboxes.data(), arrHitboxes.size());
				ImGui::Checkbox(XorStr("Slowwalk"), &C::Get<bool>(Vars.bAutoStop));
				ImGui::Checkbox(XorStr("Auto Duck"), &C::Get<bool>(Vars.bAutoDuck));
				break;
			case 1:
				ImGui::Text(XorStr("Ragebot Settings"));
				ImGui::Checkbox(XorStr("Resolver"), &C::Get<bool>(Vars.bPistolResolver));
				ImGui::Checkbox(XorStr("Visualize Angles/Aim"), &C::Get<bool>(Vars.bPistolSeeAngles));
				ImGui::SliderFloat(XorStr("Min Damage"), &C::Get<float>(Vars.fPistolRageMinDamage), 0.0f, 150.0f, " %.0fHP");
				ImGui::Checkbox(XorStr("Multipoint"), &C::Get<bool>(Vars.bPistolUseMultipoint));
				if (C::Get<bool>(Vars.bPistolUseMultipoint))
					ImGui::SliderFloat(XorStr("Scale"), &C::Get<float>(Vars.fPistolMultipointScale), 0.0f, 3.0f, "   %.2f Scale");
				ImGui::SliderInt(XorStr("Hitchance"), &C::Get<int>(Vars.iPistolHitchanceRange), 0, 100, "%dhc");
				ImGui::Text(XorStr("Ragebot Options"));
				ImGui::MultiCombo(XorStr("Hitboxes"), C::Get<std::vector<bool>>(Vars.vPistolHitboxes), arrHitboxes.data(), arrHitboxes.size());
				ImGui::Checkbox(XorStr("Slowwalk"), &C::Get<bool>(Vars.bPistolAutoStop));
				ImGui::Checkbox(XorStr("Auto Duck"), &C::Get<bool>(Vars.bPistolAutoDuck));
				break;
			case 2:
				ImGui::Text(XorStr("Ragebot Settings"));
				ImGui::Checkbox(XorStr("Resolver"), &C::Get<bool>(Vars.bHVPistolResolver));
				ImGui::Checkbox(XorStr("Visualize Angles/Aim"), &C::Get<bool>(Vars.bHVPistolSeeAngles));
				ImGui::SliderFloat(XorStr("Min Damage"), &C::Get<float>(Vars.fHVPistolRageMinDamage), 0.0f, 150.0f, " %.0fHP");
				ImGui::Checkbox(XorStr("Multipoint"), &C::Get<bool>(Vars.bHVPistolUseMultipoint));
				if (C::Get<bool>(Vars.bHVPistolUseMultipoint))
					ImGui::SliderFloat(XorStr("Scale"), &C::Get<float>(Vars.fHVPistolMultipointScale), 0.0f, 3.0f, "   %.2f Scale");
				ImGui::SliderInt(XorStr("Hitchance"), &C::Get<int>(Vars.iHVPistolHitchanceRange), 0, 100, "%dhc");
				ImGui::Text(XorStr("Ragebot Options"));
				ImGui::MultiCombo(XorStr("Hitboxes"), C::Get<std::vector<bool>>(Vars.vHVPistolHitboxes), arrHitboxes.data(), arrHitboxes.size());
				ImGui::Checkbox(XorStr("Slowwalk"), &C::Get<bool>(Vars.bHVPistolAutoStop));
				ImGui::Checkbox(XorStr("Auto Duck"), &C::Get<bool>(Vars.bHVPistolAutoDuck));
				break;
			case 3:
				ImGui::Text(XorStr("Ragebot Settings"));
				ImGui::Checkbox(XorStr("Resolver"), &C::Get<bool>(Vars.bScoutResolver));
				ImGui::Checkbox(XorStr("Visualize Angles/Aim"), &C::Get<bool>(Vars.bScoutSeeAngles));
				ImGui::SliderFloat(XorStr("Min Damage"), &C::Get<float>(Vars.fScoutRageMinDamage), 0.0f, 150.0f, " %.0fHP");
				ImGui::Checkbox(XorStr("Multipoint"), &C::Get<bool>(Vars.bScoutUseMultipoint));
				if (C::Get<bool>(Vars.bScoutUseMultipoint))
					ImGui::SliderFloat(XorStr("Scale"), &C::Get<float>(Vars.fScoutMultipointScale), 0.0f, 3.0f, "   %.2f Scale");
				ImGui::SliderInt(XorStr("Hitchance"), &C::Get<int>(Vars.iScoutHitchanceRange), 0, 100, "%dhc");
				ImGui::Text(XorStr("Ragebot Options"));
				ImGui::MultiCombo(XorStr("Hitboxes"), C::Get<std::vector<bool>>(Vars.vScoutHitboxes), arrHitboxes.data(), arrHitboxes.size());
				ImGui::Checkbox(XorStr("Slowwalk"), &C::Get<bool>(Vars.bScoutAutoStop));
				ImGui::Checkbox(XorStr("Auto Duck"), &C::Get<bool>(Vars.bScoutAutoDuck));
				break;
			case 4:
				ImGui::Text(XorStr("Ragebot Settings"));
				ImGui::Checkbox(XorStr("Resolver"), &C::Get<bool>(Vars.bAWPResolver));
				ImGui::Checkbox(XorStr("Visualize Angles/Aim"), &C::Get<bool>(Vars.bAWPSeeAngles));
				ImGui::SliderFloat(XorStr("Min Damage"), &C::Get<float>(Vars.fAWPRageMinDamage), 0.0f, 150.0f, " %.0fHP");
				ImGui::Checkbox(XorStr("Multipoint"), &C::Get<bool>(Vars.bAWPUseMultipoint));
				if (C::Get<bool>(Vars.bAWPUseMultipoint))
					ImGui::SliderFloat(XorStr("Scale"), &C::Get<float>(Vars.fAWPMultipointScale), 0.0f, 3.0f, "   %.2f Scale");
				ImGui::SliderInt(XorStr("Hitchance"), &C::Get<int>(Vars.iAWPHitchanceRange), 0, 100, "%dhc");
				ImGui::Text(XorStr("Ragebot Options"));
				ImGui::MultiCombo(XorStr("Hitboxes"), C::Get<std::vector<bool>>(Vars.vAWPHitboxes), arrHitboxes.data(), arrHitboxes.size());
				ImGui::Checkbox(XorStr("Slowwalk"), &C::Get<bool>(Vars.bAWPAutoStop));
				ImGui::Checkbox(XorStr("Auto Duck"), &C::Get<bool>(Vars.bAWPAutoDuck));
				break;
			case 5:
				ImGui::Text(XorStr("Ragebot Settings"));
				ImGui::Checkbox(XorStr("Resolver"), &C::Get<bool>(Vars.bAutoResolver));
				ImGui::Checkbox(XorStr("Visualize Angles/Aim"), &C::Get<bool>(Vars.bAutoSeeAngles));
				ImGui::SliderFloat(XorStr("Min Damage"), &C::Get<float>(Vars.fAutoRageMinDamage), 0.0f, 150.0f, " %.0fHP");
				ImGui::Checkbox(XorStr("Multipoint"), &C::Get<bool>(Vars.bAutoUseMultipoint));
				if (C::Get<bool>(Vars.bAutoUseMultipoint))
					ImGui::SliderFloat(XorStr("Scale"), &C::Get<float>(Vars.fAutoMultipointScale), 0.0f, 3.0f, "   %.2f Scale");
				ImGui::SliderInt(XorStr("Hitchance"), &C::Get<int>(Vars.iAutoHitchanceRange), 0, 100, "%dhc");
				ImGui::Text(XorStr("Ragebot Options"));
				ImGui::MultiCombo(XorStr("Hitboxes"), C::Get<std::vector<bool>>(Vars.vAutoHitboxes), arrHitboxes.data(), arrHitboxes.size());
				ImGui::Checkbox(XorStr("Slowwalk"), &C::Get<bool>(Vars.bAutoSniperAutoStop));
				ImGui::Checkbox(XorStr("Auto Duck"), &C::Get<bool>(Vars.bAutoSniperAutoDuck));
				break;
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		float flAntiAimTabSize = 0.f;
		ImGui::BeginChildWithColor(XorStr("ragebot.antiaim"), "Extras", ImVec2(0, 0), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::HotKey(XorStr("Dmg Override"), &C::Get<int>(Vars.iRageMinDamageOverrideKey));
			ImGui::HotKey(XorStr("DoubleTap"), &C::Get<int>(Vars.iRageDoubleTapKey));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::PopStyleVar();
			flAntiAimTabSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::AntiAim() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
	ImGui::Columns(2, nullptr, false);
	{
		float flAntiAimTabSize = 0.f;
		ImGui::BeginChild(XorStr("Anti-Aim"), ImVec2(0, 0), true, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("Toggle AntiAim"), &C::Get<bool>(Vars.bAntiAim));
			ImGui::PopStyleVar();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Combo(XorStr("pitch"), &C::Get<int>(Vars.iAntiAimPitch), XorStr("none\0custom\0jitter\0random\0zero (untrusted)\0\0"));
			switch (C::Get<int>(Vars.iAntiAimPitch)) {
			case 1:
				ImGui::SliderFloat(XorStr("Pitch Amount"), &C::Get<float>(Vars.fAntiAimPitchAmount), -89.0f, 89.0f, "   %.1f Deg");
				ImGui::Checkbox(XorStr("Jitter Pitch"), &C::Get<bool>(Vars.bAntiAimPitchJitter));
				break;
			}
			ImGui::Combo(XorStr("yaw"), &C::Get<int>(Vars.iAntiAimYaw), XorStr("none\0custom\0jitter\0random\0desync\0desync backwards\0\0"));
			switch (C::Get<int>(Vars.iAntiAimYaw)) {
			case 1:
				ImGui::SliderFloat(XorStr("Yaw Amount"), &C::Get<float>(Vars.fAntiAimYawAmount), -189.0f, 189.0f, "   %.1f Deg");
				ImGui::Checkbox(XorStr("Jitter Yaw"), &C::Get<bool>(Vars.bAntiAimYawJitter));
				ImGui::Checkbox(XorStr("Aim At Targets"), &C::Get<bool>(Vars.bAntiAimAtTarget));
				break;
			}
			if (C::Get<int>(Vars.iAntiAimYaw) == (int)EAntiAimYawType::DESYNC)
				ImGui::SliderInt(XorStr("desync amount"), &C::Get<int>(Vars.iAntiAimDesyncAmount), -59, 60, "   %d Amount");
				ImGui::HotKey(XorStr("desync switch"), &C::Get<int>(Vars.iAntiAimDesyncKey));
			ImGui::PopStyleVar();
			flAntiAimTabSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChildWithColor(XorStr("ragebot.fakelag"), "Fakelag", ImVec2(0, 0), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox("Fakelag", &C::Get<bool>(Vars.bMiscFakeLag));
			ImGui::SliderInt("FL Ticks", &C::Get<int>(Vars.iFakelagTicks), 0, 40, "%d Ticks");
			ImGui::MultiCombo(XorStr("Modes"), C::Get<std::vector<bool>>(Vars.vecMiscFakeLagType), fakeLagModes.data(), fakeLagModes.size());
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::LegitBot()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::Spacing();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));
		ImGui::BeginChild(XorStr("aimbot"), ImVec2());
		{
			ImGui::PopFont();
			ImGui::PushFont(F::ArialBold);
			ImGui::Text("aimbot");
			ImGui::PopFont();
			ImGui::PushFont(F::Tahoma);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Combo(XorStr("Weapon"), &C::Get<int>(Vars.iRageWeaponType), XorStr("Pistols\0Heavy Pistols\0Rifles\0Snipers\0Heavy\0\0"));
			ImGui::Checkbox(XorStr("Aimbot"), &C::Get<bool>(Vars.bLegit));
			ImGui::SliderFloat (XorStr("Smoothing"), &C::Get<float>(Vars.fSmoothAmount), 0.01f, 1.00f, XorStr("  %.2fF"));
			ImGui::HotKey(XorStr("Activation Key"), &C::Get<int>(Vars.iAimbotKey));
			ImGui::Checkbox(XorStr("Silent"), &C::Get<bool>(Vars.bSilentAim));
			ImGui::Combo(XorStr("Aimbot Type"), &C::Get<int>(Vars.iAimbotType), XorStr("Fov\0Distance\0\0"));
			switch (C::Get<int>(Vars.iAimbotType)) {
			case 0:
				ImGui::SliderFloat(XorStr("FOV"), &C::Get<float>(Vars.fMaxFovDistance), 0.0f, 30.0f, XorStr("  %.1f FOV"));
				break;
			case 1:
				break;
			}
			ImGui::Text(XorStr("Aimbot Checks"));
			ImGui::Checkbox(XorStr("Visibility Check"), &C::Get<bool>(Vars.bVisibleCheck));
			ImGui::Checkbox(XorStr("Smoke Check"), &C::Get<bool>(Vars.bSmokeCheck));
			ImGui::Checkbox(XorStr("Scoped Check"), &C::Get<bool>(Vars.bScopeCheck));
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
		ImGui::PopStyleVar();
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChildWithColor(XorStr("legitbot.triggerbot"), "Triggerbot", ImVec2(), true, menuCol, ImGuiWindowFlags_None);
		{
			if (ImGui::BeginMenu(XorStr("Filters")))
			{
				ImGui::MenuItem(XorStr("head"), nullptr, &C::Get<bool>(Vars.bTriggerHead));
				ImGui::MenuItem(XorStr("chest"), nullptr, &C::Get<bool>(Vars.bTriggerChest));
				ImGui::MenuItem(XorStr("stomach"), nullptr, &C::Get<bool>(Vars.bTriggerStomach));
				ImGui::MenuItem(XorStr("arms"), nullptr, &C::Get<bool>(Vars.bTriggerArms));
				ImGui::MenuItem(XorStr("legs"), nullptr, &C::Get<bool>(Vars.bTriggerLegs));
				ImGui::EndMenu();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("Triggerbot"), &C::Get<bool>(Vars.bTrigger));
			ImGui::HotKey(XorStr("activation key"), &C::Get<int>(Vars.iTriggerKey));
			ImGui::Separator();

			ImGui::SliderInt(XorStr("reaction delay##trigger"), &C::Get<int>(Vars.iTriggerDelay), 0, 500, "%dms");
			ImGui::Checkbox(XorStr("auto wall##trigger"), &C::Get<bool>(Vars.bTriggerAutoWall));
			ImGui::SliderInt(XorStr("minimal damage##trigger"), &C::Get<int>(Vars.iTriggerMinimalDamage), 1, 100, "%dhp");
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::Visuals()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChildWithColor(XorStr("visuals.esp"), "Visuals", ImVec2(0, 0), true, menuCol, ImGuiWindowFlags_None);
		{

			//Esp tab system
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2117f, 0.247f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.235, 0.255, 0.295, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.265, 0.285, 0.330, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			// LegitBot
			if (ImGui::ButtonUnderline(XorStr("ESP"), ImVec2(75, 25), menuCol)) {
				C::Get<int>(Vars.iOpenEspTab) = 0;
			}
			ImGui::SameLine();

			//Ragebot
			if (ImGui::ButtonUnderline(XorStr("Glow"), ImVec2(75, 25), menuCol)) {
				C::Get<int>(Vars.iOpenEspTab) = 1;
			}
			ImGui::SameLine();

			//Visuals
			if (ImGui::ButtonUnderline(XorStr("Chams"), ImVec2(75, 25), menuCol)) {
				C::Get<int>(Vars.iOpenEspTab) = 2;
			}
			// Pop dem ppl
			ImGui::PopStyleVar(5);
			ImGui::PopStyleColor(3);
			ImGui::Spacing();
			ImGui::Spacing();
			// Tab Render
			if (ImGui::BeginMenu(XorStr("Filters")))
			{
				switch (C::Get<int>(Vars.iOpenEspTab))
				{
				case 0:
				{
					ImGui::MenuItem(XorStr("enemies"), nullptr, &C::Get<bool>(Vars.bEspMainEnemies));
					ImGui::MenuItem(XorStr("allies"), nullptr, &C::Get<bool>(Vars.bEspMainAllies));
					ImGui::MenuItem(XorStr("weapons"), nullptr, &C::Get<bool>(Vars.bEspMainWeapons));
					ImGui::MenuItem(XorStr("grenades"), nullptr, &C::Get<bool>(Vars.bEspMainGrenades));
					ImGui::MenuItem(XorStr("bomb"), nullptr, &C::Get<bool>(Vars.bEspMainBomb));
					break;
				}
				case 1:
				{
					ImGui::MenuItem(XorStr("enemies"), nullptr, &C::Get<bool>(Vars.bEspGlowEnemies));
					ImGui::MenuItem(XorStr("allies"), nullptr, &C::Get<bool>(Vars.bEspGlowAllies));
					ImGui::MenuItem(XorStr("weapons"), nullptr, &C::Get<bool>(Vars.bEspGlowWeapons));
					ImGui::MenuItem(XorStr("grenades"), nullptr, &C::Get<bool>(Vars.bEspGlowGrenades));
					ImGui::MenuItem(XorStr("bomb"), nullptr, &C::Get<bool>(Vars.bEspGlowBomb));
					break;
				}
				case 2:
				{
					ImGui::MenuItem(XorStr("enemies"), nullptr, &C::Get<bool>(Vars.bEspChamsEnemies));
					ImGui::MenuItem(XorStr("allies"), nullptr, &C::Get<bool>(Vars.bEspChamsAllies));
					ImGui::MenuItem(XorStr("viewmodel"), nullptr, &C::Get<bool>(Vars.bEspChamsViewModel));
					break;
				}
				default:
					break;
				}

				ImGui::EndMenu();
			}
			switch (C::Get<int>(Vars.iOpenEspTab)) {
			case 0:
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("Enable ESP##main"), &C::Get<bool>(Vars.bEspMain));

				if (C::Get<bool>(Vars.bEspMainEnemies) || C::Get<bool>(Vars.bEspMainAllies))
				{
					if (C::Get<bool>(Vars.bEspMain)) {
						ImGui::Text(XorStr("Enemy Box Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("Box Color"), &C::Get<Color>(Vars.colEspMainBoxEnemies), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					}
					ImGui::Separator();
					ImGui::Combo(XorStr("player box"), &C::Get<int>(Vars.iEspMainPlayerBox), XorStr("none\0full\0corners\0\0"));
					ImGui::Checkbox(XorStr("far radar"), &C::Get<bool>(Vars.bEspMainPlayerFarRadar));
					ImGui::Checkbox(XorStr("players info"), &C::Get<bool>(Vars.bEspMainPlayerInfo));
					if (C::Get<bool>(Vars.bEspMainPlayerInfo))
					{
						ImGui::Checkbox(XorStr("health##player"), &C::Get<bool>(Vars.bEspMainPlayerHealth));
						ImGui::Checkbox(XorStr("money##player"), &C::Get<bool>(Vars.bEspMainPlayerMoney));
						ImGui::Checkbox(XorStr("name##player"), &C::Get<bool>(Vars.bEspMainPlayerName));
						ImGui::Checkbox(XorStr("flash##player"), &C::Get<bool>(Vars.bEspMainPlayerFlash));
						ImGui::MultiCombo(XorStr("flags##player"), C::Get<std::vector<bool>>(Vars.vecEspMainPlayerFlags), arrVisualsFlags.data(), arrVisualsFlags.size());
						ImGui::Checkbox(XorStr("weapons##player"), &C::Get<bool>(Vars.bEspMainPlayerWeapons));
						ImGui::Checkbox(XorStr("ammo##player"), &C::Get<bool>(Vars.bEspMainPlayerAmmo));
						ImGui::Checkbox(XorStr("distance##player"), &C::Get<bool>(Vars.bEspMainPlayerDistance));
					}
				}

				if (C::Get<bool>(Vars.bEspMainWeapons))
				{
					ImGui::Separator();
					ImGui::Combo(XorStr("weapon box"), &C::Get<int>(Vars.iEspMainWeaponBox), XorStr("none\0full\0corners\0\0"));

					ImGui::Checkbox(XorStr("weapons info"), &C::Get<bool>(Vars.bEspMainWeaponInfo));
					if (C::Get<bool>(Vars.bEspMainWeaponInfo))
					{
						ImGui::Checkbox(XorStr("icon##weapon"), &C::Get<bool>(Vars.bEspMainWeaponIcon));
						ImGui::Checkbox(XorStr("ammo##weapon"), &C::Get<bool>(Vars.bEspMainWeaponAmmo));
						ImGui::Checkbox(XorStr("distance##weapon"), &C::Get<bool>(Vars.bEspMainWeaponDistance));
					}
				}

				ImGui::PopStyleVar();
				break;
			case 1:
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("Enable Glow##glow"), &C::Get<bool>(Vars.bEspGlow));
				ImGui::Checkbox(XorStr("bloom"), &C::Get<bool>(Vars.bEspGlowBloom));
				if (C::Get<bool>(Vars.bEspGlowEnemies)) {
					ImGui::Text(XorStr("Glow Enemies Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("Glow Color"), &C::Get<Color>(Vars.colEspGlowEnemies), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				}
				if (C::Get<bool>(Vars.bEspGlowAllies)) {
					ImGui::Text(XorStr("Glow Allies Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("Glow2 Color"), &C::Get<Color>(Vars.colEspGlowAllies), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				}
				// @note: if u rebuild glow and wanna use styles do like that
				//ImGui::Combo(XorStr("styles example##glow"), XorStr("outer\0rim\0edge\0edge pulse\0\0"));

				ImGui::PopStyleVar();
				break;
			case 2:
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::Checkbox(XorStr("Enable Chams##chams"), &C::Get<bool>(Vars.bEspChams));

				if (C::Get<bool>(Vars.bEspChamsEnemies) || C::Get<bool>(Vars.bEspChamsAllies))
				{
					ImGui::Combo(XorStr("players style##chams"), &C::Get<int>(Vars.iEspChamsPlayer), XorStr("covered\0flat\0wireframe\0reflective\0\0"));
					ImGui::Text(XorStr("Chams Enemies Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("Chams Color"), &C::Get<Color>(Vars.colEspChamsEnemies), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					if (C::Get<bool>(Vars.bEspChamsAllies)) {
						ImGui::Text(XorStr("Chams Allies Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("Chams Color2"), &C::Get<Color>(Vars.colEspChamsAllies), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					}
					ImGui::Checkbox(XorStr("xqz"), &C::Get<bool>(Vars.bEspChamsXQZ));
					ImGui::Checkbox(XorStr("disable occlusion"), &C::Get<bool>(Vars.bEspChamsDisableOcclusion));
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XorStr("may cause \"invisible\" players models sometimes"));
				}

				if (C::Get<bool>(Vars.bEspChamsViewModel))
				{
					ImGui::Combo(XorStr("viewmodel style##chams"), &C::Get<int>(Vars.iEspChamsViewModel), XorStr("no draw\0covered\0flat\0wireframe\0glow\0scroll\0chrome\0\0"));
					ImGui::Text(XorStr("Viewmodel Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("ViewChams Color"), &C::Get<Color>(Vars.colEspChamsViewModel), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					ImGui::Text(XorStr("Viewmodel Extra Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("ViewChams Color2"), &C::Get<Color>(Vars.colEspChamsViewModelAdditional), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				}

				ImGui::PopStyleVar();
				break;
			}


			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		static float flWorldChildSize = 0.f;
		ImGui::BeginChildWithColor(XorStr("visuals.world"), "World", ImVec2(0, flWorldChildSize), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("night mode"), &C::Get<bool>(Vars.bWorldNightMode));
			ImGui::Checkbox(XorStr("Fullbright"), &C::Get<bool>(Vars.bWorldFullbright));
			ImGui::Checkbox(XorStr("Change Skybox"), &C::Get<bool>(Vars.bWorldChangeSkybox));
			if (C::Get<bool>(Vars.bWorldChangeSkybox))
				ImGui::Combo(XorStr("Skybox Type"), &C::Get<int>(Vars.iWorldSkyType), XorStr("Baggage\0Tibet\0Clear Sky\0Clear Sky HD\0Embassy\0Italy\0Daylight 1\0Daylight 2\0Daylight 3\0Daylight 4\0Cloduy\0Night 1\0Night 2\0Night Flat\0Day HD\0Day\0Rural\0Vertigo HD\0Vertigo Blue HD\0Vertigo\0Vietnam\0Dusy Sky\0Jungle\0Nuke\0Office\0\0"));
			ImGui::SliderInt(XorStr("max flash effect"), &C::Get<int>(Vars.iWorldMaxFlash), 0, 100, "%d%%");
			ImGui::MultiCombo(XorStr("removals"), C::Get<std::vector<bool>>(Vars.vecWorldRemovals), arrVisualsRemovals.data(), arrVisualsRemovals.size());
			ImGui::Separator();

			ImGui::HotKey(XorStr("thirdperson"), &C::Get<int>(Vars.iWorldThirdPersonKey));
			ImGui::SliderFloat(XorStr("camera offset"), &C::Get<float>(Vars.flWorldThirdPersonOffset), 50.f, 300.f, "%.1f units");
			ImGui::PopStyleVar();

			flWorldChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}
		ImGui::BeginChildWithColor(XorStr("visuals.screen"), "Screen", ImVec2(0, 0), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::SliderFloat(XorStr("camera fov"), &C::Get<float>(Vars.flScreenCameraFOV), -89.f, 89.f, "%.1f\xC2\xB0");
			ImGui::SliderFloat(XorStr("viewmodel fov"), &C::Get<float>(Vars.flScreenViewModelFOV), -90.f, 90.f, "%.1f\xC2\xB0");
			ImGui::Checkbox(XorStr("Overwrite Zoom Fov"), &C::Get<bool>(Vars.bScreenOverwriteZoom));
			if (C::Get<bool>(Vars.bScreenOverwriteZoom))
				ImGui::SliderFloat(XorStr("zoomed-in fov"), &C::Get<float>(Vars.flScopedFov), -180.0f, 180.f, "%.1f\xC2\xB0");
			ImGui::Checkbox(XorStr("Visual Crosshair"), &C::Get<bool>(Vars.bCrosshairAlwaysOn));
			ImGui::Separator();
			ImGui::Checkbox(XorStr("hitmarker"), &C::Get<bool>(Vars.bScreenHitMarker));
			ImGui::Checkbox(XorStr("damage"), &C::Get<bool>(Vars.bScreenHitMarkerDamage));
			ImGui::Checkbox(XorStr("sound"), &C::Get<bool>(Vars.bScreenHitMarkerSound));
			ImGui::SliderFloat(XorStr("time"), &C::Get<float>(Vars.flScreenHitMarkerTime), 0.5f, 5.f, "%.1fsec");
			ImGui::SliderInt(XorStr("gap"), &C::Get<int>(Vars.iScreenHitMarkerGap), 1, 20, "%d pixels");
			ImGui::SliderInt(XorStr("length"), &C::Get<int>(Vars.iScreenHitMarkerLenght), 1, 20, "%d pixels");
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::Miscellaneous()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::Columns(2, nullptr, false);
	{
		static float flMovementChildSize = 0.f;
		static float flOtherChildSize = 0.f;
		ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
		ImGui::BeginChildWithColor(XorStr("misc.movement"), "Movement", ImVec2(0, flMovementChildSize), true, menuCol, ImGuiWindowFlags_None);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(XorStr("movement"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("bunny hop"), &C::Get<bool>(Vars.bMiscBunnyHop));
			ImGui::SliderInt(XorStr("chance"), &C::Get<int>(Vars.iMiscBunnyHopChance), 0, 100, "%d%%");
			ImGui::Checkbox(XorStr("autostrafe"), &C::Get<bool>(Vars.bMiscAutoStrafe));
			ImGui::PopStyleVar();

			flMovementChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}
		ImGui::BeginChildWithColor(XorStr("misc.other"), "Other Options", ImVec2(), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("auto accept"), &C::Get<bool>(Vars.bMiscAutoAccept));
			ImGui::Checkbox(XorStr("auto pistol"), &C::Get<bool>(Vars.bMiscAutoPistol));
			ImGui::Checkbox(XorStr("no crouch cooldown"), &C::Get<bool>(Vars.bMiscNoCrouchCooldown));
			ImGui::Checkbox(XorStr("Always Show FOV Circle"), &C::Get<bool>(Vars.bAlwaysShowFovCircle));
			ImGui::Checkbox(XorStr("Catware Watermark"), &C::Get<bool>(Vars.bShowWatermark));
			ImGui::Checkbox(XorStr("Catware Clantag"), &C::Get<bool>(Vars.bMiscDoClantag));
			ImGui::Checkbox(XorStr("Spectator List"), &C::Get<bool>(Vars.bShowSpectators));
			ImGui::Checkbox(XorStr("Show Spread"), &C::Get<bool>(Vars.bShowSpread));
			ImGui::Checkbox(XorStr("Dom Torretism"), &C::Get<bool>(Vars.bMiscDoDomToretto));
			//flOtherChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
		// current selected configuration name
		static std::string szCurrentConfig = { };

		static float flConfigChildSize = 0.f;
		ImGui::BeginChildWithColor(XorStr("misc.config"), "Configs", ImVec2(0, flConfigChildSize), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::Columns(2, XorStr("#CONFIG"), false);
			{
				ImGui::PushItemWidth(-1);

				ImGui::ListBox(XorStr("##config.list"), &iSelectedConfig, [](int nIndex)
					{
						// return current displaying configuration name
						return C::vecFileNames.at(nIndex).c_str();
					}, C::vecFileNames.size(), 5);

				szCurrentConfig = !C::vecFileNames.empty() ? C::vecFileNames.at(iSelectedConfig) : "";
				ImGui::PopItemWidth();
			}
			ImGui::NextColumn();
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
				ImGui::PushItemWidth(-1);
				if (ImGui::InputTextWithHint(XorStr("##config.file"), XorStr("create new..."), &szConfigFile, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (!C::Save(szConfigFile))
					{
						L::PushConsoleColor(FOREGROUND_RED);
						L::Print(fmt::format(XorStr("[error] failed to create \"{}\" config"), szConfigFile));
						L::PopConsoleColor();
					}

					szConfigFile.clear();
					C::Refresh();
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("press enter to create new configuration"));

				if (ImGui::Button(XorStr("save"), ImVec2(-1, 20)))
				{
					if (!C::Save(szCurrentConfig))
					{
						L::PushConsoleColor(FOREGROUND_RED);
						L::Print(fmt::format(XorStr("[error] failed to save \"{}\" config"), szCurrentConfig));
						L::PopConsoleColor();
					}
				}

				if (ImGui::Button(XorStr("load"), ImVec2(-1, 20)))
				{
					if (!C::Load(szCurrentConfig))
					{
						L::PushConsoleColor(FOREGROUND_RED);
						L::Print(fmt::format(XorStr("[error] failed to load \"{}\" config"), szCurrentConfig));
						L::PopConsoleColor();
					}
				}

				if (ImGui::Button(XorStr("remove"), ImVec2(-1, 20)))
					ImGui::OpenPopup(XorStr("confirmation##config.remove"));

				if (ImGui::Button(XorStr("refresh"), ImVec2(-1, 20)))
					C::Refresh();

				ImGui::PopItemWidth();
				ImGui::PopStyleVar();
			}
			ImGui::Columns(1);

			if (ImGui::BeginPopupModal(XorStr("confirmation##config.remove"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(XorStr("are you sure you want to remove \"%s\" configuration?"), szCurrentConfig.c_str());
				ImGui::Spacing();

				if (ImGui::Button(XorStr("no"), ImVec2(30, 0)))
					ImGui::CloseCurrentPopup();

				ImGui::SameLine();

				if (ImGui::Button(XorStr("yes"), ImVec2(30, 0)))
				{
					C::Remove(iSelectedConfig);

					// reset current configuration
					iSelectedConfig = 0;

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			flConfigChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
			ImGui::EndChild();
		}
		ImGui::BeginChildWithColor(XorStr("misc.exploits"), "Exploits", ImVec2(0, 0), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::Checkbox(XorStr("No Recoil"), &C::Get<bool>(Vars.bNoRecoilHax));
			ImGui::Checkbox(XorStr("sv_cheats"), &C::Get<bool>(Vars.bMiscSvCheat));
			ImGui::Checkbox(XorStr("ping spike"), &C::Get<bool>(Vars.bMiscPingSpike));
			ImGui::SliderFloat(XorStr("latency factor"), &C::Get<float>(Vars.flMiscLatencyFactor), 0.1f, 1.0f, "%.1f second");
			ImGui::Checkbox(XorStr("reveal ranks"), &C::Get<bool>(Vars.bMiscRevealRanks));
			ImGui::Checkbox(XorStr("unlock inventory"), &C::Get<bool>(Vars.bMiscUnlockInventory));
			ImGui::Checkbox(XorStr("anti-untrusted"), &C::Get<bool>(Vars.bMiscAntiUntrusted));
			ImGui::Checkbox(XorStr("Bullet Impacts"), &C::Get<bool>(Vars.bShowBulletImpacts));
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::Lua()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::Spacing();
		ImGui::Text(XorStr("Scripts"));
		ImGui::Spacing();
		ImGui::BeginChildWithColor(XorStr("lua.scripts"), "Scripts", ImVec2(), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			if (ImGui::Button(XorStr("Refresh"))) {
				for (int i = 0; i < activeLuaScripts.size(); i++) {
					lua_State* L = activeLuaScripts.at(i).L;
					lua_close(L);
				}
				activeLuaScripts.clear();
				Lua::RefreshLua();
			}
			for (int i = 0; i < activeLuaScripts.size(); i++) {
				ImGui::Checkbox(activeLuaScripts.at(i).fileName.c_str(), &activeLuaScripts.at(i).enabled);
				std::cout << activeLuaScripts.at(i).fileName << std::endl;
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::Spacing();
		ImGui::Text(XorStr("Script Options"));
		ImGui::Spacing();
		ImGui::BeginChildWithColor(XorStr("lua.options"), "Script Options", ImVec2(), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::MenuSettings() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImU32 menuCol = C::Get<Color>(Vars.colMenuColor).GetU32();
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChildWithColor(XorStr("lua.scripts"), "Scripts", ImVec2(), true, menuCol, ImGuiWindowFlags_None);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));
			if (ImGui::Button(XorStr("Refresh"))) {
				for (int i = 0; i < activeLuaScripts.size(); i++) {
					lua_State* L = activeLuaScripts.at(i).L;
					lua_close(L);
				}
				activeLuaScripts.clear();
				Lua::RefreshLua();
			}
			for (int i = 0; i < activeLuaScripts.size(); i++) {
				ImGui::Checkbox(activeLuaScripts.at(i).fileName.c_str(), &activeLuaScripts.at(i).enabled);
				std::cout << activeLuaScripts.at(i).fileName << std::endl;
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChildWithColor(XorStr("misc.colors"), "Colors", ImVec2(), true, menuCol, ImGuiWindowFlags_None);
		{
			const char* szColorNames[IM_ARRAYSIZE(arrColors)];
			for (int i = 0; i < IM_ARRAYSIZE(arrColors); i++)
				szColorNames[i] = arrColors[i].first;

			ImGui::Spacing();
			ImGui::PushItemWidth(-1);

			//ImGui::ListBox(XorStr("##colors.select"), &iSelectedColor, szColorNames, IM_ARRAYSIZE(szColorNames), 4);
			//ImGui::ColorEdit4(XorStr("##colors.picker"), &C::Get<Color>(arrColors[iSelectedColor].second), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
			ImGui::Text(XorStr("Menu Color ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15);; ImGui::ColorEdit4(XorStr("Menu Color"), &C::Get<Color>(Vars.colMenuColor), ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Text(XorStr("Fov Circle ")); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15); ImGui::ColorEdit4(XorStr("Fov Circle Color"), &C::Get<Color>(Vars.colFovCircle), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::PopItemWidth();
			ImGui::EndChild();
		}
	}
	ImGui::Columns(1);
}

void T::SkinChanger()
{
	ImGui::BeginChild(XorStr("skins"), ImVec2(), true);
	{
		for (const auto& item : mapItemList) //first - itemdefindex, second - skin item struct
		{

		}

		ImGui::EndChild();
	}
}
#pragma endregion
