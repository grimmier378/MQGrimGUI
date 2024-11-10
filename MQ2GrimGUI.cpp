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
static bool s_ShowPlayerWindow			= false;
static bool s_FlashCombatFlag			= false;
static bool s_FlashTintFlag				= false;
static bool s_ShowGroupWindow			= false;
static bool s_ShowSpellsWindow			= false;
static bool s_charIniLoaded				= false;
static bool s_DefaultLoaded				= false;

static char s_SettingsFile[MAX_PATH]	= { 0 };

// Colors for Progress Bar Transitions
static ImVec4 s_MinColorHP(0.876f, 0.341f, 1.000f, 1.000f);
static ImVec4 s_MaxColorHP(0.845f, 0.151f, 0.151f, 1.000f);
static ImVec4 s_MinColorMP(0.259f, 0.114f, 0.514f, 1.000f);
static ImVec4 s_MaxColorMP(0.079f, 0.468f, 0.848f, 1.000f); 
static ImVec4 s_MinColorEnd(1.000f, 0.437f, 0.019f, 1.000f);
static ImVec4 s_MaxColorEnd(0.7f, 0.6f, 0.1f, 0.7f);

static int s_FlashInterval				= 250;
static int s_FlashBuffInterval			= 350;
static int s_PlayerBarHeight			= 15;
static int s_TargetBarHeight			= 15;
static int s_AggroBarHeight				= 10;
static int s_myAgroPct					= 0;
static int s_secondAgroPct				= 0;
static int s_TestInt					= 100; // Color Test Value for Config Window

static std::string s_secondAgroName		= "Unknown";
static std::string s_heading			= "N";
static int s_TarBuffLineSize = 0;

// Timers
std::chrono::steady_clock::time_point g_LastUpdateTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastBuffFlashTime = std::chrono::steady_clock::now();

const auto g_UpdateInterval		= std::chrono::milliseconds(250);


#pragma region Spells Inspector

class SpellsInspector 
{
	CTextureAnimation* m_pTASpellIcon = nullptr;
public:
	SpellsInspector() 
	{
		
	}

	~SpellsInspector()
	{
		if (m_pTASpellIcon)
		{
			delete m_pTASpellIcon;
			m_pTASpellIcon = nullptr;
		}
	}

	static void DoSpellBuffTableHeaders()
	{
		ImGui::TableSetupColumn("Index");
		ImGui::TableSetupColumn("Icon");
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Level");
		ImGui::TableSetupColumn("Duration");
		ImGui::TableSetupColumn("InitialDuration");
		ImGui::TableSetupColumn("HitCount");
		ImGui::TableSetupColumn("Type");
		ImGui::TableSetupColumn("ChargesRemaining");
		ImGui::TableSetupColumn("ViralTimer");
		ImGui::TableSetupColumn("Flags");
		ImGui::TableSetupColumn("Modifier");
		ImGui::TableSetupColumn("Activatable");

		for (int i = 0; i < NUM_SLOTDATA; ++i)
		{
			char temp[20];
			sprintf_s(temp, "Slot%d", i);
			ImGui::TableSetupColumn(temp);
		}

		ImGui::TableHeadersRow();
	}

	void DoSpellBuffTableRow(int index, EQ_Affect& buff)
	{
		EQ_Spell* spell = GetSpellByID(buff.SpellID);
		if (!spell)
			return;

		ImGui::PushID((void*)&buff);

		if (!m_pTASpellIcon)
		{
			m_pTASpellIcon = new CTextureAnimation();
			if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
				*m_pTASpellIcon = *temp;
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		// Index
		ImGui::Text("%d", index);

		// Icon
		ImGui::TableNextColumn();
		m_pTASpellIcon->SetCurCell(spell->SpellIcon);
		imgui::DrawTextureAnimation(m_pTASpellIcon);

		// Name
		ImGui::TableNextColumn();
		if (spell)
		{
			ImGui::Text("%s", spell->Name);
		}
		else
		{
			ImGui::Text("null");
		}

		if (ImGui::BeginPopupContextItem("BuffContextMenu"))
		{
			if (ImGui::Selectable("Inspect"))
			{
				char buffer[512] = { 0 };
				FormatSpellLink(buffer, 512, spell);

				TextTagInfo info = ExtractLink(buffer);
				ExecuteTextLink(info);
			}

			ImGui::Separator();

			if (ImGui::Selectable("Remove by Index"))
			{
				int nBuff = pLocalPC->GetEffectSlot(&buff);
				RemoveBuffByIndex(nBuff);
			}

			if (spell && ImGui::Selectable("Remove by Name"))
			{
				RemoveBuffByName(spell->Name);
			}

			if (ImGui::Selectable("Remove by Spell ID"))
			{
				RemoveBuffBySpellID(buff.SpellID);
			}

			ImGui::EndPopup();
		}

		// ID
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.SpellID);

		// Level
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.Level);

