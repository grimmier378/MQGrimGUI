#pragma once
#include <mq/Plugin.h>
#include "mq/base/Color.h"
#include <mq/imgui/Widgets.h>
#include "eqlib/Spells.h"
#include "main/MQ2SpellSearch.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include "SpellPicker.h"

char s_SettingsFile[MAX_PATH] = { 0 };
const char* s_SecondAggroName = "Unknown";
const char* s_CurrHeading = "N";
const char* HELP_TEXT_MIN = "Color you progress down to from Max";
const char* HELP_TEXT_MAX = "Color you progress up to from Min";

SpellPicker* pSpellPicker = nullptr;

bool s_ShowOutOfGame = false;
bool s_MemSpell = false;
bool s_DanNetEnabled = false;
bool s_ShowSpellBook = false;
bool s_SettingModified = false;
std::string s_MemSpellName;

int s_MemGemIndex = 0;
int s_TarBuffLineSize = 0;
int s_TestInt = 100; // Color Test Value for Config Window

void SaveSettings();

ImGuiWindowFlags s_WindowFlags = ImGuiWindowFlags_None;
ImGuiWindowFlags s_LockAllWin = ImGuiWindowFlags_None;
ImGuiChildFlags s_ChildFlags = ImGuiChildFlags_None;

CTextureAnimation* m_pTASpellIcon    = nullptr;
CTextureAnimation* m_pGemHolder      = nullptr;
CTextureAnimation* m_pGemBackground  = nullptr;
CTextureAnimation* m_pMainTankIcon   = nullptr;
CTextureAnimation* m_pPullerIcon     = nullptr;
CTextureAnimation* m_pMainAssistIcon = nullptr;
CTextureAnimation* m_pCombatIcon     = nullptr;
CTextureAnimation* m_pDebuffIcon     = nullptr;
CTextureAnimation* m_pRegenIcon      = nullptr;
CTextureAnimation* m_pStandingIcon   = nullptr;
CTextureAnimation* m_pTimerIcon      = nullptr;
CTextureAnimation* m_StatusIcon      = nullptr;

#pragma region Spells Inspector

namespace grimgui {
	// lifted from developer tools to inspect spells

	class SpellsInspector
	{

	public:

		// this is from MQ2SpellType.cpp 
		static void DoInspectSpell(int spellId)
		{
			EQ_Spell* spell = GetSpellByID(spellId);
			if (spell)
			{
#if defined(CSpellDisplayManager__ShowSpell_x)
				if (pSpellDisplayManager)
					pSpellDisplayManager->ShowSpell(spell->ID, true, true, SpellDisplayType_SpellBookWnd);
#else
				char buffer[512] = { 0 };
				FormatSpellLink(buffer, 512, spell);
				TextTagInfo info = ExtractLink(buffer);
				ExecuteTextLink(info);
#endif
			}
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
		void DrawBuffsList(const char* name, IteratorRange<PlayerBuffInfoWrapper::Iterator<T>> Buffs,
			bool petBuffs = false, bool playerBuffs = false, int baseIndex = 0)
		{
			ImGui::SetWindowFontScale(s_FontScaleSettings.buffsWinScale);
			float sizeY = ImGui::GetContentRegionAvail().y;
			sizeY = sizeY - 10 > 0 ? sizeY - 10 : 1;

			if (ImGui::BeginTable("Buffs", 3, ImGuiTableFlags_Hideable |
				ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable |
				ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetContentRegionAvail().x, sizeY)))
			{
				int slotNum = 0;
				ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, static_cast<float>(s_NumSettings.buffIconSize));
				ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 65);
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();
				for (const auto& buffInfo : Buffs)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					EQ_Spell* spell = buffInfo.GetSpell();
					if (!spell)
					{
						slotNum++;
						continue;
					}

					if (!m_pTASpellIcon)
					{
						m_pTASpellIcon = new CTextureAnimation();
						if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
							*m_pTASpellIcon = *temp;
					}

					if (spell)
					{
						m_pTASpellIcon->SetCurCell(spell->SpellIcon);
						MQColor borderCol = MQColor(0, 0, 250, 255); // Default color blue (beneficial)
						MQColor tintCol = MQColor(255, 255, 255, 255);
						if (!spell->IsBeneficialSpell())
							borderCol = MQColor(250, 0, 0, 255); // Red for detrimental spells

						if (!playerBuffs)
						{
							std::string caster = buffInfo.GetCaster();
							if (caster == pLocalPC->Name && !spell->IsBeneficialSpell())
								borderCol = MQColor(250, 250, 0, 255); // Yellow for spells cast by me
						}

						int secondsLeft = buffInfo.GetBuffTimer() / 1000;
						if (secondsLeft < 18 && secondsLeft > 0 && !petBuffs)
						{
							if (s_WinSettings.flashTintFlag)
								tintCol = MQColor(0, 0, 0, 255);

						}
						ImGui::PushID(buffInfo.GetIndex());
						imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(s_NumSettings.buffIconSize, s_NumSettings.buffIconSize), tintCol, borderCol);
						ImGui::PopID();

