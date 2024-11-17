#pragma once
#include <mq/imgui/Widgets.h>
#include "mq/base/Color.h"
#include "MQ2GrimGUI.h"

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
			if (ImGui::BeginTable("Buffs", 3,
				ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
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
							if (s_WinSettings.flashTintFlag)
								tintCol = MQColor(0, 0, 0, 255);

						}
						ImGui::PushID(buffInfo.GetIndex());
						imgui::DrawTextureAnimation(m_pTASpellIcon, CXSize(s_NumSettings.buffIconSize, s_NumSettings.buffIconSize), tintCol, borderCol);
						ImGui::PopID();

						if (ImGui::BeginPopupContextItem(("BuffPopup##" + std::to_string(spell->ID)).c_str()))
						{
							if (ImGui::MenuItem(("Remove##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
								RemoveBuffByName(spell->Name);

							if (ImGui::MenuItem(("Block##" + std::to_string(spell->ID)).c_str(), nullptr, false, true))
								EzCommand(("/blockspell add me " + std::to_string(spell->ID)).c_str());

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
					MQColor borderCol = MQColor(0, 0, 250, 255);
					MQColor tintCol = MQColor(255, 255, 255, 255);
					if (!spell->IsBeneficialSpell())
						borderCol = MQColor(250, 0, 0, 255);

					std::string caster = buffInfo.GetCaster();
					if (caster == pLocalPC->Name && !spell->IsBeneficialSpell())
						borderCol = MQColor(250, 250, 0, 255);

					int secondsLeft = buffInfo.GetBuffTimer() / 1000;
					if (secondsLeft < 18 && !petBuffs)
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
									ImGui::Text("Range: %d", spell->Range);
									ImGui::Text("Recast: %d", spell->RecastTime / 1000);
								}
							}
							ImGui::EndTooltip();

							if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0) && ImGui::IsKeyDown(ImGuiKey_ModAlt))
							{
								DoInspectSpell(spellId);
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
}

#pragma endregion
