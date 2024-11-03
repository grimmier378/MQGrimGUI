#include <mq/Plugin.h>
#include <imgui.h>
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>

PreSetup("MQ2GrimGUI");
PLUGIN_VERSION(0.1);

// Declare global plugin state variables
bool ShowMQ2GrimGUIWindow = true;
bool SplitTargetWindow = false;
static bool b_flashCombat = false;

// Colors for Progress Bar Transitions
ImVec4 minColorHP(0.876f, 0.341f, 1.000f, 1.000f);  // Purple
ImVec4 maxColorHP(0.845f, 0.151f, 0.151f, 1.000f);  // Red
ImVec4 minColorMP(0.259f, 0.114f, 0.514f, 1.000f);  // Purp 
ImVec4 maxColorMP(0.079f, 0.468f, 0.848f, 1.000f);  // ltBlue
ImVec4 minColorEnd(1.000f, 0.437f, 0.019f, 1.000f);
ImVec4 maxColorEnd(0.7f, 0.6f, 0.1f, 0.7f);  // yellowish
PGROUPINFO CurGroup;

class CharData
{
public:
	// Character attributes
	std::string m_Name = "Unknown";
	int m_Level = 0;
	int m_MaxHP = 0;
	int m_CurHP = 0;
	int m_MaxMana = 0;
	int m_CurMana = 0;
	int m_MaxEndur = 0;
	int m_CurEndur = 0;
	int m_HealthPctInt = 100;
	float m_HealthPctFloat = 0.0;
	bool b_Leader = false;
	bool b_Assist = false;
	bool b_MainTank = false;
	bool b_Puller = false;
	bool b_IsCombat = false;

	// Method to update character data
	void Update()
	{
		if (PCHARINFO pCharInfo = GetCharInfo())
		{
			m_Name = pCharInfo->Name;
			m_Level = pCharInfo->GetLevel();
			m_MaxHP = GetMaxHPS();
			m_CurHP = GetCurHPS();
			m_MaxMana = GetMaxMana();
			m_CurMana = GetCurMana();
			m_MaxEndur = GetMaxEndurance();
			m_CurEndur = GetCurEndurance();
			m_HealthPctFloat = static_cast<float>(m_CurHP) / m_MaxHP;
			m_HealthPctInt = static_cast<int>(m_HealthPctFloat * 100);
			b_IsCombat = pEverQuestInfo->bAutoAttack; //pCharInfo->InCombat;
			
			//TODO: MainTarget, Assist, Puller, Leader icons
			//TODO: CombatStatus Icons
		}
	}
};

class TargetData
{
public:
	// Target Information Holder
	std::string m_tName = "Unknown";
	int m_tConColor = 0;
	int m_tLevel = 0;
	int m_tCurHP = 0;
	std::string m_tBody = "UNKNOWN";
	std::string m_tClass = ICON_MD_HELP_OUTLINE;
	float m_tDist = 0;
	bool b_IsPC = false;
	bool b_IsVis = false;
	
	//TODO: Target Buffs table

	void Update()
	{
		if (PSPAWNINFO CurTarget = pTarget)
		{
			b_IsPC = CurTarget->Type == PC;
			m_tName = CurTarget->DisplayedName;
			m_tLevel = CurTarget->Level;
			m_tCurHP = CurTarget->HPCurrent;
			m_tDist = GetDistance(pCharSpawn, pTarget);
			b_IsVis = LineOfSight(pCharSpawn,pTarget) ;
			m_tConColor = ConColor(pTarget);
			m_tBody = GetBodyTypeDesc(GetBodyType(pTarget));
			const char* classCode = CurTarget->GetClassThreeLetterCode();
			m_tClass = (classCode && std::string(classCode) != "UNKNOWN CLASS") ? classCode : ICON_MD_HELP_OUTLINE;
		}
		else
		{
			m_tName = "Unknown";
			m_tLevel = 0;
			m_tCurHP = 0;
			b_IsPC = false;
			m_tDist = 0;
			b_IsVis = false;
			m_tConColor = 0;
			m_tBody = "UNKNOWN";
			m_tClass = ICON_MD_HELP_OUTLINE;
		}
	}
};

