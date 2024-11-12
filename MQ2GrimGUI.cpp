#include <mq/Plugin.h>
#include <imgui.h>
#include "main/MQ2Main.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include <mq/imgui/Widgets.h>
#include <chrono>
#include <string>
#include <filesystem>
#include "MQ2GrimGUI.h"
#include "imgui_theme.h"

PreSetup("MQ2GrimGUI");
PLUGIN_VERSION(0.2);

#pragma region Main Setting Variables
// Declare global plugin state variables

struct WinVisSettings {
	bool showMainWindow = false;
	bool showConfigWindow = false;
	bool showPetWindow = false;
	bool showPlayerWindow = false;
	bool showGroupWindow = false;
	bool showSpellsWindow = false;
	bool showTargetWindow = false;
	bool showBuffWindow = false;
	bool showSongWindow = false;
	bool flashCombatFlag = false;
	bool flashTintFlag = false;
	bool showTitleBars = true;
} winVis;

struct NumericSettings {
	int combatFlashInterval = 100;
	int flashBuffInterval = 40;
	int playerBarHeight = 15;
	int targetBarHeight = 15;
	int aggroBarHeight = 10;
	int myAggroPct = 0;
	int secondAggroPct = 0;
	int buffIconSize = 24;
	int buffTimerThreshold = 0;
} numSet;

struct ThemeSettings {
	std::string playerWinTheme = "Default";
	std::string petWinTheme = "Default";
	std::string groupWinTheme = "Default";
	std::string spellsWinTheme = "Default";
	std::string buffsWinTheme = "Default";
	std::string songWinTheme = "Default";
} winThemes;

struct ColorSettings {
	mq::MQColor minColorHP = mq::MQColor(223, 87, 255, 255);
	mq::MQColor maxColorHP = mq::MQColor(216, 39, 39, 255);
	mq::MQColor minColorMP = mq::MQColor(66, 29, 131, 255);
	mq::MQColor maxColorMP = mq::MQColor(20, 119, 216, 255);
	mq::MQColor minColorEnd = mq::MQColor(255, 111, 5, 255);
	mq::MQColor maxColorEnd = mq::MQColor(178, 153, 26, 178);
} barColors;

static bool s_CharIniLoaded = false;
static bool s_DefaultLoaded = false;
static int s_TestInt = 100; // Color Test Value for Config Window
static char s_SettingsFile[MAX_PATH] = { 0 };

static ImGuiWindowFlags s_WindowFlags = ImGuiWindowFlags_None;

static const char* s_SecondAggroName = "Unknown";
static const char* s_CurrHeading = "N";
static int s_TarBuffLineSize = 0;

#pragma endregion

#pragma region Timers
std::chrono::steady_clock::time_point g_LastUpdateTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastBuffFlashTime = std::chrono::steady_clock::now();

const auto g_UpdateInterval		= std::chrono::milliseconds(250);

#pragma endregion

#pragma region Pet Buttons

struct PetButtonData {
	std::string name;
	std::string command;
	bool visible;
};

static std::vector<PetButtonData> petButtons = {
	{"Attack", "/pet attack", true},
	{"Sit", "/pet sit", true},
	{"Follow", "/pet follow", true},
	{"Hold", "/pet hold", true},
	{"Taunt", "/pet taunt", true},
	{"Guard", "/pet guard", true},
	{"Back", "/pet back off", true},
	{"Focus", "/pet focus", true},
	{"Stop", "/pet stop", true},
	{"Leave", "/pet get lost", true},
	{"Regroup", "/pet regroup", true},
	{"Report", "/pet report health", true},
	{"Swarm", "/pet swarm", true},
	{"Kill", "/pet kill", true},
};

static void DisplayPetButtons() {
	int numColumns = static_cast<int>(1, ImGui::GetColumnWidth() / 60);

	if (ImGui::BeginTable("ButtonsTable", numColumns, ImGuiTableFlags_SizingStretchProp)) {
		for (auto& button : petButtons) {
			if (button.visible) {
				ImGui::TableNextColumn();
				std::string btnLabel = button.name;

				bool isAttacking = false;
				bool isSitting = false;
				if (PSPAWNINFO MyPet = pSpawnManager->GetSpawnByID(pLocalPlayer->PetID))
				{
					isAttacking = MyPet->WhoFollowing;
					isSitting = MyPet->StandState != 100;
				}

				if ((btnLabel == "Attack" && isAttacking) || (btnLabel == "Sit" && isSitting))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(GetMQColor(ColorName::Teal).ToImColor()));
					if (ImGui::Button(btnLabel.c_str(), ImVec2(55, 20)))
					{
						EzCommand(button.command.c_str());
					}
					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(btnLabel.c_str(), ImVec2(55, 20)))
					{
						EzCommand(button.command.c_str());
					}
				}
			}
		}
		ImGui::EndTable();
	}
}

static void TogglePetButtonVisibilityMenu() {
	int numColumns = static_cast<int>(1, (ImGui::GetWindowWidth() - 10) / 75);
	if (numColumns < 1)
		numColumns = 1;

	if (ImGui::BeginTable("CheckboxTable", numColumns, ImGuiTableFlags_SizingStretchProp))
	{
		for (auto& button : petButtons) {
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(70);
			if (ImGui::Checkbox(button.name.c_str(), &button.visible))
			{
				WritePrivateProfileBool("Pet", button.name.c_str(), button.visible, &s_SettingsFile[0]);
			}
		}
		ImGui::EndTable();
	}
}

