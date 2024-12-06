#include "SpellPicker.h"
#include "Theme.h"
#include <mq/Plugin.h>
#include <imgui.h>
#include "mq/base/Color.h"
#include "mq/imgui/Widgets.h"
#include "mq/imgui/ImGuiUtils.h"
#include "eqlib/Spells.h"
#include "main/MQ2SpellSearch.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include <filesystem>
#include <chrono>



PreSetup("MQGrimGUI");
PLUGIN_VERSION(0.2);

static bool        s_IsCasting              = false;
static bool        s_CharIniLoaded          = false;
static bool        s_DefaultLoaded          = false;
static bool        s_IsCaster               = false;
static bool        s_FollowClicked          = false;
static const char* PLUGIN_NAME              = "MQGrimGUI";
static std::string DEFAULT_INI              = fmt::format( "{}/{}.ini", gPathConfig, PLUGIN_NAME);

char               s_SettingsFile[MAX_PATH] = { 0 };
const char*        s_SecondAggroName        = "Unknown";
const char*        s_CurrHeading            = "N";
const char*        HELP_TEXT_MIN            = "Color you progress down to from Max";
const char*        HELP_TEXT_MAX            = "Color you progress up to from Min";

SpellPicker*       pSpellPicker             = nullptr;

bool               s_ShowOutOfGame          = false;
bool               s_MemSpell               = false;
bool               s_DanNetEnabled          = false;
bool               s_ShowSpellBook          = false;
bool               s_SettingModified        = false;
bool               s_HasRezEfx              = false;
bool               s_DoSavePosition			= false;

int                s_MemGemIndex            = 0;
int                s_TarBuffLineSize        = 0;
int                s_TestInt                = 100; // Color Test Value for Config Window

constexpr          MQColor DEF_BORDER_COL(255, 255, 255, 255);
constexpr          MQColor DEF_BENI_BORDER_COL(0, 0, 250, 255);
constexpr          MQColor DEF_DET_BORDER_COL(250, 0, 0, 255);
constexpr          MQColor DEF_SELF_CAST_BORDER_COL(250, 250, 0, 255);
constexpr          MQColor DEF_TINT_COL(255, 255, 255, 255);


ImGuiWindowFlags   s_WindowFlags            = ImGuiWindowFlags_None | ImGuiWindowFlags_NoFocusOnAppearing;
ImGuiWindowFlags   s_LockAllWin             = ImGuiWindowFlags_None;
ImGuiChildFlags    s_ChildFlags             = ImGuiChildFlags_None;

CTextureAnimation* s_pTASpellIcon           = nullptr;
CTextureAnimation* s_pGemHolder             = nullptr;
CTextureAnimation* s_pGemBackground         = nullptr;
CTextureAnimation* s_pMainTankIcon          = nullptr;
CTextureAnimation* s_pPullerIcon            = nullptr;
CTextureAnimation* s_pMainAssistIcon        = nullptr;
CTextureAnimation* s_pCombatIcon            = nullptr;
CTextureAnimation* s_pDebuffIcon            = nullptr;
CTextureAnimation* s_pRegenIcon             = nullptr;
CTextureAnimation* s_pStandingIcon          = nullptr;
CTextureAnimation* s_pTimerIcon             = nullptr;
CTextureAnimation* s_pStatusIcon            = nullptr;

void CleanUpIcons();

#pragma region Timers

std::chrono::steady_clock::time_point g_LastUpdateTime		= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime		= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastBuffFlashTime	= std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_StartCastTime;

const auto g_UpdateInterval			= std::chrono::milliseconds(500);

#pragma endregion

#pragma region Window Visibility Settings

struct WinSettings
{
	bool showMainWindow     = true;
	bool showConfigWindow   = false;
	bool showPetWindow      = false;
	bool showPlayerWindow   = false;
	bool showGroupWindow    = false;
	bool showSpellsWindow   = false;
	bool showTargetWindow   = false;
	bool showHud            = false;
	bool showBuffWindow     = false;
	bool showSongWindow     = false;
	bool flashCombatFlag    = false;
	bool flashTintFlag      = false;
	bool showTitleBars      = true;
	bool savePosition		= false;
	bool lockAllWin         = false;
	bool lockPlayerWin      = false;
	bool lockTargetWin      = false;
	bool lockPetWin         = false;
	bool lockGroupWin       = false;
	bool lockSpellsWin      = false;
	bool lockBuffsWin       = false;
	bool lockSongWin        = false;
	bool lockHudWin         = false;
	bool lockCastingWin     = false;
	bool showPetButtons     = true;
	bool showTargetBuffs    = true;
	bool showAggroMeter     = true;
	bool showSelfOnGroup    = false;
	bool showEmptyGroupSlot = true;
	bool flashSongTimer     = true;
	bool dockedPlayerWin = false;
	bool dockedTargetWin = false;
	bool dockedPetWin = false;
	bool dockedGroupWin = false;
	bool dockedSpellsWin = false;
	bool dockedBuffsWin = false;
	bool dockedSongWin = false;
	bool dockedHudWin = false;
	bool dockedCastingWin = false;
	}    s_WinSettings;

struct WinSetting
{
	const char* section;
	const char* key;
	bool* setting;
};

static std::vector<WinSetting> winSettings = {
	{"Settings",	"ShowMainGui",			&s_WinSettings.showMainWindow},
	{"Settings",	"ShowTitleBars",		&s_WinSettings.showTitleBars},
	{"Settings",    "SaveWinPos",			&s_WinSettings.savePosition},
	{"LockWindows",	"LockAllWindows",		&s_WinSettings.lockAllWin},
	{"LockWindows",	"LockPlayerWin",		&s_WinSettings.lockPlayerWin},
	{"LockWindows",	"LockTargetWin",		&s_WinSettings.lockTargetWin},
	{"LockWindows",	"LockPetWin",			&s_WinSettings.lockPetWin},
	{"LockWindows",	"LockGroupWin",			&s_WinSettings.lockGroupWin},
	{"LockWindows",	"LockSpellsWin",		&s_WinSettings.lockSpellsWin},
	{"LockWindows", "LockCastingWin",		&s_WinSettings.lockCastingWin},
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
	{"Songs",		"FlashSongTimer",		&s_WinSettings.flashSongTimer},
	{"Spells",		"ShowSpellsWindow",		&s_WinSettings.showSpellsWindow},
	{"Buffs",		"ShowBuffWindow",		&s_WinSettings.showBuffWindow},
	{"Hud",			"ShowHud",				&s_WinSettings.showHud},
	{"Docked",      "PlayerWinDocked",		&s_WinSettings.dockedPlayerWin},
	{"Docked",      "TargetWinDocked",		&s_WinSettings.dockedTargetWin},
	{"Docked",      "PetWinDocked",			&s_WinSettings.dockedPetWin},
	{"Docked",      "GroupWinDocked",		&s_WinSettings.dockedGroupWin},
	{"Docked",      "SpellsWinDocked",		&s_WinSettings.dockedSpellsWin},
	{"Docked",      "BuffsWinDocked",		&s_WinSettings.dockedBuffsWin},
	{"Docked",      "SongWinDocked",		&s_WinSettings.dockedSongWin},
	{"Docked",      "HudWinDocked",			&s_WinSettings.dockedHudWin},
	{"Docked",      "CastingWinDocked",		&s_WinSettings.dockedCastingWin},
};

#pragma endregion

#pragma region Window Size and Position Settings

struct WinSizeSettings
{
	float playerWinWidth   = 300.0f;
	float playerWinHeight  = 290.0f;
	float targetWinWidth   = 300.0f;
	float targetWinHeight  = 250.0f;
	float petWinWidth      = 300.0f;
	float petWinHeight     = 283.0f;
	float groupWinWidth    = 300.0f;
	float groupWinHeight   = 200.0f;
	float castingWinWidth = 300.0f;
	float castingWinHeight = 70.0f;
	float spellsWinWidth   = 79.0f;
	float spellsWinHeight  = 662.0f;
	float buffsWinWidth    = 210.0f;
	float buffsWinHeight   = 300.0f;
	float songWinWidth     = 210.0f;
	float songWinHeight    = 300.0f;
	float hudWinWidth      = 30.0f;
	float hudWinHeight     = 30.0f;
	// positions
	float playerWinX       = 200.0f;
	float playerWinY       = 20.0f;
	float targetWinX       = 20.0f;
	float targetWinY       = 20.0f;
	float petWinX          = 20.0f;
	float petWinY          = 20.0f;
	float groupWinX        = 200.0f;
	float groupWinY        = 200.0f;
	float castingWinX      = 300.0f;
	float castingWinY      = 300.0f;
	float spellsWinX       = 20.0f;
	float spellsWinY       = 220.0f;
	float buffsWinX        = 15.0f;
	float buffsWinY        = 10.0f;
	float songWinX         = 15.0f;
	float songWinY         = 310.0f;
	float hudWinX          = 300.0f;
	float hudWinY          = 10.0f;
} s_WinSizeSettings;

struct WinSizeSetting
{
	const char* section;
	const char* key;
	float* value;
};

std::vector<WinSizeSetting> winSizeSettings = {
	{"WinSizePosition",		"PlayerWinWidth",		&s_WinSizeSettings.playerWinWidth},
	{"WinSizePosition",		"PlayerWinHeight",		&s_WinSizeSettings.playerWinHeight},
	{"WinSizePosition",		"TargetWinWidth",		&s_WinSizeSettings.targetWinWidth},
	{"WinSizePosition",		"TargetWinHeight",		&s_WinSizeSettings.targetWinHeight},
	{"WinSizePosition",		"PetWinWidth",			&s_WinSizeSettings.petWinWidth},
	{"WinSizePosition",		"PetWinHeight",			&s_WinSizeSettings.petWinHeight},
	{"WinSizePosition",		"GroupWinWidth",		&s_WinSizeSettings.groupWinWidth},
	{"WinSizePosition",		"GroupWinHeight",		&s_WinSizeSettings.groupWinHeight},
	{"WinSizePosition",		"SpellsWinWidth",		&s_WinSizeSettings.spellsWinWidth},
	{"WinSizePosition",		"SpellsWinHeight",		&s_WinSizeSettings.spellsWinHeight},
	{"WinSizePosition",		"BuffWinWidth",			&s_WinSizeSettings.buffsWinWidth},
	{"WinSizePosition",		"BuffWinHeight",		&s_WinSizeSettings.buffsWinHeight},
	{"WinSizePosition",		"SongWinWidth",			&s_WinSizeSettings.songWinWidth},
	{"WinSizePosition",		"SongWinHeight",		&s_WinSizeSettings.songWinHeight},
	{"WinSizePosition",		"HudWinWidth",			&s_WinSizeSettings.hudWinWidth},
	{"WinSizePosition",		"HudWinHeight",			&s_WinSizeSettings.hudWinHeight},
	{"WinSizePosition",		"PlayerWinX",			&s_WinSizeSettings.playerWinX},
	{"WinSizePosition",		"PlayerWinY",			&s_WinSizeSettings.playerWinY},
	{"WinSizePosition",		"TargetWinX",			&s_WinSizeSettings.targetWinX},
	{"WinSizePosition",		"TargetWinY",			&s_WinSizeSettings.targetWinY},
	{"WinSizePosition",		"PetWinX",				&s_WinSizeSettings.petWinX},
	{"WinSizePosition",		"PetWinY",				&s_WinSizeSettings.petWinY},
	{"WinSizePosition",		"GroupWinX",			&s_WinSizeSettings.groupWinX},
	{"WinSizePosition",		"GroupWinY",			&s_WinSizeSettings.groupWinY},
	{"WinSizePosition",		"SpellsWinX",			&s_WinSizeSettings.spellsWinX},
	{"WinSizePosition",		"SpellsWinY",			&s_WinSizeSettings.spellsWinY},
	{"WinSizePosition",		"BuffWinX",				&s_WinSizeSettings.buffsWinX},
	{"WinSizePosition",		"BuffWinY",				&s_WinSizeSettings.buffsWinY},
	{"WinSizePosition",		"SongWinX",				&s_WinSizeSettings.songWinX},
	{"WinSizePosition",		"SongWinY",				&s_WinSizeSettings.songWinY},
	{"WinSizePosition",		"HudWinX",				&s_WinSizeSettings.hudWinX},
	{"WinSizePosition",		"HudWinY",				&s_WinSizeSettings.hudWinY},
	{"WinSizePosition",		"CastingWinWidth",		&s_WinSizeSettings.castingWinWidth},
	{"WinSizePosition",		"CastingWinHeight",		&s_WinSizeSettings.castingWinHeight},
	{"WinSizePosition",		"CastingWinX",			&s_WinSizeSettings.castingWinX},
	{"WinSizePosition",		"CastingWinY",			&s_WinSizeSettings.castingWinY},
};

