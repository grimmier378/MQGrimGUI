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

struct WinVisSettings 
{
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
} s_WinVis;

struct NumericSettings 
{
	int combatFlashInterval = 100;
	int flashBuffInterval = 40;
	int playerBarHeight = 15;
	int targetBarHeight = 15;
	int aggroBarHeight = 10;
	int groupBarHeight = 15;
	int myAggroPct = 0;
	int secondAggroPct = 0;
	int buffIconSize = 24;
	int buffTimerThreshold = 0;
} s_NumSettings;

struct ThemeSettings
{
	std::string playerWinTheme = "Default";
	std::string petWinTheme = "Default";
	std::string groupWinTheme = "Default";
	std::string spellsWinTheme = "Default";
	std::string buffsWinTheme = "Default";
	std::string songWinTheme = "Default";
} s_WinTheme;

struct ColorSettings
{
	mq::MQColor minColorHP = mq::MQColor(223, 87, 255, 255);
	mq::MQColor maxColorHP = mq::MQColor(216, 39, 39, 255);
	mq::MQColor minColorMP = mq::MQColor(66, 29, 131, 255);
	mq::MQColor maxColorMP = mq::MQColor(20, 119, 216, 255);
	mq::MQColor minColorEnd = mq::MQColor(255, 111, 5, 255);
	mq::MQColor maxColorEnd = mq::MQColor(178, 153, 26, 178);
} s_BarColors;

enum class GrimCommand
{
	Show,
	Player,
	Target,
	Pet,
	Group,
	Spells,
	Buffs,
	Songs,
	Config,
	Help
};

struct CommandInfo
{
	GrimCommand command;
	const char* commandText;
	const char* description;
};

const std::array<CommandInfo, 10> commandList = {
	{
		{GrimCommand::Show, "show", "Toggles Main Window"},
		{GrimCommand::Player, "player", "Toggles Player Window"},
		{GrimCommand::Target, "target", "Toggles Target Window"},
		{GrimCommand::Pet, "pet", "Toggles Pet Window"},
		{GrimCommand::Group, "group", "Toggles Group Window"},
		{GrimCommand::Spells, "spells", "Toggles Spells Window"},
		{GrimCommand::Buffs, "buffs", "Toggles Buffs Window"},
		{GrimCommand::Songs, "songs", "Toggles Songs Window"},
		{GrimCommand::Config, "config", "Opens Configuration Window"},
		{GrimCommand::Help, "help", "Displays this help message"}
	}
};

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

