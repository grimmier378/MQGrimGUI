#include <mq/Plugin.h>
#include <imgui.h>
#include "main/MQ2Main.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include <mq/imgui/Widgets.h>
#include <chrono>
#include <string>
#include <sstream>
#include <filesystem>
#include "MQ2GrimGUI.h"

PreSetup("MQ2GrimGUI");
PLUGIN_VERSION(0.1);

// Declare global plugin state variables
static bool s_ShowMainWindow			= false;
static bool s_ShowConfigWindow			= false;
static bool s_SplitTargetWindow			= false;
static bool s_ShowPetWindow = false;
static bool s_ShowPlayerWindow			= false;
static bool s_FlashCombatFlag			= false;
static bool s_FlashTintFlag				= false;
static bool s_ShowGroupWindow			= false;
static bool s_ShowSpellsWindow			= false;
static bool s_charIniLoaded				= false;
static bool s_DefaultLoaded				= false;

static char s_SettingsFile[MAX_PATH]	= { 0 };

// Colors for Progress Bar Transitions
static mq::MQColor s_MinColorHP(223, 87, 255, 255);
static mq::MQColor s_MaxColorHP(216, 39, 39, 255);
static mq::MQColor s_MinColorMP(66, 29, 131, 255);
static mq::MQColor s_MaxColorMP(20, 119, 216, 255);
static mq::MQColor s_MinColorEnd(255, 111, 5, 255);
static mq::MQColor s_MaxColorEnd(178, 153, 26, 178);


static int s_FlashInterval				= 250;
static int s_FlashBuffInterval			= 350;
static int s_PlayerBarHeight			= 15;
static int s_TargetBarHeight			= 15;
static int s_AggroBarHeight				= 10;
static int s_myAgroPct					= 0;
static int s_secondAgroPct				= 0;
static int s_BuffIconSize				= 24;
static int s_TestInt					= 100; // Color Test Value for Config Window

static std::string s_secondAgroName		= "Unknown";
static std::string s_heading			= "N";
static int s_TarBuffLineSize = 0;

// Timers
std::chrono::steady_clock::time_point g_LastUpdateTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastBuffFlashTime = std::chrono::steady_clock::now();

const auto g_UpdateInterval		= std::chrono::milliseconds(250);

static void LoadSettings()
{
	// Load settings from the INI file
	//window settings
	s_ShowMainWindow = GetPrivateProfileBool("Settings", "ShowMainGui", true, &s_SettingsFile[0]);
	s_SplitTargetWindow = GetPrivateProfileBool("PlayerTarg", "SplitTarget", false, &s_SettingsFile[0]);
	s_ShowPetWindow = GetPrivateProfileBool("Pet", "ShowPetWindow", false, &s_SettingsFile[0]);
	s_ShowPlayerWindow = GetPrivateProfileBool("PlayerTarg", "ShowPlayerWindow", false, &s_SettingsFile[0]);
	s_ShowGroupWindow = GetPrivateProfileBool("Group", "ShowGroupWindow", false, &s_SettingsFile[0]);
	s_ShowSpellsWindow = GetPrivateProfileBool("Spells", "ShowSpellsWindow", false, &s_SettingsFile[0]);

	s_FlashInterval = GetPrivateProfileInt("PlayerTarg", "FlashInterval", 250, &s_SettingsFile[0]);
	s_FlashBuffInterval = GetPrivateProfileInt("PlayerTarg", "FlashBuffInterval", 250, &s_SettingsFile[0]);
	s_PlayerBarHeight = GetPrivateProfileInt("PlayerTarg", "PlayerBarHeight", 15, &s_SettingsFile[0]);
	s_TargetBarHeight = GetPrivateProfileInt("PlayerTarg", "TargetBarHeight", 15, &s_SettingsFile[0]);
	s_AggroBarHeight = GetPrivateProfileInt("PlayerTarg", "AggroBarHeight", 10, &s_SettingsFile[0]);
	s_BuffIconSize = GetPrivateProfileInt("PlayerTarg", "BuffIconSize", 24, &s_SettingsFile[0]);

	//Color Settings
	s_MinColorHP = GetPrivateProfileColor("Colors", "MinColorHP", s_MinColorHP, &s_SettingsFile[0]);
	s_MaxColorHP = GetPrivateProfileColor("Colors", "MaxColorHP", s_MaxColorHP, &s_SettingsFile[0]);
	s_MinColorMP = GetPrivateProfileColor("Colors", "MinColorMP", s_MinColorMP, &s_SettingsFile[0]);
	s_MaxColorMP = GetPrivateProfileColor("Colors", "MaxColorMP", s_MaxColorMP, &s_SettingsFile[0]);
	s_MinColorEnd = GetPrivateProfileColor("Colors", "MinColorEnd", s_MinColorEnd, &s_SettingsFile[0]);
	s_MaxColorEnd = GetPrivateProfileColor("Colors", "MaxColorEnd", s_MaxColorEnd, &s_SettingsFile[0]);

}


