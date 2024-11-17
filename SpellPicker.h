#pragma once

#include <imgui.h>
#include <string>

struct SpellData
{
	int ID;
	std::string Name;
	std::string RankName;
	int Level;
	int Icon;
	std::string TargetType;
	std::string Category;
	std::string SubCategory;
};

class SpellPicker
{
	CTextureAnimation* m_pSpellIcon = nullptr;

public:
	SpellPicker();
	~SpellPicker();

	void InitializeSpells();
	void DrawSpellPicker();
	void SetOpen(bool open);
	void ClearSelection();
	void DrawSpellTree();

	// Selected spell
	std::shared_ptr<SpellData> SelectedSpell;

private:
	void PopulateSpellData();
	void CleanupIcons();

	std::vector<SpellData> Spells;

	bool Open = false;
	std::string Filter;
};
