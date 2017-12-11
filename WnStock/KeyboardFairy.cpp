// KeyboardFairy.cpp : 实现文件
//

#include "stdafx.h"
#include "WnStock.h"
#include "KeyboardFairy.h"
#include "afxdialogex.h"
#include "WnStockDoc.h"
#include "WnStockView.h"
#include "MainFrm.h"

// CKeyboardFairy 对话框

IMPLEMENT_DYNAMIC(CKeyboardFairy, CDialogEx)

CKeyboardFairy::CKeyboardFairy(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKeyboardFairy::IDD, pParent)
{

}

CKeyboardFairy::~CKeyboardFairy()
{
}

void CKeyboardFairy::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_stockInfo);
	DDX_Control(pDX, IDC_LIST1, m_stockList);
}


BEGIN_MESSAGE_MAP(CKeyboardFairy, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CKeyboardFairy::OnChangeInputStockInfo)
	ON_WM_CTLCOLOR()
	ON_LBN_DBLCLK(IDC_LIST1, &CKeyboardFairy::OnDblclkListStock)
END_MESSAGE_MAP()


// CKeyboardFairy 消息处理程序

BOOL CKeyboardFairy::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CDialogEx::OnInitDialog();
	//设置字体
	font.CreatePointFont(120, "仿宋");
	m_stockList.SetFont(&font);
	m_stockInfo.SetFont(&font);


	CMainFrame * pFrame =(CMainFrame *)AfxGetMainWnd();
	//CTestView * pView =(CTestView *)pFrame->GetActiveView(); 获得视图类
	//CTestDoc * pDoc = (CTestDoc *)pFrame->GetActiveDocument(); 获得文档类


	CRect cRect;
	GetWindowRect(&cRect);
	int width = cRect.Width();
	int heigh = cRect.Height();

	CWnStockView * pView =(CWnStockView *)pFrame->GetActiveView();

	CRect rect;
	pView->GetWindowRect(rect);

	CPoint bottomRight = rect.BottomRight();
	int x = bottomRight.x;
	int y = bottomRight.y;

	int x1 = x - width;
	int y1 = y - heigh;

	MoveWindow(x1, y1, width, heigh);


	//设置焦点到编辑框
	GetDlgItem(IDC_EDIT1)->SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



void CKeyboardFairy::OnChangeInputStockInfo()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	UpdateData(TRUE);
	int len = m_stockInfo.GetWindowTextLength();
	char stockInfo[1024] = {0};
	
	m_stockInfo.GetWindowTextA(stockInfo, len+1);

	if (len == 0)
	{
		m_stockList.ResetContent();
		return;
	}
	m_stockList.ResetContent();


	CMainFrame * pFrame =(CMainFrame *)AfxGetMainWnd();
	CWnStockDoc * pDoc = (CWnStockDoc *)pFrame->GetActiveDocument();


	for (int i=0; i<pDoc->vAllStock.size(); i++)
	{
		char tempbuf[32] = {0};
		if (len <= strlen(pDoc->vAllStock[i]->stockCode))
		{
			memcpy(tempbuf, pDoc->vAllStock[i]->stockCode, len);
			if (strcmp(tempbuf, stockInfo) == 0)
			{
				CString str;
				str.Format("%s    %s",pDoc->vAllStock[i]->stockCode,pDoc->vAllStock[i]->stockName);
				m_stockList.AddString(str);
			}
		}
		if (len <= strlen(pDoc->vAllStock[i]->stockName))
		{
			memcpy(tempbuf, pDoc->vAllStock[i]->stockName, len);
			if (strcmp(tempbuf, stockInfo) == 0)
			{
				CString str;
				str.Format("%s    %s",pDoc->vAllStock[i]->stockCode,pDoc->vAllStock[i]->stockName);
				m_stockList.AddString(str);
			}
		}
	}
}


HBRUSH CKeyboardFairy::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor==CTLCOLOR_LISTBOX && pWnd->GetDlgCtrlID()==IDC_LIST1)//注意此处的（pWnd->），否则没效果
	{
		pDC->SetTextColor(RGB(255,0,0));
	}
	if (nCtlColor==CTLCOLOR_EDIT && pWnd->GetDlgCtrlID()==IDC_EDIT1)
	{
		pDC->SetTextColor(RGB(255,0,0));
	}
	return hbr;
}




void CKeyboardFairy::OnDblclkListStock()
{
	CMainFrame * pFrame =(CMainFrame *)AfxGetMainWnd();
	CWnStockView * pView =(CWnStockView *)pFrame->GetActiveView(); //获得视图类
	CWnStockDoc * pDoc = (CWnStockDoc *)pFrame->GetActiveDocument();

	CString stockName;
	CString stockCode;
	m_stockList.GetText(m_stockList.GetCurSel(), stockName);
	stockCode = stockName.Left(6);
	//获取该名字对应的股票代码
	for (int i=0;i<pDoc->vAllStock.size(); i++)
	{
		if(strcmp(stockCode, pDoc->vAllStock[i]->stockCode)==0)
		{
			stockName = pDoc->vAllStock[i]->stockName;
			break;
		}
	}

	pView->m_drawStatus = 2;
	pView->m_stockCode = stockCode;
	pView->m_stockName = stockName;
	pDoc->stockDoc->GetDayMinData(stockCode.GetBuffer(stockCode.GetLength()));
	Invalidate();
	pView->Invalidate();
	//关闭当前Dialg
	OnOK();
}
