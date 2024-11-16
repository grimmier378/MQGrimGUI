#include "AbilityPicker.h"
#include <imgui.h>
#include "main/MQ2Main.h"
#include <mq/imgui/Widgets.h>
#include <chrono>
#include <string>

AbilityPicker::AbilityPicker() {
	InitializeSpells();
}

AbilityPicker::~AbilityPicker() {
	CleanupIcons();
}

void AbilityPicker::InitializeSpells() {
	Spells.clear();
	PopulateSpellData();
}

//void AbilityPicker::PopulateSpellData() {
//	if (!pSpellBookWnd) return;
//
//	for (int i = 0; i < NUM_BOOK_SLOTS; ++i) {
//		PSPELL pSpell = pSpellBookWnd->SpellId;
//		if (pSpell) {
//			SpellData spellData;
//			spellData.ID = pSpell->ID;
//			spellData.Name = pSpell->Name;
//			spellData.RankName = pSpell->Name; // Adjust as needed
//			spellData.Level = pSpell->Level;
//			spellData.Icon = pSpell->SpellIcon;
//			spellData.TargetType = pSpell->TargetType;
//
//			Spells.push_back(spellData);
//		}
//	}
//
//	// Sort spells by level
//	std::sort(Spells.begin(), Spells.end(), [](const SpellData& a, const SpellData& b) {
//		return a.Level > b.Level;
//		});
//}

void AbilityPicker::DrawAbilityPicker() {
	if (!Open) return;

	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Ability Picker", &Open, ImGuiWindowFlags_NoDocking)) {
		char buffer[256] = {};
		std::strncpy(buffer, Filter.c_str(), sizeof(buffer));
		if (ImGui::InputText("Search##AbilityPicker", buffer, sizeof(buffer))) {
			Filter = buffer;
		}

		DrawSpellTree();
	}
	ImGui::End();
}

void AbilityPicker::PopulateSpellData() {
    if (!pCharData)
		return;

    for (int i = 0; i < NUM_BOOK_SLOTS; ++i) {

        int spellId = pCharData->GetSpellBook(i); // Access spell ID from the player's spell book
        if (spellId == -1) continue; // Skip empty slots

		EQ_Spell* pSpell = mq::GetSpellByID(spellId); // Retrieve spell data
        if (pSpell) {
            SpellData spellData;
            spellData.ID = pSpell->ID;
            spellData.Name = pSpell->Name;
            spellData.RankName = pSpell->Name; // Adjust this as needed for rank names
            spellData.Level = pSpell->GetSpellLevelNeeded(pCharData->GetClass());
            spellData.Icon = pSpell->SpellIcon;
            spellData.TargetType = pSpell->TargetType;

            Spells.push_back(spellData);
        }
    }

    // Sort spells by level
    std::sort(Spells.begin(), Spells.end(), [](const SpellData& a, const SpellData& b) {
        return a.Level > b.Level;
    });
}

void AbilityPicker::SetOpen(bool open) {
	Open = open;
}

void AbilityPicker::ClearSelection() {
	SelectedSpell.reset();
}

void AbilityPicker::CleanupIcons() {
	if (m_pSpellIcon) {
		delete m_pSpellIcon;
		m_pSpellIcon = nullptr;
	}
}