		// Duration
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.Duration);

		// InitialDuration
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.InitialDuration);

		// HitCount
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.HitCount);

		// Type
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.Type);

		// ChargesRemaining
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.ChargesRemaining);

		// ViralTimer
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.ViralTimer);

		// Flags
		ImGui::TableNextColumn();
		ImGui::Text("%x", buff.Flags);

		// Modifier
		ImGui::TableNextColumn();
		ImGui::Text("%.2f", buff.Modifier);

		// Activatable
		ImGui::TableNextColumn();
		ImGui::Text("%d", buff.Activatable);

		// SlotData[0]
		for (auto& slotData : buff.SlotData)
		{
			ImGui::TableNextColumn();

			int Slot = slotData.Slot;
			int64_t Value = slotData.Value;

			if (Slot != -1)
				ImGui::Text("%d: %I64", Slot, Value);
		}

		ImGui::PopID();
	}

	template <typename Iter>
	int DoSpellAffectTable(const char* name, Iter first, Iter second, bool showEmpty = false)
	{
		ImGuiTableFlags tableFlags = 0
			| ImGuiTableFlags_SizingFixedFit
			| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX
			| ImGuiTableFlags_RowBg
			| ImGuiTableFlags_Borders
			| ImGuiTableFlags_Resizable;

		int count = 2; // start with space for header and possible scroll bar

		// calculate the size
		for (auto iter = first; iter != second; ++iter)
		{
			EQ_Affect& buff = *iter;
			if (buff.SpellID == 0 && !showEmpty)
				continue;

			count++;
		}
		count = 0;

		if (ImGui::BeginTable(name, 17 + NUM_SLOTDATA, tableFlags))
		{
			ImGui::TableSetupScrollFreeze(2, 1);
			DoSpellBuffTableHeaders();
			int i = 0;

			for (auto iter = first; iter != second; ++iter)
			{
				EQ_Affect& buff = *iter;
				++i;

				if (buff.SpellID == 0 && !showEmpty)
					continue;

				DoSpellBuffTableRow(i, buff);
				count++;
			}

			ImGui::EndTable();
		}
		return count;
	}

	static void FormatBuffDuration(char* timeLabel, size_t size, int buffTimer)
	{
		if (buffTimer < 0)
		{
			strcpy_s(timeLabel, size, "Permanent");
		}
		else if (buffTimer > 0)
		{
			int hours = 0;
			int minutes = 0;
			int seconds = 0;

			int totalSeconds = buffTimer / 1000;

			if (totalSeconds > 0)
			{
				hours = totalSeconds / 3600;
				minutes = (totalSeconds % 3600) / 60;
				seconds = totalSeconds % 60;
			}

			if (hours > 0)
			{
				if (minutes > 0 && seconds > 0)
				{
					sprintf_s(timeLabel, size, "%dh %dm %ds", hours, minutes, seconds);
				}
				else if (minutes > 0)
				{
					sprintf_s(timeLabel, size, "%dh %dm", hours, minutes);
				}
				else if (seconds > 0)
				{
					sprintf_s(timeLabel, size, "%dh %ds", hours, seconds);
				}
				else
				{
					sprintf_s(timeLabel, size, "%dh", hours);
				}
			}
			else if (minutes > 0)
			{
				if (seconds > 0)
				{
					sprintf_s(timeLabel, size, "%dm %ds", minutes, seconds);
				}
				else
				{
					sprintf_s(timeLabel, size, "%dm", minutes);
				}
			}
			else
			{
				sprintf_s(timeLabel, size, "%ds", seconds);
			}
		}
		else
		{
			strcpy_s(timeLabel, size, "0s");
		}
	}

	template <typename T>
	void DoBuffsTable(const char* name, IteratorRange<PlayerBuffInfoWrapper::Iterator<T>> Buffs,
		bool petBuffs = false, bool playerBuffs = false, int baseIndex = 0)
	{
		ImGuiTableFlags tableFlags = 0
			| ImGuiTableFlags_SizingFixedFit
			| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX
			| ImGuiTableFlags_RowBg
			| ImGuiTableFlags_Borders
			| ImGuiTableFlags_Resizable;

		//if (ImGui::BeginTable(name, 1, tableFlags))
		//{
		//	ImGui::TableSetupScrollFreeze(1, 1);

		//	//ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 30.0f);
		//	ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 24.0f);
		//	//ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		//	//ImGui::TableSetupColumn("Spell ID", ImGuiTableColumnFlags_WidthFixed, 46.0f);
		//	//ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed, 78.0f);
			//ImGui::TableSetupColumn("Caster", ImGuiTableColumnFlags_WidthStretch);
			//ImGui::TableHeadersRow();

			for (const auto& buffInfo : Buffs)
			{
				EQ_Spell* spell = buffInfo.GetSpell();
				if (!spell)
					continue;

				ImGui::PushID(buffInfo.GetIndex());

				if (!m_pTASpellIcon)
				{
					m_pTASpellIcon = new CTextureAnimation();
					if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
						*m_pTASpellIcon = *temp;
				}

				//ImGui::TableNextRow();

				// Index
				//ImGui::TableNextColumn();
				//ImGui::Text("%d", buffInfo.GetIndex() + 1 + baseIndex);

				// Icon
				//ImGui::TableNextColumn();
				int sizeX = ImGui::GetContentRegionAvailWidth();
				s_TarBuffLineSize = 0;
				if (spell)
				{
					m_pTASpellIcon->SetCurCell(spell->SpellIcon);
					MQColor borderCol = MQColor(0, 0, 250, 255); // Default color blue (beneficial)
					MQColor tintCol = MQColor(255, 255, 255, 255);
					if (!spell->IsBeneficialSpell())
						borderCol = MQColor(250, 0, 0, 255); // Red for detrimental spells

					std::string caster = buffInfo.GetCaster();
					if (caster == pLocalPC->Name)
						borderCol = MQColor(250, 250, 0, 255); // Yellow for spells cast by me

					int secondsLeft = buffInfo.GetBuffTimer() / 1000;
					if (secondsLeft < 18)
					{
						if (s_FlashTintFlag)
							tintCol = MQColor(0, 0, 0, 255);
						
					}

					imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(24,24), tintCol, borderCol);
					s_TarBuffLineSize += 24;
					if (s_TarBuffLineSize < sizeX - 20)
					{
						ImGui::SameLine(0.0f, 2);
					}
					else
					{
						s_TarBuffLineSize = 0;
					}
				}
				ImGui::PopID();
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					if (spell)
					{
						char timeLabel[64];
						FormatBuffDuration(timeLabel, 64, buffInfo.GetBuffTimer());
						ImGui::Text("%s (%s)", spell->Name, timeLabel);
						ImGui::Text("Caster: %s", buffInfo.GetCaster());

					}
					ImGui::EndTooltip();
				}
				//// Name
				//ImGui::TableNextColumn();
				//if (spell)
				//{
				//	ImGui::Text("%s", spell->Name);
				//}
				//else
				//{
				//	ImGui::Text("");
				//}

				//if (spell)
				//{
				//	if (ImGui::BeginPopupContextItem("BuffContextMenu"))
				//	{
				//		if (ImGui::Selectable("Inspect"))
				//		{
				//			char buffer[512] = { 0 };
				//			FormatSpellLink(buffer, 512, spell);

				//			TextTagInfo info = ExtractLink(buffer);
				//			ExecuteTextLink(info);
				//		}

				//		if (petBuffs)
				//		{
				//			ImGui::Separator();

				//			if (ImGui::Selectable("Remove Pet Buff"))
				//			{
				//				RemovePetBuffByName(spell->Name);
				//			}
				//		}
				//		else if (playerBuffs)
				//		{
				//			if (ImGui::Selectable("Remove by Index"))
				//			{
				//				RemoveBuffByIndex(buffInfo.GetIndex() + baseIndex);
				//			}

				//			if (ImGui::Selectable("Remove by Name"))
				//			{
				//				RemoveBuffByName(spell->Name);
				//			}

				//			if (ImGui::Selectable("Remove by Spell ID"))
				//			{
				//				RemoveBuffBySpellID(buffInfo.GetSpellID());
				//			}
				//		}

				//		ImGui::EndPopup();
				//	}
				//}

				//// ID
				//ImGui::TableNextColumn();
				//ImGui::Text("%d", buffInfo.GetSpellID());

				//// Duration
				//ImGui::TableNextColumn();

				//char timeLabel[64];
				//FormatBuffDuration(timeLabel, 64, buffInfo.GetBuffTimer());
				//ImGui::Text("%s", timeLabel);

				//// Caster
				//ImGui::TableNextColumn();
				//ImGui::Text("%s", buffInfo.GetCaster());

				
			}

		/*	ImGui::EndTable();
		}*/
	}

	virtual bool IsEnabled()
	{
		PcProfile* pcProfile = GetPcProfile();
		if (!pcProfile)
		{
			return false;
		}

		return true;
	}

	void DoSpellStackingTests()
	{
		static bool bCheckSpellBuffs = true;
		ImGui::Checkbox("Check buff stacking against active buffs", &bCheckSpellBuffs);

		if (bCheckSpellBuffs)
		{
			ImGui::Text("Enter the name of a spell to test buff stacking:");
		}
		else
		{
			ImGui::TextWrapped("Enter the name of two spells to test buff stacking. The test will check the second spell against the first.");
		}

		static char searchText[256] = { 0 };
		static char searchText2[256] = { 0 };

		if (bCheckSpellBuffs)
		{
			ImGui::InputText("Spell Name", searchText2, 256);
		}
		else
		{
			ImGui::InputText("Spell 1", searchText, 256);
			ImGui::InputText("Spell 2", searchText2, 256);
		}

		SPELL* pSpell = nullptr;
		SPELL* pSpell2 = nullptr;

		if (searchText[0])
		{
			pSpell = GetSpellByName(searchText);
			if (!pSpell)
			{
				ImGui::TextColored(ImColor(255, 0, 0), "No spell named '%s' found", searchText);
			}
		}

		if (searchText2[0])
		{
			pSpell2 = GetSpellByName(searchText2);
			if (!pSpell2)
			{
				ImGui::TextColored(ImColor(255, 0, 0), "No spell named '%s' found", searchText2);
			}
		}

		if (!bCheckSpellBuffs && ImGui::Button("Swap"))
		{
			char temp[256];
			strcpy_s(temp, searchText);
			strcpy_s(searchText, searchText2);
			strcpy_s(searchText2, temp);
		}

		if (pSpell2)
		{
			SPAWNINFO* pPlayer = pLocalPlayer;
			PcClient* pPcClient = pPlayer->GetPcClient();

			EQ_Affect affect;
			affect.Type = 2;
			EQ_Affect* affectToPass = nullptr;
			if (pSpell)
			{
				affect.SpellID = pSpell->ID;
				affectToPass = &affect;
			}
			int slotIndex = -1;

			EQ_Affect* ret = pPcClient->FindAffectSlot(pSpell2->ID, pPlayer, &slotIndex,
				true, -1, affectToPass ? affectToPass : nullptr, affectToPass ? 1 : 0);

			if (ret)
			{
				if (pSpell)
				{
					ImGui::TextColored(ImColor(0, 255, 0), "%s stacks with %s", pSpell2->Name, pSpell->Name);
				}
				else
				{
					ImGui::TextColored(ImColor(0, 255, 0), "%s stacks", pSpell2->Name);
				}
			}
			else
			{
				if (pSpell)
				{
					ImGui::TextColored(ImColor(255, 0, 0), "%s doesn't stack with %s", pSpell2->Name, pSpell->Name);
				}
				else
				{
					ImGui::TextColored(ImColor(255, 0, 0), "%s doesn't stack", pSpell2->Name);
				}
			}
		}
	}

	virtual void Draw()
	{
		PcProfile* pcProfile = GetPcProfile();
		if (!pcProfile)
		{
			ImGui::TextColored(ImColor(255, 0, 0), "You must be in game to use this");
			return;
		}

		if (ImGui::BeginTabBar("##SpellTabs"))
		{
			int arrayLength = MAX_TOTAL_BUFFS;
			int count = 0;

			// calculate the size
			for (int i = 0; i < pcProfile->GetMaxEffects(); ++i)
			{
				if (pcProfile->GetEffect(i).SpellID > 0)
					count++;
			}

			char szLabel[64];

			if (pBuffWnd)
			{
				sprintf_s(szLabel, "Buffs (%d)###Buffs", pBuffWnd->GetTotalBuffCount());

				if (ImGui::BeginTabItem(szLabel))
				{
					DoBuffsTable("BuffsTable", pBuffWnd->GetBuffRange(), false, true, pBuffWnd->firstEffectSlot);

					ImGui::EndTabItem();
				}
			}

			if (pSongWnd)
			{
				sprintf_s(szLabel, "Short Buffs (%d)###ShortBuffs", pSongWnd->GetTotalBuffCount());

				if (ImGui::BeginTabItem(szLabel))
				{
					DoBuffsTable("ShortBuffsTable", pSongWnd->GetBuffRange(), false, true, pSongWnd->firstEffectSlot);

					ImGui::EndTabItem();
				}
			}

			if (pPetInfoWnd)
			{
				sprintf_s(szLabel, "Pet Buffs (%d)###PetBuffs", pPetInfoWnd->GetTotalBuffCount());

				if (ImGui::BeginTabItem(szLabel))
				{
					DoBuffsTable("PetBuffsTable", pPetInfoWnd->GetBuffRange(), true);

					ImGui::EndTabItem();
				}
			}

			if (pTargetWnd)
			{
				sprintf_s(szLabel, "Target Buffs (%d)###TargetBuffs", pTargetWnd->GetTotalBuffCount());

				if (ImGui::BeginTabItem(szLabel))
				{
					DoBuffsTable("TargetBuffsTable", pTargetWnd->GetBuffRange(), false);

					ImGui::EndTabItem();
				}
			}

			if (ImGui::BeginTabItem("Stacking Tests"))
			{
				DoSpellStackingTests();

				ImGui::EndTabItem();
			}

			sprintf_s(szLabel, "Spell Affects (%d)###SpellBuffs", count);

			if (ImGui::BeginTabItem(szLabel))
			{
				DoSpellAffectTable("SpellAffectBuffsTable", std::begin(pcProfile->Buffs), std::end(pcProfile->Buffs), arrayLength);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
};
static SpellsInspector* s_spellsInspector = nullptr;

#pragma endregion


/**
* @fn SaveColorToIni
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
	float values[4]{};
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
	s_ShowMainWindow = GetPrivateProfileBool("Settings", "ShowMainGui", true, &s_SettingsFile[0]);
	s_SplitTargetWindow = GetPrivateProfileBool("PlayerTarg", "SplitTarget", false, &s_SettingsFile[0]);
	s_ShowPlayerWindow = GetPrivateProfileBool("PlayerTarg", "ShowPlayerWindow", false, &s_SettingsFile[0]);
	s_ShowGroupWindow = GetPrivateProfileBool("Group", "ShowGroupWindow", false, &s_SettingsFile[0]);
	s_ShowSpellsWindow = GetPrivateProfileBool("Spells", "ShowSpellsWindow", false, &s_SettingsFile[0]);

	s_FlashInterval = GetPrivateProfileInt("PlayerTarg", "FlashInterval", 250, &s_SettingsFile[0]);
	s_FlashBuffInterval = GetPrivateProfileInt("PlayerTarg", "FlashBuffInterval", 250, &s_SettingsFile[0]);
	s_PlayerBarHeight = GetPrivateProfileInt("PlayerTarg", "PlayerBarHeight", 15, &s_SettingsFile[0]);
	s_TargetBarHeight = GetPrivateProfileInt("PlayerTarg", "TargetBarHeight", 15, &s_SettingsFile[0]);
	s_AggroBarHeight = GetPrivateProfileInt("PlayerTarg", "AggroBarHeight", 10, &s_SettingsFile[0]);


	//Color Settings
	s_MinColorHP = LoadColorFromIni("Colors", "s_MinColorHP", ImVec4(0.876f, 0.341f, 1.000f, 1.000f), &s_SettingsFile[0]);
	s_MaxColorHP = LoadColorFromIni("Colors", "s_MaxColorHP", ImVec4(0.845f, 0.151f, 0.151f, 1.000f), &s_SettingsFile[0]);
	s_MinColorEnd = LoadColorFromIni("Colors", "s_MinColorEnd", ImVec4(1.000f, 0.437f, 0.019f, 1.000f), &s_SettingsFile[0]);
	s_MaxColorEnd = LoadColorFromIni("Colors", "s_MaxColorEnd", ImVec4(0.7f, 0.6f, 0.1f, 0.7f), &s_SettingsFile[0]);
	s_MinColorMP = LoadColorFromIni("Colors", "s_MinColorMP", ImVec4(0.259f, 0.114f, 0.514f, 1.000f), &s_SettingsFile[0]);
	s_MaxColorMP = LoadColorFromIni("Colors", "s_MaxColorMP", ImVec4(0.079f, 0.468f, 0.848f, 1.000f), &s_SettingsFile[0]);
}


static void SaveSettings()
{
	//Window Settings
	WritePrivateProfileBool("Settings", "ShowMainGui", s_ShowMainWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "SplitTarget", s_SplitTargetWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("PlayerTarg", "ShowPlayerWindow", s_ShowPlayerWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Group", "ShowGroupWindow", s_ShowGroupWindow, &s_SettingsFile[0]);
	WritePrivateProfileBool("Spells", "ShowSpellsWindow", s_ShowSpellsWindow, &s_SettingsFile[0]);

	WritePrivateProfileInt("PlayerTarg", "FlashInterval", s_FlashInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "FlashBuffInterval", s_FlashBuffInterval, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "PlayerBarHeight", s_PlayerBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "TargetBarHeight", s_TargetBarHeight, &s_SettingsFile[0]);
	WritePrivateProfileInt("PlayerTarg", "AggroBarHeight", s_AggroBarHeight, &s_SettingsFile[0]);

	//Color Settings
	SaveColorToIni("Colors", "s_MinColorHP", s_MinColorHP, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "s_MaxColorHP", s_MaxColorHP, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "s_MinColorEnd", s_MinColorEnd, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "s_MaxColorEnd", s_MaxColorEnd, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "s_MinColorMP", s_MinColorMP, &s_SettingsFile[0]);
	SaveColorToIni("Colors", "s_MaxColorMP", s_MaxColorMP, &s_SettingsFile[0]);
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
			static float tarPercentage = static_cast<float>(CurTarget->HPCurrent) / 100;
			static int tar_label = CurTarget->HPCurrent;
			static ImVec4 colorTarHP = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, CurTarget->HPCurrent);

			if (CurTarget->DisplayedName == pLocalPC->Name)
			{
				static float healthPctFloat = static_cast<float>(GetCurHPS()) / GetMaxHPS();
				static int healthPctInt = static_cast<int>(healthPctFloat * 100);
				tarPercentage = static_cast<float>(GetCurHPS()) / GetMaxHPS();
				tar_label = healthPctInt;
				colorTarHP = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, healthPctInt);
			}
			if (LineOfSight(pLocalPlayer, pTarget))
			{
				ImGui::TextColored(ColorToVec("green"),ICON_MD_VISIBILITY);
			}
			else
			{
				ImGui::TextColored(ColorToVec("red"), ICON_MD_VISIBILITY_OFF);
			}
			ImGui::SameLine();
			ImGui::Text(CurTarget->DisplayedName);

			ImGui::SameLine(sizeX * .75);
			ImGui::TextColored(ColorToVec("tangarine"), "%0.1f m", GetDistance(pLocalPlayer, pTarget));

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(tarPercentage, ImVec2(0.0f, s_TargetBarHeight), "##");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX - 8), yPos));
			ImGui::Text("%d %%", tar_label);
			ImGui::NewLine();
			ImGui::SameLine();
			ImGui::TextColored(ColorToVec("teal"), "Lvl %d", CurTarget->Level);

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
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ColorToVec("orange"));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ColorToVec("purple"));
			}
			ImGui::SetNextItemWidth(static_cast<int>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(static_cast<float>(s_myAgroPct) / 100, ImVec2(0.0f, s_AggroBarHeight), "##Aggro");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(10, yPos));
			ImGui::Text("%s", s_secondAgroName.c_str());
			ImGui::SetCursorPos(ImVec2((sizeX/2)-8, yPos));
			ImGui::Text("%d %%", s_myAgroPct);
			ImGui::SetCursorPos(ImVec2(sizeX - 40, yPos));
			ImGui::Text("%d %%", s_secondAgroPct);	

			if (gTargetbuffs)
			{
				//GetCachedBuffAtSlot(pTarget, 0);
				//ImGui::Text("%s", buff);
				s_spellsInspector->DoBuffsTable("TargetBuffsTable", pTargetWnd->GetBuffRange(), false);
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
					ImGui::TextColored(ColorToVec("Yellow"), s_heading.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("Lvl: %d", pLocalPC->GetLevel());
					ImGui::EndTable();
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor();
			static float healthPctFloat = static_cast<float>(GetCurHPS()) / GetMaxHPS();
			static int healthPctInt = static_cast<int>(healthPctFloat * 100);

			static ImVec4 colorHP = CalculateProgressiveColor(s_MinColorHP, s_MaxColorHP, healthPctInt);
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
				static float manaPctFloat = static_cast<float>(GetCurMana()) / GetMaxMana();
				static int manaPctInt = static_cast<int>(manaPctFloat * 100);
				static ImVec4 colorMP = CalculateProgressiveColor(s_MinColorMP, s_MaxColorMP, manaPctInt);

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

			static float endurPctFloat = static_cast<float>(GetCurEndurance()) / GetMaxEndurance();
			static int endurPctInt = static_cast<int>(endurPctFloat * 100);
			static ImVec4 colorEP = CalculateProgressiveColor(s_MinColorEnd, s_MaxColorEnd, endurPctInt);

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

				ImGui::ColorEdit4("Min HP Color", (float*)&s_MinColorHP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Minimum HP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Max HP Color", (float*)&s_MaxColorHP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Maximum HP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Min MP Color", (float*)&s_MinColorMP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Minimum MP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Max MP Color", (float*)&s_MaxColorMP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Maximum MP Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Min End Color", (float*)&s_MinColorEnd, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				DrawHelpIcon("Minimum Endurance Color");

				ImGui::TableNextColumn();

				ImGui::ColorEdit4("Max End Color", (float*)&s_MaxColorEnd, ImGuiColorEditFlags_NoInputs);
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

				SaveColorToIni("Colors", "s_MinColorHP", s_MinColorHP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "s_MaxColorHP", s_MaxColorHP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "s_MinColorMP", s_MinColorMP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "s_MaxColorMP", s_MaxColorMP, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "s_MinColorEnd", s_MinColorEnd, &s_SettingsFile[0]);
				SaveColorToIni("Colors", "s_MaxColorEnd", s_MaxColorEnd, &s_SettingsFile[0]);
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
	// Update Settings Toggle when Window State Changes

	// Main Window no state check needed 
	if (s_ShowMainWindow)
	{
		DrawMainWindow();
		
		if (!s_ShowMainWindow)
		{
			WritePrivateProfileBool("Settings", "ShowMainGui", s_ShowMainWindow, &s_SettingsFile[0]);
		}
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

		if (s_ShowConfigWindow)
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
	s_spellsInspector = new SpellsInspector();

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
