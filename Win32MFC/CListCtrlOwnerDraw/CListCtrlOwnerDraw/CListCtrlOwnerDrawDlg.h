
// CListCtrlOwnerDrawDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "MyListCtrl.h"

// CCListCtrlOwnerDrawDlg dialog
class CCListCtrlOwnerDrawDlg : public CDialogEx
{
// Construction
public:
	CCListCtrlOwnerDrawDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLISTCTRLOWNERDRAW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()


protected:
	CMyListCtrl m_listCtrl;
};
