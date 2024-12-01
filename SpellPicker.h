#pragma once
#include "eqlib/Spells.h"
#include <mq/Plugin.h>

class SpellPicker
{
public:
	struct SpellData
	{
		int ID;
		int Level;
		int IconID;
		int RankNum;
		int SpellBookIndex;
		char* Name;
		char* TargetType;
		const char* Category;
		const char* SubCategory;
	};

	bool m_pickerOpen      = false;
	bool m_NeedSpellPickup = false;
	bool m_needFilter      = false;
	bool m_useFilter       = false;

	std::shared_ptr<SpellData> m_selectedSpell;
	std::vector<SpellData>     m_mySpells;
	std::vector<SpellData>     m_filteredSpells;

	CTextureAnimation*         m_pSpellBookIcon = nullptr;
	CTextureAnimation*         m_pSpellIcon     = nullptr;

	int                        m_SpellBookIndex = -1;

	std::string m_filterString;
private:
	std::unordered_map<const char*, std::unordered_map<const char*, std::vector<SpellData>>> categorizedSpells;

public:
	void DrawSpellPicker();
	void DrawSpellTree();
	void DrawSpellTable();
	void InitializeSpells();
	void FilterSpells();

	SpellPicker();
	~SpellPicker();

	void SetOpen(bool open)
	{
		m_pickerOpen = open;
	}

	void ClearSelection()
	{
		m_selectedSpell.reset();
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

private:

	void PopulateSpellData();

	void InspectSpell(int spellId);

};