#pragma endregion

#pragma region Settings Functions

static void LoadSettings()
{
	// Load settings from the INI file
	//window settings
	winVis.showMainWindow = GetPrivateProfileBool("Settings", "ShowMainGui", true, &s_SettingsFile[0]);
	winVis.showTitleBars = GetPrivateProfileBool("Settings", "ShowTitleBars", true, &s_SettingsFile[0]);
	winVis.showTargetWindow = GetPrivateProfileBool("PlayerTarg", "SplitTarget", false, &s_SettingsFile[0]);
	winVis.showPlayerWindow = GetPrivateProfileBool("PlayerTarg", "ShowPlayerWindow", false, &s_SettingsFile[0]);
	winVis.showPetWindow = GetPrivateProfileBool("Pet", "ShowPetWindow", false, &s_SettingsFile[0]);
	winVis.showGroupWindow = GetPrivateProfileBool("Group", "ShowGroupWindow", false, &s_SettingsFile[0]);
	winVis.showSpellsWindow = GetPrivateProfileBool("Spells", "ShowSpellsWindow", false, &s_SettingsFile[0]);
	winVis.showBuffWindow = GetPrivateProfileBool("Buffs", "ShowBuffWindow", false, &s_SettingsFile[0]);
	winVis.showSongWindow = GetPrivateProfileBool("Songs", "ShowSongWindow", false, &s_SettingsFile[0]);

	numSet.combatFlashInterval = GetPrivateProfileInt("PlayerTarg", "CombatFlashInterval", 250, &s_SettingsFile[0]);
	numSet.playerBarHeight = GetPrivateProfileInt("PlayerTarg", "PlayerBarHeight", 15, &s_SettingsFile[0]);
	numSet.targetBarHeight = GetPrivateProfileInt("PlayerTarg", "TargetBarHeight", 15, &s_SettingsFile[0]);
	numSet.aggroBarHeight = GetPrivateProfileInt("PlayerTarg", "AggroBarHeight", 10, &s_SettingsFile[0]);
	numSet.flashBuffInterval = GetPrivateProfileInt("Settings", "FlashBuffInterval", 250, &s_SettingsFile[0]);
	numSet.buffIconSize = GetPrivateProfileInt("Settings", "BuffIconSize", 24, &s_SettingsFile[0]);
	numSet.buffTimerThreshold = GetPrivateProfileInt("Buffs", "BuffTimerThreshold", 30, &s_SettingsFile[0]);

	//Color Settings
	barColors.minColorHP = GetPrivateProfileColor("Colors", "MinColorHP", barColors.minColorHP, &s_SettingsFile[0]);
	barColors.maxColorHP = GetPrivateProfileColor("Colors", "MaxColorHP", barColors.maxColorHP, &s_SettingsFile[0]);
	barColors.minColorMP = GetPrivateProfileColor("Colors", "MinColorMP", barColors.minColorMP, &s_SettingsFile[0]);
	barColors.maxColorMP = GetPrivateProfileColor("Colors", "MaxColorMP", barColors.maxColorMP, &s_SettingsFile[0]);
	barColors.minColorEnd = GetPrivateProfileColor("Colors", "MinColorEnd", barColors.minColorEnd, &s_SettingsFile[0]);
	barColors.maxColorEnd = GetPrivateProfileColor("Colors", "MaxColorEnd", barColors.maxColorEnd, &s_SettingsFile[0]);

	//Theme Settings
	winThemes.playerWinTheme = GetPrivateProfileString("PlayerTarg", "Theme", winThemes.playerWinTheme, &s_SettingsFile[0]);
	winThemes.petWinTheme = GetPrivateProfileString("Pet", "Theme", winThemes.petWinTheme, &s_SettingsFile[0]);
	winThemes.groupWinTheme = GetPrivateProfileString("Group", "Theme", winThemes.groupWinTheme, &s_SettingsFile[0]);
	winThemes.spellsWinTheme = GetPrivateProfileString("Spells", "Theme", winThemes.spellsWinTheme, &s_SettingsFile[0]);
	winThemes.buffsWinTheme = GetPrivateProfileString("Buffs", "Theme", winThemes.buffsWinTheme, &s_SettingsFile[0]);
	winThemes.songWinTheme = GetPrivateProfileString("Songs", "Theme", winThemes.songWinTheme, &s_SettingsFile[0]);

	// Load Pet button visibility settings
	for (auto& button : petButtons) {
		button.visible = GetPrivateProfileBool("Pet", button.name.c_str(), true, &s_SettingsFile[0]);
	}
}