struct PetButtonData
{
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

static void DisplayPetButtons()
{
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

static void TogglePetButtonVisibilityMenu()
{
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

#pragma region Config Structs
struct WindowOption
{
	const char* label;
	bool* setting;
	const char* section;
	const char* key;
};

WindowOption options[] = {
	{"Player Win", &s_WinVis.showPlayerWindow, "PlayerTarg", "ShowPlayerWindow"},
	{"Target Win", &s_WinVis.showTargetWindow, "PlayerTarg", "SplitTarget"},
	{"Pet Win", &s_WinVis.showPetWindow, "Pet", "ShowPetWindow"},
	{"Buff Win", &s_WinVis.showBuffWindow, "Buffs", "ShowBuffWindow"},
	{"Song Win", &s_WinVis.showSongWindow, "Songs", "ShowSongWindow"},
	{"Group Win", &s_WinVis.showGroupWindow, "Group", "ShowGroupWindow"}
};

struct SliderOption
{
	const char* label;
	int* value;
	int min;
	int max;
	const char* helpText;
};

SliderOption sliderOptions[] = {
	{"Flash Speed", &s_NumSettings.combatFlashInterval, 0, 500, "Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
	{"Buff Flash Speed", &s_NumSettings.flashBuffInterval, 0, 500, "Buff Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
	{"Buff Icon Size", &s_NumSettings.buffIconSize, 10, 40, "Buff Icon Size"},
	{"Buff Timer Threshold", &s_NumSettings.buffTimerThreshold, 0, 3600, "Buff Show Timer Threshold in Seconds (0 = Always Show)"},
	{"Player Bar Height", &s_NumSettings.playerBarHeight, 10, 40, "Player Bar Height"},
	{"Target Bar Height", &s_NumSettings.targetBarHeight, 10, 40, "Target Bar Height"},
	{"Aggro Bar Height", &s_NumSettings.aggroBarHeight, 10, 40, "Aggro Bar Height"},
	{"Group Bar Height", &s_NumSettings.groupBarHeight, 10, 40, "Group Bar Height"}
};

struct ThemeOption
{
	const char* label;
	std::string* theme;
};

ThemeOption themeOptions[] = {
	{"PlayerWin", &s_WinTheme.playerWinTheme},
	{"PetWin", &s_WinTheme.petWinTheme},
	{"GroupWin", &s_WinTheme.groupWinTheme},
	{"SpellsWin", &s_WinTheme.spellsWinTheme},
	{"BuffsWin", &s_WinTheme.buffsWinTheme},
	{"SongWin", &s_WinTheme.songWinTheme}
};
#pragma endregion

#pragma region Settings Functions

static void LoadSettings()
{
	// Load settings from the INI file
	//window settings
	s_WinVis.showMainWindow = GetPrivateProfileBool("Settings", "ShowMainGui", true, &s_SettingsFile[0]);
	s_WinVis.showTitleBars = GetPrivateProfileBool("Settings", "ShowTitleBars", true, &s_SettingsFile[0]);
	s_WinVis.showTargetWindow = GetPrivateProfileBool("PlayerTarg", "SplitTarget", false, &s_SettingsFile[0]);
	s_WinVis.showPlayerWindow = GetPrivateProfileBool("PlayerTarg", "ShowPlayerWindow", false, &s_SettingsFile[0]);
	s_WinVis.showPetWindow = GetPrivateProfileBool("Pet", "ShowPetWindow", false, &s_SettingsFile[0]);
	s_WinVis.showGroupWindow = GetPrivateProfileBool("Group", "ShowGroupWindow", false, &s_SettingsFile[0]);
	s_WinVis.showSpellsWindow = GetPrivateProfileBool("Spells", "ShowSpellsWindow", false, &s_SettingsFile[0]);
	s_WinVis.showBuffWindow = GetPrivateProfileBool("Buffs", "ShowBuffWindow", false, &s_SettingsFile[0]);
	s_WinVis.showSongWindow = GetPrivateProfileBool("Songs", "ShowSongWindow", false, &s_SettingsFile[0]);

	s_NumSettings.combatFlashInterval = GetPrivateProfileInt("PlayerTarg", "CombatFlashInterval", 250, &s_SettingsFile[0]);
	s_NumSettings.playerBarHeight = GetPrivateProfileInt("PlayerTarg", "PlayerBarHeight", 15, &s_SettingsFile[0]);
	s_NumSettings.targetBarHeight = GetPrivateProfileInt("PlayerTarg", "TargetBarHeight", 15, &s_SettingsFile[0]);
	s_NumSettings.aggroBarHeight = GetPrivateProfileInt("PlayerTarg", "AggroBarHeight", 10, &s_SettingsFile[0]);
	s_NumSettings.flashBuffInterval = GetPrivateProfileInt("Settings", "FlashBuffInterval", 250, &s_SettingsFile[0]);
	s_NumSettings.buffIconSize = GetPrivateProfileInt("Settings", "BuffIconSize", 24, &s_SettingsFile[0]);
	s_NumSettings.buffTimerThreshold = GetPrivateProfileInt("Buffs", "BuffTimerThreshold", 30, &s_SettingsFile[0]);
	s_NumSettings.groupBarHeight = GetPrivateProfileInt("Group", "GroupBarHeight", 15, &s_SettingsFile[0]);

	//Color Settings
	s_BarColors.minColorHP = GetPrivateProfileColor("Colors", "MinColorHP", s_BarColors.minColorHP, &s_SettingsFile[0]);
	s_BarColors.maxColorHP = GetPrivateProfileColor("Colors", "MaxColorHP", s_BarColors.maxColorHP, &s_SettingsFile[0]);
	s_BarColors.minColorMP = GetPrivateProfileColor("Colors", "MinColorMP", s_BarColors.minColorMP, &s_SettingsFile[0]);
	s_BarColors.maxColorMP = GetPrivateProfileColor("Colors", "MaxColorMP", s_BarColors.maxColorMP, &s_SettingsFile[0]);
	s_BarColors.minColorEnd = GetPrivateProfileColor("Colors", "MinColorEnd", s_BarColors.minColorEnd, &s_SettingsFile[0]);
	s_BarColors.maxColorEnd = GetPrivateProfileColor("Colors", "MaxColorEnd", s_BarColors.maxColorEnd, &s_SettingsFile[0]);

	//Theme Settings
	s_WinTheme.playerWinTheme = GetPrivateProfileString("PlayerTarg", "Theme", s_WinTheme.playerWinTheme, &s_SettingsFile[0]);
	s_WinTheme.petWinTheme = GetPrivateProfileString("Pet", "Theme", s_WinTheme.petWinTheme, &s_SettingsFile[0]);
	s_WinTheme.groupWinTheme = GetPrivateProfileString("Group", "Theme", s_WinTheme.groupWinTheme, &s_SettingsFile[0]);
	s_WinTheme.spellsWinTheme = GetPrivateProfileString("Spells", "Theme", s_WinTheme.spellsWinTheme, &s_SettingsFile[0]);
	s_WinTheme.buffsWinTheme = GetPrivateProfileString("Buffs", "Theme", s_WinTheme.buffsWinTheme, &s_SettingsFile[0]);
	s_WinTheme.songWinTheme = GetPrivateProfileString("Songs", "Theme", s_WinTheme.songWinTheme, &s_SettingsFile[0]);

	// Load Pet button visibility settings
	for (auto& button : petButtons) {
		button.visible = GetPrivateProfileBool("Pet", button.name.c_str(), true, &s_SettingsFile[0]);
	}
}

static void SaveSettings()
{
	//Window Settings
	WritePrivateProfileBool("Settings", "ShowMainGui", s_WinVis.showMainWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Settings", "ShowTitleBars", s_WinVis.showTitleBars, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_WinVis.showTargetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_WinVis.showPlayerWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Pet", "ShowPetWindow", s_WinVis.showPetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Group", "ShowGroupWindow", s_WinVis.showGroupWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Spells", "ShowSpellsWindow", s_WinVis.showSpellsWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Buffs", "ShowBuffWindow", s_WinVis.showBuffWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Songs", "ShowSongWindow", s_WinVis.showSongWindow, &s_SettingsFile[0]);

	WritePrivateProfileInt("PlayerTarg", "CombatFlashInterval", s_NumSettings.combatFlashInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", s_NumSettings.playerBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", s_NumSettings.targetBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", s_NumSettings.aggroBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("Settings", "BuffIconSize", s_NumSettings.buffIconSize, &s_SettingsFile[0]);
	WritePrivateProfileInt("Settings", "FlashBuffInterval", s_NumSettings.flashBuffInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("Buffs", "BuffTimerThreshold", s_NumSettings.buffTimerThreshold, &s_SettingsFile[0]);
	WritePrivateProfileInt("Group", "GroupBarHeight", s_NumSettings.groupBarHeight, &s_SettingsFile[0]);

	//Color Settings
	WritePrivateProfileColor("Colors", "MinColorHP", s_BarColors.minColorHP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorHP", s_BarColors.maxColorHP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MinColorMP", s_BarColors.minColorMP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorMP", s_BarColors.maxColorMP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MinColorEnd", s_BarColors.minColorEnd, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorEnd", s_BarColors.maxColorEnd, &s_SettingsFile[0]);

	//Theme Settings
	WritePrivateProfileString("Spells", "Theme", s_WinTheme.spellsWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Group", "Theme", s_WinTheme.groupWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("PlayerTarg", "Theme", s_WinTheme.playerWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Pet", "Theme", s_WinTheme.petWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Buffs", "Theme", s_WinTheme.buffsWinTheme, &s_SettingsFile[0]);
	WritePrivateProfileString("Songs", "Theme", s_WinTheme.songWinTheme, &s_SettingsFile[0]);

	// Save Pet button visibility settings
	for (const auto& button : petButtons) {
		WritePrivateProfileBool("Pet", button.name.c_str(), button.visible, &s_SettingsFile[0]);
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
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Orange).ToImColor()));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Purple).ToImColor()));
			}
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
					{
						s_WinVis.showConfigWindow = !s_WinVis.showConfigWindow;
					}

					if (ImGui::MenuItem("Show Main", NULL, s_WinVis.showMainWindow))
					{
						s_WinVis.showMainWindow = !s_WinVis.showMainWindow;
						WritePrivateProfileBool("Settings", "ShowMainGui", s_WinVis.showMainWindow, &s_SettingsFile[0]);
					}

					ImGui::EndMenu();
				}
				
				ImGui::EndMenuBar();
			}

			if (pEverQuestInfo->bAutoAttack)
			{
				if (s_WinVis.flashCombatFlag)
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

			ImVec4 colorHP = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, healthPctInt);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorHP);
			ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
			float yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(healthPctFloat, ImVec2(0.0f, static_cast<float>(s_NumSettings.playerBarHeight)), "##hp");
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
				ImVec4 colorMP = CalculateProgressiveColor(s_BarColors.minColorMP, s_BarColors.maxColorMP, manaPctInt);

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorMP);
				ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
				yPos = ImGui::GetCursorPosY();
				ImGui::ProgressBar(manaPctFloat, ImVec2(0.0f, static_cast<float>(s_NumSettings.playerBarHeight)), "##Mana");
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
			ImVec4 colorEP = CalculateProgressiveColor(s_BarColors.minColorEnd, s_BarColors.maxColorEnd, endurPctInt);

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorEP);
			ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(endurPctFloat, ImVec2(0.0f, static_cast<float>(s_NumSettings.playerBarHeight)), "##Endur");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Endur: %d / %d", GetCurEndurance(), GetMaxEndurance());
				ImGui::EndTooltip();
			}
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
			ImGui::Text("%d %%", endurPctInt);

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
		PCHARINFO pChar = GetCharInfo();
		if (pChar && pChar->pGroupInfo)
		{
			int groupSize = pChar->pGroupInfo->GetNumberOfMembersExcludingSelf() + 1; // including self

			for (int i = 1; i < MAX_GROUP_SIZE; i++)
			{
				CGroupMember* pMember = pChar->pGroupInfo->GetGroupMember(i);
				if (pMember && pMember->GetPlayer())
				{	
					SPAWNINFO* pSpawn = pMember->GetPlayer();
					if (ImGui::BeginTable("Group", 2))
					{
						ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, -1);
						ImGui::TableSetupColumn("Lvl", ImGuiTableColumnFlags_WidthFixed, 80);
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("%s", pMember->GetName());

						ImGui::TableNextColumn();
						ImGui::Text("%d", pSpawn->Level);
						ImGui::EndTable();
					}

					if (pSpawn->HPCurrent && pSpawn->HPMax)
					{
						float healthPercentage = static_cast<float>(pSpawn->HPCurrent) / pSpawn->HPMax;
						ImVec4 healthColor = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, healthPercentage * 100);
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, healthColor);
						ImGui::ProgressBar(healthPercentage, ImVec2(ImGui::GetColumnWidth() - 5, static_cast<float>(s_NumSettings.groupBarHeight)), "##");
						ImGui::PopStyleColor();
					}

					if (pSpawn->ManaCurrent && pSpawn->ManaMax)
					{
						float manaPercentage = static_cast<float>(pSpawn->ManaCurrent) / pSpawn->ManaMax;
						ImVec4 manaColor = CalculateProgressiveColor(s_BarColors.minColorMP, s_BarColors.maxColorMP, manaPercentage * 100);
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, manaColor);
						ImGui::ProgressBar(manaPercentage, ImVec2(ImGui::GetColumnWidth() - 5, static_cast<float>(s_NumSettings.groupBarHeight)), "##");
						ImGui::PopStyleColor();
					}
					if (pSpawn->EnduranceCurrent && pSpawn->EnduranceMax)
					{
						float endurPercentage = static_cast<float>(pSpawn->EnduranceCurrent) / pSpawn->EnduranceMax;
						ImVec4 endurColor = CalculateProgressiveColor(s_BarColors.minColorEnd, s_BarColors.maxColorEnd, endurPercentage * 100);
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, endurColor);
						ImGui::ProgressBar(endurPercentage, ImVec2(ImGui::GetColumnWidth() - 5, static_cast<float>(s_NumSettings.groupBarHeight)), "##");
						ImGui::PopStyleColor();
					}
				}

			}
		}

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
			float petPercentage = static_cast<float>(MyPet->HPCurrent) / 100;
			int petLabel = MyPet->HPCurrent;
			
			ImVec4 colorTarHP = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, MyPet->HPCurrent);
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
					ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth() - 5, static_cast<float>(s_NumSettings.playerBarHeight)), "##");
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
						ImVec4 colorTarHPTarget = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, pPetTarget->HPCurrent);
						ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHPTarget);
						ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
						yPos = ImGui::GetCursorPosY();
						ImGui::ProgressBar(petTargetPercentage, ImVec2(ImGui::GetColumnWidth() - 5, static_cast<float>(s_NumSettings.playerBarHeight)), "##");
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
	if (!s_WinVis.showBuffWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.buffsWinTheme);
	if (ImGui::Begin("Buffs##MQ2GrimGUI", &s_WinVis.showBuffWindow, s_WindowFlags | ImGuiWindowFlags_NoScrollbar))
	{

		s_spellsInspector->DrawBuffsList("BuffTable", pBuffWnd->GetBuffRange(), false, true);

	}
	PopTheme(popCounts);
	ImGui::End();

}

