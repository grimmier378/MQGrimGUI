#pragma once
#include "eqlib/Spells.h"

struct SpellData
{
	int ID;
	int Level;
	int IconID;
	int SpellBookIndex;
	std::string Name;
	std::string RankName;
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
	void DrawSpellTable();

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
	std::string FilterTable;

private:
	static void InspectSpell(int spellId)
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

};