static void SaveSettings()
{
	//Window Settings
	WritePrivateProfileBool("Settings", "ShowMainGui", winVis.showMainWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Settings", "ShowTitleBars", winVis.showTitleBars, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "SplitTarget", winVis.showTargetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", winVis.showPlayerWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Pet", "ShowPetWindow", winVis.showPetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Group", "ShowGroupWindow", winVis.showGroupWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Spells", "ShowSpellsWindow", winVis.showSpellsWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Buffs", "ShowBuffWindow", winVis.showBuffWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Songs", "ShowSongWindow", winVis.showSongWindow, &s_SettingsFile[0]);

	WritePrivateProfileInt("PlayerTarg", "CombatFlashInterval", numSet.combatFlashInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", numSet.playerBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", numSet.targetBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", numSet.aggroBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("Settings", "BuffIconSize", numSet.buffIconSize, &s_SettingsFile[0]);
	WritePrivateProfileInt("Settings", "FlashBuffInterval", numSet.flashBuffInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("Buffs", "BuffTimerThreshold", numSet.buffTimerThreshold, &s_SettingsFile[0]);

	//Color Settings
	WritePrivateProfileColor("Colors", "MinColorHP", barColors.minColorHP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorHP", barColors.maxColorHP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MinColorMP", barColors.minColorMP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorMP", barColors.maxColorMP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MinColorEnd", barColors.minColorEnd, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorEnd", barColors.maxColorEnd, &s_SettingsFile[0]);

	//Theme Settings
	WritePrivateProfileString("Spells", "Theme", winThemes.spellsWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Group", "Theme", winThemes.groupWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("PlayerTarg", "Theme", winThemes.playerWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Pet", "Theme", winThemes.petWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Buffs", "Theme", winThemes.buffsWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Songs", "Theme", winThemes.songWinTheme, &s_SettingsFile[0]);

	// Save Pet button visibility settings
	for (const auto& button : petButtons) {
		WritePrivateProfileBool("Pet", button.name.c_str(), button.visible, &s_SettingsFile[0]);
	}
}

// Update the settings file to use the character-specific INI file if the player is in-game
static void UpdateSettingFile()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (!s_CharIniLoaded)
		{
			if (PSPAWNINFO pCharInfo = pLocalPlayer)
			{
				char CharIniFile[MAX_PATH] = { 0 };
				fmt::format_to(CharIniFile, "{}/MQ2GrimGUI_{}_{}.ini", gPathConfig, GetServerShortName(), pCharInfo->Name);

				if (!std::filesystem::exists(CharIniFile))
				{
					// file missing load defaults and save
					memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
					strcpy_s(s_SettingsFile, CharIniFile);

					LoadSettings();
					SaveSettings();
				}

				// Update the settings file to use the character-specific INI file
				memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
				strcpy_s(s_SettingsFile, CharIniFile);

				LoadSettings();
				s_CharIniLoaded = true;
			}
		}
	}
	else
	{
		if (s_CharIniLoaded || !s_DefaultLoaded)
		{
			memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
			fmt::format_to(s_SettingsFile, "{}/MQ2GrimGUI.ini", gPathConfig);
			s_CharIniLoaded = false;
			LoadSettings();
			s_DefaultLoaded = true;
		}
	}
}

#pragma endregion

#pragma region Helpers 

static void GetHeading()
{
	static PSPAWNINFO pSelfInfo = pLocalPlayer;
	s_CurrHeading = szHeadingShort[static_cast<int>((pSelfInfo->Heading / 32.0f) + 8.5f) % 16];
}

/**
 * @fn GrimCommandHandler
 *
 * Command handler for the /Grimgui command
 *
 * @param pPC PlayerClient* Pointer to the player client structure
 * @param pszLine const char* Command line text
*/
static void GrimCommandHandler(PlayerClient*, const char*)
{
	winVis.showMainWindow = !winVis.showMainWindow;
	WritePrivateProfileBool("Settings", "ShowMainGui", winVis.showMainWindow, &s_SettingsFile[0]);
}

/**
* @fn DrawLineOfSight
 *
 * Draws a line of sight indicator based on the result of the LineOfSight function
 * 
 * @param pFrom PSPAWNINFO Pointer to the source spawn
 * @param pTo PSPAWNINFO Pointer to the target spawn
*/
static void DrawLineOfSight(PSPAWNINFO pFrom, PSPAWNINFO pTo)
{
	if (LineOfSight(pFrom, pTo))
	{
		ImGui::TextColored(GetMQColor(ColorName::Green).ToImColor(), ICON_MD_VISIBILITY);
	}
	else
	{
		ImGui::TextColored(GetMQColor(ColorName::Red).ToImColor(), ICON_MD_VISIBILITY_OFF);
	}
}

/**
 * @fn DrawHelpIcon
 *
 * Draws a help icon with a tooltip
 * 
 * @param helpText const char* Text to display in the tooltip
*/
static void DrawHelpIcon(const char* helpText)
{
	ImGui::SameLine();
	ImGui::TextDisabled(ICON_FA_QUESTION_CIRCLE_O);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("%s", helpText);
		ImGui::EndTooltip();
	}
}

static void GiveItem(PSPAWNINFO pSpawn)
{
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		pTarget = pSpawn;

		if (ItemPtr pItem = GetPcProfile()->GetInventorySlot(InvSlot_Cursor))
		{
			EzCommand("/click left target");
		}
	}
}

#pragma endregion

#pragma region GUI Windows 