static void SaveSettings()
{
	//Window Settings
	WritePrivateProfileBool("Settings", "ShowMainGui", s_ShowMainWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_SplitTargetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Pet", "ShowPetWindow", s_ShowPetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_ShowPlayerWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Group", "ShowGroupWindow", s_ShowGroupWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Spells", "ShowSpellsWindow", s_ShowSpellsWindow, &s_SettingsFile[0]);

	WritePrivateProfileInt("PlayerTarg", "FlashInterval", s_FlashInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "FlashBuffInterval", s_FlashBuffInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", s_PlayerBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", s_TargetBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", s_AggroBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "BuffIconSize", s_BuffIconSize, &s_SettingsFile[0]);

	//Color Settings
	WritePrivateProfileColor("Colors", "MinColorHP", s_MinColorHP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorHP", s_MaxColorHP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MinColorMP", s_MinColorMP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorMP", s_MaxColorMP, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MinColorEnd", s_MinColorEnd, &s_SettingsFile[0]);
	WritePrivateProfileColor("Colors", "MaxColorEnd", s_MaxColorEnd, &s_SettingsFile[0]);
}


// Update the settings file to use the character-specific INI file if the player is in-game
static void UpdateSettingFile()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (!s_charIniLoaded)
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
				s_charIniLoaded = true;
			}
		}
	}
	else
	{
		if (s_charIniLoaded || !s_DefaultLoaded)
		{
			memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
			fmt::format_to(s_SettingsFile, "{}/MQ2GrimGUI.ini", gPathConfig);
			s_charIniLoaded = false;
			LoadSettings();
			s_DefaultLoaded = true;
		}
	}
}

static void GetHeading()
{
	static PSPAWNINFO pSelfInfo = pLocalPlayer;
	s_heading = szHeadingShort[static_cast<int>((pSelfInfo->Heading / 32.0f) + 8.5f) % 16];
}

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

// GUI Windows
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

static void DrawTargetWindow()
	{
		if (PSPAWNINFO CurTarget = pTarget)
		{
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2);
			float tarPercentage = static_cast<float>(CurTarget->HPCurrent) / 100;
			int tar_label = CurTarget->HPCurrent;
			ImVec4 colorTarHP = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, CurTarget->HPCurrent);

			if (strncmp(pTarget->DisplayedName, pLocalPC->Name, 64) == 0)
			{
				tarPercentage = static_cast<float>(CurTarget->HPCurrent) / CurTarget->HPMax;
				int healthIntPct = static_cast<int>(tarPercentage * 100);
				tar_label = healthIntPct;
				colorTarHP = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, healthIntPct);
			}
			DrawLineOfSight(pLocalPlayer, pTarget);
			ImGui::SameLine();
			ImGui::Text(CurTarget->DisplayedName);

			ImGui::SameLine(sizeX * .75);
			ImGui::TextColored(GetMQColor(ColorName::Tangerine).ToImColor(), "%0.1f m", GetDistance(pLocalPlayer, pTarget));

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(tarPercentage, ImVec2(0.0f, s_TargetBarHeight), "##");
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

			ImGui::SameLine(sizeX * .5);
			ImGui::TextColored(ConColorToVec(ConColor(pTarget)),ICON_MD_LENS);


			if (s_myAgroPct < 100)
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Orange).ToImColor()));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Purple).ToImColor()));
			}
			ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(static_cast<float>(s_myAgroPct) / 100, ImVec2(0.0f, s_AggroBarHeight), "##Aggro");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(10, yPos));
			ImGui::Text("%s", &s_secondAgroName);
			ImGui::SetCursorPos(ImVec2((sizeX/2)-8, yPos));
			ImGui::Text("%d %%", s_myAgroPct);
			ImGui::SetCursorPos(ImVec2(sizeX - 40, yPos));
			ImGui::Text("%d %%", s_secondAgroPct);	

			if (gTargetbuffs)
			{
				//GetCachedBuffAtSlot(pTarget, 0);
				//ImGui::Text("%s", buff);
				s_spellsInspector->DoBuffs("TargetBuffsTable", pTargetWnd->GetBuffRange(), false);
			}
		}
	}

