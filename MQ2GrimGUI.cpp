#include <mq/Plugin.h>
#include <imgui.h>
#include "main/MQ2Globals.h"
#include "main/MQ2Inlines.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include <chrono>
#include <string>
#include <filesystem>
#include "SpellPicker.h"
#include "SpellInspector.h"
#include "MQ2GrimGUI.h"
#include "Theme.h"

PreSetup("MQ2GrimGUI");
PLUGIN_VERSION(0.2);

#pragma region Main Setting Variables
// Declare global plugin state variables

static bool s_IsCasting = false;
static bool s_CharIniLoaded = false;
static bool s_DefaultLoaded = false;
static bool s_IsCaster = false;
static int s_TestInt = 100; // Color Test Value for Config Window
static char s_SettingsFile[MAX_PATH] = { 0 };

static ImGuiWindowFlags s_WindowFlags = ImGuiWindowFlags_None;
static ImGuiChildFlags s_ChildFlags = ImGuiChildFlags_None;

static const char* s_SecondAggroName = "Unknown";
static const char* s_CurrHeading = "N";
static int s_TarBuffLineSize = 0;

SpellPicker* pSpellPicker = nullptr;
GrimGui::SpellsInspector* pSpellInspector = nullptr;

static bool s_MemSpell = false;
std::string s_MemSpellName;
static int s_MemGemIndex = 0;

#pragma endregion



#pragma region Timers
std::chrono::steady_clock::time_point g_LastUpdateTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastBuffFlashTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_StartCastTime;

const auto g_UpdateInterval		= std::chrono::milliseconds(250);

#pragma endregion


#pragma region Settings Functions

static void LoadSettings()
{
	// Load settings from the INI file

	for (const auto& setting : winVisSettings)
	{
		bool defaultVal = false;
		if (setting.key == "ShowMainGui")
			defaultVal = true;
		else if (setting.key == "ShowTitleBars")
			defaultVal = true;

		*setting.setting = GetPrivateProfileBool(setting.section, setting.key, defaultVal , &s_SettingsFile[0]);
	}

	for (const auto& setting : numericSettings)
	{
		*setting.value = GetPrivateProfileInt(setting.section, setting.key, *setting.value, &s_SettingsFile[0]);
	}

	for (const auto& setting : themeSettings)
	{
		*setting.theme = GetPrivateProfileString(setting.section, setting.key, "Default", &s_SettingsFile[0]);
	}

	for (const auto& setting : colorSettings)
	{
		*setting.value = GetPrivateProfileColor(setting.section, setting.key, *setting.value, &s_SettingsFile[0]);
	}

	for (auto& button : petButtons)
	{
		button.visible = GetPrivateProfileBool("Pet", button.name.c_str(), &button.visible, &s_SettingsFile[0]);
	}
}

static void SaveSettings()
{
	for (const auto& setting : winVisSettings)
	{
		WritePrivateProfileBool(setting.section, setting.key, *setting.setting, &s_SettingsFile[0]);
	}

	for (const auto& setting : numericSettings)
	{
		WritePrivateProfileInt(setting.section, setting.key, *setting.value, &s_SettingsFile[0]);
	}

	for (const auto& setting : themeSettings)
	{
		WritePrivateProfileString(setting.section, setting.key, setting.theme->c_str(), &s_SettingsFile[0]);
	}

	for (const auto& setting : colorSettings)
	{
		WritePrivateProfileColor(setting.section, setting.key, *setting.value, &s_SettingsFile[0]);
	}

	for (const auto& button : petButtons)
	{
		WritePrivateProfileBool("Pet", button.name.c_str(), button.visible, &s_SettingsFile[0]);
	}
}

static void TogglePetButtonVisibilityMenu()
{
	int numColumns = static_cast<int>(1, (ImGui::GetWindowWidth() - 10) / 75);
	if (numColumns < 1)
		numColumns = 1;

	if (ImGui::BeginTable("CheckboxTable", numColumns, ImGuiTableFlags_SizingStretchProp))
	{
		for (auto& button : petButtons)
		{
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

/**
* @brief Updates the settings file based on the current game state
*/
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
				if (GetMaxMana() > 0)
					s_IsCaster = true;
			}		
			pSpellPicker->InitializeSpells();
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
			s_IsCaster = false;
		}
		pSpellPicker->~SpellPicker();
	}
}

#pragma endregion



#pragma region Helpers 

static void GetHeading()
{
	static PSPAWNINFO pSelfInfo = pLocalPlayer;
	s_CurrHeading = szHeadingShort[static_cast<int>((pSelfInfo->Heading / 32.0f) + 8.5f) % 16];
}


static void PrintGrimHelp()
{
	for (const auto& cmdInfo : commandList)
	{
		WriteChatf("\aw[\ayGrimGUI\ax] \at/grimgui\ax [\ag%s\ax] - \ay%s", cmdInfo.commandText, cmdInfo.description);
	}
}