static void DrawTargetWindow()
	{
		if (PSPAWNINFO CurTarget = pTarget)
		{
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2);
			float tarPercentage = static_cast<float>(CurTarget->HPCurrent) / 100;
			int tar_label = CurTarget->HPCurrent;
			ImVec4 colorTarHP = CalculateProgressiveColor(barColors.minColorHP, barColors.maxColorHP, CurTarget->HPCurrent);

			if (strncmp(pTarget->DisplayedName, pLocalPC->Name, 64) == 0)
			{
				tarPercentage = static_cast<float>(CurTarget->HPCurrent) / CurTarget->HPMax;
				int healthIntPct = static_cast<int>(tarPercentage * 100);
				tar_label = healthIntPct;
				colorTarHP = CalculateProgressiveColor(barColors.minColorHP, barColors.maxColorHP, healthIntPct);
			}
			DrawLineOfSight(pLocalPlayer, pTarget);
			ImGui::SameLine();
			ImGui::Text(CurTarget->DisplayedName);

			ImGui::SameLine(sizeX * .75f);
			ImGui::TextColored(GetMQColor(ColorName::Tangerine).ToImColor(), "%0.1f m", GetDistance(pLocalPlayer, pTarget));

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(tarPercentage, ImVec2(0.0f, static_cast<float>(numSet.targetBarHeight)), "##");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX - 8), yPos));
			ImGui::Text("%d %%", tar_label);
			ImGui::NewLine();
			ImGui::SameLine();
			ImGui::TextColored(GetMQColor(ColorName::Teal).ToImColor(), "Lvl %d", CurTarget->Level);

			ImGui::SameLine();
			const char* classCode = CurTarget->GetClassThreeLetterCode();
			
			std::string tClass = (classCode && std::string(classCode) != "UNKNOWN CLASS") ? classCode : ICON_MD_HELP_OUTLINE;
			ImGui::Text(tClass.c_str());

			ImGui::SameLine();
			ImGui::Text(GetBodyTypeDesc(GetBodyType(pTarget)));

			ImGui::SameLine(sizeX * .5f);
			ImGui::TextColored(ImVec4(GetConColor(ConColor(pTarget)).ToImColor()),ICON_MD_LENS);


			if (numSet.myAggroPct < 100)
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Orange).ToImColor()));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Purple).ToImColor()));
			}
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(static_cast<float>(numSet.myAggroPct) / 100, ImVec2(0.0f, static_cast<float>(numSet.aggroBarHeight)), "##Aggro");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(10, yPos));
			ImGui::Text(s_SecondAggroName);
			ImGui::SetCursorPos(ImVec2((sizeX/2)-8, yPos));
			ImGui::Text("%d %%", numSet.myAggroPct);
			ImGui::SetCursorPos(ImVec2(sizeX - 40, yPos));
			ImGui::Text("%d %%", numSet.secondAggroPct);	

			if (ImGui::BeginChild("TargetBuffs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar))
			{

				if (gTargetbuffs)
				{
					//GetCachedBuffAtSlot(pTarget, 0);
					//ImGui::Text("%s", buff);
					s_spellsInspector->DrawBuffsIcons("TargetBuffsTable", pTargetWnd->GetBuffRange(), false);
				}
			}
			ImGui::EndChild();
		}
	}

static void DrawPlayerWindow()
	{
		if (!winVis.showPlayerWindow)
			return;
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(winThemes.playerWinTheme);
		if (ImGui::Begin("Player##MQ2GrimGUI", &winVis.showPlayerWindow, s_WindowFlags | ImGuiWindowFlags_MenuBar))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int midX = (sizeX / 2) - 8;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Main"))
				{
					if (ImGui::MenuItem("Split Target", NULL, winVis.showTargetWindow))
					{
						winVis.showTargetWindow = !winVis.showTargetWindow;
						WritePrivateProfileBool("PlayerTarg", "SplitTarget", winVis.showTargetWindow, &s_SettingsFile[0]);
					}

					if (ImGui::MenuItem("Show Config", NULL, winVis.showConfigWindow))
					{
						winVis.showConfigWindow = !winVis.showConfigWindow;
					}

					if (ImGui::MenuItem("Show Main", NULL, winVis.showMainWindow))
					{
						winVis.showMainWindow = !winVis.showMainWindow;
						WritePrivateProfileBool("Settings", "ShowMainGui", winVis.showMainWindow, &s_SettingsFile[0]);
					}

					ImGui::EndMenu();
				}
				
				ImGui::EndMenuBar();
			}

			if (pEverQuestInfo->bAutoAttack)
			{
				if (winVis.flashCombatFlag)
				{
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(GetMQColor(ColorName::Red).ToImColor()));
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(GetMQColor(ColorName::White).ToImColor()));
				}
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(GetMQColor(ColorName::White).ToImColor()));
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
			if (ImGui::BeginChild("info", ImVec2(ImGui::GetContentRegionAvail().x, 26), true, ImGuiChildFlags_Border + ImGuiChildFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar ))
			{
				if (ImGui::BeginTable("##Player", 3))
				{
					ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch, ImGui::GetContentRegionAvail().x * .5f);
					ImGui::TableSetupColumn("##Heading", ImGuiTableColumnFlags_WidthFixed, 30);
					ImGui::TableSetupColumn("##Lvl", ImGuiTableColumnFlags_WidthStretch, 60);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::SameLine();
					ImGui::Text(pLocalPC->Name);
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(GetMQColor(ColorName::Yellow).ToImColor()), s_CurrHeading);
					ImGui::TableNextColumn();
					ImGui::Text("Lvl: %d", pLocalPC->GetLevel());
					ImGui::EndTable();
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor();
			float healthPctFloat = static_cast<float>(GetCurHPS()) / GetMaxHPS();
			int healthPctInt = static_cast<int>(healthPctFloat * 100);

			ImVec4 colorHP = CalculateProgressiveColor(barColors.minColorHP, barColors.maxColorHP, healthPctInt);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorHP);
			ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
			float yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(healthPctFloat, ImVec2(0.0f, static_cast<float>(numSet.playerBarHeight)), "##hp");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("HP: %d / %d", GetCurHPS(), GetMaxHPS());
				ImGui::EndTooltip();
			}
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
			ImGui::Text("%d %%", healthPctInt);

			if (GetMaxMana() > 0)
			{
				float manaPctFloat = static_cast<float>(GetCurMana()) / GetMaxMana();
				int manaPctInt = static_cast<int>(manaPctFloat * 100);
				ImVec4 colorMP = CalculateProgressiveColor(barColors.minColorMP, barColors.maxColorMP, manaPctInt);

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorMP);
				ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
				yPos = ImGui::GetCursorPosY();
				ImGui::ProgressBar(manaPctFloat, ImVec2(0.0f, static_cast<float>(numSet.playerBarHeight)), "##Mana");
				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Mana: %d / %d", GetCurMana(), GetMaxMana());
					ImGui::EndTooltip();
				}
				ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
				ImGui::Text("%d %%", manaPctInt);

			}

			float endurPctFloat = static_cast<float>(GetCurEndurance()) / GetMaxEndurance();
			int endurPctInt = static_cast<int>(endurPctFloat * 100);
			ImVec4 colorEP = CalculateProgressiveColor(barColors.minColorEnd, barColors.maxColorEnd, endurPctInt);

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorEP);
			ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(endurPctFloat, ImVec2(0.0f, static_cast<float>(numSet.playerBarHeight)), "##Endur");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Endur: %d / %d", GetCurEndurance(), GetMaxEndurance());
				ImGui::EndTooltip();
			}
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
			ImGui::Text("%d %%", endurPctInt);

			if (!winVis.showTargetWindow)
			{
				ImGui::Separator();
				DrawTargetWindow();
			}
		}
		PopTheme(popCounts);
		ImGui::End();
	}

