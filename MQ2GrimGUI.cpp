#include <mq/Plugin.h>
#include <imgui.h>
#include <imgui/imgui_internal.h>
#include "imgui/implot/implot.h"
#include "main/MQ2Main.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include "main/MQ2Internal.h"
#include "main/MQ2Globals.h"
#include "main/MQ2Inlines.h"
#include <sstream>
#include <filesystem>
#include "MQ2GrimGUI.h"

PreSetup("MQ2GrimGUI");
PLUGIN_VERSION(0.1);

// Declare global plugin state variables
bool b_ShowMainWindow = false;
bool b_ShowConfigWindow = false;
bool b_SplitTargetWindow = false;
bool b_ShowPlayerWindow = false;
bool b_flashCombat = false;
int s_FlashInterval = 250;
static bool b_ShowGroupWindow = false;
static bool b_ShowSpellsWindow = false;
static bool b_charIniLoaded = false;
static bool b_DefaultLoaded = false;
static char s_SettingsFile[MAX_PATH] = { 0 };

// Colors for Progress Bar Transitions
static ImVec4 minColorHP(0.876f, 0.341f, 1.000f, 1.000f);
static ImVec4 maxColorHP(0.845f, 0.151f, 0.151f, 1.000f);
static ImVec4 minColorMP(0.259f, 0.114f, 0.514f, 1.000f);
static ImVec4 maxColorMP(0.079f, 0.468f, 0.848f, 1.000f); 
static ImVec4 minColorEnd(1.000f, 0.437f, 0.019f, 1.000f);
static ImVec4 maxColorEnd(0.7f, 0.6f, 0.1f, 0.7f);

static int s_PlayerBarHeight = 15;
static int s_TargetBarHeight = 15;
static int s_AggroBarHeight = 10;

// Color Test Value for Config Window
static int testInt = 100;


// setup data classes and timers.
CharData g_CharData;
TargetData g_TargetData;
std::chrono::steady_clock::time_point g_LastUpdateTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime = std::chrono::steady_clock::now();
const auto g_UpdateInterval = std::chrono::milliseconds(250);


/**
* @fn WritePrivateProfileBool
* 
* Function to write Color ImVec4 as String to the INI file
* 
* @param section const char* Section name in the INI file
* @param key const char* Key name in the INI file
* @param color ImVec4 Color value to write to the INI file
* @param file const char* INI file to write the color to
 */
static void SaveColorToIni(const char* section, const char* key, const ImVec4& color, const char* file)
{
	std::ostringstream oss;
	oss << color.x << "," << color.y << "," << color.z << "," << color.w;
	WritePrivateProfileString(section, key, oss.str().c_str(), file);
}


/** 
* @fn LoadColorFromIni
* 
* Function to load an ImVec4 color from the INI file
* 
* @param section const char* Section name in the INI file
* @param key const char* Key name in the INI file
* @param defaultColor ImVec4 Default color value to return if the key does not exist
* @param file const char* INI file to load the color from
* @return ImVec4 color value
 */
static ImVec4 LoadColorFromIni(const char* section, const char* key, const ImVec4& defaultColor, const char* file)
{
	char buffer[64];
	GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), file);

	// If the key does not exist, return the default color
	if (strlen(buffer) == 0)
	{
		return defaultColor;
	}

	// Parse the color from the comma-separated string
	ImVec4 color = defaultColor;
	std::istringstream iss(buffer);
	std::string value;
	float values[4];
	int i = 0;

	while (std::getline(iss, value, ',') && i < 4)
	{
		values[i++] = std::stof(value);
	}

	if (i == 4)
	{
		color.x = values[0];
		color.y = values[1];
		color.z = values[2];
		color.w = values[3];
	}

	return color;
}


