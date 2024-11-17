#pragma once
// MQ2GrimGUI.h
#include <mq/imgui/Widgets.h>
#include "mq/base/Color.h"

extern struct WinVisSettings s_WinVis;
extern struct NumericSettings s_NumSettings;
extern class SpellPicker* pSpellPicker;
extern std::string s_MemSpellName;
extern int s_MemGemIndex;
extern bool s_MemSpell;

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


#pragma region Spells Inspector
namespace GrimGui {
	// lifted from developer tools to inspect spells

	class SpellsInspector
	{
		CTextureAnimation* m_pTASpellIcon = nullptr;
		CTextureAnimation* m_pGemHolder = nullptr;
		CTextureAnimation* m_pGemBackground = nullptr;
	public:

		~SpellsInspector()
		{
			if (m_pTASpellIcon = nullptr)
			{
				delete m_pTASpellIcon;
				m_pTASpellIcon = nullptr;
			}

			if (m_pGemBackground != nullptr)
			{
				delete m_pGemBackground;
				m_pGemBackground = nullptr;
			}

			if (m_pGemHolder = nullptr)
			{
				delete m_pGemHolder;
				m_pGemHolder = nullptr;
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

			if (ImGui::BeginTable("Buffs", 3, ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
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
						if (secondsLeft < 18 && !petBuffs)
						{
							if (s_WinVis.flashTintFlag)
								tintCol = MQColor(0, 0, 0, 255);

						}
						ImGui::PushID(buffInfo.GetIndex());
						imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(s_NumSettings.buffIconSize, s_NumSettings.buffIconSize), tintCol, borderCol);
						ImGui::PopID();

						if (ImGui::BeginPopupContextItem(("BuffPopup##" + std::to_string(spell->ID)).c_str()))
						{
							if (ImGui::MenuItem(("Remove##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
							{
								RemoveBuffByName(spell->Name);
							}

							if (ImGui::MenuItem(("Block##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
							{
								EzCommand(("/blockspell add me " + std::to_string(spell->ID)).c_str());
							}

							if (ImGui::MenuItem("Inspect##", nullptr, false, true))
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
					MQColor borderCol = MQColor(0, 0, 250, 255); // Default color blue (beneficial)
					MQColor tintCol = MQColor(255, 255, 255, 255);
					if (!spell->IsBeneficialSpell())
						borderCol = MQColor(250, 0, 0, 255); // Red for detrimental spells

					std::string caster = buffInfo.GetCaster();
					if (caster == pLocalPC->Name && !spell->IsBeneficialSpell())
						borderCol = MQColor(250, 250, 0, 255); // Yellow for spells cast by me

					int secondsLeft = buffInfo.GetBuffTimer() / 1000;
					if (secondsLeft < 18 && !petBuffs)
					{
						if (s_WinVis.flashTintFlag)
							tintCol = MQColor(0, 0, 0, 255);
					}

					imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(s_NumSettings.buffIconSize, s_NumSettings.buffIconSize), tintCol, borderCol);
					s_TarBuffLineSize += s_NumSettings.buffIconSize + 2;
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
				if (ImGui::BeginPopupContextItem(("BuffPopup##" + std::to_string(spell->ID)).c_str()))
				{

					if (ImGui::MenuItem("Inspect##", nullptr, false, true))
					{
#if defined(CSpellDisplayManager__ShowSpell_x)
						if (pSpellDisplayManager)
							pSpellDisplayManager->ShowSpell(spell->ID, true, true, SpellDisplayType_SpellBookWnd);
#else
						char buffer[512] = { 0 };
						//if (Index > -1)
						//{
						//	const char* Slot = std::to_string(Index);
						//	FormatSpellLink(buffer, 512, spell, Slot);
						//	Index = -1;
						//}
						//else
							FormatSpellLink(buffer, 512, spell);
						TextTagInfo info = ExtractLink(buffer);
						ExecuteTextLink(info);
#endif
					}

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

		bool IsGemReady(int ID)
		{
			if (GetSpellByID(GetMemorizedSpell(ID)))
			{
				if (pDisplay->TimeStamp > pLocalPlayer->SpellGemETA[ID] && pDisplay->TimeStamp > pLocalPlayer->GetSpellCooldownETA())
				{
					return true;
				}
			}
			return false;
		}

		void DrawSpellBarIcons(int gemHeight)
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
			{
				m_pGemBackground = pSidlMgr->FindAnimation("A_SpellGemBackground");
			}

			if (!m_pGemHolder)
			{
				m_pGemHolder = pSidlMgr->FindAnimation("A_SpellGemHolder");
			}

			// calculate max pSpellGem count
			int spellGemCount = 8;
			int aaIndex = GetAAIndexByName("Mnemonic Retention");
			if (CAltAbilityData* pAbility = GetAAById(aaIndex))
			{
				if (PlayerHasAAAbility(aaIndex))
				{
					spellGemCount += pAbility->CurrentRank;
				}
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

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
						{
							pSpellGem->ParentWndNotification(pSpellGem, XWM_LCLICK, nullptr);
						}
						else if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
						{
							if (pSpellPicker)
							{
								pSpellPicker->SetOpen(true);
								s_MemGemIndex = i + 1;
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

						if (!IsGemReady(i))
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

						if (!IsGemReady(i))
						{
							float coolDown = (pLocalPlayer->SpellGemETA[i] - pDisplay->TimeStamp) / 1000;
							if (coolDown < 1801)
							{
								ImGui::SetCursorPosX(posX + gemSize.cx * 0.5);
								ImGui::SetCursorPosY(posY - gemSize.cy * 0.75);

								ImGui::TextColored(GetMQColor(ColorName::Teal).ToImColor(), "%0.0f", coolDown);

								ImGui::SetCursorPosX(posX);
								ImGui::SetCursorPosY(posY);
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
									ImGui::Text("Range: %d", spell->Range);
									ImGui::Text("Recast: %d", spell->RecastTime / 1000);
								}
							}
							ImGui::EndTooltip();

							if (ImGui::IsMouseClicked(0))
							{
								if (spellId != -1)
								{
									Cast(pLocalPlayer, spell->Name);
								}

							}
							else if (ImGui::IsMouseClicked(1))
							{
								if (spellId != -1)
								{
									pSpellGem->ParentWndNotification(pSpellGem, XWM_RCLICK, nullptr);
								}
							}
							else if (ImGui::GetIO().MouseDownDuration[ImGuiMouseButton_Left] > 0.750f)
							{
								pSpellGem->ParentWndNotification(pSpellGem, XWM_LCLICKHOLD, nullptr);
								pSpellGem->ParentWndNotification(pSpellGem, XWM_LBUTTONUPAFTERHELD, nullptr);
							}
						}
					}
				}

				ImGui::PopID();
			}
		}
	};

	static SpellsInspector* s_spellsInspector = nullptr;
}
#pragma endregion


