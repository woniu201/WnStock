/********************************************************
Copyright (C), 2016-2017,
FileName: 	MainFrm
Author: 	woniu201
Email: 		wangpengfei.201@163.com
Created: 	2017/11/07
Description:单文档框架类
********************************************************/

#include "stdafx.h"
#include "WnStock.h"

#include "MainFrm.h"
#include "WnStockDoc.h"
#include "WnStockView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, &CMainFrame::OnToolbarReset)
	ON_COMMAND(ID_TOOLBAR_BUT_ESC, &CMainFrame::OnToolbarButEsc)
	ON_COMMAND(ID_TOOLBAR_BUT_DAYK, &CMainFrame::OnToolbarButDayk)
	ON_COMMAND(ID_TOOLBAR_BUT_WEEKK, &CMainFrame::OnToolbarButWeekk)
	ON_COMMAND(ID_TOOLBAR_BUT_MONTHK, &CMainFrame::OnToolbarButMonthk)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	//开启定时器
	SetTimer(1, 1000, NULL);

	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 设置用于绘制所有用户界面元素的视觉管理器
	//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC ) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME_256))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	//修改工具栏背景颜色
	HBRUSH newBr=CreateSolidBrush(RGB(212,122,212));
	SetClassLong(m_wndToolBar.m_hWnd,GCL_HBRBACKGROUND,(long)newBr);
	
	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	//工具栏自定义下拉菜单
// 	CString strCustomize;
// 	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
// 	ASSERT(bNameValid);
// 	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	//工具栏显示文字
	m_wndToolBar.EnableTextLabels(TRUE);

/*************************状态栏部分**********************************/

	//修改状态栏外观
	CRect cRect;
	GetClientRect(&cRect);
	int width = (cRect.right - cRect.left)/7;
	m_wndStatusBar.SetPaneInfo(0,m_wndStatusBar.GetDlgCtrlID(),SBPS_STRETCH,  2*width);
	m_wndStatusBar.SetPaneInfo(1,m_wndStatusBar.GetDlgCtrlID(),SBPS_STRETCH,  2*width);
	m_wndStatusBar.SetPaneInfo(2,m_wndStatusBar.GetDlgCtrlID(),SBPS_STRETCH,  2*width);
	m_wndStatusBar.SetPaneInfo(3,m_wndStatusBar.GetDlgCtrlID(),SBPS_STRETCH,  width);

	for(int i=0; i<4; i++)
	{
		m_wndStatusBar.SetPaneBackgroundColor(i,RGB(0,0,0), 1);
	}

// 	// 启用 Visual Studio 2005 样式停靠窗口行为
// 	CDockingManager::SetDockingMode(DT_SMART);
// 	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
// 	EnableAutoHidePanes(CBRS_ALIGN_ANY);
// 
// 	// 启用工具栏和停靠窗口菜单替换
// 	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);
// 
// 	// 启用快速(按住 Alt 拖动)工具栏自定义
// 	CMFCToolBar::EnableQuickCustomization();
// 
// 	if (CMFCToolBar::GetUserImages() == NULL)
// 	{
// 		// 加载用户定义的工具栏图像
// 		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
// 		{
// 			CMFCToolBar::SetUserImages(&m_UserImages);
// 		}
// 	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。

// 	CList<UINT, UINT> lstBasicCommands;
// 
// 	lstBasicCommands.AddTail(ID_FILE_NEW);
// 	lstBasicCommands.AddTail(ID_FILE_OPEN);
// 	lstBasicCommands.AddTail(ID_FILE_SAVE);
// 	lstBasicCommands.AddTail(ID_FILE_PRINT);
// 	lstBasicCommands.AddTail(ID_APP_EXIT);
// 	lstBasicCommands.AddTail(ID_EDIT_CUT);
// 	lstBasicCommands.AddTail(ID_EDIT_PASTE);
// 	lstBasicCommands.AddTail(ID_EDIT_UNDO);
// 	lstBasicCommands.AddTail(ID_APP_ABOUT);
// 	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
// 	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
// 	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
// 
// 	CMFCToolBar::SetBasicCommands(lstBasicCommands);

// 	m_wndToolBar.ModifyStyle(0, BTNS_SHOWTEXT | TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_LIST);
// 	m_wndToolBar.SetButtonText(0, "返回");
// 	m_wndToolBar.SetButtonText(1, "周期");

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	
	//固定边框属性不能拖动大小
	//cs.style &= ~WS_THICKFRAME;
//  	cs.cx = 1400;
//  	cs.cy = 680;
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