static void LoadSettings()
{
	// Load settings from the INI file
	//window settings
	b_ShowMainWindow = GetPrivateProfileBool("Settings", "ShowMainGui", true, &s_SettingsFile[0]);
	b_SplitTargetWindow = GetPrivateProfileBool("PlayerTarg", "SplitTarget", false, &s_SettingsFile[0]);
	b_ShowPlayerWindow = GetPrivateProfileBool("PlayerTarg", "ShowPlayerWindow", false, &s_SettingsFile[0]);
	b_ShowGroupWindow = GetPrivateProfileBool("Group", "ShowGroupWindow", false, &s_SettingsFile[0]);
	b_ShowSpellsWindow = GetPrivateProfileBool("Spells", "ShowSpellsWindow", false, &s_SettingsFile[0]);

	s_FlashInterval = GetPrivateProfileInt("PlayerTarg", "FlashInterval", 250, &s_SettingsFile[0]);
	s_PlayerBarHeight = GetPrivateProfileInt("PlayerTarg", "PlayerBarHeight", 15, &s_SettingsFile[0]);
	s_TargetBarHeight = GetPrivateProfileInt("PlayerTarg", "TargetBarHeight", 15, &s_SettingsFile[0]);
	s_AggroBarHeight = GetPrivateProfileInt("PlayerTarg", "AggroBarHeight", 10, &s_SettingsFile[0]);


	//Color Settings
	minColorHP = LoadColorFromIni("Colors", "MinColorHP", ImVec4(0.876f, 0.341f, 1.000f, 1.000f), &s_SettingsFile[0]);
	maxColorHP = LoadColorFromIni("Colors", "MaxColorHP", ImVec4(0.845f, 0.151f, 0.151f, 1.000f), &s_SettingsFile[0]);
	minColorEnd = LoadColorFromIni("Colors", "MinColorEnd", ImVec4(1.000f, 0.437f, 0.019f, 1.000f), &s_SettingsFile[0]);
	maxColorEnd = LoadColorFromIni("Colors", "MaxColorEnd", ImVec4(0.7f, 0.6f, 0.1f, 0.7f), &s_SettingsFile[0]);
	minColorMP = LoadColorFromIni("Colors", "MinColorMP", ImVec4(0.259f, 0.114f, 0.514f, 1.000f), &s_SettingsFile[0]);
	maxColorMP = LoadColorFromIni("Colors", "MaxColorMP", ImVec4(0.079f, 0.468f, 0.848f, 1.000f), &s_SettingsFile[0]);
}