// Calculate the progress bar color based on Min, Max, and optional Mid Values.
ImVec4 CalculateColor(const ImVec4& minColor, const ImVec4& maxColor, int value, const ImVec4* midColor = nullptr, int midValue = 50)
{
	// Clamp value between 0 and 100
	value = std::max(0, std::min(100, value));

	float r, g, b, a;

	if (midColor)
	{
		// If midColor is provided, calculate in two segments
		if (value > midValue)
		{
			float proportion = static_cast<float>(value - midValue) / (100 - midValue);
			r = midColor->x + proportion * (maxColor.x - midColor->x);
			g = midColor->y + proportion * (maxColor.y - midColor->y);
			b = midColor->z + proportion * (maxColor.z - midColor->z);
			a = midColor->w + proportion * (maxColor.w - midColor->w);
		}
		else
		{
			float proportion = static_cast<float>(value) / midValue;
			r = minColor.x + proportion * (midColor->x - minColor.x);
			g = minColor.y + proportion * (midColor->y - minColor.y);
			b = minColor.z + proportion * (midColor->z - minColor.z);
			a = minColor.w + proportion * (midColor->w - minColor.w);
		}
	}
	else
	{
		// Calculate between minColor and maxColor
		float proportion = static_cast<float>(value) / 100;
		r = minColor.x + proportion * (maxColor.x - minColor.x);
		g = minColor.y + proportion * (maxColor.y - minColor.y);
		b = minColor.z + proportion * (maxColor.z - minColor.z);
		a = minColor.w + proportion * (maxColor.w - minColor.w);
	}

	return ImVec4(r, g, b, a);
}