static void DrawGroupWindow()
{
	//TODO: Group Window
}

static void DrawPetWindow()
{
	if (PSPAWNINFO MyPet = pSpawnManager->GetSpawnByID(pLocalPlayer->PetID))
	{
		const char* petName = MyPet->DisplayedName;

		ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(winThemes.petWinTheme);
		if (ImGui::Begin("Pet##MQ2GrimGUI", &winVis.showPetWindow, s_WindowFlags ))
		{
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2);
			float petPercentage = static_cast<float>(MyPet->HPCurrent) / 100;
			int petLabel = MyPet->HPCurrent;
			
			ImVec4 colorTarHP = CalculateProgressiveColor(barColors.minColorHP, barColors.maxColorHP, MyPet->HPCurrent);
			if (ImGui::BeginTable("Pet", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
			{
				ImGui::TableSetupColumn(petName, ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupColumn("Buffs", ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				if (ImGui::BeginChild("Pet", ImVec2(ImGui::GetColumnWidth(), 55), true, ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar))
				{
					DrawLineOfSight(pLocalPlayer, MyPet);
					ImGui::SameLine();
					ImGui::Text("Lvl");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(GetMQColor(ColorName::Teal).ToImColor()), "%d", MyPet->Level);
					ImGui::SameLine();
					ImGui::Text("Dist:");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(GetMQColor(ColorName::Tangerine).ToImColor()), "%0.1f m", GetDistance(pLocalPlayer, MyPet));
					// pet health bar
					ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
					ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
					yPos = ImGui::GetCursorPosY();
					ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth() - 5, static_cast<float>(numSet.playerBarHeight)), "##");
					ImGui::PopStyleColor();
					ImGui::SetCursorPos(ImVec2(ImGui::GetColumnWidth() / 2, yPos));
					ImGui::Text("%d %%", petLabel);

				}
				ImGui::EndChild();
				if (ImGui::IsItemHovered())
				{
					GiveItem(pSpawnManager->GetSpawnByID(pLocalPlayer->PetID));
				}

				if (ImGui::BeginChild("PetTarget", ImVec2(ImGui::GetColumnWidth(), 55), true, ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar))
				{
					if (PSPAWNINFO pPetTarget = MyPet->WhoFollowing)
					{
						ImGui::Text("Lvl");
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(GetMQColor(ColorName::Teal).ToImColor()), "%d", pPetTarget->Level);
						ImGui::SameLine();
						ImGui::Text(pPetTarget->DisplayedName);
						float petTargetPercentage = static_cast<float>(pPetTarget->HPCurrent) / 100;
						int petTargetLabel = pPetTarget->HPCurrent;
						ImVec4 colorTarHPTarget = CalculateProgressiveColor(barColors.minColorHP, barColors.maxColorHP, pPetTarget->HPCurrent);
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHPTarget);
						ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
						yPos = ImGui::GetCursorPosY();
						ImGui::ProgressBar(petTargetPercentage, ImVec2(ImGui::GetColumnWidth() - 5, static_cast<float>(numSet.playerBarHeight)), "##");
						ImGui::PopStyleColor();
						ImGui::SetCursorPos(ImVec2(ImGui::GetColumnWidth() / 2, yPos));
						ImGui::Text("%d %%", petTargetLabel);
					}
					else
					{
						ImGui::Text("No Target");
						ImGui::NewLine();
					}
				}
				ImGui::EndChild();
				
				if (ImGui::BeginChild("PetButtons", ImVec2(ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y), true, ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar))
				{
					DisplayPetButtons();
				}
				ImGui::EndChild();

				// Pet Buffs Section (Column)
				ImGui::TableNextColumn();
				if (ImGui::BeginChild("PetBuffs", ImVec2(ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y), true, ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar))
				{
					s_spellsInspector->DrawBuffsIcons("PetBuffsTable", pPetInfoWnd->GetBuffRange(), true);
				}
				ImGui::EndChild();

				ImGui::EndTable();
			}

		}
		PopTheme(popCounts);
		ImGui::End();
	}

}

