// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CListCtrlOwnerDraw.h"
#include "MyListCtrl.h"


// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{

}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CMyListCtrl message handlers

void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	TRACE("Owner drawing item %d...\n", lpdis->itemID);
	
	if (lpdis->itemID == static_cast<UINT>(-1)) return;

	CDC dc;
	CRect rect = lpdis->rcItem;
	ASSERT(lpdis->itemID >= 0);
	dc.Attach(lpdis->hDC);

	//Text metrics for position calculation
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	if ((lpdis->itemAction | ODA_SELECT) && (lpdis->itemState  & ODS_SELECTED))
	{
		//This changes highlight colour to light yellow
		dc.FillSolidRect(rect, RGB(255, 255, 150));
		//dc.FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
		//if (bHasFocus) dc.DrawFocusRect(rect);
	}
	else
	{
		dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
	}

	//item
	auto x = rect.left + 5;
	auto s = GetItemText(lpdis->itemID, 0);
	dc.TextOut(x, rect.top + 5, s);

	//subitem 1
	s = GetItemText(lpdis->itemID, 1);
	x += GetColumnWidth(0);
	dc.TextOut(x, rect.top + 5, s);

	//subitem 2
	s = GetItemText(lpdis->itemID, 2);
	x += GetColumnWidth(1);
	dc.TextOut(x, rect.top + 5, s);

	//subitem 3
	x += GetColumnWidth(2);
	//Highlight found text in a different colour
	const CString criteria = L"for column";
	s = GetItemText(lpdis->itemID, 3);
	auto findIndex = s.Find(criteria, 0);

	if (findIndex > -1)
	{
		CRect r = rect;
		r.left = (x - 1) + (tm.tmAveCharWidth * findIndex);
		r.right = r.left + (tm.tmAveCharWidth * criteria.GetLength()) + 1;
		dc.FillSolidRect(r, RGB(255, 0, 0));
	}

	dc.TextOut(x, rect.top + 5, s);

	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);
	dc.Detach();
}