#pragma region Numeric Settings

struct NumericSettings
	{
	int combatFlashInterval = 15;
	int flashBuffInterval   = 4;
	int playerBarHeight     = 12;
	int targetBarHeight     = 12;
	int aggroBarHeight      = 12;
	int groupBarHeight      = 12;
	int myAggroPct          = 0;
	int secondAggroPct      = 0;
	int buffIconSize        = 20;
	int buffTimerThreshold  = 0;
	int spellGemHeight      = 32;
	int hudAlpha            = 120;
	}   s_NumSettings;

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
	const  char* section;
	const  char* key;
	float* value;
	const  char* helpText;
};

std::vector<FontScaleSetting> fontScaleSettings = {
	{ "PlayerTarg", "PlayerWinScale",	&s_FontScaleSettings.playerWinScale,	"Player Window Scale" },
	{ "PlayerTarg", "TargetWinScale",	&s_FontScaleSettings.targetWinScale,	"Target Window Scale" },
	{ "Pet",		"PetWinScale",		&s_FontScaleSettings.petWinScale,		"Pet Window Scale" },
	{ "Group",		"GroupWinScale",	&s_FontScaleSettings.groupWinScale,		"Group Window Scale" },
	{ "Spells",		"SpellsWinScale",	&s_FontScaleSettings.spellsWinScale,	"Spells Window Scale" },
	{ "Buffs",		"BuffsWinScale",	&s_FontScaleSettings.buffsWinScale,		"Buffs Window Scale" },
};

#pragma endregion

#pragma region Theme and Color Settings

struct ThemeSettings
{
	std::string playerWinTheme = "Default";
	std::string petWinTheme    = "Default";
	std::string groupWinTheme  = "Default";
	std::string spellsWinTheme = "Default";
	std::string buffsWinTheme  = "Default";
	std::string songWinTheme   = "Default";
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
	MQColor minColorHP   = MQColor(223, 87, 255, 255);
	MQColor maxColorHP   = MQColor(216, 39, 39, 255);
	MQColor minColorMP   = MQColor(66, 29, 131, 255);
	MQColor maxColorMP   = MQColor(20, 119, 216, 255);
	MQColor minColorEnd  = MQColor(255, 111, 5, 255);
	MQColor maxColorEnd  = MQColor(178, 153, 26, 178);
	MQColor minColorCast = MQColor(216, 39, 39, 255);
	MQColor maxColorCast = MQColor(20, 119, 216, 255);
} s_BarColors;

struct ColorSetting
{
	const char* section;
	const char* key;
	MQColor* value;
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
	{"Flash Buff Speed",		&s_NumSettings.flashBuffInterval,	0, 50,		true,	"Buff Flash Speed: Lower is slower, Higher is faster. 0 = Disabled"},
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
	{"Pet Buttons",			&s_WinSettings.showPetButtons,		false,	"Pet Buttons: Show or Hide the pet command buttons"},
	{"Target Win",			&s_WinSettings.showTargetWindow,	false,	"Toggle Splitting the Target Window" },
	{"Target Buffs",		&s_WinSettings.showTargetBuffs,		false,	"Target Buffs: Show or Hide the target buffs"},
	{"Aggro Meter",			&s_WinSettings.showAggroMeter,		false,	"Aggro Meter: Show or Hide the aggro meter"},
	{"Group Show Self",		&s_WinSettings.showSelfOnGroup,		false,	"Group Show Self: Show or Hide Yourself on the group window"},
	{"Group Show Empty",	&s_WinSettings.showEmptyGroupSlot,	false,	"Group Show Empty: Show or Hide Empty Group Slots"},
	{"Flash Song Timer",	&s_WinSettings.flashSongTimer,		false,	"Flash Song Timer: Flash the song timer when it is about to expire"},
	{"Lock ALL",			&s_WinSettings.lockAllWin,			true,	"Lock Windows: Locks All Windows in place"},
	{"Lock Group",			&s_WinSettings.lockGroupWin,		true,	"Lock Group: Toggle locking the Group Window"},
	{"Lock Player",			&s_WinSettings.lockPlayerWin,		true,	"Lock Player: Toggle locking the Player Window"},
	{"Lock Target",			&s_WinSettings.lockTargetWin,		true,	"Lock Target: Toggle locking the Target Window"},
	{"Lock Pet",			&s_WinSettings.lockPetWin,			true,	"Lock Pet: Toggle locking the Pet Window"},
	{"Lock Spell",			&s_WinSettings.lockSpellsWin,		true,	"Lock Group: Toggle locking the Group Window"},
	{"Lock Buffs",			&s_WinSettings.lockBuffsWin,		true,	"Lock Player: Toggle locking the Player Window"},
	{"Lock Songs",			&s_WinSettings.lockSongWin,			true,	"Lock Target: Toggle locking the Target Window"},
	{"Lock Hud",			&s_WinSettings.lockHudWin,			true,	"Lock Pet: Toggle locking the Pet Window"},
	{"Lock Casting",		&s_WinSettings.lockCastingWin,		true,	"Lock Casting: Toggle locking the Casting Window"},
	{"Saved Position",		&s_WinSettings.savePosition,		true,	"Saved Position: Use local Stored saved Size and Positions. Does not Affect Docked Windows."},

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
	Config,
	Help
};

struct CommandInfo
{
	GrimCommand command;
	const char* commandText;
	const char* description;
};

const std::array<CommandInfo, 12> commandList = {
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
		{GrimCommand::Help,			"help",			"Displays this help message"}
	}
};

#pragma endregion

#pragma region Pet Buttons