static void DrawSpellWindow()
{
	//TODO: Spell Window
}

static void DrawBuffWindow()
{
	if (!winVis.showBuffWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(winThemes.buffsWinTheme);
	if (ImGui::Begin("Buffs##MQ2GrimGUI", &winVis.showBuffWindow, s_WindowFlags | ImGuiWindowFlags_NoScrollbar))
	{

		s_spellsInspector->DrawBuffsList("BuffTable", pBuffWnd->GetBuffRange(), false, true);

	}
	PopTheme(popCounts);
	ImGui::End();

	//TODO: Buff Window
}

static void DrawSongWindow()
{
	if (!winVis.showSongWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(winThemes.songWinTheme);
	if (ImGui::Begin("Songs##MQ2GrimGUI", &winVis.showSongWindow, s_WindowFlags | ImGuiWindowFlags_NoScrollbar))
	{
		s_spellsInspector->DrawBuffsList("SongTable", pSongWnd->GetBuffRange(), false, true);
	}
	PopTheme(popCounts);
	ImGui::End();
}

static void DrawConfigWindow()
	{
		if (!winVis.showConfigWindow)
			return;

		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Config##ConfigWindow", &winVis.showConfigWindow, s_WindowFlags))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());

			if (ImGui::CollapsingHeader("Color Settings"))
			{
				if (ImGui::BeginTable("##Settings", 2))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImVec4 minHPColor = barColors.minColorHP.ToImColor();
					if (ImGui::ColorEdit4("Min HP Color", (float*)&minHPColor, ImGuiColorEditFlags_NoInputs))
					{
						barColors.minColorHP = MQColor(minHPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum HP Color");

					ImGui::TableNextColumn();

					ImVec4 maxHPColor = barColors.maxColorHP.ToImColor();
					if (ImGui::ColorEdit4("Max HP Color", (float*)&maxHPColor, ImGuiColorEditFlags_NoInputs))
					{
						barColors.maxColorHP = MQColor(maxHPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum HP Color");

					ImGui::TableNextColumn();

					ImVec4 minMPColor = barColors.minColorMP.ToImColor();
					if (ImGui::ColorEdit4("Min MP Color", (float*)&minMPColor, ImGuiColorEditFlags_NoInputs))
					{
						barColors.minColorMP = MQColor(minMPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum MP Color");

					ImGui::TableNextColumn();

					ImVec4 maxMPColor = barColors.maxColorMP.ToImColor();
					if (ImGui::ColorEdit4("Max MP Color", (float*)&maxMPColor, ImGuiColorEditFlags_NoInputs))
					{
						barColors.maxColorMP = MQColor(maxMPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum MP Color");

					ImGui::TableNextColumn();

					ImVec4 minEndColor = barColors.minColorEnd.ToImColor();
					if (ImGui::ColorEdit4("Min End Color", (float*)&minEndColor, ImGuiColorEditFlags_NoInputs))
					{
						barColors.minColorEnd = MQColor(minEndColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum Endurance Color");

					ImGui::TableNextColumn();

					ImVec4 maxEndColor = barColors.maxColorEnd.ToImColor();
					if (ImGui::ColorEdit4("Max End Color", (float*)&maxEndColor, ImGuiColorEditFlags_NoInputs))
					{
						barColors.maxColorEnd = MQColor(maxEndColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum Endurance Color");

					ImGui::EndTable();
				}

				ImGui::SeparatorText("Test Color");

				ImGui::SliderInt("Test Value", &s_TestInt, 0, 100);
				float testVal = static_cast<float>(s_TestInt) / 100;
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(barColors.minColorHP, barColors.maxColorHP, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "HP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(barColors.minColorMP, barColors.maxColorMP, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "MP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(barColors.minColorEnd, barColors.maxColorEnd, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "End##Test");
				ImGui::PopStyleColor();
			}
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Window Settings Sliders"))
			{
				struct SliderOption {
				    const char* label;
				    int* value;
				    int min;
				    int max;
				    const char* helpText;
				};
						
				SliderOption sliderOptions[] = {
				    {"Flash Speed", &numSet.combatFlashInterval, 0, 500, "Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
				    {"Buff Flash Speed", &numSet.flashBuffInterval, 0, 500, "Buff Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
				    {"Buff Icon Size", &numSet.buffIconSize, 10, 40, "Buff Icon Size"},
				    {"Buff Timer Threshold", &numSet.buffTimerThreshold, 0, 3600, "Buff Show Timer Threshold in Seconds (0 = Always Show)"},
				    {"Player Bar Height", &numSet.playerBarHeight, 10, 40, "Player Bar Height"},
				    {"Target Bar Height", &numSet.targetBarHeight, 10, 40, "Target Bar Height"},
				    {"Aggro Bar Height", &numSet.aggroBarHeight, 10, 40, "Aggro Bar Height"}
				};
			
				int sizeX = static_cast<int>(ImGui::GetWindowWidth());
				int col = sizeX / 200;
				if (col < 1)
					col = 1;
				
				if (ImGui::BeginTable("Slider Controls", col))
				{
				    ImGui::TableNextRow();
				    for (const auto& slider : sliderOptions)
					{
						ImGui::TableNextColumn();
				        ImGui::SetNextItemWidth(100);
				        ImGui::SliderInt(slider.label, slider.value, slider.min, slider.max);
				
				        ImGui::SameLine();
				        if (*slider.value == 0 && (std::string(slider.label).find("Flash") != std::string::npos))
						{
							DrawHelpIcon((std::string(slider.label) + " Disabled").c_str());
						} else {
				            DrawHelpIcon(slider.helpText);
				        }
				
				    }
				    ImGui::EndTable();
				}
			
			}
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Window Settings Toggles"))
			{
				if (ImGui::Checkbox("Show Title Bars", &winVis.showTitleBars))
				{
					s_WindowFlags = winVis.showTitleBars ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoTitleBar;
				}
				ImGui::SameLine();
				DrawHelpIcon("Show Title Bars");
			}
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Window Themes"))
			{
				struct ThemeOption {
					const char* label;
					std::string* theme;
				};

				ThemeOption themeOptions[] = {
					{"PlayerWin", &winThemes.playerWinTheme},
					{"PetWin", &winThemes.petWinTheme},
					{"GroupWin", &winThemes.groupWinTheme},
					{"SpellsWin", &winThemes.spellsWinTheme},
					{"BuffsWin", &winThemes.buffsWinTheme},
					{"SongWin", &winThemes.songWinTheme}
				};

				int sizeX = static_cast<int>(ImGui::GetWindowWidth());
				int col = sizeX / 180;
				if (col < 1)
					col = 1;

				if (ImGui::BeginTable("Theme List", col))
				{
					ImGui::TableNextRow();
					for (const auto& theme : themeOptions)
					{
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(100);
						*theme.theme = DrawThemePicker(*theme.theme, theme.label);
					}
					ImGui::EndTable();
				}
			}
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Pet Buttons"))
			{
				TogglePetButtonVisibilityMenu();
			}
			ImGui::Spacing();

			if (ImGui::Button("Save & Close"))
			{
				// only Save when the user clicks the button. 
				// If they close the window and don't click the button the settings will not be saved and only be temporary.
				WritePrivateProfileInt("PlayerTarg", "CombatFlashInterval", numSet.combatFlashInterval, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", numSet.playerBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", numSet.targetBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", numSet.aggroBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("Settings", "BuffIconSize", numSet.buffIconSize, &s_SettingsFile[0]);
				WritePrivateProfileInt("Settings", "FlashBuffInterval", numSet.flashBuffInterval, &s_SettingsFile[0]);
				WritePrivateProfileInt("Buffs", "BuffTimerThreshold", numSet.buffTimerThreshold, &s_SettingsFile[0]);

				WritePrivateProfileBool("Settings", "ShowTitleBars", winVis.showTitleBars, &s_SettingsFile[0]);

				WritePrivateProfileColor("Colors", "MinColorHP", barColors.minColorHP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MaxColorHP", barColors.maxColorHP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MinColorMP", barColors.minColorMP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MaxColorMP", barColors.maxColorMP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MinColorEnd", barColors.minColorEnd, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MaxColorEnd", barColors.maxColorEnd, &s_SettingsFile[0]);

				WritePrivateProfileString("Spells", "Theme", winThemes.spellsWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("Group", "Theme", winThemes.groupWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("PlayerTarg", "Theme", winThemes.playerWinTheme,  &s_SettingsFile[0]);
				WritePrivateProfileString("Pet", "Theme", winThemes.petWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("Buffs", "Theme", winThemes.buffsWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("Songs", "Theme", winThemes.songWinTheme, &s_SettingsFile[0]);

				winVis.showConfigWindow = false;
			}
		}
		ImGui::End();
	}


// Main Window, toggled with /Grimgui command, contains Toggles to show other windows
static void DrawMainWindow()
	{
		if (winVis.showMainWindow)
		{

			ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("GrimGUI##MainWindow", &winVis.showMainWindow, s_WindowFlags))
			{

				struct WindowOption {
					const char* label;
					bool* setting;
					const char* section;
					const char* key;
				};

				WindowOption options[] = {
					{"Player Win", &winVis.showPlayerWindow, "PlayerTarg", "ShowPlayerWindow"},
					{"Target Win", &winVis.showTargetWindow, "PlayerTarg", "SplitTarget"},
					{"Pet Win", &winVis.showPetWindow, "Pet", "ShowPetWindow"},
					{"Buff Win", &winVis.showBuffWindow, "Buffs", "ShowBuffWindow"},
					{"Song Win", &winVis.showSongWindow, "Songs", "ShowSongWindow"},
				};

				int sizeX = static_cast<int>(ImGui::GetWindowWidth());
				int col = 1;
				col = sizeX / 100;
				if (col < 1)
					col = 1;

				if (ImGui::BeginTable("Window List", col))
				{
					ImGui::TableNextRow();
					for (const auto& option : options)
					{
						if (ImGui::Checkbox(option.label, option.setting))
						{
							WritePrivateProfileBool(option.section, option.key, *option.setting, &s_SettingsFile[0]);
						}
						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
				}


				//TODO: More Windows
				//ImGui::Separator();

				//if (ImGui::Checkbox("Spells Win", &winVis.showSpellsWindow))
				//{
				//	WritePrivateProfileBool("Spells", "ShowSpellsWindow", winVis.showSpellsWindow, &s_SettingsFile[0]);
				//}

				//ImGui::SameLine();

				//if (ImGui::Checkbox("Group Win", &winVis.showGroupWindow))
				//{
				//	WritePrivateProfileBool("Group", "ShowGroupWindow", winVis.showGroupWindow, &s_SettingsFile[0]);
				//}

				ImGui::Separator();

				if (ImGui::Button("Config"))
				{
					winVis.showConfigWindow = true;
				}

			}
			ImGui::End();
		}
	}

#pragma endregion

#pragma region Plugin API
PLUGIN_API void OnPulse()
{
	auto now = std::chrono::steady_clock::now();
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (now - g_LastUpdateTime >= g_UpdateInterval)
		{
			if (pAggroInfo)
			{
				numSet.myAggroPct = pAggroInfo->aggroData[AD_Player].AggroPct;
				numSet.secondAggroPct = pAggroInfo->aggroData[AD_Secondary].AggroPct;
				if (pAggroInfo->AggroSecondaryID)
				{
					s_SecondAggroName = GetSpawnByID(pAggroInfo->AggroSecondaryID)->DisplayedName;
				}
				else
				{
					s_SecondAggroName = "Unknown";
				}
			}

			g_LastUpdateTime = now;
		}

		if (numSet.flashBuffInterval > 0)
		{
			if (now - g_LastBuffFlashTime >= std::chrono::milliseconds(500 - numSet.flashBuffInterval))
			{
				winVis.flashTintFlag = !winVis.flashTintFlag;
				g_LastBuffFlashTime = now;
			}
		}
		else
		{
			winVis.flashTintFlag = false;
		}

		if (numSet.combatFlashInterval > 0)
		{
			if (now - g_LastFlashTime >= std::chrono::milliseconds(500 - numSet.combatFlashInterval))
			{
				winVis.flashCombatFlag = !winVis.flashCombatFlag;
				g_LastFlashTime = now;
			}
		}
		else
		{
			winVis.flashCombatFlag = false;
		}

		GetHeading();

	}

	UpdateSettingFile();
}

PLUGIN_API void OnUpdateImGui()
{
	// Draw the GUI elements

	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (winVis.showMainWindow)
		{
			DrawMainWindow();

			if (!winVis.showMainWindow)
			{
				WritePrivateProfileBool("Settings", "ShowMainGui", winVis.showMainWindow, &s_SettingsFile[0]);
			}
		}

		if (winVis.showConfigWindow)
			DrawConfigWindow();

		// Player Window (also target if not split)
		if (winVis.showPlayerWindow)
		{
			DrawPlayerWindow();

			if (!winVis.showPlayerWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", winVis.showPlayerWindow, &s_SettingsFile[0]);
			}
		}

		// Pet Window
		if (winVis.showPetWindow)
		{

			DrawPetWindow();

			if (!winVis.showPetWindow)
			{
				WritePrivateProfileBool("Pet", "ShowPetWindow", winVis.showPetWindow, &s_SettingsFile[0]);
			}
		}

		//Buff Window
		if (winVis.showBuffWindow)
		{
			DrawBuffWindow();

			if (!winVis.showBuffWindow)
			{
				WritePrivateProfileBool("Buffs", "ShowBuffWindow", winVis.showBuffWindow, &s_SettingsFile[0]);
			}
		}

		// Song Window
		if (winVis.showSongWindow)
		{
			DrawSongWindow();

			if (!winVis.showSongWindow)
			{
				WritePrivateProfileBool("Songs", "ShowSongWindow", winVis.showSongWindow, &s_SettingsFile[0]);
			}
		}
		
		// Split Target Window
		if (winVis.showTargetWindow)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
			int popCountsPlay = PushTheme(winThemes.playerWinTheme);
			if (ImGui::Begin("Tar##MQ2GrimGUI", &winVis.showTargetWindow, s_WindowFlags))
			{
				DrawTargetWindow();
			}
			PopTheme(popCountsPlay);
			ImGui::End();

			if (!winVis.showTargetWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "SplitTarget", winVis.showTargetWindow, &s_SettingsFile[0]);
			}
		}
	}
}

/**
 * @fn OnLoadPlugin
 *
 * This is called each time a plugin is loaded (ex: /plugin someplugin), after the
 * plugin has been loaded and any associated -AutoExec.cfg file has been launched.
 * This means it will be executed after the plugin's @ref InitializePlugin callback.
 *
 * This is also called when THIS plugin is loaded, but initialization tasks should
 * still be done in @ref InitializePlugin.
 *
 * @param Name const char* - The name of the plugin that was loaded
 */
PLUGIN_API void OnLoadPlugin(const char* Name)
{
	// check settings file, if logged in use character specific INI else default
	UpdateSettingFile();
	//load settings
	LoadSettings();
	SaveSettings();

	if (!winVis.showTitleBars)
	{
		s_WindowFlags = ImGuiWindowFlags_NoTitleBar;
	}
}

/**
 * @fn OnUnloadPlugin
 *
 * This is called each time a plugin is unloaded (ex: /plugin someplugin unload),
 * just prior to the plugin unloading.  This means it will be executed prior to that
 * plugin's @ref ShutdownPlugin callback.
 *
 * This is also called when THIS plugin is unloaded, but shutdown tasks should still
 * be done in @ref ShutdownPlugin.
 *
 * @param Name const char* - The name of the plugin that is to be unloaded
 */
PLUGIN_API void OnUnloadPlugin(const char* Name)
{
	// DebugSpewAlways("MQ2GrimGUI::OnUnloadPlugin(%s)", Name);
	RemoveCommand("/Grimgui");
	SaveSettings();
}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2GrimGUI");
	AddCommand("/grimgui", GrimCommandHandler, false, false, false);
	WriteChatf("\aw[\ayGrimGUI\ax]\ag /grimgui \atToggles Main Window");
	s_spellsInspector = new SpellsInspector();
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2GrimGUI");
	RemoveCommand("/grimui");
}

#pragma endregion
