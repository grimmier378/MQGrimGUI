/** Spell Picker GUI
* Original Author: Aquietone
* Converted to C++ by: Grimmier
* 
* This is a conversion from Aquietone's Ability Picker lua to C++ for displaying and selecing spells in  your spell book
* 
* Displays your spell book information in a searchable tree, sorted by Category, Subcategory and Level (highest at top) 
*/
#include "SpellPicker.h"
#include <mq/imgui/Widgets.h>
#include <algorithm>

/**
* @fn DrawSpellTree
* 
* @brief Displays the spells from your Spellbook in a tree format, sorted by Category, Subcategory and Level
* 
*/
void SpellPicker::DrawSpellTree()
{

	if (ImGui::TreeNode("Spells"))
	{
		// since this is conversion from the lua lets follow the example
		// make the table layout the same first

		for (const auto& [categoryName, subCategories] : categorizedSpells)
		{
			if (ImGui::TreeNode(categoryName)) 
			{
				for (const auto& [subCategoryName, spells] : subCategories)
				{
					if (ImGui::TreeNode(subCategoryName))
					{
						for (const auto& spell : spells)
						{
							ImGui::PushID(spell.ID);
							ImGui::BeginGroup();

							m_pSpellIcon->SetCurCell(spell.IconID);
							imgui::DrawTextureAnimation(m_pSpellIcon, CXSize(20, 20));

							ImGui::SameLine();

							if (ImGui::Selectable(spell.Name))
							{
								m_selectedSpell = std::make_shared<SpellData>(spell);
								m_pickerOpen = false;
							}

							ImGui::SameLine();
							ImGui::Text("Level: %d", spell.Level);

							ImGui::EndGroup();
							ImGui::PopID();

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Name: %s", spell.Name);
								ImGui::Text("Level: %d", spell.Level);
								ImGui::Text("Rank: %s", spell.RankNum);
								ImGui::EndTooltip();
							}
						}
						ImGui::TreePop(); 
					}
				}
				ImGui::TreePop(); 
			}
		}
		ImGui::TreePop();
	}
}


void SpellPicker::DrawSpellTable()
{
	char buffer[256] = {};
	strncpy_s(buffer, m_filterString.c_str(), sizeof(buffer));
	if (ImGui::InputTextWithHint("Search##SpellTable", "Filters, Name, Rank, Cat, or SubCat", buffer, sizeof(buffer)))
	{
		if (!ci_equals(buffer, m_filterString))
		{
			m_filterString = buffer;
			m_needFilter = true;
		}
	}
	else if (buffer[0] == '\0')
	{
		m_useFilter = false;
		m_filterString = buffer;
	}

	if (ImGui::BeginTable("SpellTable##GrimGui", 7, ImGuiTableFlags_Reorderable |
		ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
		ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY))
	{
		// Table Headers
		ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 40.0f);
		ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 40.0f);
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Rank Name");
		ImGui::TableSetupColumn("Category");
		ImGui::TableSetupColumn("SubCategory");
		ImGui::TableSetupColumn("BookIndex", ImGuiTableColumnFlags_WidthFixed, 40.0f);
		ImGui::TableHeadersRow();
		for (const auto& spell : (m_useFilter ? m_filteredSpells : m_mySpells))
		{
			ImGui::TableNextRow();

			ImGui::PushID(spell.ID + spell.IconID);
			ImGui::BeginGroup();
			ImGui::TableSetColumnIndex(0);
			m_pSpellIcon->SetCurCell(spell.IconID);

			imgui::DrawTextureAnimation(m_pSpellIcon, CXSize(20, 20));

			ImGui::TableNextColumn();
			ImGui::Text("%d", spell.Level);

			ImGui::TableNextColumn();
			//ImGui::Text(spell.Name.c_str());
			ImGui::Selectable(spell.Name, false, ImGuiSelectableFlags_SpanAllColumns);
			if (ImGui::IsItemClicked(0))
			{
				InspectSpell(spell.ID);

				// NOTE:
				// incase i decide to add this as an option for the picker instead of the tree.
				// 
				// SelectedSpell = std::make_shared<SpellData>(spell);
				// Open = false;
			}

			// FIXME: See notes in header.
			// 
			//if (ImGui::IsItemClicked(1))
			//{
			//	m_NeedSpellPickup = true;
			//	m_SpellBookIndex = spell.ID;
			//	m_SpellBookIcon = m_pSpellIcon;
			//}

			ImGui::TableNextColumn();
			ImGui::Text("%s", spell.RankNum);

			ImGui::TableNextColumn();
			ImGui::Text("%s", spell.Category);

			ImGui::TableNextColumn();
			ImGui::Text("%s", spell.SubCategory);

			ImGui::TableNextColumn();
			ImGui::Text("%d", spell.SpellBookIndex);

			ImGui::EndGroup();

			//ImGui::PopID();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Level: %d", spell.Level);
				ImGui::Text("Name: %s", spell.Name);
				ImGui::Text("Rank: %s", spell.RankNum);
				ImGui::Text("Category: %s", spell.Category);
				ImGui::Text("SubCategory: %s", spell.SubCategory);
				ImGui::Text("SpellBookIndex: %d", spell.SpellBookIndex);
				ImGui::EndTooltip();
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
	}
}