static void GrimCommandHandler(PlayerClient* pPC, const char* szLine)
{
	char arg[MAX_STRING] = {};
	GetArg(arg, szLine, 1);

	if (strlen(arg))
	{
		GrimCommand command;

		if (strcmp(arg, "show") == 0)
			command = GrimCommand::Show;
		else if (strcmp(arg, "player") == 0)
			command = GrimCommand::Player;
		else if (strcmp(arg, "target") == 0)
			command = GrimCommand::Target;
		else if (strcmp(arg, "pet") == 0)
			command = GrimCommand::Pet;
		else if (strcmp(arg, "group") == 0)
			command = GrimCommand::Group;
		else if (strcmp(arg, "spells") == 0)
			command = GrimCommand::Spells;
		else if (strcmp(arg, "buffs") == 0)
			command = GrimCommand::Buffs;
		else if (strcmp(arg, "songs") == 0)
			command = GrimCommand::Songs;
		else if (strcmp(arg, "config") == 0)
			command = GrimCommand::Config;
		else if (strcmp(arg, "help") == 0)
			command = GrimCommand::Help;
		else
			return;

		switch (command)
		{
		case GrimCommand::Help:
			PrintGrimHelp();
			break;
		case GrimCommand::Show:
			s_WinVis.showMainWindow = !s_WinVis.showMainWindow;
			WritePrivateProfileBool("Settings", "ShowMainGui", s_WinVis.showMainWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Player:
			s_WinVis.showPlayerWindow = !s_WinVis.showPlayerWindow;
			WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_WinVis.showPlayerWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Target:
			s_WinVis.showTargetWindow = !s_WinVis.showTargetWindow;
			WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_WinVis.showTargetWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Pet:
			s_WinVis.showPetWindow = !s_WinVis.showPetWindow;
			WritePrivateProfileBool("Pet", "ShowPetWindow", s_WinVis.showPetWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Group:
			s_WinVis.showGroupWindow = !s_WinVis.showGroupWindow;
			WritePrivateProfileBool("Group", "ShowGroupWindow", s_WinVis.showGroupWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Spells:
			s_WinVis.showSpellsWindow = !s_WinVis.showSpellsWindow;
			WritePrivateProfileBool("Spells", "ShowSpellsWindow", s_WinVis.showSpellsWindow, & s_SettingsFile[0]);
			break;
		case GrimCommand::Buffs:
			s_WinVis.showBuffWindow = !s_WinVis.showBuffWindow;
			WritePrivateProfileBool("Buffs", "ShowBuffWindow", s_WinVis.showBuffWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Songs:
			s_WinVis.showSongWindow = !s_WinVis.showSongWindow;
			WritePrivateProfileBool("Songs", "ShowSongWindow", s_WinVis.showSongWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Config:
			s_WinVis.showConfigWindow = !s_WinVis.showConfigWindow;
			break;
		}
	}
	else
	{
		s_WinVis.showMainWindow = !s_WinVis.showMainWindow;
		PrintGrimHelp();
	}
}


/**
* @fn DrawLineOfSight
*
* @brief Draws a line of sight indicator based on the result of the LineOfSight function
* 
* @param pFrom PSPAWNINFO Pointer to the source spawn
* @param pTo PSPAWNINFO Pointer to the target spawn
*/
static void DrawLineOfSight(PSPAWNINFO pFrom, PSPAWNINFO pTo)
{
	if (LineOfSight(pFrom, pTo))
		ImGui::TextColored(GetMQColor(ColorName::Green).ToImColor(), ICON_MD_VISIBILITY);
	else
		ImGui::TextColored(GetMQColor(ColorName::Red).ToImColor(), ICON_MD_VISIBILITY_OFF);
}


/**
* @fn DrawHelpIcon
*
* @brief Draws a help icon with a tooltip
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
	if (!pSpawn)
		return;

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		pTarget = pSpawn;

		if (ItemPtr pItem = GetPcProfile()->GetInventorySlot(InvSlot_Cursor))
			EzCommand("/click left target");
	}
}


/**
* @fn DrawBar
* 
*  @brief Draws a progress bar, using Progressive coloring with a label and tooltip
* 
*  @param label const char* Label to display on the progress bar
*  @param current int Current value of the progress bar
*  @param max int Maximum value of the progress bar
*  @param height int Height of the progress bar
*  @param minColor MQColor Minimum color of the progress bar
*  @param maxColor MQColor Maximum color of the progress bar
*  @param tooltip const char* Tooltip text to display when hovering over the progress bar
*/
static void DrawBar(const char* label, int current, int max, int height, mq::MQColor minColor, mq::MQColor maxColor, const char* tooltip)
{
	float percentage = static_cast<float>(current) / max;
	int percentageInt = static_cast<int>(percentage * 100);
	ImVec4 color = CalculateProgressiveColor(minColor, maxColor, percentageInt);

	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
	ImGui::ProgressBar(percentage, ImVec2(ImGui::GetWindowWidth() - 15, static_cast<float>(height)), label);
	ImGui::PopStyleColor();

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("%s: %d / %d", tooltip, current, max);
		ImGui::EndTooltip();
	}
}


static void DrawPetInfo(PSPAWNINFO petInfo, bool showAll = true)
{
	const char* petName = petInfo->DisplayedName;
	float sizeX = ImGui::GetWindowWidth();
	float yPos = ImGui::GetCursorPosY();
	float midX = (sizeX / 2);
	float petPercentage = static_cast<float>(petInfo->HPCurrent) / 100;
	int petLabel = petInfo->HPCurrent;

	ImVec4 colorTarHP = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, petInfo->HPCurrent);
	if (showAll)
	{
		if (ImGui::BeginChild("Pet", ImVec2(ImGui::GetColumnWidth(), 0),
			ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
		{
			DrawLineOfSight(pLocalPlayer, petInfo);
			ImGui::SameLine();
			ImGui::Text("Lvl");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(GetMQColor(ColorName::Teal).ToImColor()), "%d", petInfo->Level);
			ImGui::SameLine();
			ImGui::Text("Dist:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(GetMQColor(ColorName::Tangerine).ToImColor()), "%0.1f m", GetDistance(pLocalPlayer, petInfo));
			
			// health bar
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth() , static_cast<float>(s_NumSettings.playerBarHeight)), "##");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnWidth() / 2, yPos));
			ImGui::Text("%d %%", petLabel);
		}
		ImGui::EndChild();
	}
	else
	{
		// just draw a green pet health bar at size 8 for group window.
		if (ImGui::BeginChild("PetBar", ImVec2(ImGui::GetColumnWidth(), 0),
			ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysAutoResize ))
		{
			float barSize = static_cast<float>(s_NumSettings.groupBarHeight * 0.75);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Green2).ToImColor()));
			ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth() , barSize), "##");
			ImGui::PopStyleColor();
		}
		ImGui::EndChild();
	}
	if (ImGui::IsItemHovered())
	{
		GiveItem(pSpawnManager->GetSpawnByID(pLocalPlayer->PetID));
		ImGui::BeginTooltip();
		ImGui::Text("%s  %d%", petName, petInfo->HPCurrent);
		ImGui::EndTooltip();
	}
}