static void DrawPlayerWindow()
	{
		if (!s_ShowPlayerWindow)
			return;

		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Player##MQ2GrimGUI", &s_ShowPlayerWindow, ImGuiWindowFlags_MenuBar))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int midX = (sizeX / 2) - 8;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Main"))
				{
					if (ImGui::MenuItem("Split Target", NULL, s_SplitTargetWindow))
					{
						s_SplitTargetWindow = !s_SplitTargetWindow;
						WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_SplitTargetWindow, &s_SettingsFile[0]);
					}

					if (ImGui::MenuItem("Show Config", NULL, s_ShowConfigWindow))
					{
						s_ShowConfigWindow = !s_ShowConfigWindow;
					}

					if (ImGui::MenuItem("Show Main", NULL, s_ShowMainWindow))
					{
						s_ShowMainWindow = !s_ShowMainWindow;
						WritePrivateProfileBool("Settings", "ShowMainGui", s_ShowMainWindow, &s_SettingsFile[0]);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (pEverQuestInfo->bAutoAttack)
			{
				if (s_FlashCombatFlag)
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
					ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch, ImGui::GetContentRegionAvail().x * .5);
					ImGui::TableSetupColumn("##Heading", ImGuiTableColumnFlags_WidthFixed, 30);
					ImGui::TableSetupColumn("##Lvl", ImGuiTableColumnFlags_WidthStretch, 60);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::SameLine();
					ImGui::Text(pLocalPC->Name);
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(GetMQColor(ColorName::Yellow).ToImColor()), s_heading.c_str());
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

			ImVec4 colorHP = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, healthPctInt);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorHP);
			ImGui::SetNextItemWidth(sizeX - 15);
			float yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(healthPctFloat, ImVec2(0.0f, s_PlayerBarHeight), "##hp");
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
				ImVec4 colorMP = CalculateProgressiveColor(s_MinColorMP, s_MaxColorMP, manaPctInt);

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorMP);
				ImGui::SetNextItemWidth(sizeX - 15);
				yPos = ImGui::GetCursorPosY();
				ImGui::ProgressBar(manaPctFloat, ImVec2(0.0f, s_PlayerBarHeight), "##Mana");
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
			ImVec4 colorEP = CalculateProgressiveColor(s_MinColorEnd, s_MaxColorEnd, endurPctInt);

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorEP);
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(endurPctFloat, ImVec2(0.0f, s_PlayerBarHeight), "##Endur");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Endur: %d / %d", GetCurEndurance(), GetMaxEndurance());
				ImGui::EndTooltip();
			}
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
			ImGui::Text("%d %%", endurPctInt);

			if (!s_SplitTargetWindow)
			{
				ImGui::Separator();
				DrawTargetWindow();
			}
		}
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
		if (ImGui::Begin("Pet##MQ2GrimGUI", &s_ShowPetWindow))
		{
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2);
			float petPercentage = static_cast<float>(MyPet->HPCurrent) / 100;
			int petLabel = MyPet->HPCurrent;
			
			ImVec4 colorTarHP = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, MyPet->HPCurrent);
			if (ImGui::BeginTable("Pet", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
			{
				ImGui::TableSetupColumn(petName, ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupColumn("Buffs", ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				DrawLineOfSight(pLocalPlayer, MyPet);
				ImGui::SameLine();
				ImGui::Text("Lvl");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(GetMQColor(ColorName::Teal).ToImColor()),"%d", MyPet->Level);
				ImGui::SameLine();
				ImGui::Text("Dist:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(GetMQColor(ColorName::Tangerine).ToImColor()), "%0.1f m", GetDistance(pLocalPlayer, MyPet));

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
				ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
				yPos = ImGui::GetCursorPosY();
				ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth() - 5, s_PlayerBarHeight), "##");
				ImGui::PopStyleColor();
				ImGui::SetCursorPos(ImVec2(ImGui::GetColumnWidth() / 2, yPos));
				ImGui::Text("%d %%", petLabel);

				if (PSPAWNINFO pPetTarget = MyPet->WhoFollowing)
				{
					ImGui::Text(pPetTarget->DisplayedName);
					float petTargetPercentage = static_cast<float>(pPetTarget->HPCurrent) / 100;
					int petTargetLabel = pPetTarget->HPCurrent;
					ImVec4 colorTarHPTarget = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, pPetTarget->HPCurrent);
					ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHPTarget);
					ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
					yPos = ImGui::GetCursorPosY();
					ImGui::ProgressBar(petTargetPercentage, ImVec2(ImGui::GetColumnWidth() - 5, s_PlayerBarHeight), "##");
					ImGui::PopStyleColor();
					ImGui::SetCursorPos(ImVec2(ImGui::GetColumnWidth() / 2, yPos));
					ImGui::Text("%d %%", petTargetLabel);
				}
				else
				{
					ImGui::Text("No Target");
					ImGui::NewLine();
				}
				// Pet Buffs Section (Column)
				ImGui::TableNextColumn();
				
				s_spellsInspector->DoBuffs("PetBuffsTable", pPetInfoWnd->GetBuffRange(), true);

				ImGui::EndTable();
			}


			// TODO: Pet Target Information

			// TODO: Pet Window Buttons

			//ImGui::Separator();


		}
		ImGui::End();
	}

}

