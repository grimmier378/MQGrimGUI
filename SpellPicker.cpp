/** Spell Picker GUI
* Original Author: Aquietone
* Converted to C++ by: Grimmier
* 
* This is a conversion from Aquietone's Ability Picker lua to C++ for displaying and selecing spells in  your spell book
* 
* Displays your spell book information in a searchable tree, sorted by Category, Subcategory and Level (highest at top) 
*/
#include <mq/Plugin.h>
#include <mq/imgui/Widgets.h>
#include <algorithm>
#include "SpellPicker.h"

const std::map<int, std::string> SpellCategoryMap = {
	{1,		"SPELLCAT_AEGOLISM"},
	{2,		"SPELLCAT_AGILITY"},
	{3,		"SPELLCAT_ALLIANCE"},
	{4,		"SPELLCAT_ANIMAL"},
	{5,		"SPELLCAT_ANTONICA"},
	{6,		"SPELLCAT_ARMOR_CLASS"},
	{7,		"SPELLCAT_ATTACK"},
	{8,		"SPELLCAT_BANE"},
	{9,		"SPELLCAT_BLIND"},
	{10,	"SPELLCAT_BLOCK"},
	{11,	"SPELLCAT_CALM"},
	{12,	"SPELLCAT_CHARISMA"},
	{13,	"SPELLCAT_CHARM"},
	{14,	"SPELLCAT_COLD"},
	{15,	"SPELLCAT_COMBAT_ABILITIES"},
	{16,	"SPELLCAT_COMBAT_INNATES"},
	{17,	"SPELLCAT_CONVERSIONS"},
	{18,	"SPELLCAT_CREATE_ITEM"},
	{19,	"SPELLCAT_CURE"},
	{20,	"SPELLCAT_DAMAGE_OVER_TIME"},
	{21,	"SPELLCAT_DAMAGE_SHIELD"},
	{22,	"SPELLCAT_DEFENSIVE"},
	{23,	"SPELLCAT_DESTROY"},
	{24,	"SPELLCAT_DEXTERITY"},
	{25,	"SPELLCAT_DIRECT_DAMAGE"},
	{26,	"SPELLCAT_DISARM_TRAPS"},
	{27,	"SPELLCAT_DISCIPLINES"},
	{28,	"SPELLCAT_DISCORD"},
	{29,	"SPELLCAT_DISEASE"},
	{30,	"SPELLCAT_DISEMPOWERING"},
	{31,	"SPELLCAT_DISPEL"},
	{32,	"SPELLCAT_DURATION_HEALS"},
	{33,	"SPELLCAT_DURATION_TAP"},
	{34,	"SPELLCAT_ENCHANT_METAL"},
	{35,	"SPELLCAT_ENTHRALL"},
	{36,	"SPELLCAT_FAYDWER"},
	{37,	"SPELLCAT_FEAR"},
	{38,	"SPELLCAT_FIRE"},
	{39,	"SPELLCAT_FIZZLE_RATE"},
	{40,	"SPELLCAT_FUMBLE"},
	{41,	"SPELLCAT_HASTE"},
	{42,	"SPELLCAT_HEALS"},
	{43,	"SPELLCAT_HEALTH"},
	{44,	"SPELLCAT_HEALTH_MANA"},
	{45,	"SPELLCAT_HP_BUFFS"},
	{46,	"SPELLCAT_HP_TYPE_ONE"},
	{47,	"SPELLCAT_HP_TYPE_TWO"},
	{48,	"SPELLCAT_ILLUSION_OTHER"},
	{49,	"SPELLCAT_ILLUSION_ADVENTURER"},
	{50,	"SPELLCAT_IMBUE_GEM"},
	{51,	"SPELLCAT_INVISIBILITY"},
	{52,	"SPELLCAT_INVULNERABILITY"},
	{53,	"SPELLCAT_JOLT"},
	{54,	"SPELLCAT_KUNARK"},
	{55,	"SPELLCAT_LEVITATE"},
	{56,	"SPELLCAT_LIFE_FLOW"},
	{57,	"SPELLCAT_LUCLIN"},
	{58,	"SPELLCAT_MAGIC"},
	{59,	"SPELLCAT_MANA"},
	{60,	"SPELLCAT_MANA_DRAIN"},
	{61,	"SPELLCAT_MANA_FLOW"},
	{62,	"SPELLCAT_MELEE_GUARD"},
	{63,	"SPELLCAT_MEMORY_BLUR"},
	{64,	"SPELLCAT_MISC"},
	{65,	"SPELLCAT_MOVEMENT"},
	{66,	"SPELLCAT_OBJECTS"},
	{67,	"SPELLCAT_ODUS"},
	{68,	"SPELLCAT_OFFENSIVE"},
	{69,	"SPELLCAT_PET"},
	{70,	"SPELLCAT_PET_HASTE"},
	{71,	"SPELLCAT_PET_MISC_BUFFS"},
	{72,	"SPELLCAT_PHYSICAL"},
	{73,	"SPELLCAT_PICKLOCK"},
	{74,	"SPELLCAT_PLANT"},
	{75,	"SPELLCAT_POISON"},
	{76,	"SPELLCAT_POWER_TAP"},
	{77,	"SPELLCAT_QUICK_HEAL"},
	{78,	"SPELLCAT_REFLECTION"},
	{79,	"SPELLCAT_REGEN"},
	{80,	"SPELLCAT_RESIST_BUFF"},
	{81,	"SPELLCAT_RESIST_DEBUFFS"},
	{82,	"SPELLCAT_RESURRECTION"},
	{83,	"SPELLCAT_ROOT"},
	{84,	"SPELLCAT_RUNE"},
	{85,	"SPELLCAT_SENSE_TRAP"},
	{86,	"SPELLCAT_SHADOWSTEP"},
	{87,	"SPELLCAT_SHIELDING"},
	{88,	"SPELLCAT_SLOW"},
	{89,	"SPELLCAT_SNARE"},
	{90,	"SPELLCAT_SPECIAL"},
	{91,	"SPELLCAT_SPELL_FOCUS"},
	{92,	"SPELLCAT_SPELL_GUARD"},
	{93,	"SPELLCAT_SPELLSHIELD"},
	{94,	"SPELLCAT_STAMINA"},
	{95,	"SPELLCAT_STATISTIC_BUFFS"},
	{96,	"SPELLCAT_STRENGTH"},
	{97,	"SPELLCAT_STUN"},
	{98,	"SPELLCAT_SUM_AIR"},
	{99,	"SPELLCAT_SUM_ANIMATION"},
	{100,	"SPELLCAT_SUM_EARTH"},
	{101,	"SPELLCAT_SUM_FAMILIAR"},
	{102,	"SPELLCAT_SUM_FIRE"},
	{103,	"SPELLCAT_SUM_UNDEAD"},
	{104,	"SPELLCAT_SUM_WARDER"},
	{105,	"SPELLCAT_SUM_WATER"},
	{106,	"SPELLCAT_SUMMON_ARMOR"},
	{107,	"SPELLCAT_SUMMON_FOCUS"},
	{108,	"SPELLCAT_SUMMON_FOOD_WATER"},
	{109,	"SPELLCAT_SUMMON_UTILITY"},
	{110,	"SPELLCAT_SUMMON_WEAPON"},
	{111,	"SPELLCAT_SUMMONED"},
	{112,	"SPELLCAT_SYMBOL"},
	{113,	"SPELLCAT_TAELOSIA"},
	{114,	"SPELLCAT_TAPS"},
	{115,	"SPELLCAT_TECHNIQUES"},
	{116,	"SPELLCAT_THE_PLANES"},
	{117,	"SPELLCAT_TIMER_1"},
	{118,	"SPELLCAT_TIMER_2"},
	{119,	"SPELLCAT_TIMER_3"},
	{120,	"SPELLCAT_TIMER_4"},
	{121,	"SPELLCAT_TIMER_5"},
	{122,	"SPELLCAT_TIMER_6"},
	{123,	"SPELLCAT_TRANSPORT"},
	{124,	"SPELLCAT_UNDEAD"},
	{125,	"SPELLCAT_UTILITY_BENEFICIAL"},
	{126,	"SPELLCAT_UTILITY_DETRIMENTAL"},
	{127,	"SPELLCAT_VELIOUS"},
	{128,	"SPELLCAT_VISAGES"},
	{129,	"SPELLCAT_VISION"},
	{130,	"SPELLCAT_WISDOM_INTELLIGENCE"},
	{131,	"SPELLCAT_TRAPS"},
	{132,	"SPELLCAT_AURAS"},
	{133,	"SPELLCAT_ENDURANCE"},
	{134,	"SPELLCAT_SERPENTS_SPINE"},
	{135,	"SPELLCAT_CORRUPTION"},
	{136,	"SPELLCAT_LEARNING"},
	{137,	"SPELLCAT_CHROMATIC"},
	{138,	"SPELLCAT_PRISMATIC"},
	{139,	"SPELLCAT_SUM_SWARM"},
	{140,	"SPELLCAT_DELAYED"},
	{141,	"SPELLCAT_TEMPORARY"},
	{142,	"SPELLCAT_TWINCAST"},
	{143,	"SPELLCAT_SUM_BODYGUARD"},
	{144,	"SPELLCAT_HUMANOID"},
	{145,	"SPELLCAT_HASTE_SPELL_FOCUS"},
	{146,	"SPELLCAT_TIMER_7"},
	{147,	"SPELLCAT_TIMER_8"},
	{148,	"SPELLCAT_TIMER_9"},
	{149,	"SPELLCAT_TIMER_10"},
	{150,	"SPELLCAT_TIMER_11"},
	{151,	"SPELLCAT_TIMER_12"},
	{152,	"SPELLCAT_HATRED"},
	{153,	"SPELLCAT_FAST"},
	{154,	"SPELLCAT_ILLUSION_SPECIAL"},
	{155,	"SPELLCAT_TIMER_13"},
	{156,	"SPELLCAT_TIMER_14"},
	{157,	"SPELLCAT_TIMER_15"},
	{158,	"SPELLCAT_TIMER_16"},
	{159,	"SPELLCAT_TIMER_17"},
	{160,	"SPELLCAT_TIMER_18"},
	{161,	"SPELLCAT_TIMER_19"},
	{162,	"SPELLCAT_TIMER_20"},
	{163,	"SPELLCAT_ALARIS"},
	{164,	"SPELLCAT_COMBINATION"},
	{165,	"SPELLCAT_INDEPENDENT"},
	{166,	"SPELLCAT_SKILL_ATTACKS"},
	{167,	"SPELLCAT_INCOMING"},
	{168,	"SPELLCAT_CURSE"},
	{169,	"SPELLCAT_TIMER_21"},
	{170,	"SPELLCAT_TIMER_22"},
	{171,	"SPELLCAT_TIMER_23"},
	{172,	"SPELLCAT_TIMER_24"},
	{173,	"SPELLCAT_TIMER_25"},
	{174,	"SPELLCAT_DRUNKENNESS"},
	{175,	"SPELLCAT_THROWING"},
	{176,	"SPELLCAT_MELEE_DAMAGE"}
};