static void SaveSettings()
{
	//Window Settings
	WritePrivateProfileBool("Settings", "ShowMainGui", b_ShowMainWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "SplitTarget", b_SplitTargetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", b_ShowPlayerWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Group", "ShowGroupWindow", b_ShowGroupWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Spells", "ShowSpellsWindow", b_ShowSpellsWindow, &s_SettingsFile[0]);

	WritePrivateProfileInt("PlayerTarg", "FlashInterval", s_FlashInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", s_PlayerBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", s_TargetBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", s_AggroBarHeight, &s_SettingsFile[0]);

	//Color Settings
	SaveColorToIni("Colors", "MinColorHP", minColorHP, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "MaxColorHP", maxColorHP, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "MinColorEnd", minColorEnd, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "MaxColorEnd", maxColorEnd, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "MinColorMP", minColorMP, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "MaxColorMP", maxColorMP, &s_SettingsFile[0]);
}


// Update the settings file to use the character-specific INI file if the player is in-game
static void UpdateSettingFile()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (!b_charIniLoaded)
		{
			if (PCHARINFO pCharInfo = GetCharInfo())
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
				b_charIniLoaded = true;
			}
		}
	}
	else
	{
		if (b_charIniLoaded || !b_DefaultLoaded)
		{
			memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
			fmt::format_to(s_SettingsFile, "{}/MQ2GrimGUI.ini", gPathConfig);
			b_charIniLoaded = false;
			LoadSettings();
			b_DefaultLoaded = true;
		}
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
		if (g_TargetData.m_tName != "Unknown")
		{
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2);
			float tarPercentage = static_cast<float>(g_TargetData.m_tCurHP) / 100;
			int tar_label = g_TargetData.m_tCurHP;
			ImVec4 colorTarHP = CalculateProgressiveColor(minColorHP, maxColorHP, g_TargetData.m_tCurHP);

			if (g_TargetData.m_tName == g_CharData.m_Name)
			{
				tarPercentage = static_cast<float>(g_CharData.m_CurHP) / g_CharData.m_MaxHP;
				tar_label = g_CharData.m_HealthPctInt;
				colorTarHP = CalculateProgressiveColor(minColorHP, maxColorHP, g_CharData.m_HealthPctInt);
			}
			if (g_TargetData.b_IsVis)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ColorToVec("green"));
				ImGui::Text(ICON_MD_VISIBILITY);
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ColorToVec("red"));
				ImGui::Text(ICON_MD_VISIBILITY_OFF);
				ImGui::PopStyleColor();
			}
			ImGui::SameLine();
			ImGui::Text(g_TargetData.m_tName.c_str());

			ImGui::SameLine(sizeX * .75);
			ImGui::TextColored(ColorToVec("tangarine"), "%0.1f m", g_TargetData.m_tDist);

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(tarPercentage, ImVec2(0.0f, s_TargetBarHeight), "##");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX - 8), yPos));
			ImGui::Text("%d %%", tar_label);

			ImGui::TextColored(ColorToVec("teal"), "Lvl %d", g_TargetData.m_tLevel);

			ImGui::SameLine();
			ImGui::Text(g_TargetData.m_tClass.c_str());

			ImGui::SameLine();
			ImGui::Text(g_TargetData.m_tBody.c_str());

			ImGui::SameLine(sizeX * .5);
			ImGui::PushStyleColor(ImGuiCol_Text, ConColorToVec(g_TargetData.m_tConColor));
			ImGui::Text(ICON_MD_LENS);
			ImGui::PopStyleColor();

			if (g_TargetData.m_myAgroPct < 100)
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ColorToVec("orange"));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ColorToVec("purple"));
			}
			ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(static_cast<float>(g_TargetData.m_myAgroPct) / 100, ImVec2(0.0f, s_AggroBarHeight), "##Aggro");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(10, yPos));
			ImGui::Text("%s", g_TargetData.m_secondAgroName.c_str());
			ImGui::SetCursorPos(ImVec2((sizeX/2)-8, yPos));
			ImGui::Text("%d %%", g_TargetData.m_myAgroPct);
			ImGui::SetCursorPos(ImVec2(sizeX - 40, yPos));
			ImGui::Text("%d %%", g_TargetData.m_secondAgroPct);	
		}
	}