static void DrawSpellWindow()
{
	//TODO: Spell Window
}

static void DrawBuffWindow()
{
	//TODO: Buff Window
}



static void DrawConfigWindow()
	{
		if (!s_ShowConfigWindow)
			return;

		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Config##ConfigWindow", &s_ShowConfigWindow))
		{
			if (ImGui::CollapsingHeader("Color Settings"))
			{
				if (ImGui::BeginTable("##Settings", 2))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImVec4 minHPColor = s_MinColorHP.ToImColor();
					if (ImGui::ColorEdit4("Min HP Color", (float*)&minHPColor, ImGuiColorEditFlags_NoInputs))
					{
						s_MinColorHP = MQColor(minHPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum HP Color");

					ImGui::TableNextColumn();

					ImVec4 maxHPColor = s_MaxColorHP.ToImColor();
					if (ImGui::ColorEdit4("Max HP Color", (float*)&maxHPColor, ImGuiColorEditFlags_NoInputs))
					{
						s_MaxColorHP = MQColor(maxHPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum HP Color");

					ImGui::TableNextColumn();

					ImVec4 minMPColor = s_MinColorMP.ToImColor();
					if (ImGui::ColorEdit4("Min MP Color", (float*)&minMPColor, ImGuiColorEditFlags_NoInputs))
					{
						s_MinColorMP = MQColor(minMPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum MP Color");

					ImGui::TableNextColumn();

					ImVec4 maxMPColor = s_MaxColorMP.ToImColor();
					if (ImGui::ColorEdit4("Max MP Color", (float*)&maxMPColor, ImGuiColorEditFlags_NoInputs))
					{
						s_MaxColorMP = MQColor(maxMPColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum MP Color");

					ImGui::TableNextColumn();

					ImVec4 minEndColor = s_MinColorEnd.ToImColor();
					if (ImGui::ColorEdit4("Min End Color", (float*)&minEndColor, ImGuiColorEditFlags_NoInputs))
					{
						s_MinColorEnd = MQColor(minEndColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Minimum Endurance Color");

					ImGui::TableNextColumn();

					ImVec4 maxEndColor = s_MaxColorEnd.ToImColor();
					if (ImGui::ColorEdit4("Max End Color", (float*)&maxEndColor, ImGuiColorEditFlags_NoInputs))
					{
						s_MaxColorEnd = MQColor(maxEndColor);
					}
					ImGui::SameLine();
					DrawHelpIcon("Maximum Endurance Color");

					ImGui::EndTable();
				}

				ImGui::SeparatorText("Test Color");

				ImGui::SliderInt("Test Value", &s_TestInt, 0, 100);
				float testVal = static_cast<float>(s_TestInt) / 100;
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "HP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_MinColorMP, s_MaxColorMP, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "MP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(s_MinColorEnd, s_MaxColorEnd, s_TestInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "End##Test");
				ImGui::PopStyleColor();
			}

			if (ImGui::CollapsingHeader("Window Settings"))
			{
				// Flash Interval Control
				ImGui::SliderInt("Flash Speed", &s_FlashInterval, 0, 500);
				ImGui::SameLine();
				if (s_FlashInterval == 0)
				{
					DrawHelpIcon("Flash Interval Disabled");
				}
				else
				{
					std::string label = "Flash Speed: " + std::to_string(s_FlashInterval) + " \nLower is slower, Higher is faster. 0 = Disabled";
					DrawHelpIcon(label.c_str());
				}

				ImGui::SliderInt("Buff Flash Speed", &s_FlashBuffInterval, 0, 500);
				ImGui::SameLine();
				if (s_FlashBuffInterval == 0)
				{
					DrawHelpIcon("Buff Flash Interval Disabled");
				}
				else
				{
					std::string label = "Buff Flash Speed: " + std::to_string(s_FlashBuffInterval) + " \nLower is slower, Higher is faster. 0 = Disabled";
					DrawHelpIcon(label.c_str());
				}

				// Buff Icon Size Control
				ImGui::SliderInt("Buff Icon Size", &s_BuffIconSize, 10, 40);
				ImGui::SameLine();
				DrawHelpIcon("Buff Icon Size");

				// Bar Height Controls

				ImGui::SliderInt("Player Bar Height", &s_PlayerBarHeight, 10, 40);
				ImGui::SameLine();
				DrawHelpIcon("Player Bar Height");

				ImGui::SliderInt("Target Bar Height", &s_TargetBarHeight, 10, 40);
				ImGui::SameLine();
				DrawHelpIcon("Target Bar Height");

				ImGui::SliderInt("Aggro Bar Height", &s_AggroBarHeight, 10, 40);
				ImGui::SameLine();
				DrawHelpIcon("Aggro Bar Height");

			}


			if (ImGui::Button("Save & Close"))
			{

				// only Save when the user clicks the button. 
// If they close the window and don't click the button the settings will not be saved and only be temporary.
				WritePrivateProfileInt("PlayerTarg", "FlashInterval", s_FlashInterval, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "FlashBuffInterval", s_FlashBuffInterval, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", s_PlayerBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", s_TargetBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", s_AggroBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileInt("PlayerTarg", "BuffIconSize", s_BuffIconSize, &s_SettingsFile[0]);

				WritePrivateProfileColor("Colors", "MinColorHP", s_MinColorHP, s_SettingsFile);
				WritePrivateProfileColor("Colors", "MaxColorHP", s_MaxColorHP, s_SettingsFile);
				WritePrivateProfileColor("Colors", "MinColorMP", s_MinColorMP, s_SettingsFile);
				WritePrivateProfileColor("Colors", "MaxColorMP", s_MaxColorMP, s_SettingsFile);
				WritePrivateProfileColor("Colors", "MinColorEnd", s_MinColorEnd, s_SettingsFile);
				WritePrivateProfileColor("Colors", "MaxColorEnd", s_MaxColorEnd, s_SettingsFile);

				s_ShowConfigWindow = false;
			}
		}
		ImGui::End();
	}


// Main Window, toggled with /Grimgui command, contains Toggles to show other windows
static void DrawMainWindow()
	{
		if (s_ShowMainWindow)
		{

			ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("GrimGUI##MainWindow", &s_ShowMainWindow))
			{
				if (ImGui::Checkbox("Player Win", &s_ShowPlayerWindow))
				{
					WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_ShowPlayerWindow, &s_SettingsFile[0]);
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Split Target", &s_SplitTargetWindow))
				{
					WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_SplitTargetWindow, &s_SettingsFile[0]);
				}

				if (ImGui::Checkbox("Pet Win", &s_ShowPetWindow))
				{
					WritePrivateProfileBool("Pet", "ShowPetWindow", s_ShowPetWindow, &s_SettingsFile[0]);
				}

				//TODO: More Windows
				//ImGui::Separator();

				//if (ImGui::Checkbox("Spells Win", &s_ShowSpellsWindow))
				//{
				//	WritePrivateProfileBool("Spells", "ShowSpellsWindow", s_ShowSpellsWindow, &s_SettingsFile[0]);
				//}

				//ImGui::SameLine();

				//if (ImGui::Checkbox("Group Win", &s_ShowGroupWindow))
				//{
				//	WritePrivateProfileBool("Group", "ShowGroupWindow", s_ShowGroupWindow, &s_SettingsFile[0]);
				//}

				ImGui::Separator();

				if (ImGui::Button("Config"))
				{
					s_ShowConfigWindow = true;
				}

			}
			ImGui::End();
		}
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
	s_ShowMainWindow = !s_ShowMainWindow;
	WritePrivateProfileBool("Settings", "ShowMainGui", s_ShowMainWindow, &s_SettingsFile[0]);
}

// Called periodically by MQ2
PLUGIN_API void OnPulse()
{
	auto now = std::chrono::steady_clock::now();
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (now - g_LastUpdateTime >= g_UpdateInterval)
		{
			if (pAggroInfo)
			{
				s_myAgroPct = pAggroInfo->aggroData[AD_Player].AggroPct;
				s_secondAgroPct = pAggroInfo->aggroData[AD_Secondary].AggroPct;
				if (pAggroInfo->AggroSecondaryID)
				{
					s_secondAgroName = GetSpawnByID(pAggroInfo->AggroSecondaryID)->DisplayedName;
				}
				else
				{
					s_secondAgroName = "Unknown";
				}
			}

			g_LastUpdateTime = now;
		}

		if (s_FlashBuffInterval > 0)
		{
			if (now - g_LastBuffFlashTime >= std::chrono::milliseconds(500 - s_FlashBuffInterval))
			{
				s_FlashTintFlag = !s_FlashTintFlag;
				g_LastBuffFlashTime = now;
			}
		}
		else
		{
			s_FlashTintFlag = false;
		}

		if (s_FlashInterval > 0)
		{
			if (now - g_LastFlashTime >= std::chrono::milliseconds(500 - s_FlashInterval))
			{
				s_FlashCombatFlag = !s_FlashCombatFlag;
				g_LastFlashTime = now;
			}
		}
		else
		{
			s_FlashCombatFlag = false;
		}

		GetHeading();

	}

	UpdateSettingFile();
}

PLUGIN_API void OnUpdateImGui()
{
	// Draw the GUI elements

	// Main Window no state check needed 
	if (s_ShowMainWindow)
	{
		DrawMainWindow();
		
		if (!s_ShowMainWindow)
		{
			WritePrivateProfileBool("Settings", "ShowMainGui", s_ShowMainWindow, &s_SettingsFile[0]);
		}

		if (s_ShowConfigWindow)
			DrawConfigWindow();
	}

	if (GetGameState() == GAMESTATE_INGAME)
	{
		// Player Window (also target if not split)
		if (s_ShowPlayerWindow)
		{
			DrawPlayerWindow();

			if (!s_ShowPlayerWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_ShowPlayerWindow, &s_SettingsFile[0]);
			}
		}

		// Pet Window
		if (s_ShowPetWindow)
		{

			DrawPetWindow();

			if (!s_ShowPetWindow)
			{
				WritePrivateProfileBool("Pet", "ShowPetWindow", s_ShowPetWindow, &s_SettingsFile[0]);
			}
		}
		
		// Split Target Window
		if (s_SplitTargetWindow)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Tar##MQ2GrimGUI", &s_SplitTargetWindow))
			{
				DrawTargetWindow();
			}
			ImGui::End();

			if (!s_SplitTargetWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_SplitTargetWindow, &s_SettingsFile[0]);
			}
		}
	}
}


/**
 * @fn OnMacroStart
 *
 * This is called each time a macro starts (ex: /mac somemacro.mac), prior to
 * launching the macro.
 *
 * @param Name const char* - The name of the macro that was launched
 */
PLUGIN_API void OnMacroStart(const char* Name)
{
	// DebugSpewAlways("MQ2GrimGUI::OnMacroStart(%s)", Name);
}

/**
 * @fn OnMacroStop
 *
 * This is called each time a macro stops (ex: /endmac), after the macro has ended.
 *
 * @param Name const char* - The name of the macro that was stopped.
 */
PLUGIN_API void OnMacroStop(const char* Name)
{
	// DebugSpewAlways("MQ2GrimGUI::OnMacroStop(%s)", Name);
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
	AddCommand("/Grimgui", GrimCommandHandler, false, false, false);
	// check settings file, if logged in use character specific INI else default
	UpdateSettingFile();
	//load settings
	LoadSettings();
	SaveSettings();
	s_spellsInspector = new SpellsInspector();
	WriteChatf("/grimgui to toggle main window");

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
