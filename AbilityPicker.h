#pragma once

#include <imgui.h>
#include <string>
#include <filesystem>

// Spell data structure
struct SpellData {
	int ID;
	std::string Name;
	std::string RankName;
	int Level;
	int Icon;
	std::string TargetType;
};

// AbilityPicker class
class AbilityPicker {
	CTextureAnimation* m_pSpellIcon = nullptr;

public:
	AbilityPicker();
	~AbilityPicker();

	// Main methods
	void InitializeSpells();
	void DrawAbilityPicker();
	void SetOpen(bool open);
	void ClearSelection();

	// Selected spell
	std::shared_ptr<SpellData> SelectedSpell;

private:
	void PopulateSpellData();
	void DrawSpellTree();
	void CleanupIcons();

	// Data
	std::vector<SpellData> Spells;

	// State
	bool Open = false;
	std::string Filter;
};