/*
*	@ 定时器Timer,更新Maket数据和时间
*
*/
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	//状态栏显示时间
	//显示时间
	m_wndStatusBar.SetPaneTextColor(3, RGB(255,255,0), 1);
	CFont font;
	font.CreateFont(18,8,0,0,FW_NORMAL,false,false,false,DEFAULT_CHARSET,OUT_DEVICE_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,_T("隶书"));
	GetDlgItem(m_wndStatusBar.GetDlgCtrlID())->SetFont(&font);   
	CTime curTime = CTime::GetCurrentTime();   
	CString strTime = curTime.Format(_T("%H:%M:%S"));   
	m_wndStatusBar.SetPaneText(3, strTime);  


	RealTimeMarket* ralTimeMarket_SH = (RealTimeMarket*)malloc(sizeof(RealTimeMarket));
	RealTimeMarket* ralTimeMarket_SZ = (RealTimeMarket*)malloc(sizeof(RealTimeMarket));
	RealTimeMarket* ralTimeMarket_CY = (RealTimeMarket*)malloc(sizeof(RealTimeMarket));
	memset(ralTimeMarket_SH, 0, sizeof(RealTimeMarket));
	memset(ralTimeMarket_SZ, 0, sizeof(RealTimeMarket));
	memset(ralTimeMarket_CY, 0, sizeof(RealTimeMarket));

	StockData stockData;
	stockData.GetRealTimeMarketData("000001", ralTimeMarket_SH);
	stockData.GetRealTimeMarketData("399001", ralTimeMarket_SZ);
	stockData.GetRealTimeMarketData("399006", ralTimeMarket_CY);

	CString str;

	//SH
	if (ralTimeMarket_SH->marketRatio < 0)
	{
		m_wndStatusBar.SetPaneTextColor(0, RGB(0,255,255), 1);
	}
	else
	{
		m_wndStatusBar.SetPaneTextColor(0, RGB(255,0,0), 1);
	}
	//str.Format("%s：%.2f %.2f %.2f%%", ralTimeMarket_SH->marketName, ralTimeMarket_SH->marketIndex,  ralTimeMarket_SH->marketRose, ralTimeMarket_SH->marketRatio);
	str.Format("%s：%.2f %.2f%%", ralTimeMarket_SH->marketName, ralTimeMarket_SH->marketIndex,  ralTimeMarket_SH->marketRatio);
	m_wndStatusBar.SetPaneText(0, str); 

	//SZ
	if (ralTimeMarket_SZ->marketRatio < 0)
	{
		m_wndStatusBar.SetPaneTextColor(1, RGB(0,255,255), 1);
	}
	else
	{
		m_wndStatusBar.SetPaneTextColor(1, RGB(255,0,0), 1);
	}
	//str.Format("%s：%.2f %.2f %.2f%%", ralTimeMarket_SZ->marketName, ralTimeMarket_SZ->marketIndex,  ralTimeMarket_SZ->marketRose, ralTimeMarket_SZ->marketRatio);
	str.Format("%s：%.2f %.2f%%", ralTimeMarket_SZ->marketName, ralTimeMarket_SZ->marketIndex, ralTimeMarket_SZ->marketRatio);
	m_wndStatusBar.SetPaneText(1, str); 

	//CY
	if (ralTimeMarket_CY->marketRatio < 0)
	{
		m_wndStatusBar.SetPaneTextColor(2, RGB(0,255,255), 1);
	}
	else
	{
		m_wndStatusBar.SetPaneTextColor(2, RGB(255,0,0), 1);
	}
	str.Format("%s：%.2f %.2f%%", ralTimeMarket_CY->marketName, ralTimeMarket_CY->marketIndex,  ralTimeMarket_CY->marketRatio);
	m_wndStatusBar.SetPaneText(2, str); 


	free(ralTimeMarket_SH);
	ralTimeMarket_SH = NULL;
	free(ralTimeMarket_SZ);
	ralTimeMarket_SZ = NULL;
	free(ralTimeMarket_CY);
	ralTimeMarket_CY = NULL;

	CFrameWndEx::OnTimer(nIDEvent);
}


/*
*	@ 检测Fram类的系统消息
*/
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
 	CWnStockView* pView = (CWnStockView *)GetActiveView();
	CWnStockDoc* pDoc = (CWnStockDoc *)GetActiveDocument();

	if (pView->m_drawStatus == 1) //K线部分
	{
		if (nID == SC_MAXIMIZE)
		{
			pView->bFirstCrossLine = TRUE;
			pView->bCrossLine = FALSE;
		}
		else if (nID == SC_MINIMIZE)
		{
		}
		else if (nID == SC_RESTORE)
		{
			pView->bFirstCrossLine = TRUE;
			pView->bCrossLine = FALSE;
		}
	}else if (pView->m_drawStatus == 2) //分时部分
	{
		if (nID == SC_MAXIMIZE)
		{
			pView->bGetPoint = TRUE;
		}
		else if (nID == SC_MINIMIZE)
		{
		}
		else if (nID == SC_RESTORE)
		{
			
		}else if (nID == 0xF012)
		{
			pView->bGetPoint = TRUE;
		}
	}

	CFrameWndEx::OnSysCommand(nID, lParam);
}

