#include <mq/Plugin.h>
#include <chrono>
#include <imgui.h>
#include <filesystem>
#include "MQ2GrimGUI.h"
#include "Theme.h"

PreSetup("MQ2GrimGUI");
PLUGIN_VERSION(0.2);

static bool s_IsCasting						= false;
static bool s_CharIniLoaded					= false;
static bool s_DefaultLoaded					= false;
static bool s_IsCaster						= false;
static bool s_FollowClicked					= false;
static const char* PLUGIN_NAME				= "MQ2GrimGUI";
static std::string DEFAULT_INI				= fmt::format( "{}/{}.ini", gPathConfig, PLUGIN_NAME);


#pragma region Timers

std::chrono::steady_clock::time_point g_LastUpdateTime		= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime		= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastBuffFlashTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_StartCastTime;

const auto g_UpdateInterval			= std::chrono::milliseconds(500);

#pragma endregion

#pragma region Settings Functions

static void LoadSettings()
{
	// Load settings from the INI file

	for (const auto& setting : winSettings)
	{
		*setting.setting = GetPrivateProfileBool(setting.section, setting.key,
			*setting.setting, &s_SettingsFile[0]);
	}

	for (const auto& setting : numericSettings)
	{
		*setting.value = GetPrivateProfileInt(setting.section, setting.key,
			*setting.value, &s_SettingsFile[0]);
	}

	for (const auto& setting : themeSettings)
	{
		*setting.theme = GetPrivateProfileString(setting.section, setting.key,
			"Default", &s_SettingsFile[0]);
	}

	for (const auto& setting : colorSettings)
	{
		*setting.value = GetPrivateProfileColor(setting.section, setting.key,
			*setting.value, &s_SettingsFile[0]);
	}

	for (auto& button : petButtons)
	{
		button.visible = GetPrivateProfileBool("Pet", button.name.c_str(),
			&button.visible, &s_SettingsFile[0]);
	}
}

