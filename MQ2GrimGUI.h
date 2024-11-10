#pragma once
// MQ2GrimGUI.h
#include <string>
#include <imgui.h>


extern bool s_FlashTintFlag;
extern int s_BuffIconSize;
extern int s_TarBuffLineSize;

#pragma region Spells Inspector

class SpellsInspector
{
	CTextureAnimation* m_pTASpellIcon = nullptr;
public:

	~SpellsInspector()
	{
		if (m_pTASpellIcon)
		{
			delete m_pTASpellIcon;
			m_pTASpellIcon = nullptr;
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
	void DoBuffs(const char* name, IteratorRange<PlayerBuffInfoWrapper::Iterator<T>> Buffs,
		bool petBuffs = false, bool playerBuffs = false, int baseIndex = 0)
	{
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

				imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(s_BuffIconSize, s_BuffIconSize), tintCol, borderCol);
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
		}
	}
};
static SpellsInspector* s_spellsInspector = nullptr;

#pragma endregion


// Color utility functions

/**
* @fn CalculateProgressiveColor
*
* Function to calculate a color between two colors based on a value between 0 and 100
*
* @param minColor const ImVec4& Minimum color value
* @param maxColor const ImVec4& Maximum color value
* @param value int Value between 0 and 100 to calculate the color between minColor and maxColor
* @param midColor const ImVec4* Optional midColor value to calculate in two segments
* @param midValue int Optional midValue to split the value between minColor and maxColor
*
* @return ImVec4 color value
*/
static ImVec4 CalculateProgressiveColor(const ImVec4& minColor, const ImVec4& maxColor, int value, const ImVec4* midColor = nullptr, int midValue = 50)
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


/**
* @fn ColorToVec
*
* Function to convert a color name to an ImVec4 color value
*
* @param color_name const std::string& Color name to convert to ImVec4
*
* @return ImVec4 color value
*/
static ImVec4 ColorToVec(const std::string& color_name)
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

	// Default White if no match
	return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}


/**
* @fn ConColorToVec
*
* Function to convert a CONCOLOR value to an ImVec4 color value
*
* @param color_code int CONCOLOR value to convert to ImVec4
* @return ImVec4 color value
*/
static ImVec4 ConColorToVec(int color_code)
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