/*
*	@ 启动最大化
*/
void CMainFrame::ActivateFrame(int nCmdShow)
{
	//nCmdShow   =   SW_SHOWMAXIMIZED;
	//nCmdShow   =   SW_MAX;
	CFrameWndEx::ActivateFrame(nCmdShow);
}


/*
函数名称：OnToolbarReset(WPARAM wp,LPARAM)
函数功能：重绘工具栏，实现工具栏下拉按钮功能
*/
LRESULT CMainFrame::OnToolbarReset(WPARAM wp,LPARAM)
{
	UINT uiToolBarId = (UINT) wp;
	switch (uiToolBarId)
	{
	case IDR_MAINFRAME_256:
		{   
			//Replace CMFCToolBarComboBoxButton
			m_comboButton = new CMFCToolBarComboBoxButton(ID_TOOLBAR_BUT_K, GetCmdMgr()->GetCmdImage(ID_TOOLBAR_BUT_K/*, FALSE*/), CBS_DROPDOWNLIST,100);
			
			HMENU hMenu = CreateMenu();//CreatePopupMenu();
			AppendMenu(hMenu, MF_STRING, ID_TOOLBAR_BUT_DAYK, _T("日   线"));
			AppendMenu(hMenu, MF_STRING, ID_TOOLBAR_BUT_WEEKK, _T("周   线"));
			AppendMenu(hMenu, MF_STRING, ID_TOOLBAR_BUT_MONTHK, _T("月   线"));
			CMFCToolBarMenuButton btnMenuRoot( (UINT)-1, hMenu, 0, "周  期");
			btnMenuRoot.m_bText = FALSE;   
			btnMenuRoot.m_bImage = TRUE;   
			btnMenuRoot.m_nStyle |= TBBS_DROPDOWN;
			btnMenuRoot.m_nStyle &= ~TBBS_BUTTON;
			m_wndToolBar.ReplaceButton(ID_TOOLBAR_BUT_K, btnMenuRoot);

		}                              
		break;                                        
	}

	return 0;
}

void CMainFrame::OnToolbarButEsc()
{
	CWnStockDoc* pDoc = (CWnStockDoc*)GetActiveDocument();
	CWnStockView* pView = (CWnStockView*)GetActiveView();
	pView->bScroll = TRUE;
	pView->OnInitialUpdate();

	pView->m_drawStatus = 0;
	pView->m_stockCode = "";
	pView->m_stockName = "";
	pDoc->GetMytockData();
	pView->Invalidate();
}


void CMainFrame::OnToolbarButDayk()
{
	CWnStockDoc* pDoc = (CWnStockDoc*)GetActiveDocument();
	CWnStockView* pView = (CWnStockView*)GetActiveView();

	pView->bScroll = false;
	pView->OnInitialUpdate();
	pView->m_drawStatus = 1;
	pDoc->stockDoc->KDay = 60;
	pDoc->stockDoc->GetDayKData(pView->m_stockCode.GetBuffer(pView->m_stockCode.GetLength()));
	Invalidate();
}


void CMainFrame::OnToolbarButWeekk()
{
	CWnStockDoc* pDoc = (CWnStockDoc*)GetActiveDocument();
	CWnStockView* pView = (CWnStockView*)GetActiveView();

	pView->bScroll = false;
	pView->OnInitialUpdate();
	pView->m_drawStatus = 1;
	//pDoc->stockDoc->KDay = 60;
	pDoc->stockDoc->GetWeekKStockData(pView->m_stockCode.GetBuffer(pView->m_stockCode.GetLength()));

//初始化变量
	//获取周数
	int weekNum = pDoc->stockDoc->vKData.size();
	if (weekNum < 60)
	{
		pDoc->stockDoc->KDay = weekNum/2;
	}
	else
	{
		pDoc->stockDoc->KDay = 60;
	}
	pDoc->stockDoc->KBegin = 0;
	pDoc->stockDoc->KPos = 0;
	Invalidate();
}



void CMainFrame::OnToolbarButMonthk()
{
	CWnStockDoc* pDoc = (CWnStockDoc*)GetActiveDocument();
	CWnStockView* pView = (CWnStockView*)GetActiveView();

	pView->bScroll = false;
	pView->OnInitialUpdate();
	pView->m_drawStatus = 1;
	//pDoc->stockDoc->KDay = 60;
	pDoc->stockDoc->GetMonthKStockData(pView->m_stockCode.GetBuffer(pView->m_stockCode.GetLength()));

	//初始化变量
	//获取周数
	int monthNum = pDoc->stockDoc->vKData.size();
	if (monthNum < 60)
	{
		pDoc->stockDoc->KDay = monthNum/2;
	}
	else
	{
		pDoc->stockDoc->KDay = 60;
	}
	pDoc->stockDoc->KBegin = 0;
	pDoc->stockDoc->KPos = 0;
	Invalidate();
}
