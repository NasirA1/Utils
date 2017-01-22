
// CListCtrlOwnerDrawDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CListCtrlOwnerDraw.h"
#include "CListCtrlOwnerDrawDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()




// CCListCtrlOwnerDrawDlg dialog

CCListCtrlOwnerDrawDlg::CCListCtrlOwnerDrawDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLISTCTRLOWNERDRAW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CCListCtrlOwnerDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CCListCtrlOwnerDrawDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
END_MESSAGE_MAP()



void InitList(CDialog& parent, CListCtrl& listCtrl)
{
	listCtrl.SetExtendedStyle(listCtrl.GetStyle() | LVS_EX_FULLROWSELECT | LVS_REPORT);

	for (auto i = 0; i < 4; ++i)
	{
		CString s;
		s.Format(L"Column %d", i);
		listCtrl.InsertColumn(i, s, 0, 100);
	}

	parent.SendMessage(WM_SIZE);
}


void PopulateSampleData(CListCtrl& listView)
{
	constexpr const auto ITEM_COUNT = 10;

	for (int i = 0; i < ITEM_COUNT; i++)
	{
		CString s;
		s.Format(L"%d", i);

		// insert first column
		listView.InsertItem(i, s);

		// insert other columns
		for (int col = 1; col < 4; col++)
		{
			s.Format(L"Data for column %d", col);
			listView.SetItemText(i, col, s);
		}
	}

	listView.EnsureVisible(listView.GetItemCount() - 1, TRUE);
	listView.SetItemState(0, ~LVIS_SELECTED, LVIS_SELECTED);
	listView.SetItemState(listView.GetItemCount() - 1, LVIS_SELECTED, LVIS_SELECTED);
	listView.SetSelectionMark(listView.GetItemCount() - 1);
	listView.SetFocus();
}



// CCListCtrlOwnerDrawDlg message handlers
BOOL CCListCtrlOwnerDrawDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitList(*this, m_listCtrl);
	PopulateSampleData(m_listCtrl);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CCListCtrlOwnerDrawDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCListCtrlOwnerDrawDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCListCtrlOwnerDrawDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CCListCtrlOwnerDrawDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (!m_listCtrl.m_hWnd) return;

	//Stretch last column to fill window
	m_listCtrl.SetColumnWidth(0, 65);
	m_listCtrl.SetColumnWidth(1, 100);
	m_listCtrl.SetColumnWidth(2, 100);

	CRect myBounds;
	m_listCtrl.GetClientRect(myBounds);
	m_listCtrl.SetColumnWidth(3, myBounds.Width() - (65 + 100 + 100));
}