static void DrawPlayerWindow()
	{
		if (!b_ShowPlayerWindow)
			return;

		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Player##MQ2GrimGUI", &b_ShowPlayerWindow, ImGuiWindowFlags_MenuBar))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int midX = (sizeX / 2) - 8;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Main"))
				{
					if (ImGui::MenuItem("Split Target", NULL, b_SplitTargetWindow))
					{
						b_SplitTargetWindow = !b_SplitTargetWindow;
						WritePrivateProfileBool("PlayerTarg", "SplitTarget", b_SplitTargetWindow, &s_SettingsFile[0]);
					}

					if (ImGui::MenuItem("Show Config", NULL, b_ShowConfigWindow))
					{
						b_ShowConfigWindow = !b_ShowConfigWindow;
					}

					if (ImGui::MenuItem("Show Main", NULL, b_ShowMainWindow))
					{
						b_ShowMainWindow = !b_ShowMainWindow;
						WritePrivateProfileBool("Settings", "ShowMainGui", b_ShowMainWindow, &s_SettingsFile[0]);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (g_CharData.b_IsCombat)
			{
				if (b_flashCombat)
				{
					ImGui::PushStyleColor(ImGuiCol_Border, ColorToVec("Red"));
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Border, ColorToVec("White"));
				}
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Border, ColorToVec("White"));
			}

			if (ImGui::BeginChild("info", ImVec2(ImGui::GetContentRegionAvail().x, 35), true, ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar))
			{
				if (ImGui::BeginTable("##Player", 3))
				{
					ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch, ImGui::GetContentRegionAvail().x * .5);
					ImGui::TableSetupColumn("##Vis", ImGuiTableColumnFlags_WidthFixed, 24);
					ImGui::TableSetupColumn("##Lvl", ImGuiTableColumnFlags_WidthStretch, 60);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("%s", g_CharData.m_Name.c_str());
					ImGui::TableNextColumn();
					if (g_CharData.b_MainTank)
					{
						ImGui::Text("Tank");
					}

					ImGui::TableNextColumn();
					ImGui::Text("Lvl: %d", g_CharData.m_Level);
					ImGui::EndTable();
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImVec4 colorHP = CalculateProgressiveColor(minColorHP, maxColorHP, g_CharData.m_HealthPctInt);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorHP);
			ImGui::SetNextItemWidth(sizeX - 15);
			float yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(g_CharData.m_HealthPctFloat, ImVec2(0.0f, s_PlayerBarHeight), "##hp");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("HP: %d / %d", g_CharData.m_CurHP, g_CharData.m_MaxHP);
				ImGui::EndTooltip();
			}
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
			ImGui::Text("%d %%", g_CharData.m_HealthPctInt);

			if (g_CharData.m_MaxMana > 0)
			{
				float manaPercentage = static_cast<float>(g_CharData.m_CurMana) / g_CharData.m_MaxMana;
				int manaPercentageInt = static_cast<int>(manaPercentage * 100);
				ImVec4 colorMP = CalculateProgressiveColor(minColorMP, maxColorMP, manaPercentageInt);

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorMP);
				ImGui::SetNextItemWidth(sizeX - 15);
				yPos = ImGui::GetCursorPosY();
				ImGui::ProgressBar(manaPercentage, ImVec2(0.0f, s_PlayerBarHeight), "##Mana");
				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Mana: %d / %d", g_CharData.m_CurMana, g_CharData.m_MaxMana);
					ImGui::EndTooltip();
				}
				ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
				ImGui::Text("%d %%", manaPercentageInt);

			}

			float endurancePercentage = static_cast<float>(g_CharData.m_CurEndur) / g_CharData.m_MaxEndur;
			int endPercentageInt = static_cast<int>(endurancePercentage * 100);
			ImVec4 colorEP = CalculateProgressiveColor(minColorEnd, maxColorEnd, endPercentageInt);

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorEP);
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(endurancePercentage, ImVec2(0.0f, s_PlayerBarHeight), "##Endur");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Endur: %d / %d", g_CharData.m_CurEndur, g_CharData.m_MaxEndur);
				ImGui::EndTooltip();
			}
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
			ImGui::Text("%d %%", endPercentageInt);

			if (!b_SplitTargetWindow)
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
	//TODO: Pet Window
}

static void DrawSpellWindow()
{
	//TODO: Spell Window
}

static void DrawBuffWindow()
{
	//TODO: Buff Window
}



void DrawConfigWindow()
	{
		if (!b_ShowConfigWindow)
			return;

		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Config##ConfigWindow", &b_ShowConfigWindow))
		{

			if (ImGui::CollapsingHeader("Color Settings"))
			{

				if (ImGui::BeginTable("##Settings", 2))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Min HP Color", (float*)&minColorHP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Minimum HP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Max HP Color", (float*)&maxColorHP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Maximum HP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Min MP Color", (float*)&minColorMP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Minimum MP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Max MP Color", (float*)&maxColorMP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Maximum MP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Min End Color", (float*)&minColorEnd, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Minimum Endurance Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Max End Color", (float*)&maxColorEnd, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Maximum Endurance Color");

				ImGui::EndTable();
			}

				ImGui::SeparatorText("Test Color");

				ImGui::SliderInt("Test Value", &testInt, 0, 100);
				float testVal = static_cast<float>(testInt) / 100;
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(minColorHP, maxColorHP, testInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "HP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(minColorMP, maxColorMP, testInt));
				ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "MP##Test");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, CalculateProgressiveColor(minColorEnd, maxColorEnd, testInt));
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
				WritePrivateProfileBool("PlayerTarg", "PlayerBarHeight", s_PlayerBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileBool("PlayerTarg", "TargetBarHeight", s_TargetBarHeight, &s_SettingsFile[0]);
				WritePrivateProfileBool("PlayerTarg", "AggroBarHeight", s_AggroBarHeight, &s_SettingsFile[0]);

				SaveColorToIni("Colors", "MinColorHP", minColorHP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "MaxColorHP", maxColorHP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "MinColorMP", minColorMP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "MaxColorMP", maxColorMP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "MinColorEnd", minColorEnd, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "MaxColorEnd", maxColorEnd, &s_SettingsFile[0]);
				b_ShowConfigWindow = false;
			}
		}
		ImGui::End();
	}


