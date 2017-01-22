// ListCtrlWithCustomDraw.cpp : implementation file
//

#include "stdafx.h"
#include "ListViewCustomDraw.h"
#include "ListCtrlWithCustomDraw.h"


// CListCtrlWithCustomDraw

IMPLEMENT_DYNAMIC(CListCtrlWithCustomDraw, CListCtrl)

CListCtrlWithCustomDraw::CListCtrlWithCustomDraw()
{

}

CListCtrlWithCustomDraw::~CListCtrlWithCustomDraw()
{
}


BEGIN_MESSAGE_MAP(CListCtrlWithCustomDraw, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlWithCustomDraw::OnNMCustomdraw)
END_MESSAGE_MAP()




#define CUSTOM_DRAW
#define CUSTOM_DRAW_ALTERNATING_ROWS


// CListCtrlWithCustomDraw message handlers
//https://msdn.microsoft.com/en-us/library/ms364048(v=vs.80).aspx

void CListCtrlWithCustomDraw::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
#ifdef CUSTOM_DRAW
	//custom drawing
	//auto pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR); //more generic
	auto lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR); //specific to CListCtrl

	switch (lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:

#ifdef CUSTOM_DRAW_ALTERNATING_ROWS
		if ((lpLVCustomDraw->nmcd.dwItemSpec) % 2 == 0)
#else
		if ((lpLVCustomDraw->nmcd.dwItemSpec + lpLVCustomDraw->iSubItem) % 2 == 0)
#endif
		{
			lpLVCustomDraw->clrText = RGB(255, 255, 255); // white text
			lpLVCustomDraw->clrTextBk = RGB(0, 0, 0); // black background
		}
		else
		{
			lpLVCustomDraw->clrText = CLR_DEFAULT;
			lpLVCustomDraw->clrTextBk = CLR_DEFAULT;
		}
		break;

	default: break;
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
#else
	*pResult = CDRF_DODEFAULT;
#endif
}