static void DrawSongWindow()
{
	if (!s_WinVis.showSongWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.songWinTheme);
	if (ImGui::Begin("Songs##MQ2GrimGUI", &s_WinVis.showSongWindow, s_WindowFlags | ImGuiWindowFlags_NoScrollbar))
	{
		s_spellsInspector->DrawBuffsList("SongTable", pSongWnd->GetBuffRange(), false, true);
	}
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
					{
						s_BarColors.minColorHP = MQColor(minHPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum HP Color");

					ImGui::TableNextColumn();

					ImVec4 maxHPColor = s_BarColors.maxColorHP.ToImColor();
					if (ImGui::ColorEdit4("Max HP Color", (float*)&maxHPColor, ImGuiColorEditFlags_NoInputs))
					{
						s_BarColors.maxColorHP = MQColor(maxHPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum HP Color");

					ImGui::TableNextColumn();

					ImVec4 minMPColor = s_BarColors.minColorMP.ToImColor();
					if (ImGui::ColorEdit4("Min MP Color", (float*)&minMPColor, ImGuiColorEditFlags_NoInputs))
					{
						s_BarColors.minColorMP = MQColor(minMPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum MP Color");

					ImGui::TableNextColumn();

					ImVec4 maxMPColor = s_BarColors.maxColorMP.ToImColor();
					if (ImGui::ColorEdit4("Max MP Color", (float*)&maxMPColor, ImGuiColorEditFlags_NoInputs))
					{
						s_BarColors.maxColorMP = MQColor(maxMPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum MP Color");

					ImGui::TableNextColumn();

					ImVec4 minEndColor = s_BarColors.minColorEnd.ToImColor();
					if (ImGui::ColorEdit4("Min End Color", (float*)&minEndColor, ImGuiColorEditFlags_NoInputs))
					{
						s_BarColors.minColorEnd = MQColor(minEndColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum Endurance Color");

					ImGui::TableNextColumn();

					ImVec4 maxEndColor = s_BarColors.maxColorEnd.ToImColor();
					if (ImGui::ColorEdit4("Max End Color", (float*)&maxEndColor, ImGuiColorEditFlags_NoInputs))
					{
						s_BarColors.maxColorEnd = MQColor(maxEndColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum Endurance Color");

					ImGui::EndTable();
				}

				ImGui::SeparatorText("Test Color");

				ImGui::SliderInt("Test Value", &s_TestInt, 0, 100);
				float testVal = static_cast<float>(s_TestInt) / 100;
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "HP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_BarColors.minColorMP, s_BarColors.maxColorMP, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "MP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_BarColors.minColorEnd, s_BarColors.maxColorEnd, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "End##Test");
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
				{
					s_WindowFlags = s_WinVis.showTitleBars ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoTitleBar;
				}
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
			{
				TogglePetButtonVisibilityMenu();
			}
			ImGui::Spacing();

			if (ImGui::Button("Save & Close"))
			{
				// only Save when the user clicks the button. 
				// If they close the window and don't click the button the settings will not be saved and only be temporary.
				WritePrivateProfileInt("PlayerTarg", "CombatFlashInterval", s_NumSettings.combatFlashInterval, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", s_NumSettings.playerBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", s_NumSettings.targetBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", s_NumSettings.aggroBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("Group", "GroupBarHeight", s_NumSettings.groupBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("Settings", "BuffIconSize", s_NumSettings.buffIconSize, &s_SettingsFile[0]);
				WritePrivateProfileInt("Settings", "FlashBuffInterval", s_NumSettings.flashBuffInterval, &s_SettingsFile[0]);
				WritePrivateProfileInt("Buffs", "BuffTimerThreshold", s_NumSettings.buffTimerThreshold, &s_SettingsFile[0]);

				WritePrivateProfileBool("Settings", "ShowTitleBars", s_WinVis.showTitleBars, &s_SettingsFile[0]);

				WritePrivateProfileColor("Colors", "MinColorHP", s_BarColors.minColorHP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MaxColorHP", s_BarColors.maxColorHP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MinColorMP", s_BarColors.minColorMP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MaxColorMP", s_BarColors.maxColorMP, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MinColorEnd", s_BarColors.minColorEnd, &s_SettingsFile[0]);
				WritePrivateProfileColor("Colors", "MaxColorEnd", s_BarColors.maxColorEnd, &s_SettingsFile[0]);

				WritePrivateProfileString("Spells", "Theme", s_WinTheme.spellsWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("Group", "Theme", s_WinTheme.groupWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("PlayerTarg", "Theme", s_WinTheme.playerWinTheme,  &s_SettingsFile[0]);
				WritePrivateProfileString("Pet", "Theme", s_WinTheme.petWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("Buffs", "Theme", s_WinTheme.buffsWinTheme, &s_SettingsFile[0]);
				WritePrivateProfileString("Songs", "Theme", s_WinTheme.songWinTheme, &s_SettingsFile[0]);

				s_WinVis.showConfigWindow = false;
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

				//if (ImGui::Checkbox("Spells Win", &s_WinVis.showSpellsWindow))
				//{
				//	WritePrivateProfileBool("Spells", "ShowSpellsWindow", s_WinVis.showSpellsWindow, &s_SettingsFile[0]);
				//}


				ImGui::Separator();

				if (ImGui::Button("Config"))
				{
					s_WinVis.showConfigWindow = true;
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
				s_NumSettings.myAggroPct = pAggroInfo->aggroData[AD_Player].AggroPct;
				s_NumSettings.secondAggroPct = pAggroInfo->aggroData[AD_Secondary].AggroPct;
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
			{
				WritePrivateProfileBool("Settings", "ShowMainGui", s_WinVis.showMainWindow, &s_SettingsFile[0]);
			}
		}

		if (s_WinVis.showConfigWindow)
			DrawConfigWindow();

		// Player Window (also target if not split)
		if (s_WinVis.showPlayerWindow)
		{
			DrawPlayerWindow();

			if (!s_WinVis.showPlayerWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_WinVis.showPlayerWindow, &s_SettingsFile[0]);
			}
		}

		// Pet Window
		if (s_WinVis.showPetWindow)
		{

			DrawPetWindow();

			if (!s_WinVis.showPetWindow)
			{
				WritePrivateProfileBool("Pet", "ShowPetWindow", s_WinVis.showPetWindow, &s_SettingsFile[0]);
			}
		}

		//Buff Window
		if (s_WinVis.showBuffWindow)
		{
			DrawBuffWindow();

			if (!s_WinVis.showBuffWindow)
			{
				WritePrivateProfileBool("Buffs", "ShowBuffWindow", s_WinVis.showBuffWindow, &s_SettingsFile[0]);
			}
		}

		// Song Window
		if (s_WinVis.showSongWindow)
		{
			DrawSongWindow();

			if (!s_WinVis.showSongWindow)
			{
				WritePrivateProfileBool("Songs", "ShowSongWindow", s_WinVis.showSongWindow, &s_SettingsFile[0]);
			}
		}
		
		// Split Target Window
		if (s_WinVis.showTargetWindow)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
			int popCountsPlay = PushTheme(s_WinTheme.playerWinTheme);
			if (ImGui::Begin("Tar##MQ2GrimGUI", &s_WinVis.showTargetWindow, s_WindowFlags))
			{
				DrawTargetWindow();
			}
			PopTheme(popCountsPlay);
			ImGui::End();

			if (!s_WinVis.showTargetWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_WinVis.showTargetWindow, &s_SettingsFile[0]);
			}
		}

		// Group Window
		if (s_WinVis.showGroupWindow)
		{
			DrawGroupWindow();

			if (!s_WinVis.showGroupWindow)
			{
				WritePrivateProfileBool("Group", "ShowGroupWindow", s_WinVis.showGroupWindow, &s_SettingsFile[0]);
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

	if (!s_WinVis.showTitleBars)
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
	AddCommand("/grimgui", GrimCommandHandler, false, true, true);
	PrintGrimHelp();
	s_spellsInspector = new SpellsInspector();
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2GrimGUI");
	RemoveCommand("/grimui");
}

#pragma endregion
