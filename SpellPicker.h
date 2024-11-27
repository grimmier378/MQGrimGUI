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

	void PickUpSpell()
	{
		// TODO: Pick up spell
		// 
		// FIXME: This is not working, and causes a crash.
		//if (!m_NeedSpellPickup)
		//	return;

		// tried this passing spell ID
		// 
		//pCursorAttachment->AttachSpellToCursor(m_SpellID);
		// 
		// also tried this passing the icon and book index.
		// 
		////pCursorAttachment->AttachToCursor(m_pSpellIcon, nullptr, eCursorAttachment_MemorizeSpell, m_SpellBookIndex, nullptr, nullptr);
		//m_NeedSpellPickup = false;
		//m_SpellID = -1;
		//m_SpellBookIndex = -1;
		//m_SpellBookIcon = nullptr;
	}

	// Selected spell
	std::shared_ptr<SpellData> SelectedSpell;

	// Pick Up a Spell from the book?
	bool m_NeedSpellPickup = false;

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
	int m_SpellBookIndex = -1;
	CTextureAnimation* m_SpellBookIcon = nullptr;
	std::string m_PickerFilter;
	std::string m_TableFilter;

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
