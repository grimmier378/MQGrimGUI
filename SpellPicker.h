#pragma once
#include "eqlib/Spells.h"

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

	void DrawSpellPicker();
	void DrawSpellTree();

	SpellPicker()
	{
		InitializeSpells();
	}

	~SpellPicker()
	{
		CleanupIcons();
	}

	void InitializeSpells()
	{
		Spells.clear();
		PopulateSpellData();
	}

	void SetOpen(bool open)
	{
		Open = open;
	}

	void ClearSelection()
	{
		SelectedSpell.reset();
	}


	// Selected spell
	std::shared_ptr<SpellData> SelectedSpell;

private:
	void PopulateSpellData();
	void CleanupIcons()
	{
		if (m_pSpellIcon)
		{
			delete m_pSpellIcon;
			m_pSpellIcon = nullptr;
		}
	}

	std::vector<SpellData> Spells;

	bool Open = false;
	std::string Filter;
};