static void SaveSettings()
{
	for (const auto& setting : winSettings)
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

/**
* @brief Updates the settings file based on the current game state
*		 between the character specific and default settings file
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
				snprintf(CharIniFile, MAX_PATH, "%s/%s_%s_%s.ini", gPathConfig, PLUGIN_NAME, GetServerShortName(), pCharInfo->Name);
				
				if (!std::filesystem::exists(CharIniFile))
				{
					// Check for character-specific file if missing then check for the default file to copy from incase edited at char select
					// this way we can copy their settings over. 
					// This allows you to set up the settings once and all characters can use that for a base.
					if (std::filesystem::exists(DEFAULT_INI))
					{
						std::filesystem::copy_file(DEFAULT_INI, CharIniFile, std::filesystem::copy_options::overwrite_existing);
					}
					else
					{
						// just save defaults to char config if the default is missing, the means the plugin was first loaded in game.
						memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
						strcpy_s(s_SettingsFile, CharIniFile);
						SaveSettings();
						s_CharIniLoaded = true;
					}

				}

				if (!s_CharIniLoaded)
				{
					memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
					strcpy_s(s_SettingsFile, CharIniFile);

					LoadSettings();
					s_CharIniLoaded = true;
				}

				
				s_IsCaster = GetMaxMana() && GetMaxMana() > 0;

				if (s_IsCaster) 	// new char logged in, load their spell book
					pSpellPicker->InitializeSpells();

			}
		}
	}
	else
	{
		if (s_CharIniLoaded || !s_DefaultLoaded)
		{
			s_CharIniLoaded = false;

			if (strcmp(s_SettingsFile, DEFAULT_INI.c_str()) != 0) 
			{
				memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
				strcpy_s(s_SettingsFile, DEFAULT_INI.c_str());
			}
			LoadSettings();

			s_DefaultLoaded = true;
			s_IsCaster = false;
		}
	}
}

#pragma endregion

#pragma region GUI Windows 
// each window has a default initial size and window position on first use ever
// all of the initial sizes have been verified to be on the screen down to 800x600 resolution.
// resolutions below that is questionable on many lvls. you may need a bigger monitor.

static void DrawTargetWindow()
	{
	if (PSPAWNINFO CurTarget = pTarget)
	{
		const char* tarName = CurTarget->DisplayedName;
		if (mq::IsAnonymized())
		{
			if (CurTarget->Type == SPAWN_PLAYER)
				tarName = MaskName(tarName);
		}

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
			// LoS Idicator Icon
			DrawLineOfSight(pLocalPlayer, pTarget);
			ImGui::SameLine();
			//name
			ImGui::Text(tarName);

			ImGui::SameLine(sizeX * .75f);
			//distance
			ImGui::TextColored(GetMQColor(ColorName::Tangerine).ToImColor(), "%0.1f m", GetDistance(pLocalPlayer, pTarget));

			// Target HP Bar
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(tarPercentage, ImVec2(0.0f, static_cast<float>(s_NumSettings.targetBarHeight)), "##");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX - 8), yPos));
			ImGui::Text("%d %%", tar_label);
			ImGui::NewLine();
			//tar level
			ImGui::SameLine();
			ImGui::TextColored(GetMQColor(ColorName::Teal).ToImColor(), "Lvl %d", CurTarget->Level);

			//target class
			ImGui::SameLine();
			const char* classCode = CurTarget->GetClassThreeLetterCode();
			
			const char* tClass = (classCode && classCode != "UNKNOWN CLASS") ? classCode : ICON_MD_HELP_OUTLINE;
			ImGui::Text(tClass);
			//body type
			ImGui::SameLine();
			ImGui::Text(GetBodyTypeDesc(GetBodyType(pTarget)));

			ImGui::SameLine(sizeX * .5f);
			ImGui::TextColored(ImVec4(GetConColor(ConColor(pTarget)).ToImColor()),ICON_MD_LENS);
			
			//aggro meter
			if (s_WinSettings.showAggroMeter)
			{

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
				ImGui::SetCursorPos(ImVec2((sizeX / 2) - 8, yPos));
				ImGui::Text("%d %%", s_NumSettings.myAggroPct);
				ImGui::SetCursorPos(ImVec2(sizeX - 40, yPos));
				ImGui::Text("%d %%", s_NumSettings.secondAggroPct);
			}
			
			// Target Buffs Section
			if (s_WinSettings.showTargetBuffs)
			{
				if (ImGui::BeginChild("TargetBuffs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Border , ImGuiWindowFlags_NoScrollbar))
				{

					if (gTargetbuffs)
						pSpellInspector->DrawBuffsIcons("TargetBuffsTable", pTargetWnd->GetBuffRange(), false);
				}
				ImGui::EndChild();
			}
		}
	}

static void DrawPlayerWindow()
	{
		if (!s_WinSettings.showPlayerWindow)
			return;
		float displayX = ImGui::GetIO().DisplaySize.x;
		ImGui::SetNextWindowPos(ImVec2(displayX - 310, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 290), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.playerWinTheme);
		ImGuiWindowFlags menuFlag = s_WinSettings.showTitleBars ? ImGuiWindowFlags_MenuBar : ImGuiWindowFlags_None;
		if (ImGui::Begin("Player##MQ2GrimGUI", &s_WinSettings.showPlayerWindow, s_WindowFlags | s_WinLockFlags | menuFlag | ImGuiWindowFlags_NoScrollbar))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int midX = (sizeX / 2) - 8;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Main"))
				{
					if (ImGui::MenuItem("Lock Windows", NULL, s_WinSettings.lockWindows))
					{
						s_WinSettings.lockWindows = !s_WinSettings.lockWindows;
						SaveSetting(&s_WinSettings.lockWindows, &s_SettingsFile[0]);
					}

					if (ImGui::MenuItem("Split Target", NULL, s_WinSettings.showTargetWindow))
					{
						s_WinSettings.showTargetWindow = !s_WinSettings.showTargetWindow;
						SaveSetting(&s_WinSettings.showTargetWindow, &s_SettingsFile[0]);
					}

					if (ImGui::MenuItem("Show Config", NULL, s_WinSettings.showConfigWindow))
						s_WinSettings.showConfigWindow = !s_WinSettings.showConfigWindow;

					if (ImGui::MenuItem("Show Main", NULL, s_WinSettings.showMainWindow))
					{
						s_WinSettings.showMainWindow = !s_WinSettings.showMainWindow;
						SaveSetting(&s_WinSettings.showMainWindow, &s_SettingsFile[0]);
					}

					ImGui::EndMenu();
				}
				
				ImGui::EndMenuBar();
			}

			DrawPlayerBars(true);

			if (!s_WinSettings.showTargetWindow)
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
	if (!s_WinSettings.showGroupWindow)
		return;
	float displayX = ImGui::GetIO().DisplaySize.x;
	ImGui::SetNextWindowPos(ImVec2(displayX - 310, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.groupWinTheme);

	if (ImGui::Begin("Group##MQ2GrimGUI", &s_WinSettings.showGroupWindow,
		s_WindowFlags | s_WinLockFlags | ImGuiWindowFlags_NoScrollbar))
	{
		if (s_WinSettings.showSelfOnGroup)
			DrawPlayerBars(false, s_NumSettings.groupBarHeight, true);

		PCHARINFO pChar = GetCharInfo();
		if (pChar && pChar->pGroupInfo)
		{
			for (int i = 1; i < MAX_GROUP_SIZE; i++)
			{
				if (CGroupMember* pMember = pChar->pGroupInfo->GetGroupMember(i))
				{
					DrawGroupMemberBars(pMember, true, i);
				}
				else
				{
					if (s_WinSettings.showEmptyGroupSlot)
						DrawEmptyMember(i);
				}
			}
		}
		else
		{
			if (s_WinSettings.showEmptyGroupSlot)
			{
				for (int i = 1; i < MAX_GROUP_SIZE; i++)
				{

					DrawEmptyMember(i);
				}
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


		if (mq::IsPluginLoaded("MQ2DanNet"))
		{
			posX = ImGui::GetWindowWidth() * 0.5f - 80;
			if (posX < 0)
				posX = 0;

			ImGui::SetCursorPosX(posX);

			int myID = pLocalPlayer->GetId();
			const char* followLabel = "Follow Me";
			if (s_FollowClicked)
				followLabel = "Stop Follow##";

			if (ImGui::Button(followLabel, ImVec2(75, 20)))
			{
				if (s_FollowClicked)
				{
					EzCommand(fmt::format("/dgge /multiline ; /afollow off; /nav stop ; /timed 5, /dgge /afollow spawn {}", myID).c_str());
				}
				else
				{
					EzCommand("/dgge /multiline ; /afollow off; /nav stop");
				}
				s_FollowClicked = !s_FollowClicked;
			}

			ImGui::SameLine();

			if (ImGui::Button("Come Here##", ImVec2(75, 20)))
			{
				EzCommand(fmt::format("/dgge /multiline ; /afollow off; /nav stop ;  /timed 5, /nav id {}", myID).c_str());
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
		if (mq::IsAnonymized())
			petName = "Pet";
		float displayX = ImGui::GetIO().DisplaySize.x;
		float displayY = ImGui::GetIO().DisplaySize.y;
		ImGui::SetNextWindowPos(ImVec2(displayX * 0.75f, displayY * 0.5f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 283), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.petWinTheme);
		if (ImGui::Begin("Pet##MQ2GrimGUI", &s_WinSettings.showPetWindow, s_WindowFlags | s_WinLockFlags | ImGuiWindowFlags_NoScrollbar))
		{
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2);

			if (ImGui::BeginTable("Pet", 2,
				ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable ))
			{
				ImGui::TableSetupColumn(petName, ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupColumn("Buffs", ImGuiTableColumnFlags_None, -1);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				
				DrawPetInfo(MyPet);

				// Pet Target Section
				if (ImGui::BeginChild("PetTarget", ImVec2(ImGui::GetColumnWidth(), 0),
					ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY,
					ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
				{
					if (PSPAWNINFO pPetTarget = MyPet->WhoFollowing)
					{
						const char* petTargetName = pPetTarget->DisplayedName;
						if (mq::IsAnonymized())
							petTargetName = "Pet Target";

						ImGui::Text("Lvl");
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(GetMQColor(ColorName::Teal).ToImColor()), "%d", pPetTarget->Level);
						ImGui::SameLine();
						ImGui::Text(petTargetName);
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
				
				//Pet Buttons Section
				if (s_WinSettings.showPetButtons)
				{
					if (ImGui::BeginChild("PetButtons", ImVec2(ImGui::GetColumnWidth(), 0),
						ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
						DisplayPetButtons();

					ImGui::EndChild();
				}
				// Pet Buffs Section (Column)
				ImGui::TableNextColumn();
				
				if (ImGui::BeginChild("PetBuffs", ImVec2(ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y), 
					ImGuiChildFlags_Border , ImGuiWindowFlags_NoScrollbar))
					pSpellInspector->DrawBuffsIcons("PetBuffsTable", pPetInfoWnd->GetBuffRange(), true);
				
				ImGui::EndChild();

				ImGui::EndTable();
			}

		}
		PopTheme(popCounts);
		ImGui::End();
	}

}

static void DrawCastingBarWindow()
{
	if (pCastingWnd && pCastingWnd->IsVisible())
	{
		if (!s_IsCasting)
		{
			g_StartCastTime = std::chrono::steady_clock::now();
			s_IsCasting = true;

		}
		float displayX = ImGui::GetIO().DisplaySize.x;
		float displayY = ImGui::GetIO().DisplaySize.y;
		ImGui::SetNextWindowPos(ImVec2((displayX * 0.5f) - 150, displayY * 0.3f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 60), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.spellsWinTheme);
		if (ImGui::Begin("Casting##MQ2GrimGUI1", &s_IsCasting,
			s_WinLockFlags | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar))
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
					float castingTime = static_cast<float>(pSpell->CastTime - GetCastingTimeModifier(pSpell));
					float spellProgress = 1.0f - static_cast<float>(spellTimer / castingTime);
					ImVec4 colorCastBar = CalculateProgressiveColor(s_BarColors.minColorCast, s_BarColors.maxColorCast, static_cast<int>(spellProgress * 100));

					ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorCastBar);
					ImGui::ProgressBar(spellProgress, ImVec2(ImGui::GetContentRegionAvail().x, 12), "##CastingProgress");
					ImGui::PopStyleColor();
					ImGui::Text("%s %0.1fs", spellName, (castingTime - spellTimer) / 1000);
						
					switch (pSpell->TargetType)
					{
					case TargetType_TargetPC:
						if (pTarget)
							ImGui::Text("Target: %s", pTarget->DisplayedName);
						break;
					case TargetType_Single:
						if (pTarget)
							ImGui::Text("Target: %s", pTarget->DisplayedName);
						break;
					case TargetType_Self:
						ImGui::Text("Target: %s", pLocalPlayer->DisplayedName);
						break;
					}
				}
			}

		}
		ImGui::End();
		PopTheme(popCounts);
	}
	else
		s_IsCasting = false;
}

static void DrawSpellWindow()
{
	if (!s_WinSettings.showSpellsWindow)
		return;

	if (s_IsCaster)
	{
		float displayX = ImGui::GetIO().DisplaySize.x;
		float displayY = ImGui::GetIO().DisplaySize.y;
		ImGui::SetNextWindowPos(ImVec2(230,80), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(79, 662), ImGuiCond_FirstUseEver);
		int popCounts = PushTheme(s_WinTheme.spellsWinTheme);
	
		if (ImGui::Begin("Spells##MQ2GrimGUI", &s_WinSettings.showSpellsWindow,
			s_WindowFlags | s_WinLockFlags | ImGuiWindowFlags_AlwaysAutoResize))
		{
			DrawSpellBarIcons(s_NumSettings.spellGemHeight);

			ImGui::Spacing();
			ImGui::Dummy(ImVec2(0, 15));
		}
		ImGui::End();
		PopTheme(popCounts);
	}
}

static void DrawBuffWindow()
{
	if (!s_WinSettings.showBuffWindow)
		return;

	ImGui::SetNextWindowPos(ImVec2(15,10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(210, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.buffsWinTheme);
	if (ImGui::Begin("Buffs##MQ2GrimGUI", &s_WinSettings.showBuffWindow,
		s_WindowFlags | s_WinLockFlags | ImGuiWindowFlags_NoScrollbar))
		pSpellInspector->DrawBuffsList("BuffTable", pBuffWnd->GetBuffRange(), false, true);

	PopTheme(popCounts);
	ImGui::End();

}

static void DrawSongWindow()
{
	if (!s_WinSettings.showSongWindow)
		return;

	ImGui::SetNextWindowPos(ImVec2(15, 310), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(210, 300), ImGuiCond_FirstUseEver);
	int popCounts = PushTheme(s_WinTheme.songWinTheme);
	if (ImGui::Begin("Songs##MQ2GrimGUI", &s_WinSettings.showSongWindow,
		s_WindowFlags | s_WinLockFlags | ImGuiWindowFlags_NoScrollbar))
		pSpellInspector->DrawBuffsList("SongTable", pSongWnd->GetBuffRange(), false, true);
	
	PopTheme(popCounts);
	ImGui::End();
}

static void DrawHudWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (s_WinSettings.hudClickThrough)
		window_flags |= ImGuiWindowFlags_NoInputs;

	float displayX = ImGui::GetIO().DisplaySize.x;
	ImGui::SetNextWindowPos(ImVec2((displayX * 0.5f) - 15, 0), ImGuiCond_FirstUseEver);
	float alpha = (s_NumSettings.hudAlpha / 255.0f);
	ImGui::SetNextWindowBgAlpha(alpha); // Transparent background
	if (ImGui::Begin("Hud##GrimGui", &s_WinSettings.showHud, s_WinLockFlags | window_flags))
	{

		DrawStatusEffects();

		if (ImGui::BeginPopupContextWindow("HudContext##GrimGui",ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Toggle Click Through"))
			{
				s_WinSettings.hudClickThrough = !s_WinSettings.hudClickThrough;
				SaveSetting(&s_WinSettings.hudClickThrough, s_SettingsFile);
			}

			if (ImGui::MenuItem("Close Hud"))
			{
				s_WinSettings.showHud = false;
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

static void DrawConfigWindow()
{
	if (!s_WinSettings.showConfigWindow)
		return;
	float displayX = ImGui::GetIO().DisplaySize.x;
	float displayY = ImGui::GetIO().DisplaySize.y;
	ImGui::SetNextWindowPos(ImVec2((displayX* 0.5f) -150, displayY * 0.3f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Config##ConfigWindow", &s_WinSettings.showConfigWindow, s_WindowFlags))
	{
		int sizeX = static_cast<int>(ImGui::GetWindowWidth());

		if (ImGui::CollapsingHeader("Color Settings"))
		{
			if (ImGui::BeginTable("##Settings", 2))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				for (const auto& setting : colorSettings)
				{
					ImVec4 colorValue = setting.value->ToImColor();
					if (ImGui::ColorEdit4(setting.label, (float*)&colorValue, ImGuiColorEditFlags_NoInputs))
						*setting.value = MQColor(colorValue);

					ImGui::SameLine();
					DrawHelpIcon(setting.helpText);

					ImGui::TableNextColumn();
				}

				ImGui::EndTable();
			}

			ImGui::SeparatorText("Test Color");

			ImGui::SliderInt("Test Value", &s_TestInt, 0, 100);
			float testVal = static_cast<float>(s_TestInt) / 100;
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
				CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Health##Test");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
				CalculateProgressiveColor(s_BarColors.minColorMP, s_BarColors.maxColorMP, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Mana##Test");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
				CalculateProgressiveColor(s_BarColors.minColorEnd, s_BarColors.maxColorEnd, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Endur##Test");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
				CalculateProgressiveColor(s_BarColors.minColorCast, s_BarColors.maxColorCast, s_TestInt));
			ImGui::ProgressBar(testVal, ImVec2(0.0f, 15.0f), "Cast##Test");
			ImGui::PopStyleColor();
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Window Settings Sliders"))
		{

			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int col = sizeX / 220;
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
			for (const auto& toggle : settingToggleOptions)
			{
				if (ImGui::Checkbox(toggle.label, toggle.setting))
					SaveSetting(toggle.setting, &s_SettingsFile[0]);

				ImGui::SameLine();
				DrawHelpIcon(toggle.helpText);
			}
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
			s_WinSettings.showConfigWindow = false;
			SaveSettings();
		}
	}
	ImGui::End();
}

// Main Window, toggled with /Grimgui command, contains Toggles to show other windows
static void DrawMainWindow()
{
	if (s_WinSettings.showMainWindow)
	{
		float displayX = ImGui::GetIO().DisplaySize.x;
		float displayY = ImGui::GetIO().DisplaySize.y;
		ImGui::SetNextWindowPos(ImVec2((displayX * 0.5f) - 150, displayY * 0.5f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("GrimGUI##MainWindow", &s_WinSettings.showMainWindow))
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
						SaveSetting(option.setting, &s_SettingsFile[0]);
					
					ImGui::TableNextColumn();
				}
				ImGui::EndTable();
			}

			ImGui::Separator();

			if (ImGui::Button("Config"))
				s_WinSettings.showConfigWindow = true;

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
		s_IsCaster = CheckCaster();
		s_WinLockFlags = s_WinSettings.lockWindows ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;
		s_WindowFlags = s_WinSettings.showTitleBars ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoTitleBar;

		if (s_ShowOutOfGame)
			s_ShowOutOfGame = false; // reset incase we logged back in. =)

		if (!s_CharIniLoaded)
			UpdateSettingFile();

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

			GetHeading();

			g_LastUpdateTime = now;

			s_DanNetEnabled = mq::IsPluginLoaded("MQ2DanNet");
		}

		// update buff flash timers
		if (s_NumSettings.flashBuffInterval > 0)
		{
			if (now - g_LastBuffFlashTime >= std::chrono::milliseconds(500 - (s_NumSettings.flashBuffInterval * 10)))
			{
				s_WinSettings.flashTintFlag = !s_WinSettings.flashTintFlag;
				g_LastBuffFlashTime = now;
			}
		}
		else
		{
			s_WinSettings.flashTintFlag = false;
		}

		// update combat flash timers
		if (s_NumSettings.combatFlashInterval > 0)
		{
			if (now - g_LastFlashTime >= std::chrono::milliseconds(500 - (s_NumSettings.combatFlashInterval * 10)))
			{
				s_WinSettings.flashCombatFlag = !s_WinSettings.flashCombatFlag;
				g_LastFlashTime = now;
			}
		}
		else
		{
			s_WinSettings.flashCombatFlag = false;
		}

		if (pSpellPicker->SelectedSpell)
		{
			s_MemSpellName = pSpellPicker->SelectedSpell->Name;
			MemSpell(pLocalPlayer, fmt::format("{} \"{}\"", s_MemGemIndex, s_MemSpellName).c_str());
			pSpellPicker->ClearSelection();
			s_MemGemIndex = 0;
		}
	}
	else
	{
		UpdateSettingFile();
	}
}

PLUGIN_API void OnUpdateImGui()
{
	// Windows that can be shown out of game or in game. 
	// Main Toggle window for other windows and Config window.
	// This way you can at least configure coloring and sizing as well as your default set of windows. 
	// The out of game ini file is for building new character ini's whtn they do not exist.
	if (s_WinSettings.showMainWindow && (s_ShowOutOfGame || GetGameState() == GAMESTATE_INGAME))
	{
		DrawMainWindow();

		if (!s_WinSettings.showMainWindow)
		{
			SaveSetting(&s_WinSettings.showMainWindow, &s_SettingsFile[0]);
			if (s_ShowOutOfGame)
				s_ShowOutOfGame = false;
		}
	}

	if (s_WinSettings.showConfigWindow)
		DrawConfigWindow();

	// ImGame windows only.
	if (GetGameState() == GAMESTATE_INGAME)
	{
		// Player Window (also target if not split)
		if (s_WinSettings.showPlayerWindow)
		{
			DrawPlayerWindow();

			if (!s_WinSettings.showPlayerWindow)
				SaveSetting(&s_WinSettings.showPlayerWindow, &s_SettingsFile[0]);
		}

		// Pet Window
		if (s_WinSettings.showPetWindow)
		{
			DrawPetWindow();

			if (!s_WinSettings.showPetWindow)
				SaveSetting(&s_WinSettings.showPetWindow, &s_SettingsFile[0]);
		}

		//Buff Window
		if (s_WinSettings.showBuffWindow)
		{
			DrawBuffWindow();

			if (!s_WinSettings.showBuffWindow)
				SaveSetting(&s_WinSettings.showBuffWindow, &s_SettingsFile[0]);
		}

		// Song Window
		if (s_WinSettings.showSongWindow)
		{
			DrawSongWindow();

			if (!s_WinSettings.showSongWindow)
				SaveSetting(&s_WinSettings.showSongWindow, &s_SettingsFile[0]);
		}
		
		// Split Target Window
		if (s_WinSettings.showTargetWindow)
		{
			float displayX = ImGui::GetIO().DisplaySize.x;
			ImGui::SetNextWindowPos(ImVec2(displayX - 620, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(300, 185), ImGuiCond_FirstUseEver);
			int popCountsTarg = PushTheme(s_WinTheme.playerWinTheme);
			if (ImGui::Begin("Target##MQ2GrimGUI", &s_WinSettings.showTargetWindow, s_WindowFlags | s_WinLockFlags))
				DrawTargetWindow();
			
			PopTheme(popCountsTarg);
			ImGui::End();

			if (!s_WinSettings.showTargetWindow)
				SaveSetting(&s_WinSettings.showTargetWindow, &s_SettingsFile[0]);
		}

		// Group Window
		if (s_WinSettings.showGroupWindow)
		{
			DrawGroupWindow();

			if (!s_WinSettings.showGroupWindow)
				SaveSetting(&s_WinSettings.showGroupWindow, &s_SettingsFile[0]);
		}

		// Spell Window
		if (s_WinSettings.showSpellsWindow)
		{
			DrawSpellWindow();

			if (!s_WinSettings.showSpellsWindow)
				SaveSetting(&s_WinSettings.showSpellsWindow, &s_SettingsFile[0]);
		}

		// Casting Bar Window
		// This applies to all characters not just spell casters.
		DrawCastingBarWindow();

		// Spell Picker
		if (pSpellPicker)
			pSpellPicker->DrawSpellPicker();

		// Status Effects HUD Window
		if (s_WinSettings.showHud)
			DrawHudWindow();
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
		pSpellInspector = new grimgui::SpellsInspector();

	if (!pSpellPicker)
		pSpellPicker = new SpellPicker();

	// check settings file, if logged in use character specific INI else default
	UpdateSettingFile();

	if (!s_WinSettings.showTitleBars)
		s_WindowFlags = ImGuiWindowFlags_NoTitleBar;

	if (s_WinSettings.lockWindows)
		s_WinLockFlags = ImGuiWindowFlags_NoMove;
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
	DebugSpewAlways("MQ2GrimGUI::OnUnloadPlugin(%s)", Name);
	RemoveCommand("/grimui");
}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2GrimGUI");
	AddCommand("/grimgui", GrimCommandHandler, false, true, false);
	PrintGrimHelp();
	pSpellInspector = new grimgui::SpellsInspector();
	pSpellPicker = new SpellPicker();

	static bool s_DefaultExists = std::filesystem::exists(DEFAULT_INI);
	memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
	strcpy_s(s_SettingsFile, DEFAULT_INI.c_str());

	if (!s_DefaultExists)
		SaveSettings();

	UpdateSettingFile();
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2GrimGUI");
	RemoveCommand("/grimui");
	pSpellPicker = nullptr;
	pSpellInspector = nullptr;
}

#pragma endregion