struct PetButtonData
{
	char* name;
	char* command;
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

#pragma region Hud Stuff

struct StatusFXData
{
	eEQSPA spaValue;
	int iconID;
	bool positveFX;
	char* tooltip;
};


std::vector<StatusFXData> statusFXData = {
	{SPA_POISON,			42,		false,	"Poisoned"},
	{SPA_DISEASE,			41,		false,	"Diseased"},
	{SPA_BLINDNESS,			200,	false,	"Blind"},
	{SPA_ROOT,				117,	false,	"Rooted"},
	{SPA_CURSE,				159,	false,	"Cursed"},
	{SPA_SLOW,				17,		false,	"Slowed"},
	{SPA_CORRUPTION,		160,	false,	"Corrupted"},
	{SPA_SILENCE,			95,		false,	"Silenced"},
	{SPA_MOVEMENT_RATE,		5,		false,	"Snared"},
	{SPA_FEAR,				164,	false,	"Feared"},
	// invis effects
	{SPA_INVISIBILITY,		18,		true,	"Invisible"},
	{SPA_INVIS_VS_UNDEAD,	33,		true,	"Invis vs Undead"},
	{SPA_INVIS_VS_ANIMALS,	34,		true,	"Invis vs Animals"}
};

#pragma endregion

#pragma region Settings Functions

static void LoadSettings()
{
	// Load settings from the INI file

	for (const auto& setting : winSettings)
	{
		*setting.setting = GetPrivateProfileBool(setting.section, setting.key,
			*setting.setting, s_SettingsFile);
	}

	for (const auto& setting : numericSettings)
	{
		*setting.value = GetPrivateProfileInt(setting.section, setting.key,
			*setting.value, s_SettingsFile);
	}

	for (const auto& setting : themeSettings)
	{
		*setting.theme = GetPrivateProfileString(setting.section, setting.key,
			"Default", s_SettingsFile);
	}

	for (const auto& setting : colorSettings)
	{
		*setting.value = GetPrivateProfileColor(setting.section, setting.key,
			*setting.value, s_SettingsFile);
	}

	for (auto& button : petButtons)
	{
		button.visible = GetPrivateProfileBool("Pet", button.name,
			&button.visible, s_SettingsFile);
	}

	for (const auto& scale : fontScaleSettings)
	{
		*scale.value = GetPrivateProfileFloat("FontScale", scale.key,
			*scale.value, s_SettingsFile);
	}

	for (const auto& winsizes : winSizeSettings)
	{
		*winsizes.value = GetPrivateProfileFloat(winsizes.section, winsizes.key,
			*winsizes.value, s_SettingsFile);
	}
}

static void SaveSettings()
{
	for (const auto& setting : winSettings)
	{
		WritePrivateProfileBool(setting.section, setting.key, *setting.setting, s_SettingsFile);
	}

	for (const auto& setting : numericSettings)
	{
		WritePrivateProfileInt(setting.section, setting.key, *setting.value, s_SettingsFile);
	}

	for (const auto& setting : themeSettings)
	{
		WritePrivateProfileString(setting.section, setting.key, *setting.theme, s_SettingsFile);
	}

	for (const auto& setting : colorSettings)
	{
		WritePrivateProfileColor(setting.section, setting.key, *setting.value, s_SettingsFile);
	}

	for (const auto& button : petButtons)
	{
		WritePrivateProfileBool("Pet", button.name, button.visible, s_SettingsFile);
	}

	for (const auto& scale : fontScaleSettings)
	{
		WritePrivateProfileFloat("FontScale", scale.key, *scale.value, s_SettingsFile);
	}

	for (const auto& winsizes : winSizeSettings)
	{
		WritePrivateProfileFloat(winsizes.section, winsizes.key, *winsizes.value, s_SettingsFile);
	}

}

static void SaveSetting(bool* setting, const char* settingsFile)
{
	auto it = std::find_if(winSettings.begin(), winSettings.end(),
		[setting](const WinSetting& ws) { return ws.setting == setting; });
	if (it != winSettings.end())
		WritePrivateProfileBool(it->section, it->key, *setting, settingsFile);
}

static void SaveSetting(int* value, const char* settingsFile)
{
	auto it = std::find_if(numericSettings.begin(), numericSettings.end(),
		[value](const NumericSetting& ns) { return ns.value == value; });
	if (it != numericSettings.end())
		WritePrivateProfileInt(it->section, it->key, *value, settingsFile);
}

static void SaveSetting(std::string* theme, const char* settingsFile)
{
	auto it = std::find_if(themeSettings.begin(), themeSettings.end(),
		[theme](const ThemeSetting& ts) { return ts.theme == theme; });
	if (it != themeSettings.end())
		WritePrivateProfileString(it->section, it->key, *theme, settingsFile);
}

static void SaveSetting(MQColor* color, const char* settingsFile)
{
	auto it = std::find_if(colorSettings.begin(), colorSettings.end(),
		[color](const ColorSetting& cs) { return cs.value == color; });
	if (it != colorSettings.end()) 
		WritePrivateProfileColor(it->section, it->key, *color, settingsFile);
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
			if (pLocalPC)
			{

				char CharIniFile[MAX_PATH] = { 0 };
				fmt::format_to(CharIniFile, "{}/{}_{}_{}.ini", gPathConfig, PLUGIN_NAME, GetServerShortName(), pLocalPC->Name);

				std::error_code ec;
				if (!std::filesystem::exists(CharIniFile, ec))
				{

					// Check for character-specific file if missing then check for the default file to copy from in case edited at char select
					// this way we can copy their settings over. 
					// This allows you to set up the settings once and all characters can use that for a base.
					if (std::filesystem::exists(DEFAULT_INI, ec))
					{
						std::filesystem::copy_file(DEFAULT_INI, CharIniFile, ec);
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

				s_IsCaster = GetMaxMana() > 0;

				if (s_IsCaster) 	// new char logged in, load their spell book
				{
					if (!pSpellPicker)
						pSpellPicker = new SpellPicker();
					else 
						pSpellPicker->InitializeSpells();
				}
				s_DefaultLoaded = false;
			}
		}
	}
	else
	{
		if (s_CharIniLoaded || !s_DefaultLoaded)
		{
			s_CharIniLoaded = false;

			memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
			strcpy_s(s_SettingsFile, DEFAULT_INI.c_str());
			std::error_code ec;
			if (!std::filesystem::exists(DEFAULT_INI, ec))
				SaveSettings();

			LoadSettings();

			s_DefaultLoaded = true;
			s_IsCaster = false;

			if (pSpellPicker)
			{
				delete pSpellPicker;
				pSpellPicker = nullptr;
			}

			CleanUpIcons();
		}
	}
}

#pragma endregion

#pragma region Spells Stuff

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
				sprintf_s(timeLabel, size, "%dh %dm %ds", hours, minutes, seconds);
			else if (minutes > 0)
				sprintf_s(timeLabel, size, "%dh %dm", hours, minutes);
			else if (seconds > 0)
				sprintf_s(timeLabel, size, "%dh %ds", hours, seconds);
			else
				sprintf_s(timeLabel, size, "%dh", hours);
		}
		else if (minutes > 0)
		{
			if (seconds > 0)
				sprintf_s(timeLabel, size, "%dm %ds", minutes, seconds);
			else
				sprintf_s(timeLabel, size, "%dm", minutes);
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
void DrawBuffsTable(const char* name, IteratorRange<PlayerBuffInfoWrapper::Iterator<T>> Buffs,
	bool petBuffs = false, bool playerBuffs = false, bool isSong = false, int baseIndex = 0)
{

	bool sickFound = false;
	ImGui::SetWindowFontScale(s_FontScaleSettings.buffsWinScale);
	float sizeY = ImGui::GetContentRegionAvail().y;
	sizeY = sizeY - 10 > 0 ? sizeY - 10 : 1;

	if (ImGui::BeginTable("Buffs", 3, ImGuiTableFlags_Hideable |
		ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY,
		ImVec2(ImGui::GetContentRegionAvail().x, sizeY)))
	{
		int slotNum = 0;
		ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, static_cast<float>(s_NumSettings.buffIconSize));
		ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 65);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableHeadersRow();
		sickFound = false;
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

			if (!s_pTASpellIcon)
			{
				s_pTASpellIcon = new CTextureAnimation();
				if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
					*s_pTASpellIcon = *temp;
			}

			if (spell)
				{
					s_pTASpellIcon->SetCurCell(spell->SpellIcon);
					MQColor borderCol = DEF_BENI_BORDER_COL; // Default color blue (beneficial)
					MQColor tintCol = DEF_TINT_COL;
					if (!spell->IsBeneficialSpell())
						borderCol = DEF_DET_BORDER_COL; // Red for detrimental spells

					if (!playerBuffs)
					{
						if (ci_equals(buffInfo.GetCaster(), pLocalPC->Name) && !spell->IsBeneficialSpell())
							borderCol = DEF_SELF_CAST_BORDER_COL; // Yellow for spells cast by me
					}

					int secondsLeft = buffInfo.GetBuffTimer() / 1000;
					if (!isSong || isSong && s_WinSettings.flashSongTimer)
					{
						if (secondsLeft < 18 && secondsLeft > 0 && !petBuffs)
						{
							if (s_WinSettings.flashTintFlag)
								tintCol = MQColor(0, 0, 0, 255);

						}
					}
					ImGui::PushID(spell->ID);
					ImGui::Selectable("##", false, ImGuiSelectableFlags_SpanAllColumns);
					if (ImGui::BeginPopupContextItem(("BuffPopup##" + std::to_string(spell->ID)).c_str(), ImGuiPopupFlags_MouseButtonRight))
					{
						ImGui::SetWindowFontScale(s_FontScaleSettings.buffsWinScale);

						if (ImGui::MenuItem(("Remove##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
							RemoveBuffByName(spell->Name);

						if (ImGui::MenuItem(("Block##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
							DoCommandf("/blockspell add me %d", spell->ID);

						if (ImGui::MenuItem(("Inspect##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
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

							ImGui::Text("Right Click for Options");
						}
						ImGui::EndTooltip();
						if (ImGui::IsMouseClicked(0))
						{
							if (ci_equals(buffInfo.GetCaster(), pLocalPC->Name))
							{
								DoCommandf("/cast \"%s\"", spell->Name);
							}
						}

					}
					ImGui::SameLine(0.0f, 0.0f);
					imgui::DrawTextureAnimation(s_pTASpellIcon, CXSize(s_NumSettings.buffIconSize, s_NumSettings.buffIconSize), tintCol, borderCol);

					ImGui::TableNextColumn();
					if (secondsLeft < s_NumSettings.buffTimerThreshold || s_NumSettings.buffTimerThreshold == 0)
					{
						char timeLabel[64];
						FormatBuffDuration(timeLabel, 64, buffInfo.GetBuffTimer());
						ImGui::TextColored(COLOR_TANGERINE.ToImColor(), "%s", timeLabel);
					}
					ImGui::TableNextColumn();

					ImGui::Text("%s", spell->Name);

					ImGui::PopID();

					if (!sickFound)
					{
						if (eqlib::IsResEffectSpell(spell->ID) || spell->ID == 757)
							sickFound = true;
					}
				}
			slotNum++;
			s_HasRezEfx = sickFound;

		}

		ImGui::EndTable();
	}
}

template <typename T>
void DrawBuffsIconList(const char* name, IteratorRange<PlayerBuffInfoWrapper::Iterator<T>> Buffs,
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

		if (!s_pTASpellIcon)
		{
			s_pTASpellIcon = new CTextureAnimation();
			if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
				*s_pTASpellIcon = *temp;
		}

		int sizeX = static_cast<int>(ImGui::GetContentRegionAvail().x);
		s_TarBuffLineSize = 0;
		if (spell)
		{
			s_pTASpellIcon->SetCurCell(spell->SpellIcon);
			MQColor borderCol = DEF_BENI_BORDER_COL;
			MQColor tintCol = DEF_TINT_COL;
			if (!spell->IsBeneficialSpell())
				borderCol = DEF_DET_BORDER_COL;

			if (ci_equals(buffInfo.GetCaster(), pLocalPC->Name) && !spell->IsBeneficialSpell())
				borderCol = DEF_SELF_CAST_BORDER_COL;

			int secondsLeft = buffInfo.GetBuffTimer() / 1000;
			if (secondsLeft < 18 && secondsLeft > 0 && !petBuffs)
			{
				if (s_WinSettings.flashTintFlag)
					tintCol = MQColor(0, 0, 0, 255);
			}

			imgui::DrawTextureAnimation(s_pTASpellIcon, CXSize(s_NumSettings.buffIconSize, s_NumSettings.buffIconSize), tintCol, borderCol);
			s_TarBuffLineSize += s_NumSettings.buffIconSize + 2;
			if (s_TarBuffLineSize < sizeX - 20)
				ImGui::SameLine(0.0f, 2);
			else
				s_TarBuffLineSize = 0;

		}
		ImGui::PopID();
		if (ImGui::BeginPopupContextItem(("BuffPopup##" + std::to_string(spell->ID)).c_str()))
		{

			if (ImGui::MenuItem(("Inspect##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
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

static bool IsGemReady(int ID)
{
	if (GetSpellByID(GetMemorizedSpell(ID)))
	{
		if (pDisplay->TimeStamp > pLocalPlayer->SpellGemETA[ID] && pDisplay->TimeStamp > pLocalPlayer->GetSpellCooldownETA())
			return true;
	}
	return false;
}

#pragma endregion

#pragma region Utility Functions

const char* MaskName(const char* name)
{
	static char anonymizedName[32];
	if (name && name[0] != '\0')
		snprintf(anonymizedName, sizeof(anonymizedName), "%c****", name[0]);
	else
		snprintf(anonymizedName, sizeof(anonymizedName), "****");

	return anonymizedName;
}

const SettingToggleOption* FindToggleOption(const std::vector<SettingToggleOption>& options, const char* targetLabel)
{
	for (const auto& option : options)
	{
		if (option.label && ci_equals(option.label, targetLabel))
			return &option;
	}
	return nullptr;
}

void CleanUpIcons()
{

	if (s_pTASpellIcon)
	{
		delete s_pTASpellIcon;
		s_pTASpellIcon = nullptr;
	}
	if (s_pGemBackground)
	{
		delete s_pGemBackground;
		s_pGemBackground = nullptr;
	}
	if (s_pGemHolder)
	{
		delete s_pGemHolder;
		s_pGemHolder = nullptr;
	}
	if (s_pStatusIcon)
	{
		delete s_pStatusIcon;
		s_pStatusIcon = nullptr;
	}
}

void LockAll()
{
	for (const auto& lockWin : settingToggleOptions)
	{
		if (lockWin.label && strncmp(lockWin.label, "Lock ", 5) == 0)
		{
			if (!(lockWin.label && strncmp(lockWin.label, "Lock ALL", 9) == 0))
				*lockWin.setting = s_WinSettings.lockAllWin;
		}
	}
	SaveSettings();
}

void GetHeading()
{
	if (PlayerClient* pSelfInfo = pLocalPlayer)
		s_CurrHeading = szHeadingShort[static_cast<int>((pLocalPlayer->Heading / 32.0f) + 8.5f) % 16];
}


void PrintGrimHelp()
{
	for (const auto& cmdInfo : commandList)
	{
		WriteChatf("\aw[\ayGrimGUI\ax] \at/grimgui\ax [\ag%s\ax] - \ay%s", cmdInfo.commandText, cmdInfo.description);
	}
}

void GiveItem(PlayerClient* pSpawn)
{
	if (!pSpawn)
		return;

	pTarget = pSpawn;

	if (ItemPtr pItem = GetPcProfile()->GetInventorySlot(InvSlot_Cursor))
		pEverQuest->LeftClickedOnPlayer(pSpawn);

}

void GrimCommandHandler(PlayerClient* pPC, const char* szLine)
{
	char arg[15] = { 0 };
	GetArg(arg, szLine, 1);

	if (arg[0] != '\0')
	{

		GrimCommand command;
		if (ci_equals(arg, "show"))
			command = GrimCommand::Show;
		else if (ci_equals(arg, "lock"))
			command = GrimCommand::Lock;
		else if (ci_equals(arg, "player"))
			command = GrimCommand::Player;
		else if (ci_equals(arg, "target"))
			command = GrimCommand::Target;
		else if (ci_equals(arg, "pet"))
			command = GrimCommand::Pet;
		else if (ci_equals(arg, "group"))
			command = GrimCommand::Group;
		else if (ci_equals(arg, "spells"))
			command = GrimCommand::Spells;
		else if (ci_equals(arg, "buffs"))
			command = GrimCommand::Buffs;
		else if (ci_equals(arg, "songs"))
			command = GrimCommand::Songs;
		else if (ci_equals(arg, "hud"))
			command = GrimCommand::Hud;
		else if (ci_equals(arg, "config"))
			command = GrimCommand::Config;
		else
			command = GrimCommand::Help;

		switch (command)
		{
			case GrimCommand::Help:
				PrintGrimHelp();
				break;
			case GrimCommand::Show:
				if (GetGameState() != GAMESTATE_INGAME)
					s_ShowOutOfGame = true;
				s_WinSettings.showMainWindow = !s_WinSettings.showMainWindow;
				SaveSetting(&s_WinSettings.showMainWindow, s_SettingsFile);
				break;
			case GrimCommand::Lock:
				s_WinSettings.lockAllWin = !s_WinSettings.lockAllWin;
				SaveSetting(&s_WinSettings.lockAllWin, s_SettingsFile);
				break;
			case GrimCommand::Player:
				s_WinSettings.showPlayerWindow = !s_WinSettings.showPlayerWindow;
				SaveSetting(&s_WinSettings.showPlayerWindow, s_SettingsFile);
				break;
			case GrimCommand::Target:
				s_WinSettings.showTargetWindow = !s_WinSettings.showTargetWindow;
				SaveSetting(&s_WinSettings.showTargetWindow, s_SettingsFile);
				break;
			case GrimCommand::Pet:
				s_WinSettings.showPetWindow = !s_WinSettings.showPetWindow;
				SaveSetting(&s_WinSettings.showPetWindow, s_SettingsFile);
				break;
			case GrimCommand::Group:
				s_WinSettings.showGroupWindow = !s_WinSettings.showGroupWindow;
				SaveSetting(&s_WinSettings.showGroupWindow, s_SettingsFile);
				break;
			case GrimCommand::Spells:
				s_WinSettings.showSpellsWindow = !s_WinSettings.showSpellsWindow;
				SaveSetting(&s_WinSettings.showSpellsWindow, s_SettingsFile);
				break;
			case GrimCommand::Buffs:
				s_WinSettings.showBuffWindow = !s_WinSettings.showBuffWindow;
				SaveSetting(&s_WinSettings.showBuffWindow, s_SettingsFile);
				break;
			case GrimCommand::Songs:
				s_WinSettings.showSongWindow = !s_WinSettings.showSongWindow;
				SaveSetting(&s_WinSettings.showSongWindow, s_SettingsFile);
				break;
			case GrimCommand::Config:
				s_WinSettings.showConfigWindow = !s_WinSettings.showConfigWindow;
				break;
			case GrimCommand::Hud:
				s_WinSettings.showHud = !s_WinSettings.showHud;
				SaveSetting(&s_WinSettings.showHud, s_SettingsFile);
				break;
			default:
				PrintGrimHelp();
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

#pragma region Color utility functions

/**
* @fn CalculateProgressiveColor
*
* @brief Function to calculate a color between two colors based on a value between 0 and 100
*
* @param minColor const MQColor& Minimum color value
* @param maxColor const MQColor& Maximum color value
* @param value int Value between 0 and 100 to calculate the color between minColor and maxColor
*
* @return ImVec4 color value Since this is what is used and returned by the picker.
*/
ImVec4 CalculateProgressiveColor(const MQColor& minColor, const MQColor& maxColor, int value)
{
	value = std::max(0, std::min(100, value));

	float r, g, b, a;

	float proportion = static_cast<float>(value) / 100;
	r = (minColor.Red) + proportion * ((maxColor.Red) - (minColor.Red));
	g = (minColor.Green) + proportion * ((maxColor.Green) - (minColor.Green));
	b = (minColor.Blue) + proportion * ((maxColor.Blue) - (minColor.Blue));
	a = (minColor.Alpha) + proportion * ((maxColor.Alpha) - (minColor.Alpha));

	return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}


/**
* @fn CalculateProgressiveColor
*
* @brief Function to calculate a color between two colors based on a value between 0 and 100
*
* @param minColor const MQColor& Minimum color value
* @param maxColor const MQColor& Maximum color value
* @param value int Value between 0 and 100 to calculate the color between minColor and maxColor
* @param midColor const MQColor& midColor value to calculate in two segments
* @param midValue int Optional midValue to split the value between minColor and maxColor
*
* @return ImVec4 color value Since this is what is used and returned by the picker.
*/
ImVec4 CalculateProgressiveColor(const MQColor& minColor, const MQColor& maxColor, int value, const MQColor* midColor , int midValue)
{
	value = std::max(0, std::min(100, value));

	float r, g, b, a;

		if (value > midValue)
		{
			float proportion = static_cast<float>(value - midValue) / (100 - midValue);
			r = (midColor->Red) + proportion * ((maxColor.Red) - (midColor->Red));
			g = (midColor->Green) + proportion * ((maxColor.Green) - (midColor->Green));
			b = (midColor->Blue) + proportion * ((maxColor.Blue) - (midColor->Blue));
			a = (midColor->Alpha) + proportion * ((maxColor.Alpha) - (midColor->Alpha));
		}
		else
		{
			float proportion = static_cast<float>(value) / midValue;
			r = (minColor.Red) + proportion * ((midColor->Red) - (minColor.Red));
			g = (minColor.Green) + proportion * ((midColor->Green) - (minColor.Green));
			b = (minColor.Blue) + proportion * ((midColor->Blue) - (minColor.Blue));
			a = (minColor.Alpha) + proportion * ((midColor->Alpha) - (minColor.Alpha));
		}
	
	return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
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
constexpr MQColor COLOR_YELLOW2(135, 135, 8, 253);
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

constexpr MQColor GetConColor(int color_code)
{
	switch (color_code)
	{
	case CONCOLOR_GREY:		 return COLOR_GREY; 
	case CONCOLOR_GREEN:	 return COLOR_GREEN; 
	case CONCOLOR_LIGHTBLUE: return COLOR_SOFT_BLUE;
	case CONCOLOR_BLUE:		 return COLOR_BLUE; 
	case CONCOLOR_BLACK:	 return COLOR_WHITE; 
	case CONCOLOR_WHITE:	 return COLOR_WHITE; 
	case CONCOLOR_YELLOW:	 return COLOR_YELLOW; 
	case CONCOLOR_RED:		 return COLOR_RED; 

		// Default color if the color code doesn't match any known values
	default: return COLOR_DEFAULT_WHITE;
	}
}

#pragma endregion

#pragma region Some ImGui Stuff *Draw functions*

static void DrawSpellBarIcons(int gemHeight)
{
	if (!pLocalPC)
		return;

	if (!s_pTASpellIcon)
	{
		s_pTASpellIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
			*s_pTASpellIcon = *temp;
	}

	if (!s_pGemBackground)
	{	
		s_pGemBackground = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGemBackground"))
			*s_pGemBackground = *temp;
	}
	if (!s_pGemHolder)
	{
		s_pGemHolder = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGemHolder"))
			*s_pGemHolder = *temp;
	}

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

		if (spellId == -1)
		{

			if (s_pGemHolder)
			{
				imgui::DrawTextureAnimation(s_pGemHolder, gemSize);

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
				s_pTASpellIcon->SetCurCell(spell->SpellIcon);
				MQColor gemTint;
				MQColor tintCol = DEF_TINT_COL;
				gemTint = pSpellGem->SpellGemTintArray[pSpellGem->TintIndex];

				if (!IsGemReady(i))
					tintCol = MQColor(50, 50, 50, 255);

				ImGui::BeginGroup();
				float posX = ImGui::GetCursorPosX();
				float posY = ImGui::GetCursorPosY();

				ImGui::SetCursorPos(ImVec2(posX + ((gemSize.cx - spellIconSize) / 2), posY + ((gemSize.cy - spellIconSize) / 2)));
				imgui::DrawTextureAnimation(s_pTASpellIcon, CXSize(spellIconSize, spellIconSize), tintCol);
				ImGui::SetCursorPos(ImVec2(posX, posY));
				imgui::DrawTextureAnimation(s_pGemBackground, gemSize, gemTint);

				posX = ImGui::GetCursorPosX();
				posY = ImGui::GetCursorPosY();

				if (!IsGemReady(i))
				{
					// gem not ready cooldown timer
					int coolDown = (pLocalPlayer->SpellGemETA[i] - pDisplay->TimeStamp) / 1000;
					if (coolDown < 1801)
					{
						ImVec2 textSize = ImGui::CalcTextSize(std::to_string(coolDown).c_str());
						float centeredX = posX + static_cast<float>(gemSize.cx - textSize.x) / 2.0f;
						float centeredY = posY - static_cast<float>(gemSize.cy * 0.75);
						ImGui::SetCursorPos(ImVec2(centeredX, centeredY));
						ImGui::TextColored(COLOR_TEAL.ToImColor(), "%d", coolDown);
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
							ImGui::TextColored(COLOR_TEAL.ToImColor(), "Mana: %d", spell->ManaCost);
							ImGui::Text("Recast: %d", spell->RecastTime / 1000);
							ImGui::Spacing();
							ImGui::Text("Ctrl + Click to pick up gem");
							ImGui::Text("Alt + Click to inspect spell");
						}
					}
					ImGui::EndTooltip();

					if (ImGui::IsMouseClicked(0) && ImGui::IsKeyDown(ImGuiKey_ModAlt))
					{
						DoInspectSpell(spellId);
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

static void DrawLockOption(const char* winName)
{
	if (!winName) return;

	const std::string targetLabel = fmt::format("Lock {}", winName);
	const SettingToggleOption* option = FindToggleOption(settingToggleOptions, targetLabel.c_str());

	if (option)
	{
		const char* lockIcon = *option->setting ? ICON_FA_LOCK : ICON_FA_UNLOCK;

		if(ImGui::SmallButton(fmt::format("{}##{}",lockIcon, winName).c_str()))
		{
			if (*option->setting && s_WinSettings.lockAllWin)
			{
				s_WinSettings.lockAllWin = false;
				SaveSetting(&s_WinSettings.lockAllWin, s_SettingsFile);
			}

			*option->setting = !*option->setting;
			SaveSetting(option->setting, s_SettingsFile);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("%s %s Window", *option->setting ? "Unlock" : "Lock", winName);
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
				char* btnLabel = button.name;

				bool isAttacking = false;
				bool isSitting = false;
				if (PlayerClient* MyPet = GetSpawnByID(pLocalPlayer->PetID))
				{
					isAttacking = MyPet->WhoFollowing;
					isSitting = MyPet->StandState != 100;
				}

				if ((ci_equals(btnLabel, "Attack") && isAttacking) || (ci_equals(btnLabel ,"Sit") && isSitting))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, COLOR_TEAL.ToImU32());
					if (ImGui::Button(btnLabel, btnSize))
						DoCommandf(button.command);

					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(btnLabel, btnSize))
						DoCommandf(button.command);
				}
			}
		}
		ImGui::EndTable();
	}
}

static bool CheckWinPos(float &x, float &y, float& w, float& h, ImVec2 curPos, ImVec2 curSize)
{
	bool changed = false;
	if (x != curPos.x || y != curPos.y)
	{
		x = curPos.x;
		y = curPos.y;
		changed = true;
	}
	if (w != curSize.x || h != curSize.y)
	{
		w = curSize.x;
		h = curSize.y;
		changed = true;
	}
	return changed;
}



/**
* @fn DrawLineOfSight
*
* @brief Draws a line of sight indicator based on the result of the LineOfSight function
*
* @param pFrom PlayerClient Pointer to the source spawn
* @param pTo PlayerClient Pointer to the target spawn
*/
void DrawLineOfSight(PlayerClient* pFrom, PlayerClient* pTo)
{
	if (LineOfSight(pFrom, pTo))
		ImGui::TextColored(COLOR_GREEN.ToImColor(), ICON_MD_VISIBILITY);
	else
		ImGui::TextColored(COLOR_RED.ToImColor(), ICON_MD_VISIBILITY_OFF);
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
	if (!s_pStatusIcon)
	{
		s_pStatusIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellIcons"))
			*s_pStatusIcon = *temp;
	}

	bool efxflag = false;
	CXSize iconSize(30, 30);
	MQColor borderCol = DEF_DET_BORDER_COL;
	MQColor tintCol = DEF_TINT_COL;

	if (pLocalPlayer->StandState == STANDSTATE_SIT)
	{
		efxflag = true;
		//m_StatusIcon->SetCurCell(93);
		//imgui::DrawTextureAnimation(m_StatusIcon, iconSize, tintCol, borderCol);
		ImGui::SetWindowFontScale(2.0f);
		ImGui::TextColored(COLOR_TANGERINE.ToImColor(), ICON_FA_MOON_O);
		ImGui::SetWindowFontScale(1.0f);
		if (ImGui::IsItemClicked())
			pLocalPlayer->StandState = STANDSTATE_STAND;
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Sitting");
		ImGui::SameLine();
	}

	for (const auto& debuff : statusFXData)
	{
		int check = GetSelfBuff(SpellAffect(debuff.spaValue, debuff.positveFX));
		if (check >= 0)
		{

			efxflag = true;
			s_pStatusIcon->SetCurCell(debuff.iconID);
			if (debuff.positveFX)
			{
				borderCol = COLOR_SOFT_BLUE;
			}
			imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
			if (ImGui::IsItemHovered())
				ImGui::SetItemTooltip(debuff.tooltip);
			ImGui::SameLine();
		}
		borderCol = DEF_DET_BORDER_COL;
	}

	if (GetSelfBuff([](EQ_Spell* spell) { return SpellAffect(SPA_HP, false)(spell) && spell->IsDetrimentalSpell() && spell->IsDoTSpell(); }) >= 0)
	{
		efxflag = true;
		s_pStatusIcon->SetCurCell(140);
		imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Dotted");
		ImGui::SameLine();

	}

	if (GetSelfBuff(SpellSubCat(SPELLCAT_RESIST_DEBUFFS) && SpellClassMask(Shaman, Mage)) >= 0)
	{
		efxflag = true;
		s_pStatusIcon->SetCurCell(55);
		imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Malo");
		ImGui::SameLine();
	}

	if (GetSelfBuff(SpellSubCat(SPELLCAT_RESIST_DEBUFFS) && SpellClassMask(Enchanter)) >= 0)
	{
		efxflag = true;
		s_pStatusIcon->SetCurCell(72);
		imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Tash");
		ImGui::SameLine();
	}

	if (s_HasRezEfx)
	{
		efxflag = true;
		s_pStatusIcon->SetCurCell(154);
		imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Resurrection Sickness");
		ImGui::SameLine();

	}

	if (pLocalPlayer->StandState == STANDSTATE_FEIGN)
	{
		efxflag = true;
		s_pStatusIcon->SetCurCell(92);
		imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Feign Death");
		if (ImGui::IsItemClicked())
			pLocalPlayer->StandState = STANDSTATE_STAND;

		ImGui::SameLine();
	}

	if (pLocalPC->Stunned)
	{
		efxflag = true;
		s_pStatusIcon->SetCurCell(25);
		imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Stunned");
		ImGui::SameLine();
	}

	if (pLocalPC->bIsCorrupted)
	{
		efxflag = true;
		s_pStatusIcon->SetCurCell(160);
		imgui::DrawTextureAnimation(s_pStatusIcon, iconSize, tintCol, borderCol);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Corrupted");
		ImGui::SameLine();
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
		s_pMainTankIcon = pSidlMgr->FindAnimation("A_Tank");
		imgui::DrawTextureAnimation(s_pMainTankIcon, iconSize);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Main Tank");

	}
	if (pMember->IsMainAssist())
	{
		s_pMainAssistIcon = pSidlMgr->FindAnimation("A_Assist");
		imgui::DrawTextureAnimation(s_pMainAssistIcon, iconSize);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Main Assist");
	}
	if (pMember->IsPuller())
	{
		s_pPullerIcon = pSidlMgr->FindAnimation("A_Puller");
		imgui::DrawTextureAnimation(s_pPullerIcon, iconSize);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Puller");
	}
	if (pMember == pLocalPC->pGroupInfo->GetGroupLeader())
	{
		ImGui::TextColored(COLOR_TEAL.ToImColor(), ICON_MD_STAR);
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
		s_pCombatIcon = pSidlMgr->FindAnimation("A_PWCSInCombat");
		imgui::DrawTextureAnimation(s_pCombatIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("In Combat");
		break;
	case eCombatState_Debuff:
		s_pDebuffIcon = pSidlMgr->FindAnimation("A_PWCSDebuff");
		imgui::DrawTextureAnimation(s_pDebuffIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("You are Debuffed. You may not rest.");
		break;
	case eCombatState_Timer:
		s_pTimerIcon = pSidlMgr->FindAnimation("A_PWCSTimer");
		imgui::DrawTextureAnimation(s_pTimerIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("You are recovering from combat, and may not rest yet!");
		break;
	case eCombatState_Standing:
		s_pStandingIcon = pSidlMgr->FindAnimation("A_PWCSStanding");
		imgui::DrawTextureAnimation(s_pStandingIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("You are not in combat. You may rest at any time.");
		break;
	case eCombatState_Regen:
		s_pRegenIcon = pSidlMgr->FindAnimation("A_PWCSRegen");
		imgui::DrawTextureAnimation(s_pRegenIcon, iconSize);
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("You are resting.");
		break;
	default:
		break;
	}
}


void DrawMenu(const char* winName)
{
	const char* lockAllIcon = s_WinSettings.lockAllWin ? ICON_MD_LOCK : ICON_FA_UNLOCK;
	if (ImGui::BeginMenuBar())
	{

		// lock Icon for winName
		DrawLockOption(winName);

		// Config Window Icon
		ImGui::SameLine();
		if(ImGui::SmallButton(ICON_FA_COG))
			s_WinSettings.showConfigWindow = !s_WinSettings.showConfigWindow;

		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("Config Window");

		// the main menu drop downs
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::BeginMenu("Windows"))
			{

				for (const auto& window : options)
				{
					ImGui::MenuItem(window.label, nullptr, window.setting);
				}

				ImGui::Separator();
				ImGui::MenuItem("Main Window", nullptr, s_WinSettings.showMainWindow);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Locking"))
			{
				for (const auto& lockWin : settingToggleOptions)
				{
					if (lockWin.lockSetting)
					{
						if (ImGui::MenuItem(lockWin.label, nullptr, lockWin.setting))
						{
							if (lockWin.label && ci_equals(lockWin.label, "Lock ALL"))
								LockAll();
						}
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Other"))
			{
				for (const auto& lockWin : settingToggleOptions)
				{
					if (!lockWin.lockSetting)
					{
						ImGui::MenuItem(lockWin.label, nullptr, lockWin.setting);
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		
		// Lock All Windows Icon
		ImGui::SameLine();
		
		float winWidth = ImGui::GetWindowWidth();
		if (winWidth > 140)
		{
			ImGui::SetCursorPosX(winWidth - 40);
		}
		ImGui::PushStyleColor(ImGuiCol_Button, COLOR_BTN_RED.ToImU32());
		if (ImGui::SmallButton(lockAllIcon))
		{
			s_WinSettings.lockAllWin = !s_WinSettings.lockAllWin;
			LockAll();
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
			ImGui::SetItemTooltip("%s All Windows", s_WinSettings.lockAllWin ? "Unlock" : "Lock");
		
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
	const MQColor minColor, const MQColor maxColor, const char* tooltip)
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


void DrawPetInfo(PlayerClient* petInfo, bool showAll = true)
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
			ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoScrollbar))
		{
			DrawLineOfSight(pLocalPlayer, petInfo);
			ImGui::SameLine();
			ImGui::Text("Lvl");
			ImGui::SameLine();
			ImGui::TextColored(COLOR_TEAL.ToImColor(), "%d", petInfo->Level);
			ImGui::SameLine();
			ImGui::Text("Dist:");
			ImGui::SameLine();
			ImGui::TextColored(COLOR_TANGERINE.ToImColor(), "%0.0f m", GetDistance(pLocalPlayer, petInfo));

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
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, COLOR_GREEN2.ToImU32());
			ImGui::ProgressBar(petPercentage, ImVec2(ImGui::GetColumnWidth(), barSize), "##");
			ImGui::PopStyleColor();
		}
		ImGui::EndChild();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetItemTooltip("%s  %d%", petName, petInfo->HPCurrent);

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			GiveItem(GetSpawnByID(petInfo->GetId()));
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
void DrawPlayerBars(bool drawCombatBorder = false, int barHeight = s_NumSettings.playerBarHeight,
	bool drawPet = false, float fontScale = 1.0f)
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
			MQColor borderColor = s_WinSettings.flashCombatFlag ? DEF_DET_BORDER_COL : DEF_BORDER_COL;
			ImGui::PushStyleColor(ImGuiCol_Border, borderColor.ToImU32());
		}
		else if (drawCombatBorder)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, DEF_BORDER_COL.ToImU32());
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));

		if (ImGui::BeginChild("info", ImVec2(ImGui::GetContentRegionAvail().x, 26 * fontScale),
			s_ChildFlags | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoScrollbar))
		{

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
				if (pLocalPC->pGroupInfo)
				{
					CGroupMember* pMember = pLocalPC->pGroupInfo->GetGroupMember(pLocalPlayer);
					ImGui::SameLine(0.0f, 10.0f);
					DrawPlayerIcons(pMember, fontScale);
				}
				ImGui::TableNextColumn();
				ImGui::TextColored(COLOR_YELLOW.ToImColor(), s_CurrHeading);
				ImGui::TableNextColumn();
				ImGui::Text("Lvl: %d", pLocalPC->GetLevel());
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();

		ImGui::PopStyleVar(2);
		if (drawCombatBorder)
			ImGui::PopStyleColor();

		// Health bar because if we have no health we are dead!
		DrawBar("##hp", GetCurHPS(), GetMaxHPS(), barHeight, s_BarColors.minColorHP, s_BarColors.maxColorHP, "HP");

		// Mana bar if you have mana that is
		if (s_IsCaster)
			DrawBar("##Mana", GetCurMana(), GetMaxMana(), barHeight, s_BarColors.minColorMP, s_BarColors.maxColorMP, "Mana");

		// Endurance bar does anyone even use this?
		DrawBar("##Endur", GetCurEndurance(), GetMaxEndurance(), barHeight, s_BarColors.minColorEnd, s_BarColors.maxColorEnd, "Endur");

		if (drawPet)
		{
			// Go Fluffy Go!
			if (PlayerClient* MyPet = GetSpawnByID(pLocalPlayer->PetID))
				DrawPetInfo(MyPet, false);
		}
	}
	//ImGui::SetWindowFontScale(1.0f);
	ImGui::EndChild();
	if (pLocalPC->pGroupInfo && pLocalPC->Name == pLocalPC->pGroupInfo->GetGroupLeader()->Name)
	{
		if (ImGui::BeginPopupContextItem(("##%s", pLocalPC->Name)))
		{
			if (ImGui::BeginMenu("Roles"))
			{
				if (ImGui::MenuItem("Main Assist"))
					DoCommandf("/grouproles set %s 2", pLocalPC->Name);

				if (ImGui::MenuItem("Main Tank"))
					DoCommandf("/grouproles set %s 1", pLocalPC->Name);

				if (ImGui::MenuItem("Puller"))
					DoCommandf("/grouproles set %s 3", pLocalPC->Name);

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
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

		if (PlayerClient* pSpawn = pMember->GetPlayer())
			distToMember = GetDistance(pLocalPlayer, pSpawn);

		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, (ImGui::CalcTextSize(nameLabel).x + ImGui::CalcTextSize(ICON_MD_VISIBILITY_OFF).x));
		ImGui::TableSetupColumn("Vis", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize(ICON_MD_VISIBILITY_OFF).x);
		ImGui::TableSetupColumn("Roles");
		ImGui::TableSetupColumn("Dist", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 30 * s_FontScaleSettings.groupWinScale);
		ImGui::TableSetupColumn("Lvl", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 30 * s_FontScaleSettings.groupWinScale);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImGui::Text(nameLabel);

		if (PlayerClient* pSpawn = pMember->GetPlayer())
		{
			if (pSpawn->StandState && pSpawn->StandState == STANDSTATE_SIT)
			{
				ImGui::SameLine();
				ImGui::TextColored(COLOR_TANGERINE.ToImColor(), ICON_FA_MOON_O);
				if (ImGui::IsItemHovered())
					ImGui::SetItemTooltip("Sitting");
			}
		}

		ImGui::TableNextColumn();
		if (PlayerClient* pSpawn = pMember->GetPlayer())
		{
			DrawLineOfSight(pLocalPlayer, pSpawn);
		}
		else
		{
			ImGui::TextColored(COLOR_RED.ToImColor(), ICON_MD_VISIBILITY_OFF);
		}
		ImGui::TableNextColumn();
		DrawPlayerIcons(pMember, fontScale);
		ImGui::TableNextColumn();
		ImGui::TextColored(COLOR_TANGERINE.ToImColor(), "%0.0f m", distToMember);
		ImGui::TableNextColumn();
		int lvl = 999;
		if (PlayerClient* pSpawn = pMember->GetPlayer())
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
			const char* memberName = pMember->GetName();
			if (ImGui::BeginPopupContextItem(("##%s", memberName)))
			{
				if (s_DanNetEnabled)
				{
					if (ImGui::MenuItem("Switch To"))
					{
						DoCommandf("/dex %s /foreground", memberName);
					}
				}

				ImGui::EndPopup();
			}
		}
		ImGui::EndChild();

		return;
	}

	PlayerClient* pSpawn = pMember->GetPlayer();

	if (ImGui::BeginChild(pSpawn->Name, ImVec2(ImGui::GetContentRegionAvail().x, 0),
		ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoScrollbar))
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
					DoCommandf("/dex %s /multiline ; /afollow off; /nav stop ;  /timed 5, /nav id %d",
						pSpawn->Name, pLocalPlayer->GetId());
				}
			}
			if (ImGui::MenuItem("Go To"))
				DoCommandf("/nav spawn %s", spawnName);

			if (s_DanNetEnabled)
			{
				if (ImGui::MenuItem("Switch To"))
					DoCommandf("/dex %s /foreground", spawnName);
			}

			if (ImGui::BeginMenu("Roles"))
			{
				if (ImGui::MenuItem("Main Assist"))
					DoCommandf("/grouproles set %s 2", spawnName);

				if (ImGui::MenuItem("Main Tank"))
					DoCommandf("/grouproles set %s 1", spawnName);

				if (ImGui::MenuItem("Puller"))
					DoCommandf("/grouproles set %s 3", spawnName);

				if (pLocalPlayer->Name == pLocalPC->pGroupInfo->GetGroupLeader()->Name)
				{
					if (ImGui::MenuItem("Make Leader"))
						DoCommandf("/makeleader %s", spawnName);
				}
				else if (pMember == pLocalPC->pGroupInfo->GetGroupLeader())
				{
					if (s_DanNetEnabled)
					{
						if (ImGui::MenuItem("Make Me Leader"))
							DoCommandf("/dex %s /makeleader %s", spawnName, pLocalPlayer->Name);
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		if (drawPet)
		{
			// fluffy
			if (PlayerClient* petInfo = GetSpawnByID(pSpawn->PetID))
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
			if (ImGui::Checkbox(button.name, &button.visible))
				SaveSetting(&button.visible, s_SettingsFile);
		}
		ImGui::EndTable();
	}
}

#pragma endregion

#pragma region GUI Windows 
// each window has a default initial size and window position on first use ever
// all of the initial sizes have been verified to be on the screen down to 800x600 resolution.
// resolutions below that is questionable on many lvls. you may need a bigger monitor.

static void DrawTargetWindow(bool splitTar = false)
{
	if (pTarget)
	{
		if (splitTar)
			ImGui::SetWindowFontScale(s_FontScaleSettings.targetWinScale);

		const char* tarName = pTarget->DisplayedName;
		if (mq::IsAnonymized())
		{
			if (pTarget->Type == SPAWN_PLAYER)
				tarName = MaskName(tarName);
		}

		float sizeX = ImGui::GetWindowWidth();
		float yPos = ImGui::GetCursorPosY();
		float midX = (sizeX / 2);
		float tarPercentage = static_cast<float>(pTarget->HPCurrent) / 100;
		int tar_label = pTarget->HPCurrent;
		ImVec4 colorTarHP = CalculateProgressiveColor(s_BarColors.minColorHP, s_BarColors.maxColorHP, pTarget->HPCurrent);

		if (strncmp(pTarget->DisplayedName, pLocalPC->Name, 64) == 0)
		{
			tarPercentage = static_cast<float>(pTarget->HPCurrent) / pTarget->HPMax;
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
		ImGui::TextColored(COLOR_TANGERINE.ToImColor(), "%0.1f m", GetDistance(pLocalPlayer, pTarget));

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
		ImGui::TextColored(COLOR_TEAL.ToImColor(), "Lvl %d", pTarget->Level);

		//target class
		ImGui::SameLine();
		const char* classCode = pTarget->GetClassThreeLetterCode();
		
		const char* tClass = (classCode && !ci_equals(classCode, "UNKNOWN CLASS") ? classCode : ICON_MD_HELP_OUTLINE);
		ImGui::Text(tClass);
		//body type
		ImGui::SameLine();
		ImGui::Text(GetBodyTypeDesc(GetBodyType(pTarget)));

		ImGui::SameLine(sizeX * .5f);
		ImGui::TextColored(GetConColor(ConColor(pTarget)).ToImColor(), ICON_MD_LENS);
		
		//aggro meter
		if (s_WinSettings.showAggroMeter)
		{
			const char* secAgName = s_SecondAggroName;
			if (mq::IsAnonymized())
				secAgName = MaskName(secAgName);
			
			if (s_NumSettings.myAggroPct < 100)
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, COLOR_ORANGE.ToImU32());
			else
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, COLOR_PURPLE1.ToImU32());

			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(static_cast<float>(s_NumSettings.myAggroPct) / 100, ImVec2(0.0f, static_cast<float>(s_NumSettings.aggroBarHeight)), "##Aggro");
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(ImVec2(10, yPos));
			ImGui::Text(secAgName);
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
					DrawBuffsIconList("TargetBuffsTable", pTargetWnd->GetBuffRange(), false);
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
	if (s_WinSettings.savePosition && !s_WinSettings.dockedPlayerWin)
	{
		ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.playerWinX, s_WinSizeSettings.playerWinY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.playerWinWidth, s_WinSizeSettings.playerWinHeight), ImGuiCond_Appearing);
	}

	ImGuiStyle originalStyle = ApplyTheme(s_WinTheme.playerWinTheme);

	ImGuiWindowFlags menuFlag = ImGuiWindowFlags_MenuBar ;
	ImGuiWindowFlags lockFlag = (s_WinSettings.lockPlayerWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;
	
	if (ImGui::Begin("Player", &s_WinSettings.showPlayerWindow, s_WindowFlags | lockFlag | menuFlag | ImGuiWindowFlags_NoScrollbar))
	{
		bool isDocked = ImGui::IsWindowDocked();
		if (isDocked != s_WinSettings.dockedPlayerWin)
		{
			s_WinSettings.dockedPlayerWin = isDocked;
			SaveSetting(&s_WinSettings.dockedPlayerWin, s_SettingsFile);
		}

		int sizeX = static_cast<int>(ImGui::GetWindowWidth());
		int midX = (sizeX / 2) - 8;

		DrawMenu("Player");

		ImGui::SetWindowFontScale(s_FontScaleSettings.playerWinScale);
		DrawPlayerBars(true,s_NumSettings.playerBarHeight,false,s_FontScaleSettings.playerWinScale);

		if (!s_WinSettings.showTargetWindow)
		{
			ImGui::Separator();
			DrawTargetWindow();
		}
	}
	
	if (ImGui::BeginPopupContextWindow("PlayerContext", ImGuiPopupFlags_MouseButtonRight))
	{
		ImGui::SetWindowFontScale(s_FontScaleSettings.playerWinScale);

		if (ImGui::MenuItem("Lock Player Window", NULL, &s_WinSettings.lockPlayerWin))
		{
			s_WinSettings.lockAllWin = false;
			SaveSetting(&s_WinSettings.lockPlayerWin, s_SettingsFile);
		}

		if (ImGui::MenuItem("Show Title Bars", NULL, &s_WinSettings.showTitleBars))
			SaveSetting(&s_WinSettings.showTitleBars, s_SettingsFile);

		if (ImGui::MenuItem("Split Target", NULL, &s_WinSettings.showTargetWindow))
			SaveSetting(&s_WinSettings.showTargetWindow, s_SettingsFile);

		ImGui::MenuItem("Show Config", NULL, s_WinSettings.showConfigWindow);

		if (ImGui::MenuItem("Close Player Window"))
			s_WinSettings.showPlayerWindow = false;

		ImGui::EndPopup();
	}


	bool checkTest = CheckWinPos(s_WinSizeSettings.playerWinX, s_WinSizeSettings.playerWinY, s_WinSizeSettings.playerWinWidth, s_WinSizeSettings.playerWinHeight,
		ImGui::GetWindowPos(), ImGui::GetWindowSize());
	if (checkTest)
		s_DoSavePosition = true;

	ImGui::End();
	ResetTheme(originalStyle);
}

static void DrawGroupWindow()
{
	if (!s_WinSettings.showGroupWindow)
		return;

	float displayX = ImGui::GetIO().DisplaySize.x;
	ImGui::SetNextWindowPos(ImVec2(displayX - 310, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

	if (s_WinSettings.savePosition && !s_WinSettings.dockedGroupWin)
	{
		ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.groupWinX, s_WinSizeSettings.groupWinY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.groupWinWidth, s_WinSizeSettings.groupWinHeight), ImGuiCond_Appearing);
	}

	ImGuiStyle originalStyle = ApplyTheme(s_WinTheme.groupWinTheme);

	ImGuiWindowFlags lockFlag = (s_WinSettings.lockGroupWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;
	ImGuiWindowFlags menuFlag = ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin("Group", &s_WinSettings.showGroupWindow,
		s_WindowFlags | menuFlag | lockFlag | ImGuiWindowFlags_NoScrollbar))
	{
		bool isDocked = ImGui::IsWindowDocked();
		if (isDocked != s_WinSettings.dockedGroupWin)
		{
			s_WinSettings.dockedGroupWin = isDocked;
			SaveSetting(&s_WinSettings.dockedGroupWin, s_SettingsFile);
		}

		DrawMenu("Group");

		ImGui::SetWindowFontScale(s_FontScaleSettings.groupWinScale);
		if (s_WinSettings.showSelfOnGroup)
			DrawPlayerBars(false, s_NumSettings.groupBarHeight, true, s_FontScaleSettings.groupWinScale);

		if (pLocalPC->pGroupInfo)
		{
			for (int i = 1; i < MAX_GROUP_SIZE; i++)
			{
				if (CGroupMember* pMember = pLocalPC->pGroupInfo->GetGroupMember(i))
					DrawGroupMemberBars(pMember, true, i);
				else if (s_WinSettings.showEmptyGroupSlot)
					DrawEmptyMember(i);
			}
		}
		else
		{
			if (s_WinSettings.showEmptyGroupSlot)
			{
				for (int i = 1; i < MAX_GROUP_SIZE; i++)
					DrawEmptyMember(i);
			}
		}
		ImGui::Spacing();
		ImGui::Separator();

		ImVec2 btnSize = CalcButtonSize("Disband", 1.0f, s_FontScaleSettings.groupWinScale);

		float posX = ImGui::GetWindowWidth() * 0.5f - btnSize.x;
		if (posX < 0)
			posX = 0;

		ImGui::SetCursorPosX(posX);

		if (pLocalPlayer->InvitedToGroup)
		{
			if (ImGui::Button("Accept", btnSize))
				DoCommand("/invite");
		}
		else
		{
			if (ImGui::Button("Invite", btnSize))
			{
				if (pTarget)
					DoCommandf("/invite %s", pTarget->Name);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Disband", btnSize))
			DoCommand("/disband");


		if (mq::IsPluginLoaded("MQ2DanNet"))
		{
			ImVec2 MaxBtnSize = CalcButtonSize("Follow Me", 1.0f, s_FontScaleSettings.groupWinScale);
			float posX = (ImGui::GetWindowWidth() * 0.5f) - MaxBtnSize.x;
			if (posX < 0)
				posX = 0;

			ImGui::SetCursorPosX(posX);

			int myID = pLocalPlayer->GetId();
			const char* followLabel = "Follow Me";
			if (s_FollowClicked)
				followLabel = "Stop Follow##";

			if (ImGui::Button(followLabel, MaxBtnSize))
			{
				if (!s_FollowClicked)
				{
					DoCommandf("/dgge /multiline ; /afollow off; /nav stop ; /timed 5, /dgge /afollow spawn %d", myID);
					s_FollowClicked = true;
				}
				else
				{
					DoCommandf("/dgge /multiline ; /afollow off; /nav stop");
					s_FollowClicked = false;
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Come Here##", MaxBtnSize))
				DoCommandf("/dgge /multiline ; /afollow off; /nav stop ; /timed 5, /nav id %d", myID);

		}	
	}
	if (ImGui::BeginPopupContextWindow("GroupContext", ImGuiPopupFlags_MouseButtonRight))
	{
		if (s_WinSettings.lockGroupWin = ImGui::MenuItem("Lock Group Window", NULL, &s_WinSettings.lockGroupWin))
		{
			s_WinSettings.lockAllWin = false;
			SaveSetting(&s_WinSettings.lockGroupWin, s_SettingsFile);
		}

		if (ImGui::MenuItem("Show Self", NULL, &s_WinSettings.showSelfOnGroup))
			SaveSetting(&s_WinSettings.showSelfOnGroup, s_SettingsFile);

		if (ImGui::MenuItem("Show Empty Slots", NULL, &s_WinSettings.showEmptyGroupSlot))
			SaveSetting(&s_WinSettings.showEmptyGroupSlot, s_SettingsFile);

		if (ImGui::MenuItem("Close Group Window"))
			s_WinSettings.showGroupWindow = false;

		ImGui::EndPopup();
	}


	bool checkTest = CheckWinPos(s_WinSizeSettings.groupWinX, s_WinSizeSettings.groupWinY, s_WinSizeSettings.groupWinWidth, s_WinSizeSettings.groupWinHeight,
		ImGui::GetWindowPos(), ImGui::GetWindowSize());
	if (checkTest)
		s_DoSavePosition = true;

	ResetTheme(originalStyle);
	ImGui::End();
}

static void DrawPetWindow()
{
	if (PlayerClient* MyPet = GetSpawnByID(pLocalPlayer->PetID))
	{
		const char* petName = MyPet->DisplayedName;
		if (mq::IsAnonymized())
			petName = "Pet";
		float displayX = ImGui::GetIO().DisplaySize.x;
		float displayY = ImGui::GetIO().DisplaySize.y;
		ImGui::SetNextWindowPos(ImVec2(displayX * 0.75f, displayY * 0.5f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 283), ImGuiCond_FirstUseEver);
		
		if (s_WinSettings.savePosition && !s_WinSettings.dockedPetWin)
		{
			ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.petWinX, s_WinSizeSettings.petWinY), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.petWinWidth, s_WinSizeSettings.petWinHeight), ImGuiCond_Appearing);
		}

		ImGuiStyle originalStyle = ApplyTheme(s_WinTheme.petWinTheme);
		
		ImGuiWindowFlags menuFlag = ImGuiWindowFlags_MenuBar;
		ImGuiWindowFlags lockFlag = (s_WinSettings.lockPetWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;

		if (ImGui::Begin("Pet", &s_WinSettings.showPetWindow, s_WindowFlags | menuFlag | lockFlag | ImGuiWindowFlags_NoScrollbar))
		{
			bool isDocked = ImGui::IsWindowDocked();
			if (isDocked != s_WinSettings.dockedPetWin)
			{
				s_WinSettings.dockedPetWin = isDocked;
				SaveSetting(&s_WinSettings.dockedPetWin, s_SettingsFile);
			}

			DrawMenu("Pet");

			if (ImGui::BeginPopupContextWindow("PetContext", ImGuiPopupFlags_MouseButtonRight))
			{
				ImGui::SetWindowFontScale(s_FontScaleSettings.petWinScale);
				if (ImGui::MenuItem("Lock Pet Window", NULL, &s_WinSettings.lockPetWin))
				{
					s_WinSettings.lockAllWin = false;
					SaveSetting(&s_WinSettings.lockPetWin, s_SettingsFile);
				}

				if (ImGui::MenuItem("Show Pet Buttons", NULL, &s_WinSettings.showPetButtons))
					SaveSetting(&s_WinSettings.showPetButtons, s_SettingsFile);

				if (ImGui::MenuItem("Close Pet Window"))
					s_WinSettings.showPetWindow = false;

				ImGui::EndPopup();
			}

			ImGui::SetWindowFontScale(s_FontScaleSettings.petWinScale);
			float sizeX = ImGui::GetWindowWidth();
			float yPos = ImGui::GetCursorPosY();
			float midX = (sizeX / 2) > 1 ? (sizeX / 2) : 2.0f;

			if (ImGui::BeginTable("Pet", 2,
				ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable ))
			{
				ImGui::TableSetupColumn(petName, ImGuiTableColumnFlags_NoHide);
				ImGui::TableSetupColumn("Buffs");
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				
				DrawPetInfo(MyPet);

				// Pet Target Section
				
				if (ImGui::BeginChild("PetTarget", ImVec2(ImGui::GetColumnWidth(), 0),
					ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize,
					 ImGuiWindowFlags_NoScrollbar))
				{
					if (PlayerClient* pPetTarget = MyPet->WhoFollowing)
					{
						const char* petTargetName = pPetTarget->DisplayedName;
						if (mq::IsAnonymized())
							petTargetName = "Pet Target";

						ImGui::Text("Lvl");
						ImGui::SameLine();
						ImGui::TextColored(COLOR_TEAL.ToImColor(), "%d", pPetTarget->Level);
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
					float childBtnWidth = ImGui::GetColumnWidth();
					float childBtnHeight = ImGui::GetContentRegionAvail().y ;

					if (ImGui::BeginChild("PetButtons", ImVec2(childBtnWidth, childBtnHeight),
						ImGuiChildFlags_Border , ImGuiWindowFlags_NoScrollbar))
					{
						DisplayPetButtons();
					}
					ImGui::EndChild();

				}
				// Pet Buffs Section (Column)
				ImGui::TableNextColumn();
				
				float childBuffWidth = ImGui::GetColumnWidth();
				float childBuffHeight = ImGui::GetContentRegionAvail().y;

				if (ImGui::BeginChild("PetBuffs", ImVec2(childBuffWidth, childBuffHeight),
					ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar))
				{
					DrawBuffsIconList("PetBuffsTable", pPetInfoWnd->GetBuffRange(), true);
				}
				ImGui::EndChild();
				ImGui::EndTable();
			}
		}

		bool checkTest = CheckWinPos(s_WinSizeSettings.petWinX, s_WinSizeSettings.petWinY, s_WinSizeSettings.petWinWidth, s_WinSizeSettings.petWinHeight,
			ImGui::GetWindowPos(), ImGui::GetWindowSize());
		if (checkTest)
			s_DoSavePosition = true;

		ResetTheme(originalStyle);
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
		
		if (s_WinSettings.savePosition && !s_WinSettings.dockedCastingWin)
		{
			ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.castingWinX, s_WinSizeSettings.castingWinY), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.castingWinWidth, s_WinSizeSettings.castingWinHeight), ImGuiCond_Appearing);
		}

		ImGuiStyle originalStyle = ApplyTheme(s_WinTheme.spellsWinTheme);
		
		ImGuiWindowFlags lockFlag = (s_WinSettings.lockCastingWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;
		if (ImGui::Begin("Casting1", &s_IsCasting,
			lockFlag | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar))
		{
			bool isDocked = ImGui::IsWindowDocked();
			if (isDocked != s_WinSettings.dockedCastingWin)
			{
				s_WinSettings.dockedCastingWin = isDocked;
				SaveSetting(&s_WinSettings.dockedCastingWin, s_SettingsFile);
			}

			const char* spellName = pCastingWnd->GetChildItem("Casting_SpellName")->WindowText.c_str();
			EQ_Spell* pSpell = GetSpellByName(spellName);
			if (pSpell)
			{
				ImGui::SetWindowFontScale(s_FontScaleSettings.spellsWinScale);
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
						{
							const char* tarName = pTarget->DisplayedName;
							if (mq::IsAnonymized())
							{
								if (pTarget->Type == SPAWN_PLAYER)
									tarName = MaskName(tarName);
							}
							ImGui::Text("Target: %s", tarName);
						}
						break;
					case TargetType_Single:
						if (pTarget)
						{
							const char* tarName = pTarget->DisplayedName;
							if (mq::IsAnonymized())
							{
								if (pTarget->Type == SPAWN_PLAYER)
									tarName = MaskName(tarName);
							}
							ImGui::Text("Target: %s", tarName);
						}						
						break;
					case TargetType_Self:
						{
							const char* tarName = pLocalPlayer->DisplayedName;
							if (mq::IsAnonymized())
								tarName = MaskName(tarName);

							ImGui::Text("Target: %s", tarName);
						}
						break;
					}
				}
			}

			bool checkTest = CheckWinPos(s_WinSizeSettings.castingWinX, s_WinSizeSettings.castingWinY, s_WinSizeSettings.castingWinWidth, s_WinSizeSettings.castingWinHeight,
				ImGui::GetWindowPos(), ImGui::GetWindowSize());
			if (checkTest)
				s_DoSavePosition = true;
			
		}
		if (ImGui::BeginPopupContextWindow("CastingContext", ImGuiPopupFlags_MouseButtonRight))
		{
			ImGui::SetWindowFontScale(s_FontScaleSettings.spellsWinScale);

			if (ImGui::MenuItem("Lock Casting Window", NULL, &s_WinSettings.lockCastingWin))
			{
				s_WinSettings.lockAllWin = false;
				SaveSetting(&s_WinSettings.lockCastingWin, s_SettingsFile);
			}

			ImGui::EndPopup();
		}


		ResetTheme(originalStyle);
		ImGui::End();
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

		if (s_WinSettings.savePosition && !s_WinSettings.dockedSpellsWin)
		{
			ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.spellsWinX, s_WinSizeSettings.spellsWinY), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.spellsWinWidth, s_WinSizeSettings.spellsWinHeight), ImGuiCond_Appearing);
		}

		ImGuiStyle originalStyle = ApplyTheme(s_WinTheme.spellsWinTheme);

		ImGuiWindowFlags lockFlag = (s_WinSettings.lockSpellsWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;

		if (ImGui::Begin("Spells", &s_WinSettings.showSpellsWindow,
			s_WindowFlags | lockFlag | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			bool isDocked = ImGui::IsWindowDocked();
			if (isDocked != s_WinSettings.dockedSpellsWin)
			{
				s_WinSettings.dockedSpellsWin = isDocked;
				SaveSetting(&s_WinSettings.dockedSpellsWin, s_SettingsFile);
			}

			ImGui::SetWindowFontScale(s_FontScaleSettings.spellsWinScale);
			DrawSpellBarIcons(s_NumSettings.spellGemHeight);

			ImVec2 btnSize = CalcButtonSize(ICON_FA_BOOK, 3.0f, s_FontScaleSettings.spellsWinScale);
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btnSize.x) * 0.5f);
			if (ImGui::Button(ICON_FA_BOOK, btnSize))
				s_ShowSpellBook = !s_ShowSpellBook;

			ImGui::Spacing();
		}

		if (ImGui::BeginPopupContextItem("SpellsContext", ImGuiPopupFlags_MouseButtonRight))
		{
			ImGui::SetWindowFontScale(s_FontScaleSettings.spellsWinScale);

			if (ImGui::MenuItem("Lock Spells Window", NULL, &s_WinSettings.lockSpellsWin))
			{
				s_WinSettings.lockAllWin = false;
				SaveSetting(&s_WinSettings.lockSpellsWin, s_SettingsFile);
			}

			if (ImGui::MenuItem("Show Title Bars", NULL, &s_WinSettings.showTitleBars))

			if (ImGui::MenuItem("Close Spells Window"))
				s_WinSettings.showSpellsWindow = false;

			ImGui::EndPopup();

			bool checkTest = CheckWinPos(s_WinSizeSettings.spellsWinX, s_WinSizeSettings.spellsWinY, s_WinSizeSettings.spellsWinWidth, s_WinSizeSettings.spellsWinHeight,
				ImGui::GetWindowPos(), ImGui::GetWindowSize());
			if (checkTest)
				s_DoSavePosition = true;
			
		}

		ResetTheme(originalStyle);
		ImGui::End();

	}
}

static void DrawBuffWindow()
{
	if (!s_WinSettings.showBuffWindow)
		return;

	ImGui::SetNextWindowPos(ImVec2(15,10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(210, 300), ImGuiCond_FirstUseEver);

	ImGuiStyle originalStyle = ApplyTheme(s_WinTheme.buffsWinTheme);

	ImGuiWindowFlags lockFlag = (s_WinSettings.lockBuffsWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;
	ImGuiWindowFlags menuFlag = ImGuiWindowFlags_MenuBar;

	if (s_WinSettings.savePosition && !s_WinSettings.dockedBuffsWin)
	{
		ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.buffsWinX, s_WinSizeSettings.buffsWinY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.buffsWinWidth, s_WinSizeSettings.buffsWinHeight), ImGuiCond_Appearing);
	}

	if (ImGui::Begin("Buffs", &s_WinSettings.showBuffWindow, s_WindowFlags | menuFlag | lockFlag | ImGuiWindowFlags_NoScrollbar))
	{
		bool isDocked = ImGui::IsWindowDocked();
		if (isDocked != s_WinSettings.dockedBuffsWin)
		{
			s_WinSettings.dockedBuffsWin = isDocked;
			SaveSetting(&s_WinSettings.dockedBuffsWin, s_SettingsFile);
		}

		DrawMenu("Buffs");

		DrawBuffsTable("BuffTable", pBuffWnd->GetBuffRange(), false, true);

		ImGui::Spacing();
	}
	if (ImGui::BeginPopupContextWindow("BuffContext", ImGuiPopupFlags_MouseButtonRight))
	{
		ImGui::SetWindowFontScale(s_FontScaleSettings.buffsWinScale);
		if (ImGui::MenuItem("Lock Buffs Window", NULL, &s_WinSettings.lockBuffsWin))
		{
			s_WinSettings.lockAllWin = false;
			SaveSetting(&s_WinSettings.lockBuffsWin, s_SettingsFile);
		}

		if (ImGui::MenuItem("Close Buffs Window"))
			s_WinSettings.showBuffWindow = false;

		ImGui::EndPopup();


		bool checkTest = CheckWinPos(s_WinSizeSettings.buffsWinX, s_WinSizeSettings.buffsWinY, s_WinSizeSettings.buffsWinWidth, s_WinSizeSettings.buffsWinHeight,
			ImGui::GetWindowPos(), ImGui::GetWindowSize());
		if (checkTest)
			s_DoSavePosition = true;

	}

	ResetTheme(originalStyle);
	ImGui::End();
}

static void DrawSongWindow()
{
	if (!s_WinSettings.showSongWindow)
		return;

	ImGui::SetNextWindowPos(ImVec2(15, 310), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(210, 300), ImGuiCond_FirstUseEver);

	if (s_WinSettings.savePosition && !s_WinSettings.dockedSongWin)
	{
		ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.songWinX, s_WinSizeSettings.songWinY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.songWinWidth, s_WinSizeSettings.songWinHeight), ImGuiCond_Appearing);
	}

	ImGuiStyle originalStyle = ApplyTheme(s_WinTheme.songWinTheme);

	ImGuiWindowFlags lockFlag = (s_WinSettings.lockSongWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;
	ImGuiWindowFlags menuFlag = ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin("Songs", &s_WinSettings.showSongWindow,
		s_WindowFlags | menuFlag | lockFlag | ImGuiWindowFlags_NoScrollbar))
	{
		bool isDocked = ImGui::IsWindowDocked();
		if (isDocked != s_WinSettings.dockedSongWin)
		{
			s_WinSettings.dockedSongWin = isDocked;
			SaveSetting(&s_WinSettings.dockedSongWin, s_SettingsFile);
		}

		DrawMenu("Songs");

		if (ImGui::BeginPopupContextWindow("SongContext", ImGuiPopupFlags_MouseButtonRight))
		{
			ImGui::SetWindowFontScale(s_FontScaleSettings.buffsWinScale);

			if (ImGui::MenuItem("Lock Songs Window", NULL, &s_WinSettings.lockSongWin))
			{
				s_WinSettings.lockAllWin = false;
				SaveSetting(&s_WinSettings.lockSongWin, s_SettingsFile);
			}

			if (ImGui::MenuItem("Flash Song Timers", NULL, &s_WinSettings.flashSongTimer))
				SaveSetting(&s_WinSettings.flashSongTimer, s_SettingsFile);

			if (ImGui::MenuItem("Close Songs Window"))
				s_WinSettings.showSongWindow = false;

			ImGui::EndPopup();
		}

		DrawBuffsTable("SongTable", pSongWnd->GetBuffRange(), false, true, true);

		bool checkTest = CheckWinPos(s_WinSizeSettings.songWinX, s_WinSizeSettings.songWinY, s_WinSizeSettings.songWinWidth, s_WinSizeSettings.songWinHeight,
			ImGui::GetWindowPos(), ImGui::GetWindowSize());
		if (checkTest)
			s_DoSavePosition = true;
		
	}

	ResetTheme(originalStyle);
	ImGui::End();
}

static void DrawHudWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	float displayX = ImGui::GetIO().DisplaySize.x;
	ImGui::SetNextWindowPos(ImVec2((displayX * 0.5f) - 15, 0), ImGuiCond_FirstUseEver);
	float alpha = (s_NumSettings.hudAlpha / 255.0f);
	ImGui::SetNextWindowBgAlpha(alpha); // Transparent background

	if (s_WinSettings.savePosition && !s_WinSettings.dockedHudWin)
	{
		ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.hudWinX, s_WinSizeSettings.hudWinY), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.hudWinWidth, s_WinSizeSettings.hudWinHeight), ImGuiCond_Appearing);
	}

	ImGuiWindowFlags lockFlag = (s_WinSettings.lockHudWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;

	if (ImGui::Begin("Hud##GrimGui", &s_WinSettings.showHud, lockFlag | window_flags))
	{
		bool isDocked = ImGui::IsWindowDocked();
		if (isDocked != s_WinSettings.dockedHudWin)
		{
			s_WinSettings.dockedHudWin = isDocked;
			SaveSetting(&s_WinSettings.dockedHudWin, s_SettingsFile);
		}

		DrawStatusEffects();

		if (ImGui::BeginPopupContextWindow("HudContext##GrimGui",ImGuiPopupFlags_MouseButtonRight))
		{

			if (ImGui::MenuItem("Lock Hud", NULL, &s_WinSettings.lockHudWin))
			{
				s_WinSettings.lockAllWin = false;
				SaveSetting(&s_WinSettings.lockHudWin, s_SettingsFile);
			}

			if (ImGui::MenuItem("Close Hud"))
				s_WinSettings.showHud = false;

			ImGui::EndPopup();
		}


		bool checkTest = CheckWinPos(s_WinSizeSettings.hudWinX, s_WinSizeSettings.hudWinY, s_WinSizeSettings.hudWinWidth, s_WinSizeSettings.hudWinHeight,
			ImGui::GetWindowPos(), ImGui::GetWindowSize());
		if (checkTest)
			s_DoSavePosition = true;
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
					{
						*setting.value = MQColor(colorValue);
						s_SettingModified = true;
					}

					ImGui::SameLine();
					mq::imgui::HelpMarker(setting.helpText);

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
					ImGuiSliderFlags clamp = slider.clamp ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None;
				
					ImGui::TableNextColumn();
			        ImGui::SetNextItemWidth(100);
					if (ImGui::SliderInt(slider.label, slider.value, slider.min, slider.max, "%d", clamp))
					{
						s_SettingModified = true;
					}
			        ImGui::SameLine();

			        if (*slider.value == 0 && (std::string(slider.label)._Starts_with("Flash")))
					{	
						mq::imgui::HelpMarker(fmt::format("{} Disabled",slider.label).c_str());
					} else {
						mq::imgui::HelpMarker(slider.helpText);
			        }
			    }
			    ImGui::EndTable();
			}
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Font Scaling"))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int col = sizeX / 220;
			if (col < 1)
				col = 1;

			if (ImGui::BeginTable("Font Scaling", col))
			{
				ImGui::TableNextRow();
				for (const auto& font : fontScaleSettings)
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(100);
					if (ImGui::SliderFloat(font.key, font.value, 0, 2.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
						s_SettingModified = true;

					ImGui::SameLine();
					mq::imgui::HelpMarker(font.helpText);
				}
				ImGui::EndTable();
			}
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Window Settings Toggles"))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int col = sizeX / 165;
			if (col < 1)
				col = 1;

			if (ImGui::BeginTable("Toggle Controls", col))
			{
				ImGui::TableNextRow();

				for (const auto& toggle : settingToggleOptions)
				{
					if (!toggle.lockSetting)
					{
						ImGui::TableNextColumn();
						if (ImGui::Checkbox(toggle.label, toggle.setting))
						{
							s_SettingModified = true;
							
							if (toggle.label && strncmp(toggle.label, "Lock All", 8) == 0)
								LockAll();
						}

						ImGui::SameLine();
						mq::imgui::HelpMarker(toggle.helpText);
					}
				}

				ImGui::EndTable();
			}
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Window Lock Toggles"))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int col = sizeX / 165;
			if (col < 1)
				col = 1;

			if (ImGui::BeginTable("Lock Controls", col))
			{
				ImGui::TableNextRow();

				for (const auto& toggle : settingToggleOptions)
				{
					if (toggle.lockSetting)
					{
						ImGui::TableNextColumn();
						if (ImGui::Checkbox(toggle.label, toggle.setting))
							s_SettingModified = true;

						ImGui::SameLine();
						mq::imgui::HelpMarker(toggle.helpText);
					}
				}

				ImGui::EndTable();
			}
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Window Themes"))
		{
			int sizeX = static_cast<int>(ImGui::GetWindowWidth());
			int col = sizeX / 150;
			if (col < 1)
				col = 1;

			if (ImGui::BeginTable("Theme List", col))
			{
				ImGui::TableNextRow();
				for (const auto& theme : themeOptions)
				{
					ImGui::TableNextColumn();
					std::string temp = *theme.themeName;
					ImGui::SetNextItemWidth(100);
					*theme.themeName = DrawThemePicker(*theme.themeName, theme.winName);
					if (temp != *theme.themeName)
						s_SettingModified = true;

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
			s_SettingModified = false;
			s_DoSavePosition = false;
			SaveSettings();
		}

		if (s_SettingModified || s_DoSavePosition)
		{
			ImGui::SameLine();
			if (ImGui::Button("Revert"))
			{
				LoadSettings();
				s_SettingModified = false;
			}
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
						SaveSetting(option.setting, s_SettingsFile);
					
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
		s_LockAllWin = s_WinSettings.lockAllWin ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;
		s_WindowFlags = s_WinSettings.showTitleBars ? ImGuiWindowFlags_None | ImGuiWindowFlags_NoFocusOnAppearing : ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar;

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
				{
					if (PlayerClient* pSecAgSpawn = GetSpawnByID(pAggroInfo->AggroSecondaryID))
						s_SecondAggroName = pSecAgSpawn->DisplayedName;
					else
						s_SecondAggroName = "Unknown";
				}
				else
				{
					s_SecondAggroName = "None";
				}
			}

			GetHeading();

			g_LastUpdateTime = now;
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

		if (s_IsCaster)
		{
			if (pSpellPicker->m_selectedSpell)
			{
				DoCommandf("/memspell %d \"%s\"", s_MemGemIndex, pSpellPicker->m_selectedSpell->Name);
				pSpellPicker->ClearSelection();
				s_MemGemIndex = 0;
			}

			if (pSpellPicker->m_needFilter)
				pSpellPicker->FilterSpells();

			if (pSpellPicker->m_needSpellPickup)
				pSpellPicker->PickUpSpell();
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
			SaveSetting(&s_WinSettings.showMainWindow, s_SettingsFile);
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
				SaveSetting(&s_WinSettings.showPlayerWindow, s_SettingsFile);
		}

		// Pet Window
		if (s_WinSettings.showPetWindow)
		{
			DrawPetWindow();

			if (!s_WinSettings.showPetWindow)
				SaveSetting(&s_WinSettings.showPetWindow, s_SettingsFile);
		}

		//Buff Window
		if (s_WinSettings.showBuffWindow)
		{
			DrawBuffWindow();

			if (!s_WinSettings.showBuffWindow)
				SaveSetting(&s_WinSettings.showBuffWindow, s_SettingsFile);
		}

		// Song Window
		if (s_WinSettings.showSongWindow)
		{
			DrawSongWindow();

			if (!s_WinSettings.showSongWindow)
				SaveSetting(&s_WinSettings.showSongWindow, s_SettingsFile);
		}

		// Split Target Window
		if (s_WinSettings.showTargetWindow && pTarget)
		{

			float displayX = ImGui::GetIO().DisplaySize.x;
			ImGui::SetNextWindowPos(ImVec2(displayX - 620, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(300, 185), ImGuiCond_FirstUseEver);

			if (s_WinSettings.savePosition && !s_WinSettings.dockedTargetWin)
			{
				ImGui::SetNextWindowPos(ImVec2(s_WinSizeSettings.targetWinX, s_WinSizeSettings.targetWinY), ImGuiCond_Appearing);
				ImGui::SetNextWindowSize(ImVec2(s_WinSizeSettings.targetWinWidth, s_WinSizeSettings.targetWinHeight), ImGuiCond_Appearing);
			}

			ImGuiStyle oldStyle = ApplyTheme(s_WinTheme.playerWinTheme);
			ImGuiWindowFlags lockFlag = (s_WinSettings.lockTargetWin || s_WinSettings.lockAllWin) ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None;

			if (ImGui::Begin("Target", &s_WinSettings.showTargetWindow, s_WindowFlags | lockFlag | s_LockAllWin))
			{
				bool isDocked = ImGui::IsWindowDocked();
				if (isDocked != s_WinSettings.dockedTargetWin)
				{
					s_WinSettings.dockedTargetWin = isDocked;
					SaveSetting(&s_WinSettings.dockedTargetWin, s_SettingsFile);
				}

				DrawTargetWindow(true);
			}

			if (ImGui::BeginPopupContextWindow("TarContext", ImGuiPopupFlags_MouseButtonRight))
			{
				ImGui::SetWindowFontScale(s_FontScaleSettings.targetWinScale && s_WinSettings.showTargetWindow ? s_FontScaleSettings.targetWinScale : s_FontScaleSettings.playerWinScale);

				if (ImGui::MenuItem("Lock Target Window", NULL, &s_WinSettings.lockTargetWin))
				{
					s_WinSettings.lockAllWin = false;
					SaveSetting(&s_WinSettings.lockTargetWin, s_SettingsFile);
				}

				if (ImGui::MenuItem("Close (unsplits)"))
					s_WinSettings.showTargetWindow = false;

				ImGui::EndPopup();
			}

			if (CheckWinPos(s_WinSizeSettings.targetWinX, s_WinSizeSettings.targetWinY, s_WinSizeSettings.targetWinWidth, s_WinSizeSettings.targetWinHeight,
				ImGui::GetWindowPos(), ImGui::GetWindowSize()))
				SaveSettings();

			ResetTheme(oldStyle);
			ImGui::End();

			if (!s_WinSettings.showTargetWindow)
				SaveSetting(&s_WinSettings.showTargetWindow, s_SettingsFile);
		}

		// Group Window
		if (s_WinSettings.showGroupWindow)
		{
			DrawGroupWindow();

			if (!s_WinSettings.showGroupWindow)
				SaveSetting(&s_WinSettings.showGroupWindow, s_SettingsFile);
		}

		// Spell Window
		if (s_WinSettings.showSpellsWindow)
		{
			DrawSpellWindow();

			if (!s_WinSettings.showSpellsWindow)
				SaveSetting(&s_WinSettings.showSpellsWindow, s_SettingsFile);
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

		if (s_ShowSpellBook)
		{
			ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Appearing);
			if (ImGui::Begin("Spell Book Table", &s_ShowSpellBook, ImGuiWindowFlags_None))
			{
				ImGui::SetWindowFontScale(s_FontScaleSettings.spellsWinScale);
				pSpellPicker->DrawSpellTable();
			}
			ImGui::End();
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
	s_DanNetEnabled = mq::IsPluginLoaded("MQ2DanNet");
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
	s_DanNetEnabled = mq::IsPluginLoaded("MQ2DanNet");
}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQGrimGUI");
	AddCommand("/grimgui", GrimCommandHandler, false, true, false);
	PrintGrimHelp();

	static bool s_DefaultExists = std::filesystem::exists(DEFAULT_INI);
	memset(s_SettingsFile, 0, sizeof(s_SettingsFile));
	strcpy_s(s_SettingsFile, DEFAULT_INI.c_str());

	if (!s_DefaultExists)
		SaveSettings();

	UpdateSettingFile();

	s_DanNetEnabled = mq::IsPluginLoaded("MQ2DanNet");

	if (!s_WinSettings.showTitleBars)
		s_WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing;

	if (s_WinSettings.lockAllWin)
		s_LockAllWin = ImGuiWindowFlags_NoMove;
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQGrimGUI");
	RemoveCommand("/grimgui");
	
	if (pSpellPicker)
	{
		delete pSpellPicker;
		pSpellPicker = nullptr;
	}

	CleanUpIcons();

}

#pragma endregion
