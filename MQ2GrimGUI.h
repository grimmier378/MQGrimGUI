#pragma once
#include "mq/base/Color.h"

extern struct WinSettings s_WinSettings;
extern struct NumericSettings s_NumSettings;
extern class SpellPicker* pSpellPicker;
extern std::string s_MemSpellName;
extern int s_MemGemIndex;
extern bool s_MemSpell;

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
	bool lockWindows			= false;
	bool hudClickThrough		= false;
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
	{"Settings",	"LockWindows",			&s_WinSettings.lockWindows},
	{"PlayerTarg",	"SplitTarget",			&s_WinSettings.showTargetWindow},
	{"PlayerTarg",	"ShowPlayerWindow",		&s_WinSettings.showPlayerWindow},
	{"Pet",			"ShowPetWindow",		&s_WinSettings.showPetWindow},
	{"Group",		"ShowGroupWindow",		&s_WinSettings.showGroupWindow},
	{"Spells",		"ShowSpellsWindow",		&s_WinSettings.showSpellsWindow},
	{"Buffs",		"ShowBuffWindow",		&s_WinSettings.showBuffWindow},
	{"Songs",		"ShowSongWindow",		&s_WinSettings.showSongWindow},
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
};

std::vector<ColorSetting> colorSettings = {
	{"Colors",	 "MinColorHP",		&s_BarColors.minColorHP},
	{"Colors",	 "MaxColorHP",		&s_BarColors.maxColorHP},
	{"Colors",	 "MinColorMP",		&s_BarColors.minColorMP},
	{"Colors",	 "MaxColorMP",		&s_BarColors.maxColorMP},
	{"Colors",	 "MinColorEnd",		&s_BarColors.minColorEnd},
	{"Colors",	 "MaxColorEnd",		&s_BarColors.maxColorEnd},
	{"Colors",	 "MinColorCast",	&s_BarColors.minColorCast},
	{"Colors",	 "MaxColorCast",	&s_BarColors.maxColorCast}
};

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
			r = toFloat(midColor->Red)   + proportion * (toFloat(maxColor.Red)   - toFloat(midColor->Red));
			g = toFloat(midColor->Green) + proportion * (toFloat(maxColor.Green) - toFloat(midColor->Green));
			b = toFloat(midColor->Blue)  + proportion * (toFloat(maxColor.Blue)  - toFloat(midColor->Blue));
			a = toFloat(midColor->Alpha) + proportion * (toFloat(maxColor.Alpha) - toFloat(midColor->Alpha));
		}
		else
		{
			float proportion = static_cast<float>(value) / midValue;
			r = toFloat(minColor.Red)   + proportion * (toFloat(midColor->Red)   - toFloat(minColor.Red));
			g = toFloat(minColor.Green) + proportion * (toFloat(midColor->Green) - toFloat(minColor.Green));
			b = toFloat(minColor.Blue)  + proportion * (toFloat(midColor->Blue)  - toFloat(minColor.Blue));
			a = toFloat(minColor.Alpha) + proportion * (toFloat(midColor->Alpha) - toFloat(minColor.Alpha));
		}
	}
	else
	{
		// Calculate between minColor and maxColor
		float proportion = static_cast<float>(value) / 100;
		r = toFloat(minColor.Red)   + proportion * (toFloat(maxColor.Red)   - toFloat(minColor.Red));
		g = toFloat(minColor.Green) + proportion * (toFloat(maxColor.Green) - toFloat(minColor.Green));
		b = toFloat(minColor.Blue)  + proportion * (toFloat(maxColor.Blue)  - toFloat(minColor.Blue));
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
		{GrimCommand::Lock,			"lock",			"Toggles Window Lock"},
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
						EzCommand(button.command.c_str());

					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(btnLabel.c_str(), ImVec2(55, 20)))
						EzCommand(button.command.c_str());
				}
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

std::vector<WindowOption> options = {
	{"Player Win",	&s_WinSettings.showPlayerWindow,	"PlayerTarg",	"ShowPlayerWindow"},
	{"Target Win",	&s_WinSettings.showTargetWindow,	"PlayerTarg",	"SplitTarget"},
	{"Pet Win",		&s_WinSettings.showPetWindow,		"Pet",			"ShowPetWindow"},
	{"Spells Win",	&s_WinSettings.showSpellsWindow,	"Spells",		"ShowSpellsWindow"},
	{"Buff Win",	&s_WinSettings.showBuffWindow,		"Buffs",		"ShowBuffWindow"},
	{"Song Win",	&s_WinSettings.showSongWindow,		"Songs",		"ShowSongWindow"},
	{"Group Win",	&s_WinSettings.showGroupWindow,		"Group",		"ShowGroupWindow"},
	{"Hud Win",		&s_WinSettings.showHud,				"Hud",			"ShowHud"}
};


struct SliderOption
{
	const char* label;
	int* value;
	int min;
	int max;
	const char* helpText;
};

std::vector <SliderOption> sliderOptions = {
	{"Flash Speed",				&s_NumSettings.combatFlashInterval, 0, 50,		"Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
	{"Buff Flash Speed",		&s_NumSettings.flashBuffInterval,	0, 30,		"Buff Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
	{"Buff Icon Size",			&s_NumSettings.buffIconSize,		15, 40,		"Buff Icon Size"},
	{"Buff Timer Threshold",	&s_NumSettings.buffTimerThreshold,	0, 3601,	"Buff Show Timer Threshold in Seconds (0 = Always Show)"},
	{"Player Bar Height",		&s_NumSettings.playerBarHeight,		8, 40,		"Player Bar Height"},
	{"Target Bar Height",		&s_NumSettings.targetBarHeight,		8, 40,		"Target Bar Height"},
	{"Aggro Bar Height",		&s_NumSettings.aggroBarHeight,		8, 40,		"Aggro Bar Height"},
	{"Group Bar Height",		&s_NumSettings.groupBarHeight,		8, 40,		"Group Bar Height"},
	{"Spell Gem Height",		&s_NumSettings.spellGemHeight,		15, 100,	"Spell Gem Height"},
	{"Hud Alpha",				&s_NumSettings.hudAlpha,			0, 255,		"Hud Transparency (Alpha Level)" },
};


struct ThemeOption
{
	const char* label;
	std::string* theme;
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

/**
if (GetSelfBuff(SpellAffect(SPA_POISON, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(42);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Poisoned");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_DISEASE, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(41);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Diseased");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_BLINDNESS, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(200);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Blind");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_ROOT, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(117);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Rooted");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_MOVEMENT_RATE, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(5);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Movement Rate Buff");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_INVISIBILITY, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(18);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Invisible");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_INVIS_VS_UNDEAD, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(33);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Invis vs Undead");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_INVIS_VS_ANIMALS, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(34);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Invis vs Animals");
		ImGui::SameLine();
	}

	if (GetSelfBuff(SpellAffect(SPA_FEAR, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(164);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Feared");
		ImGui::SameLine();
	}
	if (GetSelfBuff(SpellAffect(SPA_STUN, false)) >= 0)
	{
		efxflag = true;
		m_StatusIcon->SetCurCell(165);
		imgui::DrawTextureAnimation(m_StatusIcon, iconSize);
		ImGui::SetItemTooltip("Stunned");
		ImGui::SameLine();
	}
	*/
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

#pragma endregion