/**
* @fn ProperCase
* 
* @brief Converts a string from any case to Proper Case (First letter of each word capitalized)
* 
* @param input - The string to convert
* 
* @return std::string - The converted string
*/
static std::string ProperCase(const std::string& input) {
	std::string result;
	bool capitalize = true;

	for (char ch : input)
	{
		if (ch == '_')
		{
			result += ' ';
			capitalize = true;
		}
		else if (capitalize)
		{
			result += std::toupper(ch);
			capitalize = false;
		}
		else
		{
			result += std::tolower(ch);
		}
	}

	return result;
}


/**
* @fn DrawSpellTree
* 
* @brief Displays the spells from your Spellbook in a tree format, sorted by Category, Subcategory and Level
* 
*/
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
		// since this is conversion from the lua lets follow the example
		// make the table layout the same first
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<SpellData>>> categorizedSpells;

		for (const auto& spell : Spells)
		{
			if (!Filter.empty() && mq::ci_find_substr(spell.Name, Filter) == -1)
				continue;

			categorizedSpells[spell.Category][spell.SubCategory].push_back(spell);
		}

		for (const auto& [categoryName, subCategories] : categorizedSpells)
		{
			if (ImGui::TreeNode(categoryName.c_str())) 
			{
				for (const auto& [subCategoryName, spells] : subCategories)
				{
					if (ImGui::TreeNode(subCategoryName.c_str()))
					{
						for (const auto& spell : spells)
						{
							ImGui::PushID(spell.ID);
							ImGui::BeginGroup();

							m_pSpellIcon->SetCurCell(spell.IconID);
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

				ImGui::TreePop(); 
			}
		}

		ImGui::TreePop();
	}
}


void SpellPicker::DrawSpellTable()
{
	// Initialize the spell icon animation if needed
	if (!m_pSpellIcon)
	{
		m_pSpellIcon = new CTextureAnimation();
		if (CTextureAnimation* temp = pSidlMgr->FindAnimation("A_SpellGems"))
			*m_pSpellIcon = *temp;
	}

	char buffer[256] = {};
	strncpy_s(buffer, FilterTable.c_str(), sizeof(buffer));
	if (ImGui::InputText("Search##SpellTable", buffer, sizeof(buffer)))
		FilterTable = buffer;

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
		for (const auto& spell : Spells)
		{
			if (!(!FilterTable.empty() &&	(mq::ci_find_substr(spell.Name, FilterTable) > -1
				|| mq::ci_find_substr(spell.Category, FilterTable) > -1
				|| mq::ci_find_substr(spell.SubCategory, FilterTable) > -1
				|| mq::ci_find_substr(spell.RankName, FilterTable) > -1
				) 
				|| FilterTable.empty()))
				continue;

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			m_pSpellIcon->SetCurCell(spell.IconID);

			ImGui::PushID(spell.ID + spell.IconID);
			imgui::DrawTextureAnimation(m_pSpellIcon, CXSize(20, 20));
			if (ImGui::BeginPopupContextItem(("SpellContextMenu##"), ImGuiPopupFlags_MouseButtonRight))
			{
				std::string label = "Inspect##" + spell.Name;
				if (ImGui::MenuItem(label.c_str()))
					InspectSpell(spell.ID);

				ImGui::EndPopup();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetItemTooltip("IconID: %d", spell.IconID);
			ImGui::PopID();


			ImGui::TableNextColumn();
			ImGui::Text("%d", spell.Level);

			ImGui::TableNextColumn();
			ImGui::PushID(spell.ID); // Ensure unique ID
			ImGui::Text(spell.Name.c_str());
			// FIX ME: allow picking up gem from table as if it was the book. 

			//if (ImGui::IsItemClicked(0))
			//{
			//	// 
			//	// Sets selected spell to the spell clicked in the table
			//	SelectedSpell = std::make_shared<SpellData>(spell);
			//	pSpellBookWnd->ParentWndNotification(pSpellBookWnd, XWM_LCLICKHOLD, (int*)spell.SpellBookIndex);
			//	pSpellBookWnd->ParentWndNotification(pSpellBookWnd, XWM_LBUTTONUPAFTERHELD, (int*)spell.SpellBookIndex);
			//}
			//
			ImGui::PopID();

			ImGui::TableNextColumn();
			ImGui::Text(spell.RankName.c_str());

			ImGui::TableNextColumn();
			ImGui::Text(spell.Category.c_str());

			ImGui::TableNextColumn();
			ImGui::Text(spell.SubCategory.c_str());

			ImGui::TableNextColumn();
			ImGui::Text("%d", spell.SpellBookIndex);


			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Level: %d", spell.Level);
				ImGui::Text("Name: %s", spell.Name.c_str());
				ImGui::Text("Rank: %s", spell.RankName.c_str());
				ImGui::Text("Category: %s", spell.Category.c_str());
				ImGui::Text("SubCategory: %s", spell.SubCategory.c_str());
				ImGui::Text("SpellBookIndex: %d", spell.SpellBookIndex);

				ImGui::EndTooltip();
			}
		}

		ImGui::EndTable();
	}
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
			SpellData spellData;
			spellData.ID = pSpell->ID;
			spellData.Name = pSpell->Name;
			spellData.RankName = pSpell->SpellRank;
			spellData.Level = pSpell->GetSpellLevelNeeded(pCharData->GetClass());
			spellData.IconID = pSpell->SpellIcon;
			spellData.SpellBookIndex = i;

			// Use Map to get the Names of the Category and SubCategory not SPELLCAT_SOMETHING
			auto categoryIt = SpellCategoryMap.find(pSpell->Category);
			spellData.Category = (categoryIt != SpellCategoryMap.end()) ? ProperCase(categoryIt->second.substr(9)) : "Unknown";

			auto subCategoryIt = SpellCategoryMap.find(pSpell->Subcategory);
			spellData.SubCategory = (subCategoryIt != SpellCategoryMap.end()) ? ProperCase(subCategoryIt->second.substr(9)) : "Unknown";

			Spells.push_back(spellData);
		}
	}

	std::sort(Spells.begin(), Spells.end(), [](const SpellData& a, const SpellData& b) 
	{
		if (a.Category != b.Category)
			return a.Category < b.Category;
		if (a.SubCategory != b.SubCategory)
			return a.SubCategory < b.SubCategory; 
		return a.Level > b.Level;
	});
}


void SpellPicker::DrawSpellPicker()
{
	if (!Open) return;

	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Spell Picker", &Open, ImGuiWindowFlags_NoDocking))
	{
		char buffer[256] = {};
		strncpy_s(buffer, Filter.c_str(), sizeof(buffer));
		if (ImGui::InputText("Search##SpellPicker", buffer, sizeof(buffer)))
			Filter = buffer;

		ImGui::SameLine();
		if (ImGui::Button("Refresh##SpellPicker"))
			SpellPicker::InitializeSpells();

		DrawSpellTree();
	}
	ImGui::End();
}