						if (ImGui::BeginPopupContextItem(("BuffPopup##" + std::to_string(spell->ID)).c_str()))
						{
							ImGui::SetWindowFontScale(s_FontScaleSettings.buffsWinScale);

							if (ImGui::MenuItem(("Remove##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
								RemoveBuffByName(spell->Name);

							if (ImGui::MenuItem(("Block##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
								EzCommand(("/blockspell add me " + std::to_string(spell->ID)).c_str());

							if (ImGui::MenuItem("Inspect##", nullptr, false, true))
								DoInspectSpell(spell->ID);

							ImGui::SetWindowFontScale(1.0f);
							ImGui::EndPopup();
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							if (spell)
							{
								char timeLabel[64];
								FormatBuffDuration(timeLabel, 64, buffInfo.GetBuffTimer());
								ImGui::Text("%s (%s)", spell->Name, timeLabel);
								if (!petBuffs)
									ImGui::Text("Caster: %s", buffInfo.GetCaster());
							}
							ImGui::EndTooltip();
						}

						ImGui::TableNextColumn();
						if (secondsLeft < s_NumSettings.buffTimerThreshold || s_NumSettings.buffTimerThreshold == 0)
						{
							char timeLabel[64];
							FormatBuffDuration(timeLabel, 64, buffInfo.GetBuffTimer());
							ImGui::TextColored(GetMQColor(ColorName::Tangerine).ToImColor(), "%s", timeLabel);
						}
						ImGui::TableNextColumn();

						ImGui::Text("%s", spell->Name);

					}
					slotNum++;
				}
				ImGui::EndTable();
				ImGui::SetWindowFontScale(1.0f);
			}
		}

		template <typename T>
		void DrawBuffsIcons(const char* name, IteratorRange<PlayerBuffInfoWrapper::Iterator<T>> Buffs,
			bool petBuffs = false, bool playerBuffs = false, int baseIndex = 0)
		{
			int Index = -1;
			for (const auto& buffInfo : Buffs)
			{
				EQ_Spell* spell = buffInfo.GetSpell();
				if (!spell)
					continue;

				Index++;
				ImGui::PushID(buffInfo.GetIndex());

				if (!m_pTASpellIcon)
				{
					m_pTASpellIcon = new CTextureAnimation();
					if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
						*m_pTASpellIcon = *temp;
				}

				int sizeX = static_cast<int>(ImGui::GetContentRegionAvail().x);
				s_TarBuffLineSize = 0;
				if (spell)
				{
					m_pTASpellIcon->SetCurCell(spell->SpellIcon);
					MQColor borderCol = MQColor(0, 0, 250, 255);
					MQColor tintCol = MQColor(255, 255, 255, 255);
					if (!spell->IsBeneficialSpell())
						borderCol = MQColor(250, 0, 0, 255);

					std::string caster = buffInfo.GetCaster();
					if (caster == pLocalPC->Name && !spell->IsBeneficialSpell())
						borderCol = MQColor(250, 250, 0, 255);

					int secondsLeft = buffInfo.GetBuffTimer() / 1000;
					if (secondsLeft < 18 && secondsLeft > 0 && !petBuffs)
					{
						if (s_WinSettings.flashTintFlag)
							tintCol = MQColor(0, 0, 0, 255);
					}

					imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(s_NumSettings.buffIconSize, s_NumSettings.buffIconSize), tintCol, borderCol);
					s_TarBuffLineSize += s_NumSettings.buffIconSize + 2;
					if (s_TarBuffLineSize < sizeX - 20)
						ImGui::SameLine(0.0f, 2);
					else
						s_TarBuffLineSize = 0;

				}
				ImGui::PopID();
				if (ImGui::BeginPopupContextItem(("BuffPopup##" + std::to_string(spell->ID)).c_str()))
				{

					if (ImGui::MenuItem("Inspect##", nullptr, false, true))
						DoInspectSpell(spell->ID);

					ImGui::EndPopup();
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					if (spell)
					{
						char timeLabel[64];
						FormatBuffDuration(timeLabel, 64, buffInfo.GetBuffTimer());
						ImGui::Text("%s (%s)", spell->Name, timeLabel);
						if (!petBuffs)
							ImGui::Text("Caster: %s", buffInfo.GetCaster());

					}
					ImGui::EndTooltip();
				}
			}
		}

		// Spell Bar

		bool IsGemReady(int ID)
		{
			if (GetSpellByID(GetMemorizedSpell(ID)))
			{
				if (pDisplay->TimeStamp > pLocalPlayer->SpellGemETA[ID] && pDisplay->TimeStamp > pLocalPlayer->GetSpellCooldownETA())
					return true;
			}
			return false;
		}

	};
}

grimgui::SpellsInspector* pSpellInspector = nullptr;

#pragma endregion

#pragma region Window Visibility Settings

struct WinSettings
{
	bool showMainWindow			= true;
	bool showConfigWindow		= false;
	bool showPetWindow			= false;
	bool showPlayerWindow		= false;
	bool showGroupWindow		= false;
	bool showSpellsWindow		= false;
	bool showTargetWindow		= false;
	bool showHud				= false;
	bool showBuffWindow			= false;
	bool showSongWindow			= false;
	bool flashCombatFlag		= false;
	bool flashTintFlag			= false;
	bool showTitleBars			= true;
	bool lockAllWin			= false;
	bool lockPlayerWin = false;
	bool lockTargetWin = false;
	bool lockPetWin = false;
	bool lockGroupWin = false;
	bool lockSpellsWin = false;
	bool lockBuffsWin = false;
	bool lockSongWin = false;
	bool lockHudWin = false;
	bool hudClickThrough		= false;
	bool showPetButtons			= true;
	bool showTargetBuffs		= true;
	bool showAggroMeter			= true;
	bool showSelfOnGroup		= false;
	bool showEmptyGroupSlot		= true;
} s_WinSettings;

struct WinSetting
{
	const char* section;
	const char* key;
	bool* setting;
};

std::vector<WinSetting> winSettings = {
	{"Settings",	"ShowMainGui",			&s_WinSettings.showMainWindow},
	{"Settings",	"ShowTitleBars",		&s_WinSettings.showTitleBars},
	{"LockWindows",	"LockAllWindows",		&s_WinSettings.lockAllWin},
	{"LockWindows",	"LockPlayerWin",		&s_WinSettings.lockPlayerWin},
	{"LockWindows",	"LockTargetWin",		&s_WinSettings.lockTargetWin},
	{"LockWindows",	"LockPetWin",			&s_WinSettings.lockPetWin},
	{"LockWindows",	"LockGroupWin",			&s_WinSettings.lockGroupWin},
	{"LockWindows",	"LockSpellsWin",		&s_WinSettings.lockSpellsWin},
	{"LockWindows",	"LockBuffsWin",			&s_WinSettings.lockBuffsWin},
	{"LockWindows",	"LockSongWin",			&s_WinSettings.lockSongWin},
	{"LockWindows",	"LockHudWin",			&s_WinSettings.lockHudWin},
	{"PlayerTarg",	"SplitTarget",			&s_WinSettings.showTargetWindow},
	{"PlayerTarg",	"ShowPlayerWindow",		&s_WinSettings.showPlayerWindow},
	{"PlayerTarg",	"ShowTargetBuffs",		&s_WinSettings.showTargetBuffs},
	{"PlayerTarg",	"ShowAggroMeter",		&s_WinSettings.showAggroMeter},
	{"Pet",			"ShowPetButtons",		&s_WinSettings.showPetButtons},
	{"Pet",			"ShowPetWindow",		&s_WinSettings.showPetWindow},
	{"Group",		"ShowGroupWindow",		&s_WinSettings.showGroupWindow},
	{"Group",		"ShowSelfOnGroup",		&s_WinSettings.showSelfOnGroup},
	{"Group",		"ShowEmptyGroup",		&s_WinSettings.showEmptyGroupSlot},
	{"Songs",		"ShowSongWindow",		&s_WinSettings.showSongWindow},
	{"Spells",		"ShowSpellsWindow",		&s_WinSettings.showSpellsWindow},
	{"Buffs",		"ShowBuffWindow",		&s_WinSettings.showBuffWindow},
	{"Hud",			"ShowHud",				&s_WinSettings.showHud},
	{"Hud",			"HudClickThrough",		&s_WinSettings.hudClickThrough},
};

#pragma endregion

#pragma region Numeric Settings

struct NumericSettings
{
	int combatFlashInterval			= 15;
	int flashBuffInterval			= 4;
	int playerBarHeight				= 12;
	int targetBarHeight				= 12;
	int aggroBarHeight				= 12;
	int groupBarHeight				= 12;
	int myAggroPct					= 0;
	int secondAggroPct				= 0;
	int buffIconSize				= 20;
	int buffTimerThreshold			= 0;
	int spellGemHeight				= 32;
	int hudAlpha					= 120;
} s_NumSettings;

struct NumericSetting
{
	const char* section;
	const char* key;
	int* value;
};

std::vector<NumericSetting> numericSettings = {
	{"PlayerTarg",		"CombatFlashInterval",		&s_NumSettings.combatFlashInterval},
	{"PlayerTarg",		"PlayerBarHeight",			&s_NumSettings.playerBarHeight},
	{"PlayerTarg",		"TargetBarHeight",			&s_NumSettings.targetBarHeight},
	{"PlayerTarg",		"AggroBarHeight",			&s_NumSettings.aggroBarHeight},
	{"Settings",		"BuffIconSize",				&s_NumSettings.buffIconSize},
	{"Settings",		"FlashBuffInterval",		&s_NumSettings.flashBuffInterval},
	{"Buffs",			"BuffTimerThreshold",		&s_NumSettings.buffTimerThreshold},
	{"Group",			"GroupBarHeight",			&s_NumSettings.groupBarHeight},
	{"Spells",			"SpellGemHeight",			&s_NumSettings.spellGemHeight},
	{"Hud",				"HudAlpha",					&s_NumSettings.hudAlpha}
};

#pragma endregion

#pragma region Font Scale Settings

struct FontScaleSettings
{
	float playerWinScale = 1.0f;
	float targetWinScale = 1.0f;
	float petWinScale    = 1.0f;
	float groupWinScale  = 1.0f;
	float spellsWinScale = 1.0f;
	float buffsWinScale  = 1.0f;

} s_FontScaleSettings;

struct FontScaleSetting
{
	const char* section;
	const char* key;
	float* value;
	const char* helpText;
};

std::vector<FontScaleSetting> fontScaleSettings = {
	{"PlayerTarg", "PlayerWinScale", &s_FontScaleSettings.playerWinScale, "Player Window Scale"},
	{"PlayerTarg", "TargetWinScale", &s_FontScaleSettings.targetWinScale, "Target Window Scale"},
	{"Pet", "PetWinScale", &s_FontScaleSettings.petWinScale, "Pet Window Scale"},
	{"Group", "GroupWinScale", &s_FontScaleSettings.groupWinScale, "Group Window Scale"},
	{"Spells", "SpellsWinScale", &s_FontScaleSettings.spellsWinScale, "Spells Window Scale"},
	{"Buffs", "BuffsWinScale", &s_FontScaleSettings.buffsWinScale, "Buffs Window Scale"},
};

#pragma endregion

#pragma region Theme and Color Settings

struct ThemeSettings
{
	std::string playerWinTheme			= "Default";
	std::string petWinTheme				= "Default";
	std::string groupWinTheme			= "Default";
	std::string spellsWinTheme			= "Default";
	std::string buffsWinTheme			= "Default";
	std::string songWinTheme			= "Default";
} s_WinTheme;

struct ThemeSetting
{
	const char* section;
	const char* key;
	std::string* theme;
};

std::vector<ThemeSetting> themeSettings = {
	{"PlayerTarg",	"Theme",	&s_WinTheme.playerWinTheme},
	{"Pet",			"Theme",	&s_WinTheme.petWinTheme},
	{"Group",		"Theme",	&s_WinTheme.groupWinTheme},
	{"Spells",		"Theme",	&s_WinTheme.spellsWinTheme},
	{"Buffs",		"Theme",	&s_WinTheme.buffsWinTheme},
	{"Songs",		"Theme",	&s_WinTheme.songWinTheme}
};


struct ColorSettings
{
	mq::MQColor minColorHP		= mq::MQColor(223, 87, 255, 255);
	mq::MQColor maxColorHP		= mq::MQColor(216, 39, 39, 255);
	mq::MQColor minColorMP		= mq::MQColor(66, 29, 131, 255);
	mq::MQColor maxColorMP		= mq::MQColor(20, 119, 216, 255);
	mq::MQColor minColorEnd		= mq::MQColor(255, 111, 5, 255);
	mq::MQColor maxColorEnd		= mq::MQColor(178, 153, 26, 178);
	mq::MQColor minColorCast	= mq::MQColor(216, 39, 39, 255);
	mq::MQColor maxColorCast	= mq::MQColor(20, 119, 216, 255);
} s_BarColors;

struct ColorSetting
{
	const char* section;
	const char* key;
	mq::MQColor* value;
	const char* label;
	const char* helpText;
};

std::vector<ColorSetting> colorSettings = {
	{"Colors",	 "MinColorHP",		&s_BarColors.minColorHP, "Min HP Color", HELP_TEXT_MIN},
	{"Colors",	 "MaxColorHP",		&s_BarColors.maxColorHP, "Max HP Color", HELP_TEXT_MAX},
	{"Colors",	 "MinColorMP",		&s_BarColors.minColorMP, "Min MP Color", HELP_TEXT_MIN},
	{"Colors",	 "MaxColorMP",		&s_BarColors.maxColorMP, "Max MP Color", HELP_TEXT_MAX},
	{"Colors",	 "MinColorEnd",		&s_BarColors.minColorEnd, "Min End Color", HELP_TEXT_MIN},
	{"Colors",	 "MaxColorEnd",		&s_BarColors.maxColorEnd, "Max End Color", HELP_TEXT_MAX},
	{"Colors",	 "MinColorCast",	&s_BarColors.minColorCast, "Min Cast Color", HELP_TEXT_MIN},
	{"Colors",	 "MaxColorCast",	&s_BarColors.maxColorCast, "Max Cast Color", HELP_TEXT_MAX}
};

#pragma endregion

#pragma region Color utility functions

/**
* @fn CalculateProgressiveColor
*
* @brief Function to calculate a color between two colors based on a value between 0 and 100
*
* @param minColor const MQColor& Minimum color value
* @param maxColor const MQColor& Maximum color value
* @param value int Value between 0 and 100 to calculate the color between minColor and maxColor
* @param midColor const MQColor* Optional midColor value to calculate in two segments
* @param midValue int Optional midValue to split the value between minColor and maxColor
*
* @return ImVec4 color value
*/
ImVec4 CalculateProgressiveColor(const MQColor& minColor, const MQColor& maxColor, int value, const MQColor* midColor = nullptr, int midValue = 50)
{
	value = std::max(0, std::min(100, value));

	float r, g, b, a;

	auto toFloat = [](uint8_t colorComponent) { return static_cast<float>(colorComponent) / 255.0f; };

	if (midColor)
	{
		// If midColor is provided, calculate in two segments
		if (value > midValue)
		{
			float proportion = static_cast<float>(value - midValue) / (100 - midValue);
			r = toFloat(midColor->Red) + proportion * (toFloat(maxColor.Red) - toFloat(midColor->Red));
			g = toFloat(midColor->Green) + proportion * (toFloat(maxColor.Green) - toFloat(midColor->Green));
			b = toFloat(midColor->Blue) + proportion * (toFloat(maxColor.Blue) - toFloat(midColor->Blue));
			a = toFloat(midColor->Alpha) + proportion * (toFloat(maxColor.Alpha) - toFloat(midColor->Alpha));
		}
		else
		{
			float proportion = static_cast<float>(value) / midValue;
			r = toFloat(minColor.Red) + proportion * (toFloat(midColor->Red) - toFloat(minColor.Red));
			g = toFloat(minColor.Green) + proportion * (toFloat(midColor->Green) - toFloat(minColor.Green));
			b = toFloat(minColor.Blue) + proportion * (toFloat(midColor->Blue) - toFloat(minColor.Blue));
			a = toFloat(minColor.Alpha) + proportion * (toFloat(midColor->Alpha) - toFloat(minColor.Alpha));
		}
	}
	else
	{
		// Calculate between minColor and maxColor
		float proportion = static_cast<float>(value) / 100;
		r = toFloat(minColor.Red) + proportion * (toFloat(maxColor.Red) - toFloat(minColor.Red));
		g = toFloat(minColor.Green) + proportion * (toFloat(maxColor.Green) - toFloat(minColor.Green));
		b = toFloat(minColor.Blue) + proportion * (toFloat(maxColor.Blue) - toFloat(minColor.Blue));
		a = toFloat(minColor.Alpha) + proportion * (toFloat(maxColor.Alpha) - toFloat(minColor.Alpha));
	}

	return ImVec4(r, g, b, a);
}

enum class ColorName {
	Red, Pink2, Pink, Orange, Tangerine, Yellow, Yellow2, White,
	Blue, SoftBlue, LightBlue2, LightBlue, Teal, Green, Green2,
	Grey, Purple, Purple2, BtnRed, BtnGreen, DefaultWhite
};

constexpr MQColor COLOR_RED(230, 26, 26, 255);
constexpr MQColor COLOR_PINK2(249, 132, 215, 255);
constexpr MQColor COLOR_PINK(230, 102, 102, 204);
constexpr MQColor COLOR_ORANGE(199, 51, 13, 204);
constexpr MQColor COLOR_TANGERINE(255, 142, 0, 255);
constexpr MQColor COLOR_YELLOW(255, 255, 0, 255);
constexpr MQColor COLOR_YELLOW2(178, 153, 26, 178);
constexpr MQColor COLOR_WHITE(255, 255, 255, 255);
constexpr MQColor COLOR_BLUE(0, 0, 255, 255);
constexpr MQColor COLOR_SOFT_BLUE(94, 180, 255);
constexpr MQColor COLOR_LIGHT_BLUE2(51, 230, 230, 128);
constexpr MQColor COLOR_LIGHT_BLUE(0, 255, 255, 255);
constexpr MQColor COLOR_TEAL(0, 255, 255, 255);
constexpr MQColor COLOR_GREEN(0, 255, 0, 255);
constexpr MQColor COLOR_GREEN2(3, 143, 0, 255);
constexpr MQColor COLOR_GREY(153, 153, 153, 255);
constexpr MQColor COLOR_PURPLE1(204, 0, 255, 255);
constexpr MQColor COLOR_PURPLE2(118, 52, 255, 255);
constexpr MQColor COLOR_BTN_RED(255, 102, 102, 102);
constexpr MQColor COLOR_BTN_GREEN(102, 255, 102, 102);
constexpr MQColor COLOR_DEFAULT_WHITE(255, 255, 255, 255);

constexpr MQColor GetMQColor(ColorName color)
{
	switch (color)
	{
	case ColorName::Red:          return COLOR_RED;
	case ColorName::Pink2:        return COLOR_PINK2;
	case ColorName::Pink:         return COLOR_PINK;
	case ColorName::Orange:       return COLOR_ORANGE;
	case ColorName::Tangerine:    return COLOR_TANGERINE;
	case ColorName::Yellow:       return COLOR_YELLOW;
	case ColorName::Yellow2:      return COLOR_YELLOW2;
	case ColorName::White:        return COLOR_WHITE;
	case ColorName::Blue:         return COLOR_BLUE;
	case ColorName::SoftBlue:     return COLOR_SOFT_BLUE;
	case ColorName::LightBlue2:   return COLOR_LIGHT_BLUE2;
	case ColorName::LightBlue:    return COLOR_LIGHT_BLUE;
	case ColorName::Teal:         return COLOR_TEAL;
	case ColorName::Green:        return COLOR_GREEN;
	case ColorName::Green2:       return COLOR_GREEN2;
	case ColorName::Grey:         return COLOR_GREY;
	case ColorName::Purple:       return COLOR_PURPLE1;
	case ColorName::Purple2:      return COLOR_PURPLE2;
	case ColorName::BtnRed:       return COLOR_BTN_RED;
	case ColorName::BtnGreen:     return COLOR_BTN_GREEN;
	default:                      return COLOR_DEFAULT_WHITE;
	}
}

MQColor GetConColor(int color_code)
{
	switch (color_code)
	{
	case 0x06: return COLOR_GREY;          // CONCOLOR_GREY
	case 0x02: return COLOR_GREEN;         // CONCOLOR_GREEN
	case 0x12: return COLOR_SOFT_BLUE;     // CONCOLOR_LIGHTBLUE
	case 0x04: return COLOR_BLUE;          // CONCOLOR_BLUE
	case 0x14: return COLOR_DEFAULT_WHITE; // CONCOLOR_BLACK (or default to white for transparency)
	case 0x0a: return COLOR_WHITE;         // CONCOLOR_WHITE
	case 0x0f: return COLOR_YELLOW;        // CONCOLOR_YELLOW
	case 0x0d: return COLOR_RED;           // CONCOLOR_RED

		// Default color if the color code doesn't match any known values
	default: return COLOR_DEFAULT_WHITE;
	}
}

#pragma endregion

#pragma region Command Definitions

enum class GrimCommand
{
	Show,
	Lock,
	Player,
	Target,
	Pet,
	Group,
	Spells,
	Buffs,
	Songs,
	Hud,
	ClickThrough,
	Config,
	Help
};

struct CommandInfo
{
	GrimCommand command;
	const char* commandText;
	const char* description;
};

const std::array<CommandInfo, 13> commandList = {
	{
		{GrimCommand::Show,			"show",			"Toggles Main Window"},
		{GrimCommand::Lock,			"lock",			"Toggles Lock All Windows"},
		{GrimCommand::Player,		"player",		"Toggles Player Window"},
		{GrimCommand::Target,		"target",		"Toggles Target Window"},
		{GrimCommand::Pet,			"pet",			"Toggles Pet Window"},
		{GrimCommand::Group,		"group",		"Toggles Group Window"},
		{GrimCommand::Spells,		"spells",		"Toggles Spells Window"},
		{GrimCommand::Buffs,		"buffs",		"Toggles Buffs Window"},
		{GrimCommand::Songs,		"songs",		"Toggles Songs Window"},
		{GrimCommand::Hud,			"hud",			"Toggles Hud Window"},
		{GrimCommand::Config,		"config",		"Opens Configuration Window"},
		{GrimCommand::ClickThrough, "clickthrough", "Toggles Hud Click Through"},
		{GrimCommand::Help,			"help",			"Displays this help message"}
	}
};

#pragma endregion

#pragma region Pet Buttons

struct PetButtonData
{
	std::string name;
	std::string command;
	bool visible;
};

static std::vector<PetButtonData> petButtons = {
	{"Attack",		"/pet attack",			true},
	{"Sit",			"/pet sit",				true},
	{"Follow",		"/pet follow",			true},
	{"Hold",		"/pet hold",			true},
	{"Taunt",		"/pet taunt",			true},
	{"Guard",		"/pet guard",			true},
	{"Back",		"/pet back off",		true},
	{"Focus",		"/pet focus",			true},
	{"Stop",		"/pet stop",			true},
	{"Leave",		"/pet get lost",		true},
	{"Regroup",		"/pet regroup",			true},
	{"Report",		"/pet report health",	true},
	{"Swarm",		"/pet swarm",			true},
	{"Kill",		"/pet kill",			true},
};

#pragma endregion

#pragma region Config Structs
struct WindowOption
{
	const char* label;
	bool* setting;
	const char* section;
	const char* key;
};

std::vector<WindowOption> options = {
	{"Player Win",	&s_WinSettings.showPlayerWindow,	"PlayerTarg",	"ShowPlayerWindow"},
	{"Target Win",	&s_WinSettings.showTargetWindow,	"PlayerTarg",	"SplitTarget"},
	{"Pet Win",		&s_WinSettings.showPetWindow,		"Pet",			"ShowPetWindow"},
	{"Spells Win",	&s_WinSettings.showSpellsWindow,	"Spells",		"ShowSpellsWindow"},
	{"Buff Win",	&s_WinSettings.showBuffWindow,		"Buffs",		"ShowBuffWindow"},
	{"Song Win",	&s_WinSettings.showSongWindow,		"Songs",		"ShowSongWindow"},
	{"Group Win",	&s_WinSettings.showGroupWindow,		"Group",		"ShowGroupWindow"},
	{"Hud Win",		&s_WinSettings.showHud,				"Hud",			"ShowHud"},
};

struct SliderOption
{
	const char* label;
	int* value;
	int min;
	int max;
	const bool clamp;
	const char* helpText;
};

std::vector <SliderOption> sliderOptions = {
	{"Flash Speed",				&s_NumSettings.combatFlashInterval, 0, 50,		true,	"Flash Speed : Lower is slower, Higher is faster. 0 = Disabled"},
	{"Buff Flash Speed",		&s_NumSettings.flashBuffInterval,	0, 50,		true,	"Buff Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
	{"Buff Icon Size",			&s_NumSettings.buffIconSize,		15, 40,		false,	"Buff Icon Size"},
	{"Buff Timer Threshold",	&s_NumSettings.buffTimerThreshold,	0, 3601,	false,	"Buff Show Timer Threshold in Seconds (0 = Always Show)"},
	{"Player Bar Height",		&s_NumSettings.playerBarHeight,		8, 40,		true,	"Player Bar Height"},
	{"Target Bar Height",		&s_NumSettings.targetBarHeight,		8, 40,		true,	"Target Bar Height"},
	{"Aggro Bar Height",		&s_NumSettings.aggroBarHeight,		8, 40,		true,	"Aggro Bar Height"},
	{"Group Bar Height",		&s_NumSettings.groupBarHeight,		8, 40,		true,	"Group Bar Height"},
	{"Spell Gem Height",		&s_NumSettings.spellGemHeight,		15, 60,		false,	"Spell Gem Height"},
	{"Hud Alpha",				&s_NumSettings.hudAlpha,			0, 255,		true,	"Hud Transparency (Alpha Level)"},
};

struct SettingToggleOption
{
	const char* label;
	bool* setting;
	const bool lockSetting;
	const char* helpText;

};

std::vector <SettingToggleOption> settingToggleOptions = {
	{"Title Bars",			&s_WinSettings.showTitleBars,		false,	"Title Bars: Show or Hide the title bars on each window"},
	{"Hud Click Through",	&s_WinSettings.hudClickThrough,		false,	"Hud Click Through: Allows clicking through the hud window"},
	{"Pet Buttons",			&s_WinSettings.showPetButtons,		false,	"Pet Buttons: Show or Hide the pet command buttons"},
	{"Target Buffs",		&s_WinSettings.showTargetBuffs,		false,	"Target Buffs: Show or Hide the target buffs"},
	{"Aggro Meter",			&s_WinSettings.showAggroMeter,		false,	"Aggro Meter: Show or Hide the aggro meter"},
	{"Group Show Self",		&s_WinSettings.showSelfOnGroup,		false,	"Group Show Self: Show or Hide Yourself on the group window"},
	{"Group Show Empty",	&s_WinSettings.showEmptyGroupSlot,	false,	"Group Show Empty: Show or Hide Empty Group Slots"},
	{"Lock ALL",			&s_WinSettings.lockAllWin,			true,	"Lock Windows: Locks All Windows in place"},
	{"Lock Group",			&s_WinSettings.lockGroupWin,		true,	"Lock Group: Toggle locking the Group Window"},
	{"Lock Player",			&s_WinSettings.lockPlayerWin,		true,	"Lock Player: Toggle locking the Player Window"},
	{"Lock Target",			&s_WinSettings.lockTargetWin,		true,	"Lock Target: Toggle locking the Target Window"},
	{"Lock Pet",			&s_WinSettings.lockPetWin,			true,	"Lock Pet: Toggle locking the Pet Window"},
	{"Lock Spell",			&s_WinSettings.lockSpellsWin,		true,	"Lock Group: Toggle locking the Group Window"},
	{"Lock Buffs",			&s_WinSettings.lockBuffsWin,		true,	"Lock Player: Toggle locking the Player Window"},
	{"Lock Songs",			&s_WinSettings.lockSongWin,			true,	"Lock Target: Toggle locking the Target Window"},
	{"Lock Hud",			&s_WinSettings.lockHudWin,			true,	"Lock Pet: Toggle locking the Pet Window"},

};

struct ThemeOption
{
	const char* winName;
	std::string* themeName;
};

std::vector<ThemeOption> themeOptions = {
	{"PlayerWin",	&s_WinTheme.playerWinTheme},
	{"PetWin",		&s_WinTheme.petWinTheme},
	{"GroupWin",	&s_WinTheme.groupWinTheme},
	{"SpellsWin",	&s_WinTheme.spellsWinTheme},
	{"BuffsWin",	&s_WinTheme.buffsWinTheme},
	{"SongWin",		&s_WinTheme.songWinTheme}
};

#pragma endregion

#pragma region Hud Stuff

struct StatusFXData
{
	eEQSPA spaValue;
	int iconID;
	bool positveFX;
	std::string tooltip;
};


std::vector<StatusFXData> statusFXData = {
	{SPA_POISON,			42,		false,	"Poisoned"},
	{SPA_DISEASE,			41,		false,	"Diseased"},
	{SPA_BLINDNESS,			200,	false,	"Blind"},
	{SPA_ROOT,				117,	false,	"Rooted"},
	{SPA_MOVEMENT_RATE,		5,		false,	"Movement Rate Buff"},
	{SPA_FEAR,				164,	false,	"Feared"},
	{SPA_STUN,				165,	false,	"Stunned"},
	// invis effects
	{SPA_INVISIBILITY,		18,		true,	"Invisible"},
	{SPA_INVIS_VS_UNDEAD,	33,		true,	"Invis vs Undead"},
	{SPA_INVIS_VS_ANIMALS,	34,		true,	"Invis vs Animals"}
};

#pragma endregion

#pragma region Menu Bar



#pragma region Utility Functions
// Rut Roh Raggy!
// Grimmier watched a "The Cherno" video on overloading functions. 

void SaveSetting(bool* setting, const char* settingsFile) {
	auto it = std::find_if(winSettings.begin(), winSettings.end(),
		[setting](const WinSetting& ws) { return ws.setting == setting; });
	if (it != winSettings.end()) {
		WritePrivateProfileBool(it->section, it->key, *setting, settingsFile);
	}
}

void SaveSetting(int* value, const char* settingsFile) {
	auto it = std::find_if(numericSettings.begin(), numericSettings.end(),
		[value](const NumericSetting& ns) { return ns.value == value; });
	if (it != numericSettings.end()) {
		WritePrivateProfileInt(it->section, it->key, *value, settingsFile);
	}
}

void SaveSetting(std::string* theme, const char* settingsFile) {
	auto it = std::find_if(themeSettings.begin(), themeSettings.end(),
		[theme](const ThemeSetting& ts) { return ts.theme == theme; });
	if (it != themeSettings.end()) {
		WritePrivateProfileString(it->section, it->key, *theme, settingsFile);
	}
}

void SaveSetting(mq::MQColor* color, const char* settingsFile) {
	auto it = std::find_if(colorSettings.begin(), colorSettings.end(),
		[color](const ColorSetting& cs) { return cs.value == color; });
	if (it != colorSettings.end()) {
		WritePrivateProfileColor(it->section, it->key, *color, settingsFile);
	}
}

const char* MaskName(const char* name)
{
	static char anonymizedName[32];
	if (name && name[0] != '\0')
		snprintf(anonymizedName, sizeof(anonymizedName), "%c****", name[0]);
	else
		snprintf(anonymizedName, sizeof(anonymizedName), "****");

	return anonymizedName;
}

void LockAll()
{
	bool setVal = s_WinSettings.lockAllWin;
	s_WinSettings.lockPlayerWin = setVal;
	s_WinSettings.lockTargetWin = setVal;
	s_WinSettings.lockPetWin    = setVal;
	s_WinSettings.lockGroupWin  = setVal;
	s_WinSettings.lockSpellsWin = setVal;
	s_WinSettings.lockBuffsWin  = setVal;
	s_WinSettings.lockSongWin   = setVal;
	s_WinSettings.lockHudWin    = setVal;
	SaveSettings();
}

void GetHeading()
{
	static PSPAWNINFO pSelfInfo = pLocalPlayer;
	s_CurrHeading = szHeadingShort[static_cast<int>((pSelfInfo->Heading / 32.0f) + 8.5f) % 16];
}

bool CheckCaster()
{
	if (PSPAWNINFO pSelfInfo = pLocalPlayer)
	{
		if (GetMaxMana() > 0)
			return true;
	}
	return false;
}

void PrintGrimHelp()
{
	for (const auto& cmdInfo : commandList)
	{
		WriteChatf("\aw[\ayGrimGUI\ax] \at/grimgui\ax [\ag%s\ax] - \ay%s", cmdInfo.commandText, cmdInfo.description);
	}
}

void GiveItem(PSPAWNINFO pSpawn)
{
	if (!pSpawn)
		return;

	pTarget = pSpawn;

	if (ItemPtr pItem = GetPcProfile()->GetInventorySlot(InvSlot_Cursor))
		EzCommand("/click left target");

}

void GrimCommandHandler(PlayerClient* pPC, const char* szLine)
{
	char arg[MAX_STRING] = {};
	GetArg(arg, szLine, 1);

	if (strlen(arg))
	{
		GrimCommand command;

		if (strcmp(arg, "show") == 0)
			command = GrimCommand::Show;
		else if (strcmp(arg, "lock") == 0)
			command = GrimCommand::Lock;
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
		else if (strcmp(arg, "hud") == 0)
			command = GrimCommand::Hud;
		else if (strcmp(arg, "config") == 0)
			command = GrimCommand::Config;
		else if (strcmp(arg, "clickthrough") == 0)
			command = GrimCommand::ClickThrough;
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
			if (GetGameState() != GAMESTATE_INGAME)
			{
				s_ShowOutOfGame = true;
			}
			s_WinSettings.showMainWindow = !s_WinSettings.showMainWindow;
			SaveSetting(&s_WinSettings.showMainWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Lock:
			s_WinSettings.lockAllWin = !s_WinSettings.lockAllWin;
			SaveSetting(&s_WinSettings.lockAllWin, &s_SettingsFile[0]);
			break;
		case GrimCommand::Player:
			s_WinSettings.showPlayerWindow = !s_WinSettings.showPlayerWindow;
			SaveSetting(&s_WinSettings.showPlayerWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Target:
			s_WinSettings.showTargetWindow = !s_WinSettings.showTargetWindow;
			SaveSetting(&s_WinSettings.showTargetWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Pet:
			s_WinSettings.showPetWindow = !s_WinSettings.showPetWindow;
			SaveSetting(&s_WinSettings.showPetWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Group:
			s_WinSettings.showGroupWindow = !s_WinSettings.showGroupWindow;
			SaveSetting(&s_WinSettings.showGroupWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Spells:
			s_WinSettings.showSpellsWindow = !s_WinSettings.showSpellsWindow;
			SaveSetting(&s_WinSettings.showSpellsWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Buffs:
			s_WinSettings.showBuffWindow = !s_WinSettings.showBuffWindow;
			SaveSetting(&s_WinSettings.showBuffWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Songs:
			s_WinSettings.showSongWindow = !s_WinSettings.showSongWindow;
			SaveSetting(&s_WinSettings.showSongWindow, &s_SettingsFile[0]);
			break;
		case GrimCommand::Config:
			s_WinSettings.showConfigWindow = !s_WinSettings.showConfigWindow;
			break;
		case GrimCommand::ClickThrough:
			s_WinSettings.hudClickThrough = !s_WinSettings.hudClickThrough;
			SaveSetting(&s_WinSettings.hudClickThrough, &s_SettingsFile[0]);
			break;
		case GrimCommand::Hud:
			s_WinSettings.showHud = !s_WinSettings.showHud;
			SaveSetting(&s_WinSettings.showHud, &s_SettingsFile[0]);
			break;
		}
	}
	else
	{
		s_WinSettings.showMainWindow = !s_WinSettings.showMainWindow;
		PrintGrimHelp();
	}
}

/**
* @fn CalcButtonSize
* 
* @brief Function to calculate the dynamic size of a button based on the label and padding.
* If you have a bunch of buttons you want to be the same size pass the largest label to this function
* 
* @param label const char* Label text for the button
* @param padding float Padding value to add to the button size
* @param scale float Font scale to use for the button
* 
* @return ImVec2 Calculated button size
*/
ImVec2 CalcButtonSize(const char* label, float padding = 2.0f, float scale = 1.0f)
{
	ImVec2 textSize = ImGui::CalcTextSize(label);
	ImVec2 framePadding = ImGui::GetStyle().FramePadding;
	return ImVec2((textSize.x * scale) + framePadding.x + padding, (textSize.y * scale) + framePadding.y + padding);
}
#pragma endregion

#pragma region Some ImGui Stuff *Draw functions*

static void DrawSpellBarIcons(int gemHeight)
{
	if (!pLocalPC)
		return;

	if (!m_pTASpellIcon)
	{
		m_pTASpellIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
			*m_pTASpellIcon = *temp;
	}

	if (!m_pGemBackground)
		m_pGemBackground = pSidlMgr->FindAnimation("A_SpellGemBackground");

	if (!m_pGemHolder)
		m_pGemHolder = pSidlMgr->FindAnimation("A_SpellGemHolder");

	// calculate max pSpellGem count
	int spellGemCount = 8;
	int aaIndex = GetAAIndexByName("Mnemonic Retention");
	if (CAltAbilityData* pAbility = GetAAById(aaIndex))
	{
		if (PlayerHasAAAbility(aaIndex))
			spellGemCount += pAbility->CurrentRank;
	}

	int spellIconSize = static_cast<int>(gemHeight * 0.75);
	CXSize gemSize = { static_cast<int>(gemHeight * 1.25), gemHeight };

	for (int i = 0; i < spellGemCount; ++i)
	{
		ImGui::PushID(i);

		int spellId = pLocalPC->GetMemorizedSpell(i);
		if (!spellId)
			continue;

		CSpellGemWnd* pSpellGem = pCastSpellWnd->SpellSlots[i];
		MQColor gemTint;

		if (spellId == -1)
		{
			if (m_pGemHolder)
			{
				imgui::DrawTextureAnimation(m_pGemHolder, gemSize);

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Right Click to Open Spell Picker, to memorize a spell");
					ImGui::Text("You can also drop a spell from your spell book here to memorize it.");
					ImGui::EndTooltip();

					if (ImGui::IsMouseClicked(0))
					{
						pSpellGem->ParentWndNotification(pSpellGem, XWM_LCLICK, nullptr);
					}
					else if (ImGui::IsMouseClicked(1))
					{
						if (pSpellPicker)
						{
							pSpellPicker->SetOpen(true);
							s_MemGemIndex = i + 1;
						}
					}
				}
			}
		}
		else
		{
			// draw spell icon
			EQ_Spell* spell = GetSpellByID(spellId);
			if (spell)
			{
				m_pTASpellIcon->SetCurCell(spell->SpellIcon);
				MQColor tintCol = MQColor(255, 255, 255, 255);
				gemTint = pSpellGem->SpellGemTintArray[pSpellGem->TintIndex];

				if (!pSpellInspector->IsGemReady(i))
					tintCol = MQColor(50, 50, 50, 255);

				ImGui::BeginGroup();
				float posX = ImGui::GetCursorPosX();
				float posY = ImGui::GetCursorPosY();

				ImGui::SetCursorPos(ImVec2(posX + ((gemSize.cx - spellIconSize) / 2), posY + ((gemSize.cy - spellIconSize) / 2)));
				imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(spellIconSize, spellIconSize), tintCol);
				ImGui::SetCursorPos(ImVec2(posX, posY));
				imgui::DrawTextureAnimation(m_pGemBackground, gemSize, gemTint);

				posX = ImGui::GetCursorPosX();
				posY = ImGui::GetCursorPosY();

				if (!pSpellInspector->IsGemReady(i))
				{
					// gem not read cooldown timer
					float coolDown = static_cast<float>(pLocalPlayer->SpellGemETA[i] - pDisplay->TimeStamp) / 1000;
					if (coolDown < 1801)
					{
						std::string label = std::to_string(static_cast<int>(std::ceil(coolDown)));
						ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
						float centeredX = posX + static_cast<float>(gemSize.cx - textSize.x) / 2.0f;
						float centeredY = posY - static_cast<float>(gemSize.cy * 0.75);
						ImGui::SetCursorPos(ImVec2(centeredX, centeredY));
						ImGui::TextColored(GetMQColor(ColorName::Teal).ToImColor(), "%s", label.c_str());
						ImGui::SetCursorPos(ImVec2(posX, posY));
					}
				}
				ImGui::EndGroup();
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					if (spellId != -1)
					{
						EQ_Spell* spell = GetSpellByID(spellId);
						if (spell)
						{
							ImGui::Text(spell->Name);
							ImGui::TextColored(GetMQColor(ColorName::Teal).ToImColor(), "Mana: %d", spell->ManaCost);
							ImGui::Text("Recast: %d", spell->RecastTime / 1000);
							ImGui::Spacing();
							ImGui::Text("Ctrl + Click to pick up gem");
							ImGui::Text("Alt + Click to inspect spell");
						}
					}
					ImGui::EndTooltip();

					if (ImGui::IsMouseClicked(0) && ImGui::IsKeyDown(ImGuiKey_ModAlt))
					{
						pSpellInspector->DoInspectSpell(spellId);
					}
					else if (ImGui::IsMouseClicked(0) && ImGui::IsKeyDown(ImGuiKey_ModCtrl))
					{
						pSpellGem->ParentWndNotification(pSpellGem, XWM_LCLICKHOLD, nullptr);
						pSpellGem->ParentWndNotification(pSpellGem, XWM_LBUTTONUPAFTERHELD, nullptr);
					}
					else if (ImGui::IsMouseClicked(0))
					{
						if (spellId != -1)
							Cast(pLocalPlayer, spell->Name);
					}
					else if (ImGui::IsMouseClicked(1))
					{
						if (spellId != -1)
							pSpellGem->ParentWndNotification(pSpellGem, XWM_RCLICK, nullptr);
					}
				}
			}
		}
		ImGui::PopID();
	}
}


static void DisplayPetButtons()
{
	ImVec2 btnSize = CalcButtonSize("Regroup", 1, s_FontScaleSettings.petWinScale);
	int numColumns = static_cast<int>(1, ImGui::GetColumnWidth() / btnSize.x);
	if (numColumns < 1)
		numColumns = 1;

	if (ImGui::BeginTable("ButtonsTable", numColumns, ImGuiTableFlags_SizingStretchProp))
	{
		for (auto& button : petButtons)
		{
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
					if (ImGui::Button(btnLabel.c_str(), btnSize))
						EzCommand(button.command.c_str());

					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(btnLabel.c_str(), btnSize))
						EzCommand(button.command.c_str());
				}
			}
		}
		ImGui::EndTable();
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
void DrawLineOfSight(PSPAWNINFO pFrom, PSPAWNINFO pTo)
{
	if (LineOfSight(pFrom, pTo))
		ImGui::TextColored(GetMQColor(ColorName::Green).ToImColor(), ICON_MD_VISIBILITY);
	else
		ImGui::TextColored(GetMQColor(ColorName::Red).ToImColor(), ICON_MD_VISIBILITY_OFF);
}

/**
* @fn DrawStatusEffects
* 
* @brief Draws negative status effects icons for the current player. 
* examples: Poisoned, Diseased, Rooted, Feared, Stunned, etc.
*
*/
void DrawStatusEffects()
{
	if (!m_StatusIcon)
	{
		m_StatusIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellIcons"))
			*m_StatusIcon = *temp;
	}

	bool efxflag = false;
	CXSize iconSize(30, 30);

	for (const auto& debuff : statusFXData)
	{
		int check = GetSelfBuff(SpellAffect(debuff.spaValue, debuff.positveFX));
		if (check >= 0)
		{
			efxflag = true;
			m_StatusIcon->SetCurCell(debuff.iconID);
			imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
			ImGui::SetItemTooltip(debuff.tooltip.c_str());
			ImGui::SameLine();
		}
	}

	if (GetSelfBuff([](EQ_Spell* spell) { return SpellAffect(SPA_HP, false)(spell) && spell->IsDetrimentalSpell() && spell->IsDoTSpell(); }) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(140);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SameLine();
		ImGui::SetItemTooltip("Dotted");
	}

	if (GetSelfBuff(SpellSubCat(SPELLCAT_RESIST_DEBUFFS) && SpellClassMask(Shaman, Mage)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(55);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SameLine();
		ImGui::SetItemTooltip("Malo");
	}

	if (GetSelfBuff(SpellSubCat(SPELLCAT_RESIST_DEBUFFS) && SpellClassMask(Enchanter)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(72);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SameLine();
		ImGui::SetItemTooltip("Tash");
	}

	if (!efxflag)
		ImGui::Dummy(iconSize);

}

/**
* @fn DrawPlayerIcons
*
* @brief Draws Role Icons for the Group Member, such as Main Tank, Main Assist, Puller, and Group Leader
* 
* @param pMember CGroupMember Pointer to the Group Member
*/
void DrawPlayerIcons(CGroupMember* pMember, float fontScale = 1.0f)
{
	if (!pMember)
		return;
	
	ImVec2 iconSize(20 * fontScale, 20 * fontScale);
	
	if (pMember->IsMainTank())
	{
		m_pMainTankIcon = pSidlMgr->FindAnimation("A_Tank");
		imgui::DrawTextureAnimation(m_pMainTankIcon, iconSize);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Main Tank");

	}
	if (pMember->IsMainAssist())
	{
		m_pMainAssistIcon = pSidlMgr->FindAnimation("A_Assist");
		imgui::DrawTextureAnimation(m_pMainAssistIcon, iconSize);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Main Assist");
	}
	if (pMember->IsPuller())
	{
		m_pPullerIcon = pSidlMgr->FindAnimation("A_Puller");
		imgui::DrawTextureAnimation(m_pPullerIcon, iconSize);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Puller");
	}
	if (pMember == GetCharInfo()->pGroupInfo->GetGroupLeader())
	{
		ImGui::TextColored(ImVec4(GetMQColor(ColorName::Teal).ToImColor()), ICON_MD_STAR);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Group Leader");
	}

	ImGui::Dummy(ImVec2(1.0f, 1.0f));
}

/**
* @fn DrawCombatStateIcon
*
* @brief Draws a combat state icon based on the current combat state
* : In Combat, Debuffed, Timer, Standing, Regen
*/
void DrawCombatStateIcon(float fontScale = 1.0f)
{
	int comState = GetCombatState();
	ImVec2 iconSize(20 * fontScale, 20 * fontScale);

	switch (comState)
	{
	case eCombatState_Combat:
		m_pCombatIcon = pSidlMgr->FindAnimation("A_PWCSInCombat");
		imgui::DrawTextureAnimation(m_pCombatIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("In Combat");
		break;
	case eCombatState_Debuff:
		m_pDebuffIcon = pSidlMgr->FindAnimation("A_PWCSDebuff");
		imgui::DrawTextureAnimation(m_pDebuffIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("You are Debuffed. You may not rest.");
		break;
	case eCombatState_Timer:
		m_pTimerIcon = pSidlMgr->FindAnimation("A_PWCSTimer");
		imgui::DrawTextureAnimation(m_pTimerIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("You are recovering from combat, and may not rest yet!");
		break;
	case eCombatState_Standing:
		m_pStandingIcon = pSidlMgr->FindAnimation("A_PWCSStanding");
		imgui::DrawTextureAnimation(m_pStandingIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("You are not in combat. You may rest at any time.");
		break;
	case eCombatState_Regen:
		m_pRegenIcon = pSidlMgr->FindAnimation("A_PWCSRegen");
		imgui::DrawTextureAnimation(m_pRegenIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("You are resting.");

		break;
	default:
		break;
	}
}


/**
* @fn DrawHelpIcon
*
* @brief Draws a help icon with a tooltip
*
* @param helpText const char* Text to display in the tooltip
*/
void DrawHelpIcon(const char* helpText)
{
	ImGui::SameLine();
	ImGui::TextDisabled(ICON_FA_QUESTION_CIRCLE_O);
	if (ImGui::IsItemHovered())
		ImGui::SetItemTooltip("%s", helpText);
}


void DrawMenu(const char* winName)
{
	const char* lockAllIcon = s_WinSettings.lockAllWin ? ICON_MD_LOCK : ICON_MD_LOCK_OPEN;
	ImVec4 lockLabelColor = s_WinSettings.lockAllWin ? GetMQColor(ColorName::Red).ToImColor() : GetMQColor(ColorName::Green).ToImColor();
	if (ImGui::BeginMenuBar())
	{
		ImGui::TextColored(lockLabelColor, lockAllIcon);
		if (ImGui::IsItemClicked())
		{
			s_WinSettings.lockAllWin = !s_WinSettings.lockAllWin;
			LockAll();
		}
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("%s All Windows", s_WinSettings.lockAllWin ? "Unlock" : "Lock");

		for (const auto& winLocks : settingToggleOptions)
		{
			if (winLocks.label && std::string(winLocks.label) == "Lock " + std::string(winName))
			{
				const char* lockIcon = *winLocks.setting ? ICON_FA_LOCK : ICON_FA_UNLOCK;
				ImVec4 lockLabelColor = *winLocks.setting ? GetMQColor(ColorName::Red).ToImColor() : GetMQColor(ColorName::Green).ToImColor();

				ImGui::SameLine();
				ImGui::TextColored(lockLabelColor, lockIcon);
				if (ImGui::IsItemClicked())
				{	
					if (*winLocks.setting && s_WinSettings.lockAllWin)
					{
						s_WinSettings.lockAllWin = false;
						SaveSetting(&s_WinSettings.lockAllWin, &s_SettingsFile[0]);
					}

					*winLocks.setting = !*winLocks.setting;
					SaveSetting(winLocks.setting, &s_SettingsFile[0]);
				}
				if (ImGui::IsItemHovered())
					ImGui::SetItemTooltip("%s %s Window", *winLocks.setting ? "Unlock" : "Lock", winName);

				break;
			}
		}
		
		ImGui::SameLine();
		ImGui::Text(ICON_FA_COG);
		if (ImGui::IsItemClicked())
			s_WinSettings.showConfigWindow = !s_WinSettings.showConfigWindow;
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Config Window");

		if (ImGui::BeginMenu("Windows"))
		{

			for (const auto& window : options)
			{
				if (ImGui::MenuItem(window.label, nullptr, *window.setting))
					*window.setting = !*window.setting;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Main Window", nullptr, s_WinSettings.showMainWindow))
				s_WinSettings.showMainWindow = !s_WinSettings.showMainWindow;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Locking"))
		{
			for (const auto& lockWin : settingToggleOptions)
			{
				if (lockWin.label && strncmp(lockWin.label, "Lock ", 5) == 0)
				{
					if (ImGui::MenuItem(lockWin.label, nullptr, *lockWin.setting))
					{
						*lockWin.setting = !*lockWin.setting;

						if (lockWin.label && strncmp(lockWin.label, "Lock All", 8) == 0)
							LockAll();
					}
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
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
void DrawBar(const char* label, int current, int max, int height,
	const mq::MQColor minColor, const mq::MQColor maxColor, const char* tooltip)
{
	float percentage = static_cast<float>(current) / max;
	int percentageInt = static_cast<int>(percentage * 100);
	ImVec4 color = CalculateProgressiveColor(minColor, maxColor, percentageInt);

	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
	ImGui::ProgressBar(percentage, ImVec2(ImGui::GetWindowWidth() - 15, static_cast<float>(height)), label);
	ImGui::PopStyleColor();

	if (ImGui::IsItemHovered())
		ImGui::SetItemTooltip("%s: %d / %d", tooltip, current, max);

}

void DrawPetInfo(PSPAWNINFO petInfo, bool showAll = true)
{
	const char* petName = petInfo->DisplayedName;
	if (mq::IsAnonymized())
		petName = "Pet";

	float sizeX = ImGui::GetWindowWidth();
	float yPos = ImGui::GetCursorPosY();
	float midX = (sizeX / 2);
	float petPercentage = static_cast<float>(petInfo->HPCurrent) / 100;
	int petLabel = petInfo->HPCurrent;

	ImVec4 colorTarHP = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, petInfo->HPCurrent);
	if (showAll)
	{
		if (ImGui::BeginChild("Pet", ImVec2(ImGui::GetColumnWidth(), 0),
			ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize , ImGuiWindowFlags_NoScrollbar))
		{
			DrawLineOfSight(pLocalPlayer, petInfo);
			ImGui::SameLine();
			ImGui::Text("Lvl");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(GetMQColor(ColorName::Teal).ToImColor()), "%d", petInfo->Level);
			ImGui::SameLine();
			ImGui::Text("Dist:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(GetMQColor(ColorName::Tangerine).ToImColor()), "%0.0f m", GetDistance(pLocalPlayer, petInfo));

			// health bar
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorTarHP);
			ImGui::SetNextItemWidth(static_cast<float>(sizeX) - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth(), static_cast<float>(s_NumSettings.playerBarHeight)), "##");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(ImGui::GetColumnWidth() / 2, yPos));
			ImGui::Text("%d %%", petLabel);
		}
		ImGui::EndChild();
	}
	else
	{
		// just draw a green pet health bar at 3/4 size of the player bars for group window.
		if (ImGui::BeginChild("PetBar", ImVec2(ImGui::GetColumnWidth(), 0),
			ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize))
		{

			float barSize = static_cast<float>(s_NumSettings.groupBarHeight * 0.75);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(GetMQColor(ColorName::Green2).ToImColor()));
			ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth(), barSize), "##");
			ImGui::PopStyleColor();
		}
		ImGui::EndChild();
	}
	if (ImGui::IsItemHovered() )
	{
		ImGui::SetItemTooltip("%s  %d%", petName, petInfo->HPCurrent);

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			GiveItem(pSpawnManager->GetSpawnByID(pLocalPlayer->PetID));		
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
void DrawPlayerBars(bool drawCombatBorder = false, int barHeight = s_NumSettings.playerBarHeight, bool drawPet = false, float fontScale = 1.0f)
{
	if (!pLocalPC)
		return;

	if (ImGui::BeginChild(pLocalPC->Name, ImVec2(ImGui::GetContentRegionAvail().x, 0),
		ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::SetWindowFontScale(fontScale);
		ImGuiChildFlags s_ChildFlags = drawCombatBorder ? ImGuiChildFlags_Border : ImGuiChildFlags_None;

		if (drawCombatBorder && pEverQuestInfo->bAutoAttack)
		{
			ImVec4 borderColor = s_WinSettings.flashCombatFlag ? ImVec4(GetMQColor(ColorName::Red).ToImColor()) : ImVec4(GetMQColor(ColorName::White).ToImColor());
			ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
		}
		else if (drawCombatBorder)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(GetMQColor(ColorName::White).ToImColor()));
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));

		if (ImGui::BeginChild("info", ImVec2(ImGui::GetContentRegionAvail().x, 26 * fontScale),
			s_ChildFlags | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoScrollbar))
		{
			//ImGui::SetWindowFontScale(fontScale);

			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int midX = (sizeX / 2) - 8;
			if (midX < 0)
				midX = 1;

			const char* nameLabel = pLocalPC->Name;
			if (mq::IsAnonymized())
				nameLabel = MaskName(nameLabel);

			if (ImGui::BeginTable("##Player", 4))
			{
				float roleColSize = ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(nameLabel).x + 100);
				float nameCalcSize = ImGui::CalcTextSize(nameLabel).x;
				ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthFixed, ceil(nameCalcSize));
				ImGui::TableSetupColumn("Roles", ImGuiTableColumnFlags_WidthStretch, roleColSize);
				ImGui::TableSetupColumn("##Heading", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 30);
				ImGui::TableSetupColumn("##Lvl", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 60);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text(nameLabel);
				ImGui::TableNextColumn();
				DrawCombatStateIcon(fontScale);
				if (GetCharInfo() && GetCharInfo()->pGroupInfo)
				{
					CGroupMember* pMember = GetCharInfo()->pGroupInfo->GetGroupMember(pLocalPlayer);
					ImGui::SameLine(0.0f, 10.0f);
					DrawPlayerIcons(pMember,fontScale);
				}
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(GetMQColor(ColorName::Yellow).ToImColor()), s_CurrHeading);
				ImGui::TableNextColumn();
				ImGui::Text("Lvl: %d", pLocalPC->GetLevel());
				ImGui::EndTable();
			}
		}
		ImGui::SetWindowFontScale(1.0f);
		ImGui::EndChild();

		ImGui::PopStyleVar(2);
		if (drawCombatBorder)
			ImGui::PopStyleColor();

		// Health bar because if we have no health we are dead!
		DrawBar("##hp", GetCurHPS(), GetMaxHPS(), barHeight, s_BarColors.minColorHP, s_BarColors.maxColorHP, "HP");

		// Mana bar if you have mana that is
		if (GetMaxMana() > 0)
			DrawBar("##Mana", GetCurMana(), GetMaxMana(), barHeight, s_BarColors.minColorMP, s_BarColors.maxColorMP, "Mana");

		// Endurance bar does anyone even use this?
		DrawBar("##Endur", GetCurEndurance(), GetMaxEndurance(), barHeight, s_BarColors.minColorEnd, s_BarColors.maxColorEnd, "Endur");

		if (drawPet)
		{
			// Go Fluffy Go!
			if (PSPAWNINFO MyPet = pSpawnManager->GetSpawnByID(pLocalPlayer->PetID))
				DrawPetInfo(MyPet, false);
		}
	}
	ImGui::SetWindowFontScale(1.0f);
	ImGui::EndChild();
}

void DrawEmptyMember(int slot)
{
	float sizeY = static_cast<float>(s_NumSettings.groupBarHeight) * 4 + 50;
	if (ImGui::BeginChild(("##Empty%d", slot), ImVec2(ImGui::GetContentRegionAvail().x, sizeY),
		ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::Text("Open");
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
void DrawMemberInfo(CGroupMember* pMember, float fontScale = 1.0f)
{
	if (!pMember)
		return;

	if (ImGui::BeginTable("GroupMember", 5))
	{
		float distToMember = 0.0f;
		const char* nameLabel = pMember->GetName();

		if (mq::IsAnonymized())
			nameLabel = MaskName(nameLabel);

		if (SPAWNINFO* pSpawn = pMember->GetPlayer())
			distToMember = GetDistance(pLocalPlayer, pSpawn);

		float rolColSize = ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(nameLabel).x + 100);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize(nameLabel).x);
		ImGui::TableSetupColumn("Vis", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize(ICON_MD_VISIBILITY_OFF).x);
		ImGui::TableSetupColumn("Roles", ImGuiTableColumnFlags_WidthStretch, rolColSize);
		ImGui::TableSetupColumn("Dist", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 30 * s_FontScaleSettings.groupWinScale);
		ImGui::TableSetupColumn("Lvl", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 30 * s_FontScaleSettings.groupWinScale);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(nameLabel);
		ImGui::TableNextColumn();
		if (SPAWNINFO* pSpawn = pMember->GetPlayer())
		{
			DrawLineOfSight(pLocalPlayer, pSpawn);
		}
		else
		{
			ImGui::TextColored(GetMQColor(ColorName::Red).ToImColor(), ICON_MD_VISIBILITY_OFF);
		}
		ImGui::TableNextColumn();
		DrawPlayerIcons(pMember, fontScale);
		ImGui::TableNextColumn();
		ImGui::TextColored(GetMQColor(ColorName::Tangerine).ToImColor(), "%0.0f m", distToMember);
		ImGui::TableNextColumn();
		int lvl = 999;
		if (SPAWNINFO* pSpawn = pMember->GetPlayer())
			lvl = pSpawn->Level;
		ImGui::Text("%d", lvl);
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
void DrawGroupMemberBars(CGroupMember* pMember, bool drawPet = true, int groupSlot = 1)
{
	if (!pMember)
		return;

	float sizeY = static_cast<float>(s_NumSettings.groupBarHeight) * 4 + 50;

	if (!pMember->GetPlayer())
	{
		if (ImGui::BeginChild(("##Empty%d", groupSlot), ImVec2(ImGui::GetContentRegionAvail().x, sizeY),
			ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar))
		{
			DrawMemberInfo(pMember, s_FontScaleSettings.groupWinScale);
		}
		ImGui::EndChild();

		return;
	}

	SPAWNINFO* pSpawn = pMember->GetPlayer();

	if (ImGui::BeginChild(pSpawn->Name, ImVec2(ImGui::GetContentRegionAvail().x, 0),
		ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY| ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoScrollbar))
	{

		ImGui::PushID(pSpawn->Name);
		ImGui::BeginGroup();
		{

			DrawMemberInfo(pMember, s_FontScaleSettings.groupWinScale);

			// Health bar "Not Deat Yet!"
			if (pSpawn->HPCurrent && pSpawn->HPMax)
				DrawBar("##hp", pSpawn->HPCurrent, pSpawn->HPMax,
					s_NumSettings.groupBarHeight, s_BarColors.minColorHP, s_BarColors.maxColorHP, "HP");

			// Mana bar maybe?
			if (pSpawn->ManaCurrent && pSpawn->ManaMax)
				DrawBar("##Mana", pSpawn->ManaCurrent, pSpawn->ManaMax,
					s_NumSettings.groupBarHeight, s_BarColors.minColorMP, s_BarColors.maxColorMP, "Mana");

			// Endurance bar again does anyone even use this? beside when you can't jump anymore.
			if (pSpawn->EnduranceCurrent && pSpawn->EnduranceMax)
				DrawBar("##Endur", pSpawn->EnduranceCurrent, pSpawn->EnduranceMax,
					s_NumSettings.groupBarHeight, s_BarColors.minColorEnd, s_BarColors.maxColorEnd, "Endurance");

			ImGui::EndGroup();
		}
		ImGui::PopID();
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			GiveItem(pSpawn);

		const char* spawnName = pSpawn->Name;
		if (ImGui::BeginPopupContextItem(("##%s", spawnName)))
		{
			if (s_DanNetEnabled)
			{
				if (ImGui::MenuItem("Come To Me"))
				{
					EzCommand(fmt::format("/dex {} /multiline ; /afollow off; /nav stop ;  /timed 5, /nav id {}",
						pSpawn->Name, pLocalPlayer->GetId()).c_str());
				}
			}
			if (ImGui::MenuItem("Go To"))
			{
				EzCommand(fmt::format("/nav spawn {}", spawnName).c_str());
			}
			if (s_DanNetEnabled)
			{
				if (ImGui::MenuItem("Switch To"))
				{
					EzCommand(fmt::format("/dex {} /foreground", spawnName).c_str());
				}
			}

			if (ImGui::BeginMenu("Roles"))
			{
				if (ImGui::MenuItem("Main Assist"))
					EzCommand(fmt::format("/grouproles set {} 2", spawnName).c_str());

				if (ImGui::MenuItem("Main Tank"))
					EzCommand(fmt::format("/grouproles set {} 1", spawnName).c_str());

				if (ImGui::MenuItem("Puller"))
					EzCommand(fmt::format("/grouproles set {} 3", spawnName).c_str());

				if (pLocalPlayer->Name == GetCharInfo()->pGroupInfo->GetGroupLeader()->Name)
				{
					if (ImGui::MenuItem("Make Leader"))
						EzCommand(fmt::format("/makeleader {}", spawnName).c_str());
				}
				else if (pMember == GetCharInfo()->pGroupInfo->GetGroupLeader())
				{
					if (s_DanNetEnabled)
					{
						if (ImGui::MenuItem("Make Me Leader"))
							EzCommand(fmt::format("/dex {} /makeleader {}",
								spawnName, pLocalPlayer->Name).c_str());
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		if (drawPet)
		{
			// fluffy
			if (PSPAWNINFO petInfo = pSpawnManager->GetSpawnByID(pSpawn->PetID))
				DrawPetInfo(petInfo, false);
		}
	}
	ImGui::EndChild();
}

void TogglePetButtonVisibilityMenu()
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
				SaveSetting(&button.visible, &s_SettingsFile[0]);
			}
		}
		ImGui::EndTable();
	}
}

#pragma endregion