// Main Window, toggled with /Grimgui command, contains Toggles to show other windows
void DrawMainWindow()
	{
		if (b_ShowMainWindow)
		{

			ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("GrimGUI##MainWindow", &b_ShowMainWindow))
			{
				if (ImGui::Checkbox("Player Win", &b_ShowPlayerWindow))
				{
					WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", b_ShowPlayerWindow, &s_SettingsFile[0]);
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Split Target", &b_SplitTargetWindow))
				{
					WritePrivateProfileBool("PlayerTarg", "SplitTarget", b_SplitTargetWindow, &s_SettingsFile[0]);
				}

				//TODO: More Windows
				//ImGui::Separator();

				//if (ImGui::Checkbox("Spells Win", &b_ShowSpellsWindow))
				//{
				//	WritePrivateProfileBool("Spells", "ShowSpellsWindow", b_ShowSpellsWindow, &s_SettingsFile[0]);
				//}

				//ImGui::SameLine();

				//if (ImGui::Checkbox("Group Win", &b_ShowGroupWindow))
				//{
				//	WritePrivateProfileBool("Group", "ShowGroupWindow", b_ShowGroupWindow, &s_SettingsFile[0]);
				//}

				ImGui::Separator();

				if (ImGui::Button("Config"))
				{
					b_ShowConfigWindow = true;
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
	b_ShowMainWindow = !b_ShowMainWindow;
	WritePrivateProfileBool("Settings", "ShowMainGui", b_ShowMainWindow, &s_SettingsFile[0]);
}

// Called periodically by MQ2
PLUGIN_API void OnPulse()
{
	auto now = std::chrono::steady_clock::now();
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (now - g_LastUpdateTime >= g_UpdateInterval)
		{
			g_CharData.Update();  // Refresh character data at the specified interval
			g_TargetData.Update();
			g_LastUpdateTime = now;
		}

		if (s_FlashInterval > 0)
		{
			if (now - g_LastFlashTime >= std::chrono::milliseconds(500 - s_FlashInterval))
			{
				b_flashCombat = !b_flashCombat;
				g_LastFlashTime = now;
			}
		}
		else
		{
			b_flashCombat = false;
		}
	}
	UpdateSettingFile();

}

PLUGIN_API void OnUpdateImGui()
{
	// Draw the GUI elements
	// Update Settings Toggle when Window State Changes

	// Main Window no state check needed 
	if (b_ShowMainWindow)
	{
		DrawMainWindow();
		
		if (!b_ShowMainWindow)
		{
			WritePrivateProfileBool("Settings", "ShowMainGui", b_ShowMainWindow, &s_SettingsFile[0]);
		}
	}

	if (GetGameState() == GAMESTATE_INGAME)
	{
		// Player Window (also target if not split)
		if (b_ShowPlayerWindow)
		{
			DrawPlayerWindow();

			if (!b_ShowPlayerWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", b_ShowPlayerWindow, &s_SettingsFile[0]);
			}
		}
		
		// Split Target Window
		if (b_SplitTargetWindow)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Tar##MQ2GrimGUI", &b_SplitTargetWindow))
			{
				DrawTargetWindow();
			}
			ImGui::End();

			if (!b_SplitTargetWindow)
			{
				WritePrivateProfileBool("PlayerTarg", "SplitTarget", b_SplitTargetWindow, &s_SettingsFile[0]);
			}
		}

		if (b_ShowConfigWindow)
		{
			DrawConfigWindow();
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

	// update data
	if (GetGameState() == GAMESTATE_INGAME)
	{
		g_CharData.Update();
		g_TargetData.Update();
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
