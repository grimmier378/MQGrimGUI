#include <mq/Plugin.h>
#include <imgui.h>
#include "main/MQ2Main.h"
#include "imgui/fonts/IconsMaterialDesign.h"
#include "imgui/fonts/IconsFontAwesome.h"
#include "main/pch.h"
#include "main/MQ2SpellSearch.h"
#include <mq/imgui/Widgets.h>
#include <chrono>
#include <string>
#include <algorithm>
#include <filesystem>
#include "SpellPicker.h"



SpellPicker::SpellPicker()
{
	InitializeSpells();
}

SpellPicker::~SpellPicker()
{
	CleanupIcons();
}

void SpellPicker::InitializeSpells()
{
	Spells.clear();
	PopulateSpellData();
}

void SpellPicker::DrawSpellTree()
{
	if (!m_pSpellIcon)
	{
		m_pSpellIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
			*m_pSpellIcon = *temp;
	}

	if (ImGui::TreeNode("Spells"))
	{
		for (const auto& spell : Spells)
		{

			if (!Filter.empty() && mq::ci_find_substr(spell.Name, Filter) == -1)
				continue;

			ImGui::PushID(spell.ID);
			ImGui::BeginGroup();

			m_pSpellIcon->SetCurCell(spell.Icon);
			imgui::DrawTextureAnimation(m_pSpellIcon, CXSize(20, 20));

			ImGui::SameLine();

			if (ImGui::Selectable(spell.Name.c_str()))
			{
				SelectedSpell = std::make_shared<SpellData>(spell);
				Open = false;
			}

			ImGui::SameLine();

			ImGui::Text("Level: %d", spell.Level);

			ImGui::EndGroup();
			ImGui::PopID();

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Name: %s", spell.Name.c_str());
				ImGui::Text("Level: %d", spell.Level);
				ImGui::Text("Rank: %s", spell.RankName.c_str());
				ImGui::EndTooltip();
			}
		}

		ImGui::TreePop();
	}
}


void SpellPicker::DrawSpellPicker()
{
	if (!Open) return;

	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Spell Picker", &Open, ImGuiWindowFlags_NoDocking))
	{
		char buffer[256] = {};
		std::strncpy(buffer, Filter.c_str(), sizeof(buffer));
		if (ImGui::InputText("Search##SpellPicker", buffer, sizeof(buffer)))
			Filter = buffer;

		DrawSpellTree();
	}
	ImGui::End();
}

void SpellPicker::PopulateSpellData()
{
	if (!pCharData)
		return;

	for (int i = 0; i < NUM_BOOK_SLOTS; ++i)
	{

		int spellId = pCharData->GetSpellBook(i);
		if (spellId == -1)
			continue; // Skip empty slots

		EQ_Spell* pSpell = mq::GetSpellByID(spellId);
		if (pSpell)
		{
			SpellData spellData;
			spellData.ID = pSpell->ID;
			spellData.Name = pSpell->Name;
			spellData.RankName = pSpell->SpellRank;
			spellData.Level = pSpell->GetSpellLevelNeeded(pCharData->GetClass());
			spellData.Icon = pSpell->SpellIcon;
			spellData.TargetType = pSpell->TargetType;

			Spells.push_back(spellData);
		}
	}

	// Sort spells by level
	std::sort(Spells.begin(), Spells.end(), [](const SpellData& a, const SpellData& b) { return a.Level > b.Level; });
}

void SpellPicker::SetOpen(bool open)
{
	Open = open;
}

void SpellPicker::ClearSelection()
{
	SelectedSpell.reset();
}

void SpellPicker::CleanupIcons()
{
	if (m_pSpellIcon)
	{
		delete m_pSpellIcon;
		m_pSpellIcon = nullptr;
	}
}