ImVec4 ColorToVec(const std::string& color_name)
{
	std::string color = color_name;
	std::transform(color.begin(), color.end(), color.begin(), ::tolower);

	if (color == "red")          return ImVec4(0.9f, 0.1f, 0.1f, 1.0f);
	if (color == "pink2")        return ImVec4(0.976f, 0.518f, 0.844f, 1.0f);
	if (color == "pink")         return ImVec4(0.9f, 0.4f, 0.4f, 0.8f);
	if (color == "orange")       return ImVec4(0.78f, 0.20f, 0.05f, 0.8f);
	if (color == "tangarine")    return ImVec4(1.0f, 0.557f, 0.0f, 1.0f);
	if (color == "yellow")       return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	if (color == "yellow2")      return ImVec4(0.7f, 0.6f, 0.1f, 0.7f);
	if (color == "white")        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (color == "blue")         return ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	if (color == "softblue")     return ImVec4(0.37f, 0.704f, 1.0f, 1.0f);
	if (color == "light blue2")  return ImVec4(0.2f, 0.9f, 0.9f, 0.5f);
	if (color == "light blue")   return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	if (color == "teal")         return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	if (color == "green")        return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	if (color == "green2")       return ImVec4(0.01f, 0.56f, 0.001f, 1.0f);
	if (color == "grey")         return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	if (color == "purple")       return ImVec4(0.8f, 0.0f, 1.0f, 1.0f);
	if (color == "purple2")      return ImVec4(0.46f, 0.204f, 1.0f, 1.0f);
	if (color == "btn_red")      return ImVec4(1.0f, 0.4f, 0.4f, 0.4f);
	if (color == "btn_green")    return ImVec4(0.4f, 1.0f, 0.4f, 0.4f);

	// Default color if no match
	return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

ImVec4 ConColorToVec(int color_code)
{
	switch (color_code)
	{
	case 0x06: return ImVec4(0.6f, 0.6f, 0.6f, 1.0f); // CONCOLOR_GREY
	case 0x02: return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // CONCOLOR_GREEN
	case 0x12: return ImVec4(0.37f, 0.704f, 1.0f, 1.0f); // CONCOLOR_LIGHTBLUE
	case 0x04: return ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // CONCOLOR_BLUE
	case 0x14: return ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // CONCOLOR_BLACK
	case 0x0a: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // CONCOLOR_WHITE
	case 0x0f: return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // CONCOLOR_YELLOW
	case 0x0d: return ImVec4(0.9f, 0.1f, 0.1f, 1.0f); // CONCOLOR_RED

		// Default color if the color code doesn't match any known values
	default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

// setup data classes and timers.

CharData g_CharData;
TargetData g_TargetData;
std::chrono::steady_clock::time_point g_LastUpdateTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point g_LastFlashTime = std::chrono::steady_clock::now();
const auto g_UpdateInterval = std::chrono::milliseconds(250);
const auto g_UpdateFlashInterval = std::chrono::milliseconds(133);

// GUI Windows
void DrawTargetWindow()
{
	if (g_TargetData.m_tName != nullptr && g_TargetData.m_tName != "Unknown")
	{
		float sizeX = ImGui::GetWindowWidth();
		float yPos = ImGui::GetCursorPosY();
		float midX = (sizeX / 2);
		float tarPercentage = static_cast<float>(g_TargetData.m_tCurHP) / 100;
		int tar_label = g_TargetData.m_tCurHP;
		ImVec4 colorTarHP = CalculateColor(minColorHP, maxColorHP, g_TargetData.m_tCurHP);
		if (g_TargetData.m_tName == g_CharData.m_Name)
		{
			tarPercentage = static_cast<float>(g_CharData.m_CurHP) / g_CharData.m_MaxHP;
			tar_label = g_CharData.m_HealthPctInt;
			colorTarHP = CalculateColor(minColorHP, maxColorHP, g_CharData.m_HealthPctInt);
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
		ImGui::ProgressBar(tarPercentage, ImVec2(0.0f, 15.0f), "##");
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
	}
}


static void DrawPlayerWindow()
{
	if (ImGui::Begin("Player##MQ2GrimGUI", &ShowMQ2GrimGUIWindow, ImGuiWindowFlags_MenuBar))
	{
		int sizeX = static_cast<int>(ImGui::GetWindowWidth());
		int midX = (sizeX / 2) - 8;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Main"))
			{
				if (ImGui::MenuItem("Split Target", NULL, SplitTargetWindow))
				{
					SplitTargetWindow = !SplitTargetWindow;
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

		if(ImGui::BeginChild("info", ImVec2(ImGui::GetContentRegionAvail().x, 35), true, ImGuiChildFlags_Border | ImGuiWindowFlags_NoScrollbar)) 
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

		ImVec4 colorHP = CalculateColor(minColorHP, maxColorHP, g_CharData.m_HealthPctInt);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorHP);
		ImGui::SetNextItemWidth(sizeX - 15);
		float yPos = ImGui::GetCursorPosY();
		ImGui::ProgressBar(g_CharData.m_HealthPctFloat, ImVec2(0.0f, 15.0f), "##hp");
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("HP: %d / %d", g_CharData.m_CurHP, g_CharData.m_MaxHP);
			ImGui::EndTooltip();
		}
		ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
		ImGui::Text("%d %%",g_CharData.m_HealthPctInt);

		if (g_CharData.m_MaxMana > 0)
		{
			float manaPercentage = static_cast<float>(g_CharData.m_CurMana) / g_CharData.m_MaxMana;
			int manaPercentageInt = static_cast<int>(manaPercentage * 100);
			ImVec4 colorMP = CalculateColor(minColorMP, maxColorMP, manaPercentageInt);

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorMP);
			ImGui::SetNextItemWidth(sizeX - 15);
			yPos = ImGui::GetCursorPosY();
			ImGui::ProgressBar(manaPercentage, ImVec2(0.0f, 15.0f), "##Mana");
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
		ImVec4 colorEP = CalculateColor(minColorEnd, maxColorEnd, endPercentageInt);

		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, colorEP);
		ImGui::SetNextItemWidth(sizeX - 15);
		yPos = ImGui::GetCursorPosY();
		ImGui::ProgressBar(endurancePercentage, ImVec2(0.0f, 15.0f), "##Endur");
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Endur: %d / %d", g_CharData.m_CurEndur, g_CharData.m_MaxEndur);
			ImGui::EndTooltip();
		}
		ImGui::SetCursorPos(ImVec2((ImGui::GetCursorPosX() + midX), yPos));
		ImGui::Text("%d %%", endPercentageInt);

		if (!SplitTargetWindow)
		{
			ImGui::Separator();
			DrawTargetWindow();
		}
	}
	ImGui::End();
}

static void DrawGroupWindow()
{


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
		if (now - g_LastFlashTime >= g_UpdateFlashInterval)
		{
			b_flashCombat = !b_flashCombat;
			g_LastFlashTime = now;
		}
	}
}

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME && ShowMQ2GrimGUIWindow)
	{
		DrawPlayerWindow();

		if (SplitTargetWindow)
		{
			if (ImGui::Begin("Tar##MQ2GrimGUI", &ShowMQ2GrimGUIWindow))
			{
				DrawTargetWindow();
			}
			ImGui::End();
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
	// update data
	if (GetGameState() == GAMESTATE_INGAME)
	{
		g_CharData.Update();
		g_TargetData.Update();
	}
	// DebugSpewAlways("MQ2GrimGUI::OnLoadPlugin(%s)", Name);
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
}