/**
* @fn DrawPlayerBars
* 
* @brief Draws the player health, mana, and endurance bars with a combat border if specified
* 
* @param drawCombatBorder bool Draw a combat border around the player bars
* @param barHeight int Height of the player bars, Defaults to playerBarHeight but you can pass a different value
*/
static void DrawPlayerBars(bool drawCombatBorder = false, int barHeight = s_NumSettings.playerBarHeight, bool drawPet = false)
{
	if (!pLocalPC)
		return;

	if (ImGui::BeginChild(pLocalPC->Name, ImVec2(ImGui::GetContentRegionAvail().x, 0),
		ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
	{
		ImGuiChildFlags s_ChildFlags = drawCombatBorder ? ImGuiChildFlags_Border : ImGuiChildFlags_None;

		if (drawCombatBorder && pEverQuestInfo->bAutoAttack)
		{
			ImVec4 borderColor = s_WinVis.flashCombatFlag ? ImVec4(GetMQColor(ColorName::Red).ToImColor()) : ImVec4(GetMQColor(ColorName::White).ToImColor());
			ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
		}
		else if (drawCombatBorder)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(GetMQColor(ColorName::White).ToImColor()));
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));

		if (ImGui::BeginChild("info", ImVec2(ImGui::GetContentRegionAvail().x, 26),
			s_ChildFlags | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int midX = (sizeX / 2) - 8;

			if (ImGui::BeginTable("##Player", 3))
			{
				ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch, ImGui::GetContentRegionAvail().x * .5f);
				ImGui::TableSetupColumn("##Heading", ImGuiTableColumnFlags_WidthFixed, 30);
				ImGui::TableSetupColumn("##Lvl", ImGuiTableColumnFlags_WidthStretch, 60);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
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
		if (drawCombatBorder)
			ImGui::PopStyleColor();

		// Health bar cause if we have no health we are dead!
		DrawBar("##hp", GetCurHPS(), GetMaxHPS(), barHeight, s_BarColors.minColorHP, s_BarColors.maxColorHP, "HP");

		// Mana bar if you have mana that is
		if (GetMaxMana() > 0)
			DrawBar("##Mana", GetCurMana(), GetMaxMana(), barHeight, s_BarColors.minColorMP, s_BarColors.maxColorMP, "Mana");

		// Endurance bar does anyone even use this?
		DrawBar("##Endur", GetCurEndurance(), GetMaxEndurance(), barHeight, s_BarColors.minColorEnd, s_BarColors.maxColorEnd, "Endur");

		if (drawPet)
		{
			// go Minion go!
			if (PSPAWNINFO MyPet = pSpawnManager->GetSpawnByID(pLocalPlayer->PetID))
				DrawPetInfo(MyPet, false);
		}
	}
	ImGui::EndChild();
}


/**
* @fn DrawMemberInfo
* 
* @brief Draws the group member Info before the bars, this will show for out of zone group members
* 
* @param pMember CGroupMember* Pointer to the group member to draw the info for
*/
static void DrawMemberInfo(CGroupMember* pMember)
{
	if (!pMember)
	return;

	if (ImGui::BeginTable("Group", 2))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, -1);
		ImGui::TableSetupColumn("Lvl", ImGuiTableColumnFlags_WidthFixed, 80);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%s", pMember->GetName());
		ImGui::TableNextColumn();
		ImGui::Text("%d", pMember->Level);
		ImGui::EndTable();
	}

}