void SpellPicker::InitializeSpells()
{
	if (!m_pSpellIcon)
	{
		m_pSpellIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
			*m_pSpellIcon = *temp;
	}
	if (!m_pSpellBookIcon)
	{
		m_pSpellBookIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellBook"))
			*m_pSpellBookIcon = *temp;
	}
	m_mySpells.clear();
	PopulateSpellData();
}


/**
* @fn PopulateSpellData
*
* @brief Populates the Spells vector with the spells from your spell book, this should only run on initialize,
* if you want to update the spells you should call InitializeSpells(). Useful for after you learned new spells into your book.
* 
*/
void SpellPicker::PopulateSpellData()
{
	if (!pCharData)
		return;

	for (int i = 0; i < NUM_BOOK_SLOTS; ++i)
	{
		int spellId = pCharData->GetSpellBook(i);
		if (spellId == -1)
			continue; // Empty Slot

		EQ_Spell* pSpell = mq::GetSpellByID(spellId);
		if (pSpell)
		{
			SpellData spellData{};
			spellData.ID = spellId;
			spellData.Name = pSpell->Name;
			spellData.RankNum = pSpell->SpellRank;
			spellData.Level = pSpell->GetSpellLevelNeeded(pCharData->GetClass());
			spellData.IconID = pSpell->SpellIcon;
			spellData.SpellBookIndex = i;
			spellData.Category = pCDBStr->GetString(pSpell->Category, eSpellCategory);
			spellData.SubCategory = pCDBStr->GetString(pSpell->Subcategory, eSpellCategory);

			m_mySpells.push_back(spellData);
		}
	}

	std::sort(m_mySpells.begin(), m_mySpells.end(), [](const SpellData& a, const SpellData& b) 
	{
		if (a.Category != b.Category)
			return a.Category < b.Category;
		if (a.SubCategory != b.SubCategory)
			return a.SubCategory < b.SubCategory; 
		return a.Level > b.Level;
	});
	
	for (const auto& spell : m_mySpells)
		categorizedSpells[spell.Category][spell.SubCategory].push_back(spell);
}


void SpellPicker::FilterSpells()
{
	if (!m_needFilter)
	{
		m_useFilter = false;
		categorizedSpells.clear();
		for (const auto& spell : m_mySpells)
			categorizedSpells[spell.Category][spell.SubCategory].push_back(spell);

		return;
	}

	m_filteredSpells.clear();
	categorizedSpells.clear();
	for (const auto& spell : m_mySpells)
	{
		if (!(m_filterString.empty() || !m_filterString.empty() &&
			(mq::ci_find_substr(spell.Name, m_filterString) > -1
				|| mq::ci_find_substr(spell.Category, m_filterString) > -1
				|| mq::ci_find_substr(spell.SubCategory, m_filterString) > -1
				|| mq::ci_find_substr(fmt::format("%d", spell.RankNum), m_filterString) > -1)
			))
			continue;

		categorizedSpells[spell.Category][spell.SubCategory].push_back(spell);
		m_filteredSpells.push_back(spell);
	}
	m_needFilter = false;
	m_useFilter = true;

}

void SpellPicker::DrawSpellPicker()
{
	if (!m_pickerOpen) return;

	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Spell Picker", &m_pickerOpen, ImGuiWindowFlags_NoDocking))
	{
		char buffer[256] = {};
		strncpy_s(buffer, m_filterString.c_str(), sizeof(buffer));
		if (ImGui::InputText("Search##SpellPicker", buffer, sizeof(buffer)))
		{
			if (!ci_equals(buffer, m_filterString))
			{
				m_filterString = buffer;
				m_needFilter = true;
			}
		} 
		else if (buffer[0] == '\0')
		{
			m_useFilter = false;
			m_filterString = buffer;
		}

		ImGui::SameLine();
		if (ImGui::Button("Refresh##SpellPicker"))
			SpellPicker::InitializeSpells();

		DrawSpellTree();
	}
	ImGui::End();
}

void SpellPicker::InspectSpell(int spellId)
{
	EQ_Spell* pSpell = GetSpellByID(spellId);
	if (pSpell)
	{
#if defined(CSpellDisplayManager__ShowSpell_x)
		if (pSpellDisplayManager)
			pSpellDisplayManager->ShowSpell(pSpell->ID, true, true, SpellDisplayType_SpellBookWnd);
#else
		char buffer[512] = { 0 };
		FormatSpellLink(buffer, 512, pSpell);
		TextTagInfo info = ExtractLink(buffer);
		ExecuteTextLink(info);
#endif
	}
}

SpellPicker::SpellPicker()
{
	InitializeSpells();
}

SpellPicker::~SpellPicker()
{
	if (m_pSpellBookIcon)
	{
		delete m_pSpellBookIcon;
		m_pSpellBookIcon = nullptr;
	}
	if (m_pSpellIcon)
	{
		delete m_pSpellIcon;
		m_pSpellIcon = nullptr;
	}
}