/**
* @fn DrawGroupMemberBars
* 
* @brief Draws the group member health, mana (If you have any), and endurance bars
* 
* @param pMember CGroupMember* Pointer to the group member to draw the bars for
*/
static void DrawGroupMemberBars(CGroupMember* pMember, bool drawPet = true, int groupSlot = 1)
{
	if (!pMember)
		return;

	float sizeY = static_cast<float>(s_NumSettings.groupBarHeight) * 4 + 50;

	if (!pMember->GetPlayer())
	{
		if (ImGui::BeginChild(("##Empty%d", groupSlot), ImVec2(ImGui::GetContentRegionAvail().x, sizeY),
			ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar))
		{
			DrawMemberInfo(pMember);
		}
		ImGui::EndChild();

		return;
	}

	SPAWNINFO* pSpawn = pMember->GetPlayer();

	if (ImGui::BeginChild(pSpawn->Name, ImVec2(ImGui::GetContentRegionAvail().x, 0),
		ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
	{
		
		ImGui::PushID(pSpawn->Name);
		ImGui::BeginGroup();
		{

			DrawMemberInfo(pMember);

			// Health bar "Not Deat Yet!"
			if (pSpawn->HPCurrent && pSpawn->HPMax)
				DrawBar("##hp", pSpawn->HPCurrent, pSpawn->HPMax, s_NumSettings.groupBarHeight, s_BarColors.minColorHP, s_BarColors.maxColorHP, "HP");

			// Mana bar maybe?
			if (pSpawn->ManaCurrent && pSpawn->ManaMax)
				DrawBar("##Mana", pSpawn->ManaCurrent, pSpawn->ManaMax, s_NumSettings.groupBarHeight, s_BarColors.minColorMP, s_BarColors.maxColorMP, "Mana");
			
			// Endurance bar again does anyone even use this? beside when you can't jump anymore.
			if (pSpawn->EnduranceCurrent && pSpawn->EnduranceMax)
				DrawBar("##Endur", pSpawn->EnduranceCurrent, pSpawn->EnduranceMax, s_NumSettings.groupBarHeight, s_BarColors.minColorEnd, s_BarColors.maxColorEnd, "Endurance");
		
			ImGui::EndGroup();
		}
		ImGui::PopID();
		if (ImGui::IsItemHovered())
			GiveItem(pSpawn);

		if (drawPet)
		{
			// fluffy
			if (PSPAWNINFO petInfo = pSpawnManager->GetSpawnByID(pSpawn->PetID))
				DrawPetInfo(petInfo, false);
		}
	}
	ImGui::EndChild();
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
			ImVec4 colorTarHP = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, CurTarget->HPCurrent);

			if (strncmp(pTarget->DisplayedName, pLocalPC->Name, 64) == 0)
			{
				tarPercentage = static_cast<float>(CurTarget->HPCurrent) / CurTarget->HPMax;
				int healthIntPct = static_cast<int>(tarPercentage * 100);
				tar_label = healthIntPct;
				colorTarHP = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, healthIntPct);
			}
			DrawLineOfSight(pLocalPlayer, pTarget);
			ImGui::SameLine();
			ImGui::Text(CurTarget->DisplayedName);

			ImGui::SameLine(sizeX * .75f);
			ImGui::TextColored(GetMQColor(ColorName::Tangerine).ToImColor(), "%0.1f m", GetDistance(pLocalPlayer, pTarget));

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(tarPercentage, ImVec2(0.0f, static_cast<float>(s_NumSettings.targetBarHeight)), "##");
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


			if (s_NumSettings.myAggroPct < 100)
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Orange).ToImColor()));
			else
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Purple).ToImColor()));
			
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(static_cast<float>(s_NumSettings.myAggroPct) / 100, ImVec2(0.0f, static_cast<float>(s_NumSettings.aggroBarHeight)), "##Aggro");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(10, yPos));
			ImGui::Text(s_SecondAggroName);
			ImGui::SetCursorPos(ImVec2((sizeX/2)-8, yPos));
			ImGui::Text("%d %%", s_NumSettings.myAggroPct);
			ImGui::SetCursorPos(ImVec2(sizeX - 40, yPos));
			ImGui::Text("%d %%", s_NumSettings.secondAggroPct);	

			if (ImGui::BeginChild("TargetBuffs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Border , ImGuiWindowFlags_NoScrollbar))
			{

				if (gTargetbuffs)
					pSpellInspector->DrawBuffsIcons("TargetBuffsTable", pTargetWnd->GetBuffRange(), false);
			}
			ImGui::EndChild();
		}
	}

static void DrawPlayerWindow()
	{
		if (!s_WinVis.showPlayerWindow)
			return;

		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.playerWinTheme);
		if (ImGui::Begin("Player##MQ2GrimGUI", &s_WinVis.showPlayerWindow, s_WindowFlags | ImGuiWindowFlags_MenuBar))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int midX = (sizeX / 2) - 8;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Main"))
				{
					if (ImGui::MenuItem("Split Target", NULL, s_WinVis.showTargetWindow))
					{
						s_WinVis.showTargetWindow = !s_WinVis.showTargetWindow;
						WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_WinVis.showTargetWindow, &s_SettingsFile[0]);
					}

					if (ImGui::MenuItem("Show Config", NULL, s_WinVis.showConfigWindow))
						s_WinVis.showConfigWindow = !s_WinVis.showConfigWindow;

					if (ImGui::MenuItem("Show Main", NULL, s_WinVis.showMainWindow))
					{
						s_WinVis.showMainWindow = !s_WinVis.showMainWindow;
						WritePrivateProfileBool("Settings", "ShowMainGui", s_WinVis.showMainWindow, &s_SettingsFile[0]);
					}

					ImGui::EndMenu();
				}
				
				ImGui::EndMenuBar();
			}

			DrawPlayerBars(true);

			if (!s_WinVis.showTargetWindow)
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
	if (!s_WinVis.showGroupWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.groupWinTheme);

	if (ImGui::Begin("Group##MQ2GrimGUI", &s_WinVis.showGroupWindow, s_WindowFlags))
	{
		DrawPlayerBars(false, s_NumSettings.groupBarHeight, true);

		PCHARINFO pChar = GetCharInfo();
		if (pChar && pChar->pGroupInfo)
		{
			for (int i = 1; i < MAX_GROUP_SIZE; i++)
			{
				if (CGroupMember* pMember = pChar->pGroupInfo->GetGroupMember(i))
					DrawGroupMemberBars(pMember, true, i);
			}
		}

		ImGui::Spacing();
		ImGui::Separator();

		float posX = ImGui::GetWindowWidth() * 0.5f - 65;
		if (posX < 0)
			posX = 0;

		ImGui::SetCursorPosX(posX);

		if (pLocalPlayer->InvitedToGroup)
		{
			if (ImGui::Button("Accept", ImVec2(60, 20)))
				EzCommand("/invite");
		}
		else
		{
			if (ImGui::Button("Invite", ImVec2(60, 20)))
			{
				if (pTarget)
					EzCommand(fmt::format("/invite {}", pTarget->Name).c_str());
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Disband", ImVec2(60, 20)))
			EzCommand("/disband");
	}
	PopTheme(popCounts);
	ImGui::End();
}



static void DrawPetWindow()
{
	if (PSPAWNINFO MyPet = pSpawnManager->GetSpawnByID(pLocalPlayer->PetID))
	{
		const char* petName = MyPet->DisplayedName;

		ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.petWinTheme);
		if (ImGui::Begin("Pet##MQ2GrimGUI", &s_WinVis.showPetWindow, s_WindowFlags ))
		{
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2);

			if (ImGui::BeginTable("Pet", 2,
				ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
			{
				ImGui::TableSetupColumn(petName, ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupColumn("Buffs", ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				
				DrawPetInfo(MyPet);

				if (ImGui::BeginChild("PetTarget", ImVec2(ImGui::GetColumnWidth(), 0),
					ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
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
						ImVec4 colorTarHPTarget = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, pPetTarget->HPCurrent);
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHPTarget);
						ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
						yPos = ImGui::GetCursorPosY();
						ImGui::ProgressBar(petTargetPercentage, ImVec2(ImGui::GetColumnWidth() , static_cast<float>(s_NumSettings.playerBarHeight)), "##");
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
				
				if (ImGui::BeginChild("PetButtons", ImVec2(ImGui::GetColumnWidth(), 0),
					ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
					DisplayPetButtons();

				ImGui::EndChild();

				// Pet Buffs Section (Column)
				ImGui::TableNextColumn();
				
				if (ImGui::BeginChild("PetBuffs", ImVec2(ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y), 
					ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar))
					pSpellInspector->DrawBuffsIcons("PetBuffsTable", pPetInfoWnd->GetBuffRange(), true);
				
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
	if (!s_WinVis.showSpellsWindow)
		return;

	if (s_IsCaster)
	{
		ImGui::SetNextWindowSize(ImVec2(100, 350), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.spellsWinTheme);
	
		if (ImGui::Begin("Spells##MQ2GrimGUI", &s_WinVis.showSpellsWindow,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			pSpellInspector->DrawSpellBarIcons(s_NumSettings.spellGemHeight);
		}
		ImGui::End();
		PopTheme(popCounts);
	}

	if (pCastingWnd && pCastingWnd->IsVisible())
	{
		if (!s_IsCasting)
		{
			g_StartCastTime = std::chrono::steady_clock::now();
			s_IsCasting = true;

		}
		ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.spellsWinTheme);
		if (ImGui::Begin("Casting##MQ2GrimGUI", &s_IsCasting,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
		{
			const char* spellName = pCastingWnd->GetChildItem("Casting_SpellName")->WindowText.c_str();
			EQ_Spell* pSpell = GetSpellByName(spellName);
			if (pSpell)
			{
				auto now = std::chrono::steady_clock::now();
				if (now - g_StartCastTime > std::chrono::milliseconds(pSpell->CastTime))
				{
					s_IsCasting = false;
				}
				else
				{
					int spellTimer = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - g_StartCastTime).count());
					float castingTime = static_cast<float>(pSpell->CastTime);
					float spellProgress = 1.0f - static_cast<float>(spellTimer / castingTime);
					ImVec4 colorCastBar = CalculateProgressiveColor(s_BarColors.minColorCast, s_BarColors.maxColorCast, static_cast<int>(spellProgress * 100));

					ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorCastBar);
					ImGui::ProgressBar(spellProgress, ImVec2(ImGui::GetContentRegionAvail().x, 12 ), "##CastingProgress");
					ImGui::PopStyleColor();
					ImGui::Text("%s %0.1fs",spellName, (castingTime - spellTimer) /1000);
					pCastingWnd->GetChildItem("Casting_Gauge")->GetSidlPiece("Casting_Gauge");
				}
			}

		}
		ImGui::End();
		PopTheme(popCounts);
	}
	else
		s_IsCasting = false;
}


static void DrawBuffWindow()
{
	if (!s_WinVis.showBuffWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.buffsWinTheme);
	if (ImGui::Begin("Buffs##MQ2GrimGUI", &s_WinVis.showBuffWindow,
		s_WindowFlags | ImGuiWindowFlags_NoScrollbar))
		pSpellInspector->DrawBuffsList("BuffTable", pBuffWnd->GetBuffRange(), false, true);

	PopTheme(popCounts);
	ImGui::End();

}

static void DrawSongWindow()
{
	if (!s_WinVis.showSongWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.songWinTheme);
	if (ImGui::Begin("Songs##MQ2GrimGUI", &s_WinVis.showSongWindow,
		s_WindowFlags | ImGuiWindowFlags_NoScrollbar))
		pSpellInspector->DrawBuffsList("SongTable", pSongWnd->GetBuffRange(), false, true);
	
	PopTheme(popCounts);
	ImGui::End();
}

static void DrawConfigWindow()
{
	if (!s_WinVis.showConfigWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Config##ConfigWindow", &s_WinVis.showConfigWindow, s_WindowFlags))
	{
		int sizeX = static_cast<int>(ImGui::GetWindowWidth());

		if (ImGui::CollapsingHeader("Color Settings"))
		{
			if (ImGui::BeginTable("##Settings", 2))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImVec4 minHPColor = s_BarColors.minColorHP.ToImColor();
				if (ImGui::ColorEdit4("Min HP Color", (float*)&minHPColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.minColorHP = MQColor(minHPColor);
				
				ImGui::SameLine();
				DrawHelpIcon("Minimum HP Color");

				ImGui::TableNextColumn();

				ImVec4 maxHPColor = s_BarColors.maxColorHP.ToImColor();
				if (ImGui::ColorEdit4("Max HP Color", (float*)&maxHPColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.maxColorHP = MQColor(maxHPColor);
				
				ImGui::SameLine();
				DrawHelpIcon("Maximum HP Color");

				ImGui::TableNextColumn();

				ImVec4 minMPColor = s_BarColors.minColorMP.ToImColor();
				if (ImGui::ColorEdit4("Min MP Color", (float*)&minMPColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.minColorMP = MQColor(minMPColor);

				ImGui::SameLine();
				DrawHelpIcon("Minimum MP Color");

				ImGui::TableNextColumn();

				ImVec4 maxMPColor = s_BarColors.maxColorMP.ToImColor();
				if (ImGui::ColorEdit4("Max MP Color", (float*)&maxMPColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.maxColorMP = MQColor(maxMPColor);

				ImGui::SameLine();
				DrawHelpIcon("Maximum MP Color");

				ImGui::TableNextColumn();

				ImVec4 minEndColor = s_BarColors.minColorEnd.ToImColor();
				if (ImGui::ColorEdit4("Min End Color", (float*)&minEndColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.minColorEnd = MQColor(minEndColor);

				ImGui::SameLine();
				DrawHelpIcon("Minimum Endurance Color");

				ImGui::TableNextColumn();

				ImVec4 maxEndColor = s_BarColors.maxColorEnd.ToImColor();
				if (ImGui::ColorEdit4("Max End Color", (float*)&maxEndColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.maxColorEnd = MQColor(maxEndColor);

				ImGui::SameLine();
				DrawHelpIcon("Maximum Endurance Color");

				ImGui::TableNextColumn();

				ImVec4 minCastColor = s_BarColors.minColorCast.ToImColor();

				if (ImGui::ColorEdit4("Min Cast Color", (float*)&minCastColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.minColorCast = MQColor(minCastColor);

				ImGui::SameLine();

				DrawHelpIcon("Minimum Cast Color");

				ImGui::TableNextColumn();

				ImVec4 maxCastColor = s_BarColors.maxColorCast.ToImColor();

				if (ImGui::ColorEdit4("Max Cast Color", (float*)&maxCastColor, ImGuiColorEditFlags_NoInputs))
					s_BarColors.maxColorCast = MQColor(maxCastColor);

				ImGui::SameLine();

				DrawHelpIcon("Maximum Cast Color");

				ImGui::EndTable();
			}

			ImGui::SeparatorText("Test Color");

			ImGui::SliderInt("Test Value", &s_TestInt, 0, 100);
			float testVal = static_cast<float>(s_TestInt) / 100;
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Health##Test");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_BarColors.minColorMP, s_BarColors.maxColorMP, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Mana##Test");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_BarColors.minColorEnd, s_BarColors.maxColorEnd, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Endur##Test");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_BarColors.minColorCast, s_BarColors.maxColorCast, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Cast##Test");
			ImGui::PopStyleColor();
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Window Settings Sliders"))
		{

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
			if (ImGui::Checkbox("Show Title Bars", &s_WinVis.showTitleBars))
				s_WindowFlags = s_WinVis.showTitleBars ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoTitleBar;
			
			ImGui::SameLine();
			DrawHelpIcon("Show Title Bars");
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Window Themes"))
		{
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
			TogglePetButtonVisibilityMenu();
		
		ImGui::Spacing();

		if (ImGui::Button("Save & Close"))
		{
			// only Save when the user clicks the button. 
			// If they close the window and don't click the button the settings will not be saved and only be temporary.
			s_WinVis.showConfigWindow = false;
			SaveSettings();
		}
	}
	ImGui::End();
}


// Main Window, toggled with /Grimgui command, contains Toggles to show other windows
static void DrawMainWindow()
{
	if (s_WinVis.showMainWindow)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("GrimGUI##MainWindow", &s_WinVis.showMainWindow, s_WindowFlags))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int col = 1;
			col = sizeX / 100;
			if (col < 1)
				col = 1;

			if (ImGui::BeginTable("Window List", col))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				for (const auto& option : options)
				{
					if (ImGui::Checkbox(option.label, option.setting))
						WritePrivateProfileBool(option.section, option.key, *option.setting, &s_SettingsFile[0]);
					
					ImGui::TableNextColumn();
				}
				ImGui::EndTable();
			}

			ImGui::Separator();

			if (ImGui::Button("Config"))
				s_WinVis.showConfigWindow = true;

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
				s_NumSettings.myAggroPct = pAggroInfo->aggroData[AD_Player].AggroPct;
				s_NumSettings.secondAggroPct = pAggroInfo->aggroData[AD_Secondary].AggroPct;
				
				if (pAggroInfo->AggroSecondaryID)
					s_SecondAggroName = GetSpawnByID(pAggroInfo->AggroSecondaryID)->DisplayedName;
				else
					s_SecondAggroName = "Unknown";
			}

			g_LastUpdateTime = now;
		}

		if (s_NumSettings.flashBuffInterval > 0)
		{
			if (now - g_LastBuffFlashTime >= std::chrono::milliseconds(500 - s_NumSettings.flashBuffInterval))
			{
				s_WinVis.flashTintFlag = !s_WinVis.flashTintFlag;
				g_LastBuffFlashTime = now;
			}
		}
		else
		{
			s_WinVis.flashTintFlag = false;
		}

		if (s_NumSettings.combatFlashInterval > 0)
		{
			if (now - g_LastFlashTime >= std::chrono::milliseconds(500 - s_NumSettings.combatFlashInterval))
			{
				s_WinVis.flashCombatFlag = !s_WinVis.flashCombatFlag;
				g_LastFlashTime = now;
			}
		}
		else
		{
			s_WinVis.flashCombatFlag = false;
		}

		GetHeading();

		if (pSpellPicker->SelectedSpell)
		{
			s_MemSpellName = pSpellPicker->SelectedSpell->Name;

			std::string memCommand = std::to_string(s_MemGemIndex) + " \"" + s_MemSpellName + "\"";

			WriteChatf("MemSpell: %s", memCommand.c_str());
			MemSpell(pLocalPlayer, memCommand.c_str());
			pSpellPicker->ClearSelection();
			s_MemGemIndex = 0;
		}
	}

	UpdateSettingFile();
}

PLUGIN_API void OnUpdateImGui()
{
	// Draw the GUI elements

	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (s_WinVis.showMainWindow)
		{
			DrawMainWindow();

			if (!s_WinVis.showMainWindow)
				WritePrivateProfileBool("Settings", "ShowMainGui", s_WinVis.showMainWindow, &s_SettingsFile[0]);
		}

		if (s_WinVis.showConfigWindow)
			DrawConfigWindow();

		// Player Window (also target if not split)
		if (s_WinVis.showPlayerWindow)
		{
			DrawPlayerWindow();

			if (!s_WinVis.showPlayerWindow)
				WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_WinVis.showPlayerWindow, &s_SettingsFile[0]);
		}

		// Pet Window
		if (s_WinVis.showPetWindow)
		{

			DrawPetWindow();

			if (!s_WinVis.showPetWindow)
				WritePrivateProfileBool("Pet", "ShowPetWindow", s_WinVis.showPetWindow, &s_SettingsFile[0]);
		}

		//Buff Window
		if (s_WinVis.showBuffWindow)
		{
			DrawBuffWindow();

			if (!s_WinVis.showBuffWindow)
				WritePrivateProfileBool("Buffs", "ShowBuffWindow", s_WinVis.showBuffWindow, &s_SettingsFile[0]);
		}

		// Song Window
		if (s_WinVis.showSongWindow)
		{
			DrawSongWindow();

			if (!s_WinVis.showSongWindow)
				WritePrivateProfileBool("Songs", "ShowSongWindow", s_WinVis.showSongWindow, &s_SettingsFile[0]);
		}
		
		// Split Target Window
		if (s_WinVis.showTargetWindow)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
			int popCountsPlay = PushTheme(s_WinTheme.playerWinTheme);
			if (ImGui::Begin("Tar##MQ2GrimGUI", &s_WinVis.showTargetWindow, s_WindowFlags))
				DrawTargetWindow();
			
			PopTheme(popCountsPlay);
			ImGui::End();

			if (!s_WinVis.showTargetWindow)
				WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_WinVis.showTargetWindow, &s_SettingsFile[0]);
		}

		// Group Window
		if (s_WinVis.showGroupWindow)
		{
			DrawGroupWindow();

			if (!s_WinVis.showGroupWindow)
				WritePrivateProfileBool("Group", "ShowGroupWindow", s_WinVis.showGroupWindow, &s_SettingsFile[0]);
		}

		// Spell Window
		if (s_WinVis.showSpellsWindow)
		{
			DrawSpellWindow();

			if (!s_WinVis.showSpellsWindow)
				WritePrivateProfileBool("Spells", "ShowSpellsWindow", s_WinVis.showSpellsWindow, &s_SettingsFile[0]);
		}

		// Spell Picker
		if (pSpellPicker)
			pSpellPicker->DrawSpellPicker();
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
	if (!pSpellInspector)
		pSpellInspector = new GrimGui::SpellsInspector();

	if (pSpellPicker == nullptr)
		pSpellPicker = new SpellPicker();

	// check settings file, if logged in use character specific INI else default
	UpdateSettingFile();

	//load settings
	LoadSettings();
	SaveSettings();

	if (!s_WinVis.showTitleBars)
		s_WindowFlags = ImGuiWindowFlags_NoTitleBar;

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
	RemoveCommand("/grimgui");
	SaveSettings();
}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2GrimGUI");
	AddCommand("/grimgui", GrimCommandHandler, false, true, true);
	PrintGrimHelp();
	pSpellInspector = new GrimGui::SpellsInspector();
	pSpellPicker = new SpellPicker();

}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2GrimGUI");
	RemoveCommand("/grimui");
	pSpellPicker = nullptr;
	pSpellInspector = nullptr;
}

#pragma endregion
