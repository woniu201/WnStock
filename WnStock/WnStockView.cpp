
// WnStockView.cpp : CWnStockView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "WnStock.h"
#endif

#include <stdio.h>
#include "WnStockDoc.h"
#include "WnStockView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWnStockView

IMPLEMENT_DYNCREATE(CWnStockView, CScrollView)

BEGIN_MESSAGE_MAP(CWnStockView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CWnStockView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_COMMAND(ID_ADD_MYSTOCK, &CWnStockView::OnAddMystock)
	ON_COMMAND(ID_DEL_MYSTOCK, &CWnStockView::OnDelMystock)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CWnStockView 构造/析构

CWnStockView::CWnStockView()
{
	bScroll = TRUE;
	m_drawStatus = 0;
	m_selectLine = 1;
	m_mouseLine = 1;
	m_posY = 0;

	bFirstCrossLine = TRUE; //用于分时和K线区画十字线
	bCrossLine = FALSE; //用于分时和K线区画十字线

}

CWnStockView::~CWnStockView()
{

}

BOOL CWnStockView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CWnStockView 绘制

void CWnStockView::OnDraw(CDC* pDC)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (m_drawStatus == 0)
	{
		DrowMyStockUI(pDC);
	}
	else if (m_drawStatus == 1)
	{
		DrowKLineUI(pDC);
	}
	else if (m_drawStatus == 2)
	{
		DrowMinLineUI(pDC);
	}
}


// CWnStockView 打印


void CWnStockView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CWnStockView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CWnStockView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CWnStockView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CWnStockView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWnStockView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CWnStockView 诊断

#ifdef _DEBUG
void CWnStockView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CWnStockView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CWnStockDoc* CWnStockView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWnStockDoc)));
	return (CWnStockDoc*)m_pDocument;
}
#endif //_DEBUG

/******************************** My Stock 界面部分 ********************************************/
// CWnStockView 消息处理程序
void CWnStockView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	if (bScroll)
	{
		//计算CY值,标题（代码）高度占30，以后每个Stock占20,大盘显示区高度占30
		CWnStockDoc* pDoc = GetDocument();
		int n =  pDoc->vMystockRealTimeData.size();

		sizeTotal.cx = 10;
		sizeTotal.cy = 30 + n*20 + 30;
	}
	else
	{
		sizeTotal.cx = 10;
		sizeTotal.cy = 10;
	}
	SetScrollSizes(MM_TEXT, sizeTotal);
}


void CWnStockView::DrowMyStockUI(CDC* pDC)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//设置背景颜色
	CRect rect;	
	GetClientRect (&rect);
	CSize totalSize= GetTotalSize();
	if (rect.bottom <= totalSize.cy)
	{
		rect.bottom = totalSize.cy;
	}

	pDC->SetBkMode(TRANSPARENT);   //透明
	//pDC->SetBkMode(OPAQUE);
	pDC->SetBkColor(RGB(0,0,0));
	pDC->SelectStockObject(BLACK_BRUSH);
	pDC->Rectangle(&rect);

	//添加修改选中股的底色
	CRect rect1;
	rect1.left = rect.left;
	rect1.right = rect.right;
	rect1.top = rect.top + 30 + 20*(m_selectLine-1);
	rect1.bottom =  rect.top + 30 +20*m_selectLine ;
	pDC->SetBkColor(RGB(255,255,0));
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->Rectangle(&rect1);

	GetClientRect (&rect);

	//获得数据
	m_totalNum = pDoc->vMystock.size();
	m_width = 90;
	m_height = 20;

	CString strLine;

	pDC->SetTextColor(RGB(200, 200, 200));

	strLine.Format("编号");
	pDC->TextOut(m_width/8, 0 +  m_posY, strLine);
	strLine.Format("代码");
	pDC->TextOut(m_width/8+m_width, 0 + m_posY, strLine);
	strLine.Format("名称");
	pDC->TextOut(m_width/8+2*m_width, 0 + m_posY, strLine);
	strLine.Format("涨幅");
// 	if (sortStatus == 0)
// 	{
// 		strLine.Format("涨幅 ↓");
// 	}
// 	else if (sortStatus == 1)
// 	{
// 		strLine.Format("涨幅 ↑");
// 	}else
// 	{
// 		strLine.Format("涨幅");
// 	}

	pDC->TextOut(m_width/8+3*m_width, 0 + m_posY, strLine);
	strLine.Format("现价");
	pDC->TextOut(m_width/8+4*m_width, 0 + m_posY, strLine);
	strLine.Format("涨跌");
	pDC->TextOut(m_width/8+5*m_width, 0 + m_posY, strLine);
	strLine.Format("开盘价");
	pDC->TextOut(m_width/8+6*m_width, 0 + m_posY, strLine);
	strLine.Format("最高价");
	pDC->TextOut(m_width/8+7*m_width, 0 + m_posY, strLine);
	strLine.Format("最低价");
	pDC->TextOut(m_width/8+8*m_width, 0 + m_posY, strLine);
	strLine.Format("成交量");
	pDC->TextOut(m_width/8+9*m_width, 0 + m_posY, strLine);
	strLine.Format("成交额");
	pDC->TextOut(m_width/8+10*m_width, 0 + m_posY, strLine);





	CPen pen(PS_DASH, 1, RGB(255, 255, 0)); //PS_SOLID：实线，PS_DASH：虚线
	pDC->SelectObject(&pen);


	CClientDC dcRed(this);
	dcRed.SetBkMode(OPAQUE);
	dcRed.SetBkColor(RGB(0,0,0));
	dcRed.SetTextAlign (TA_LEFT| TA_TOP);
	dcRed.SetTextColor(RGB(255,0,0));

	CClientDC dcGreen(this);
	dcGreen.SetBkMode(OPAQUE);
	dcGreen.SetBkColor(RGB(0,0,0));
	dcGreen.SetTextAlign (TA_LEFT| TA_TOP);
	dcGreen.SetTextColor(RGB(0,255,255));

	CClientDC dcYellow(this);
	dcYellow.SetBkMode(OPAQUE);
	dcYellow.SetBkColor(RGB(0,0,0));
	dcYellow.SetTextAlign (TA_LEFT| TA_TOP);
	dcYellow.SetTextColor(RGB(255, 255, 0));
	
	for (int i=0; i<m_totalNum; i++)
	{
		if (pDoc->vMystockRealTimeData[i]->fNow >= pDoc->vMystockRealTimeData[i]->fPassClose)
		{
			pDC->SetTextColor(RGB(200, 200, 200));//灰色
			CString strLine = "";
			strLine.Format("%d",i+1);
			pDC->TextOut(m_width/8, rect.top +30 + m_height*i, strLine);

			pDC->SetTextColor(RGB(255, 255, 0));//黄色
			pDC->TextOut(m_width/8+m_width, rect.top +30 + m_height*i, pDoc->vMystockRealTimeData[i]->strStockNum);
			pDC->TextOut(m_width/8+2*m_width, rect.top +30 + m_height*i, pDoc->vMystockRealTimeData[i]->strStockName);

			pDC->SetTextColor(RGB(255, 0, 0));//红色
			CString strRatio;
			double ratio = pDoc->vMystockRealTimeData[i]->fRatio;
			strRatio.Format("+%.2f%%", ratio);
			pDC->TextOut(m_width/8+3*m_width, rect.top +30 + m_height*i, strRatio);

			CString strNow;
			strNow.Format("%.2f", pDoc->vMystockRealTimeData[i]->fNow);
			pDC->TextOut(m_width/8+4*m_width, rect.top +30 + m_height*i, strNow);

			CString strChance;
			strChance.Format("+%.2f", ( pDoc->vMystockRealTimeData[i]->fNow -  pDoc->vMystockRealTimeData[i]->fPassClose));
			pDC->TextOut(m_width/8+5*m_width, rect.top +30 + m_height*i, strChance);

			CString strOpen;
			strOpen.Format("%.2f", pDoc->vMystockRealTimeData[i]->fOpen);
			pDC->TextOut(m_width/8+6*m_width, rect.top +30 + m_height*i, strOpen);

			CString strHigh;
			strHigh.Format("%.2f", pDoc->vMystockRealTimeData[i]->fHigh);
			pDC->TextOut(m_width/8+7*m_width, rect.top +30 + m_height*i, strHigh);

			CString strLow;
			strLow.Format("%.2f", pDoc->vMystockRealTimeData[i]->fLow);
			pDC->TextOut(m_width/8+8*m_width, rect.top +30 + m_height*i, strLow);

			pDC->SetTextColor(RGB(255, 255, 0));//黄色
			CString strVolume;
			strVolume.Format("%llu", pDoc->vMystockRealTimeData[i]->Volume);
			pDC->TextOut(m_width/8+9*m_width, rect.top +30 + m_height*i, strVolume);

			pDC->SetTextColor(RGB(0, 255, 255));//绿色
			CString strAmount;
			strAmount.Format("%llu", pDoc->vMystockRealTimeData[i]->Amount);
			pDC->TextOut(m_width/8+10*m_width, rect.top +30 + m_height*i, strAmount);

			//显示大盘指数
			// 			int width = (rect.right - rect.left)/6;
			// 			CString strSH="上证指数：3500";
			// 			pDC->TextOut(10,rect.bottom-30, strSH);
			// 			pDC->TextOut(10+width,rect.bottom-30, strSH);
			// 			pDC->TextOut(10+2*width,rect.bottom-30, strSH);
		}
		else
		{
			pDC->SetTextColor(RGB(200, 200, 200));//灰色
			CString strLine;
			strLine.Format("%d",i+1);
			pDC->TextOut(m_width/8, rect.top +30 + m_height*i, strLine);

			pDC->SetTextColor(RGB(255, 255, 0));//黄色
			pDC->TextOut(m_width/8+m_width, rect.top +30 + m_height*i, pDoc->vMystockRealTimeData[i]->strStockNum);
			pDC->TextOut(m_width/8+2*m_width, rect.top +30 + m_height*i, pDoc->vMystockRealTimeData[i]->strStockName);

			pDC->SetTextColor(RGB(0, 255, 255));//绿色
			CString strRatio;
			double ratio = pDoc->vMystockRealTimeData[i]->fRatio;
			strRatio.Format("-%.2f%%", ratio);
			pDC->TextOut(m_width/8+3*m_width, rect.top +30 + m_height*i, strRatio);

			CString strNow;
			strNow.Format("%.2f", pDoc->vMystockRealTimeData[i]->fNow);
			pDC->TextOut(m_width/8+4*m_width, rect.top +30 + m_height*i, strNow);

			CString strChance;
			strChance.Format("-%.2f", ( pDoc->vMystockRealTimeData[i]->fNow -  pDoc->vMystockRealTimeData[i]->fPassClose));
			pDC->TextOut(m_width/8+5*m_width, rect.top +30 + m_height*i, strChance);

			CString strOpen;
			strOpen.Format("%.2f", pDoc->vMystockRealTimeData[i]->fOpen);
			pDC->TextOut(m_width/8+6*m_width, rect.top +30 + m_height*i, strOpen);

			CString strHigh;
			strHigh.Format("%.2f", pDoc->vMystockRealTimeData[i]->fHigh);
			pDC->TextOut(m_width/8+7*m_width, rect.top +30 + m_height*i, strHigh);

			CString strLow;
			strLow.Format("%.2f", pDoc->vMystockRealTimeData[i]->fLow);
			pDC->TextOut(m_width/8+8*m_width, rect.top +30 + m_height*i, strLow);

			pDC->SetTextColor(RGB(255, 255, 0));//黄色
			CString strVolume;
			strVolume.Format("%llu", pDoc->vMystockRealTimeData[i]->Volume);
			pDC->TextOut(m_width/8+9*m_width, rect.top +30 + m_height*i, strVolume);

			pDC->SetTextColor(RGB(0, 255, 255));//绿色
			CString strAmount;
			strAmount.Format("%llu", pDoc->vMystockRealTimeData[i]->Amount);
			pDC->TextOut(m_width/8+10*m_width, rect.top +30 + m_height*i, strAmount);
		}
	}
	return ;
}

void CWnStockView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL)
	{
	}
	else
	{
		m_posY = nPos;
	}
	Invalidate();
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CWnStockView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return FALSE;
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

/*
*	@ 右键弹出框添加自选股
*/
void CWnStockView::OnAddMystock()
{

}

/*
*	@ 右键弹出框删除自选股
*/
void CWnStockView::OnDelMystock()
{
	CWnStockDoc* pDoc = GetDocument();
	pDoc->DelMystock(pDoc->vMystockRealTimeData[m_mouseLine-1]->strStockNum);
	pDoc->GetMytockData();
	Invalidate();
}


UINT CWnStockView::ThreadGetMytockData(LPVOID lParam)
{
	CWnStockView* pThis = (CWnStockView*)lParam;
	CWnStockDoc* pDoc = (CWnStockDoc*)pThis->GetDocument();
	StockTime	stockTime;	
	while(1)
	{
		if (stockTime.bExchange())
		{
			pDoc->GetMytockData();
			OutputDebugString("获得数据成功\n");
		}
		Sleep(2000);
	}
	return 1;
}
/******************************** K线部分 界面部分 ********************************************/
void CWnStockView::DrowKLineUI(CDC* pDC)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//设置背景颜色
	CRect rect;	
	GetClientRect (&rect);
	//pDC->SetBkMode(OPAQUE);
	pDC->SetBkMode(TRANSPARENT);   //透明
	pDC->SetBkColor(RGB(0,0,0));//背景颜色为黑色
	pDC->SelectStockObject(BLACK_BRUSH);
	pDC->Rectangle(&rect);

	CPen      redpen(PS_SOLID,2,RGB(153,0,0));
	CPen      whitepen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen      redDotpen(PS_DOT, 1, RGB(153,0,0));
	CPen      redThinPen(PS_SOLID, 1, RGB(153, 0, 0));
	char      chName[300];

	pDC->SelectObject(&redpen);
	//画左上角公共的边框
	pDC->MoveTo(2, 2);
	pDC->LineTo(rect.right - 200, 2);
	pDC->LineTo(rect.right - 200, rect.bottom - 50);
	pDC->LineTo(2, rect.bottom - 50);
	pDC->LineTo(2, 2);
	//画最右面的边框
	pDC->MoveTo(rect.right - 192, 35);
	pDC->LineTo(rect.right - 192, rect.bottom - 50);
	pDC->LineTo(rect.right - 2, rect.bottom - 50);
	pDC->LineTo(rect.right -2, 35);
	pDC->LineTo(rect.right - 192, 35);
	// 	pDC->MoveTo(rect.right - 192, 125);
	// 	pDC->LineTo(rect.right - 2, 125);
	pDC->MoveTo(rect.right - 192, 345);
	pDC->LineTo(rect.right - 2, 345);

	//画最下面的边框
	pDC->SelectObject(&whitepen);
	pDC->MoveTo(2, rect.bottom - 42);
	pDC->LineTo(rect.right - 2, rect.bottom - 42);
	pDC->LineTo(rect.right - 2, rect.bottom - 2);
	pDC->LineTo(2, rect.bottom - 2);
	pDC->LineTo(2, rect.bottom - 42);

	//写最右面方框的文字
	pDC->SetTextColor(RGB(255,255,255));
	pDC->TextOut(rect.right - 188, 44, "时    间：", lstrlen("时    间："));
	pDC->TextOut(rect.right - 188, 74, "开盘价：", lstrlen("开盘价："));
	pDC->TextOut(rect.right - 188, 104, "收盘价：", lstrlen("收盘价："));
	pDC->TextOut(rect.right - 188, 132, "最高价：", lstrlen("最高价："));

	pDC->TextOut(rect.right - 188, 162, "最低价：", lstrlen("最低价："));
	pDC->TextOut(rect.right - 188, 192, "涨    跌：", lstrlen("涨    跌："));
	pDC->TextOut(rect.right - 188, 222, "涨    幅：", lstrlen("涨    幅："));
	pDC->TextOut(rect.right - 188, 252, "成交量：", lstrlen("成交量："));
	pDC->TextOut(rect.right - 188, 282, "量    比：", lstrlen("量    比："));
	pDC->TextOut(rect.right - 188, 312, "成交额：", lstrlen("成交额："));


	//写最下面边框的文字
	pDC->SetTextColor(RGB(255,255,255));
	sprintf(chName, "%8s", "◆蜗牛201作品： QQ:731342182            email:wangpengfei.201@163.com");
	pDC->TextOut(15, rect.bottom - 28, chName, lstrlen(chName));
	//写右上角的文字	               
	pDC->SetTextColor(RGB(255,255,0));
	//pDC->TextOut(rect.right-192, 7, "股票代码",lstrlen("股票代码"));
	pDC->TextOut(rect.right-192, 7, m_stockName,lstrlen(m_stockName));
	pDC->SetTextColor(RGB(116,245,240));
	//pDC->TextOut(rect.right-100, 7, "600000",lstrlen("600000"));
	pDC->TextOut(rect.right-100, 7, m_stockCode, lstrlen(m_stockCode));  

	//画边框
	pDC->SelectObject(&redThinPen);
	pDC->MoveTo(62, 17);
	pDC->LineTo(rect.right-200, 17);
	pDC->MoveTo(62, 17);
	pDC->LineTo(62, rect.bottom-50);

	m_interKH = (rect.bottom - 67)/16;
	m_interKW = rect.right - 200 -62;
	//画中间的那条红的细的实线
	pDC->MoveTo(2, 9.5*m_interKH+17);
	pDC->LineTo(rect.right-200, 9.5*m_interKH+17);

	//画中间下方第二条红的细的实线
	pDC->MoveTo(2, 12*m_interKH+17);
	pDC->LineTo(rect.right-200, 12*m_interKH+17);

	//画上面那九条虚的线
	pDC->SelectObject(&redDotpen);
	int y;
	for(/*int*/ y = 9.5*m_interKH+17-m_interKH; y > 17; y -= m_interKH)
	{
		pDC->MoveTo(62, y);
		pDC->LineTo(rect.right-200, y);
	}

	//画下面那四条条虚的线
	pDC->MoveTo(62, rect.bottom-50-m_interKH);
	pDC->LineTo(rect.right-200, rect.bottom-50-m_interKH);
	pDC->MoveTo(62, rect.bottom-50-m_interKH*2);
	pDC->LineTo(rect.right-200, rect.bottom-50-m_interKH*2);
	pDC->MoveTo(62, rect.bottom-50-m_interKH*3);
	pDC->LineTo(rect.right-200, rect.bottom-50-m_interKH*3);
	pDC->MoveTo(62, rect.bottom-50-m_interKH*5);
	pDC->LineTo(rect.right-200, rect.bottom-50-m_interKH*5);
	pDC->MoveTo(62, rect.bottom-50-m_interKH*6);
	pDC->LineTo(rect.right-200, rect.bottom-50-m_interKH*6);

	//K线和成交量
	pDoc->stockDoc->GetKPoint(m_interKH, m_interKW);
	pDoc->stockDoc->GetKVolumePoint(m_interKH, m_interKW);
	DrawKLine();
	
	//MA线
	pDoc->stockDoc->GetMaPrice();
	pDoc->stockDoc->GetMaPoint(m_interKH, m_interKW);
	DrawMaLine();

	//K线展示数据
	ShowKData(pDoc->stockDoc->KDay-1);

	//MACD线
	pDoc->stockDoc->GetMacd();
	pDoc->stockDoc->GetMacdPoint(m_interKH, m_interKW);
	DrawMacd(pDoc->stockDoc->KDay-1);
}

void CWnStockView::DrawKLine()
{
	int j = 0;
	CClientDC dc(this);
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CPen		cyanPen(PS_SOLID,1,RGB(116, 245, 240));//创建一个宽度为1的实线画笔
	CBrush		cyanBrush(RGB(116, 245, 240));	
	CPen		redPen(PS_SOLID,1,RGB(240, 0, 0));
	CBrush		blackBrush(RGB(0, 0, 0));

	char chName[200] = {0};
	//pDoc->stockDoc->ExchangeK();

	CRect rect, kRect, kaRect;
	GetClientRect(&rect);
	float lw = (rect.right - 200 -62)/pDoc->stockDoc->KDay;//每个K线所占的宽度
	int n = pDoc->stockDoc->vKPoint.size();  //获得保存坐标个数
	if (n == 0)
	{
		return;
	}
	//显示最新的数据
	if(n < pDoc->stockDoc->KDay - pDoc->stockDoc->KBegin)
		j = 0+ pDoc->stockDoc->KPos;
	else
		j = n - (pDoc->stockDoc->KDay - pDoc->stockDoc->KBegin) + pDoc->stockDoc->KPos;

	int i;
	int pos = pDoc->stockDoc->KPos;
	for(/*int*/ i = j ; i < n; i++)
	{
		if((pDoc->stockDoc->vKPoint[i]->close.y) <= (pDoc->stockDoc->vKPoint[i]->open.y)) //收盘价大于开盘价为阳线
		{
			dc.SelectObject(&redPen);
			dc.SelectObject(&blackBrush);
		}
		else
		{
			dc.SelectObject(&cyanPen);
			dc.SelectObject(&cyanBrush);
		}
		/*		if(62 + (i-j)*lw + pDoc->stockDoc->Get_kPointArray(i).high.x + pDoc->stockDoc->Get_kBegine()*(rect.right - 200 - 62)/pDoc->stockDoc->Get_kDay() > 62 && 62 + (i-j)*lw + pDoc->stockDoc->Get_kPointArray(i).high.x + pDoc->stockDoc->Get_kBegine()*(rect.right - 200 - 62)/pDoc->stockDoc->Get_kDay() < rect.right - 200)*/
		//判断坐标是否在绘制K线的区域
		if (62 + (i-j)*lw + pDoc->stockDoc->vKPoint[i]->high.x + pDoc->stockDoc->KBegin*lw > 62 && 62 + (i-j)*lw + pDoc->stockDoc->vKPoint[i]->high.x + pDoc->stockDoc->KBegin*lw < rect.right - 200)
		{
			dc.MoveTo(62 + (i-j)*lw + pDoc->stockDoc->vKPoint[i]->high.x + pDoc->stockDoc->KBegin*lw, pDoc->stockDoc->vKPoint[i]->high.y);
			dc.LineTo(62 + (i-j)*lw + pDoc->stockDoc->vKPoint[i]->low.x + pDoc->stockDoc->KBegin*lw, pDoc->stockDoc->vKPoint[i]->low.y);
			kRect.left =62 + (i-j)*lw + pDoc->stockDoc->vKPoint[i]->open.x + pDoc->stockDoc->KBegin*lw;
			kRect.top = pDoc->stockDoc->vKPoint[i]->open.y;
			kRect.right = 62 + (i-j)*lw +pDoc->stockDoc->vKPoint[i]->close.x + pDoc->stockDoc->KBegin*lw;
			kRect.bottom = pDoc->stockDoc->vKPoint[i]->close.y;
			dc.Rectangle(&kRect);     //画矩形

			//@20150506解决开盘板没有K线问题,和开盘价等于收盘价没有横线只有竖线问题
			if (pDoc->stockDoc->vKPoint[i]->open.y == pDoc->stockDoc->vKPoint[i]->close.y)

			{
				dc.MoveTo(62 + (i-j)*lw + pDoc->stockDoc->vKPoint[i]->open.x + pDoc->stockDoc->KBegin*lw,pDoc->stockDoc->vKPoint[i]->open.y);
				dc.LineTo(62 + (i-j)*lw +pDoc->stockDoc->vKPoint[i]->close.x + pDoc->stockDoc->KBegin*lw,pDoc->stockDoc->vKPoint[i]->close.y);
			}

			//画K线总量坐标
			kaRect.left = 62 + (i-j)*lw + pDoc->stockDoc->vVolumePoint[i]->top.x + pDoc->stockDoc->KBegin*lw;
			kaRect.top = pDoc->stockDoc->vVolumePoint[i]->top.y;
			kaRect.right = 62 + (i-j)*lw +pDoc->stockDoc->vVolumePoint[i]->bottom.x + pDoc->stockDoc->KBegin*lw;
			kaRect.bottom = pDoc->stockDoc->vVolumePoint[i]->bottom.y;
			dc.Rectangle(&kaRect);
		}
	}

	//画K线价格坐标值
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(240, 0, 0));
	dc.SetTextAlign(TA_RIGHT);
	float KM = pDoc->stockDoc->KHighMax - pDoc->stockDoc->KLowMin;
	for(i = 0; i < 9; i++)
	{
		sprintf(chName, "%10.2f", pDoc->stockDoc->KHighMax - (KM/9.0)*i);
		dc.TextOut(55, 17 + 0.5 * m_interKH + i * m_interKH - 10, chName, lstrlen(chName));
	}
	sprintf(chName, "%10.2f", pDoc->stockDoc->KHighMax - (KM/9.0)*9);
	dc.TextOut(55, 17 + 9.5 * m_interKH - 20, chName, lstrlen(chName));


	//画K线成交金量坐标值
	dc.SetTextColor(RGB(116, 245, 240));
	sprintf(chName, "%8.0d万", pDoc->stockDoc->KMaxVolume/10000);
	dc.TextOut(55, rect.bottom - 50 - 6*m_interKH - 8, chName, lstrlen(chName));

	sprintf(chName, "%8.0d万", pDoc->stockDoc->KMaxVolume/10000/2);
	dc.TextOut(55, rect.bottom - 50 - 5*m_interKH -8, chName, lstrlen(chName));

	sprintf(chName, "%d", 0);
	dc.TextOut(55, rect.bottom - 50 - 4*m_interKH -8, chName, lstrlen(chName));
	return;
}

void CWnStockView::DrawMaLine()
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CClientDC ma5Pen(this);
	CClientDC ma10Pen(this);
	CClientDC ma20Pen(this);

	CPen  dcWhite(PS_SOLID,1,RGB(255, 255, 255));
	CPen  dcYellow(PS_SOLID,1,RGB(255, 255, 0));
	CPen  dcBlue(PS_SOLID,1,RGB(139, 0, 255));

	ma5Pen.SelectObject(&dcWhite);
	ma10Pen.SelectObject(&dcYellow);
	ma20Pen.SelectObject(&dcBlue);

	float b =  m_interKW;       //水平宽度
	float lw = b/pDoc->stockDoc->KDay; 


	//画MA5
	int n = pDoc->stockDoc->vMa5Price.size();
	if (n == 0)
	{
		return;
	}
	for (int i=1;i<pDoc->stockDoc->KDay;i=i+1) //没三天画一直线
	{
		if((n - pDoc->stockDoc->KDay +i + pDoc->stockDoc->KPos) >=5) //前4天不画MA5
		{
			ma5Pen.MoveTo(pDoc->stockDoc->vMa5Point[i].x + pDoc->stockDoc->KBegin*lw, pDoc->stockDoc->vMa5Point[i].y);
			ma5Pen.LineTo(pDoc->stockDoc->vMa5Point[i-1].x + pDoc->stockDoc->KBegin*lw,pDoc->stockDoc->vMa5Point[i-1].y);
		}
	}

	//画MA10
	n = pDoc->stockDoc->vMa10Price.size();
	for (int i=1;i<pDoc->stockDoc->KDay;i=i+1) //没三天画一直线
	{
		if((n - pDoc->stockDoc->KDay +i + pDoc->stockDoc->KPos) >=10) //前9天不画MA10
		{
			ma10Pen.MoveTo(pDoc->stockDoc->vMa10Point[i].x + pDoc->stockDoc->KBegin*lw, pDoc->stockDoc->vMa10Point[i].y);
			ma10Pen.LineTo(pDoc->stockDoc->vMa10Point[i-1].x + pDoc->stockDoc->KBegin*lw,pDoc->stockDoc->vMa10Point[i-1].y);
		}
	}

	//画MA5
	n = pDoc->stockDoc->vMa20Price.size();
	for (int i=1;i<pDoc->stockDoc->KDay;i=i+1) //没三天画一直线
	{
		if((n - pDoc->stockDoc->KDay +i + pDoc->stockDoc->KPos) >=20) //前19天不画MA20
		{
			ma20Pen.MoveTo(pDoc->stockDoc->vMa20Point[i].x + pDoc->stockDoc->KBegin*lw, pDoc->stockDoc->vMa20Point[i].y);
			ma20Pen.LineTo(pDoc->stockDoc->vMa20Point[i-1].x + pDoc->stockDoc->KBegin*lw,pDoc->stockDoc->vMa20Point[i-1].y);
		}
	}

	return;
}

void CWnStockView::ShowKData(int i)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CClientDC dc(this);
	CClientDC dc1(this);
	CClientDC dcWhite(this);
	CClientDC dcYellow(this);
	CClientDC dcBlue(this);
	CRect  rect;
	GetClientRect(&rect);

	char chName[300] = {0};

	dc.SetBkMode(OPAQUE);
	dc.SetBkColor(RGB(0,0,0));
	dc.SetTextAlign (TA_LEFT| TA_TOP);
	dc.SetTextColor(RGB(240,0,0));

	dc1.SetBkMode(OPAQUE);
	dc1.SetBkColor(RGB(0,0,0));
	dc1.SetTextAlign (TA_LEFT| TA_TOP);
	dc1.SetTextColor(RGB(116, 245, 240));

	dcWhite.SetBkMode(OPAQUE);
	dcWhite.SetBkColor(RGB(0,0,0));
	dcWhite.SetTextAlign (TA_LEFT| TA_TOP);
	dcWhite.SetTextColor(RGB(255,255,255));

	dcYellow.SetBkMode(OPAQUE);
	dcYellow.SetBkColor(RGB(0,0,0));
	dcYellow.SetTextAlign (TA_LEFT| TA_TOP);
	dcYellow.SetTextColor(RGB(255,255,0));

	dcBlue.SetBkMode(OPAQUE);
	dcBlue.SetBkColor(RGB(0,0,0));
	dcBlue.SetTextAlign (TA_LEFT| TA_TOP);
	dcBlue.SetTextColor(RGB(139, 0, 255));

	//显示开盘价
	int n = pDoc->stockDoc->vKData.size();
	if (n == 0)
	{
		return;
	}

	dc.TextOut(rect.right-110, 44, "                        ", strlen("                        ")-1);
	CString str = (pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->strDate);
	dc.TextOut(rect.right-110, 44,  str,  strlen(str));

	dc.TextOut(rect.right-110, 74, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fOpen);
	dc.TextOut(rect.right-110, 74, chName, lstrlen(chName)-1);

	dc.TextOut(rect.right-110, 104, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fClose);
	dc.TextOut(rect.right-110, 104, chName, lstrlen(chName)-1);

	dc.TextOut(rect.right-110, 132, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fHigh);
	dc.TextOut(rect.right-110, 132, chName, lstrlen(chName)-1);

	dc.TextOut(rect.right-110, 162, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fLow);
	dc.TextOut(rect.right-110, 162, chName, lstrlen(chName)-1);

	//画涨幅数据
	if (pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fPassClose <= pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fClose)
	{
		dc.TextOut(rect.right-110, 192, "                        ", strlen("                        ")-1);//清楚上次显示留下的痕迹
		float grow = pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fClose - pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fPassClose;
		float ratio = grow /(pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fPassClose)*100;
		sprintf(chName, "%10.2f\n", grow);
		dc.TextOut(rect.right-110, 192, chName, lstrlen(chName)-1);
		dc.TextOut(rect.right-110, 222, "                        ", strlen("                        ")-1);
		sprintf(chName, "      %.2f %%\n", ratio);
		dc.TextOut(rect.right-110, 222, chName, lstrlen(chName)-1);
	}
	else
	{
		dc.TextOut(rect.right-110, 192, "                        ", strlen("                        ")-1);//清楚上次显示留下的痕迹
		float grow = ((pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fPassClose) - (pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fClose));
		float ratio = grow/(pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->fPassClose);
		sprintf(chName, "%10.2f\n", grow);
		dc1.TextOut(rect.right-110, 192, chName, lstrlen(chName)-1);
		dc.TextOut(rect.right-110, 222, "                        ", strlen("                        ")-1);
		sprintf(chName, "     - %.2f %%\n", ratio*100);
		dc1.TextOut(rect.right-110, 222, chName, lstrlen(chName)-1);
	}

	//画成交量
	dc.TextOut(rect.right-110, 252, "                        ", lstrlen("                        ")-1);
	sprintf(chName, "%llu\n", pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->Volume); //%llu 64位无符号整形
	dc.TextOut(rect.right-110, 252, chName, lstrlen(chName)-1);


	//计算量比并显示
	//计算日量比
	if (n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos > 6)
	{
		// 		CFunction function;
		// 		CString str;
		// 		function.GetThan(n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos,str);
		// 		AfxMessageBox(str);
		long long nowVolume = pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->Volume;
		long long passVolume = 0;
		float fThan = 0;
		for (int j = n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos-1; j>n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos-6; j--)
		{
			passVolume += pDoc->stockDoc->vKData[j]->Volume/5;
		}
		fThan = (float)nowVolume/passVolume;
		sprintf(chName, "%.2f", fThan);
	}
	else
	{
		sprintf(chName, " ------");
	}
	dc.TextOut(rect.right-110, 282, "                        ", lstrlen("                        ")-1);
	dc.TextOut(rect.right-110, 282, chName, lstrlen(chName));

	//画成交额数据
	//sprintf(chName, "%10.2f\n", (float)pDoc->stockDoc->m_Stock.Pbuy3);
	dc.TextOut(rect.right-110, 312, "                        ", lstrlen("                        ")-1);
	sprintf(chName, "%llu\n", pDoc->stockDoc->vKData[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]->Amount); //%llu 64位无符号整形
	dc.TextOut(rect.right-110, 312, chName, lstrlen(chName)-1);


	//左上角显示MA5,MA10,MA20的数据
	n = pDoc->stockDoc->vMa5Price.size();
	if (pDoc->stockDoc->vMa5Price.size() > 5)
	{
		sprintf(chName, "MA5=%.2f\n", pDoc->stockDoc->vMa5Price[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]);
		dcWhite.TextOut(64, 17+0.05*m_interKH, chName, lstrlen(chName) -1);
	}
	n = pDoc->stockDoc->vMa10Price.size();
	if (pDoc->stockDoc->vMa10Price.size() > 10)
	{
		sprintf(chName, "MA10=%.2f\n", pDoc->stockDoc->vMa10Price[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]);
		dcYellow.TextOut(130, 17+0.05*m_interKH, chName, lstrlen(chName) -1);
	}
	n = pDoc->stockDoc->vMa20Price.size();
	if (pDoc->stockDoc->vMa20Price.size() > 10)
	{
		sprintf(chName, "MA20=%.2f\n", pDoc->stockDoc->vMa20Price[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]);
		dcBlue.TextOut(210, 17+0.05*m_interKH, chName, lstrlen(chName) -1);
	}
	return;
}

void CWnStockView::DrawMacd(int i)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rect;
	GetClientRect(&rect);

	int n = pDoc->stockDoc->vMacd.size();
	if (n == 0)
	{
		return;
	}


	CClientDC dc1(this);
	CClientDC dc2(this);
	CPen macdPen1(PS_SOLID, 0.5, RGB(255, 0, 0));
	CPen macdPen2(PS_SOLID, 1, RGB(116, 245, 240));
	dc1.SelectObject(macdPen1);
	dc2.SelectObject(macdPen2);

	//开始画MACD
	for (int j=0; j<pDoc->stockDoc->KDay; j=j+1)
	{

		if (pDoc->stockDoc->vMacd[n-pDoc->stockDoc->KDay + j + pDoc->stockDoc->KPos] >= 0)
		{
			dc1.MoveTo(pDoc->stockDoc->vMacdPoint[j].x, 14*m_interKH+17);
			dc1.LineTo(pDoc->stockDoc->vMacdPoint[j].x, pDoc->stockDoc->vMacdPoint[j].y);
		}
		else
		{
			dc2.MoveTo(pDoc->stockDoc->vMacdPoint[j].x, 14*m_interKH+17);
			dc2.LineTo(pDoc->stockDoc->vMacdPoint[j].x, pDoc->stockDoc->vMacdPoint[j].y);
		}
	}

	//画MACD的坐标值
	dc1.SetBkMode(OPAQUE);
	dc1.SetBkColor(RGB(0,0,0));
	dc1.SetTextAlign (TA_LEFT| TA_TOP);
	dc1.SetTextColor(RGB(240,0,0));
	char chName[1024] = {0};

	sprintf(chName, "%.2f", pDoc->stockDoc->maxMACD);
	dc1.TextOut(30, rect.bottom - 50 - 3*m_interKH - 8, chName, lstrlen(chName));
	sprintf(chName, "%d", 0);
	dc1.TextOut(40, rect.bottom - 50 - 2*m_interKH - 8, chName, lstrlen(chName));
	sprintf(chName, "%.2f", pDoc->stockDoc->minMACD);
	dc1.TextOut(30, rect.bottom - 50 - m_interKH - 8, chName, lstrlen(chName));

	//显示MACD的DIF，DEA，MACD值
	CClientDC dc3(this);
	CClientDC dc4(this);
	CClientDC dc5(this);
	dc3.SetBkMode(OPAQUE);
	dc3.SetBkColor(RGB(0,0,0));
	dc3.SetTextAlign (TA_LEFT| TA_TOP);
	dc3.SetTextColor(RGB(255,255,255));
	dc4.SetBkMode(OPAQUE);
	dc4.SetBkColor(RGB(0,0,0));
	dc4.SetTextAlign (TA_LEFT| TA_TOP);
	dc4.SetTextColor(RGB(255,255,0));
	dc5.SetBkMode(OPAQUE);
	dc5.SetBkColor(RGB(0,0,0));
	dc5.SetTextAlign (TA_LEFT| TA_TOP);
	dc5.SetTextColor(RGB(128,0,128));

	dc1.TextOut(64, rect.bottom-(24/5*m_interKH)-50,"                                                                    ", lstrlen("                                                                    ")-1);
	sprintf(chName,"DIF:%4.2f", pDoc->stockDoc->vMacdDif[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]);
	dc3.TextOut(64, rect.bottom-(24/5*m_interKH)-50, chName, lstrlen(chName));
	sprintf(chName," DEA:%4.2f", pDoc->stockDoc->vMacdDea[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]);
	dc4.TextOut(124, rect.bottom-(24/5*m_interKH)-50, chName, lstrlen(chName));
	sprintf(chName," MACD:%4.2f", pDoc->stockDoc->vMacd[n-pDoc->stockDoc->KDay+i + pDoc->stockDoc->KPos]);
	dc5.TextOut(194, rect.bottom-(24/5*m_interKH)-50, chName, lstrlen(chName));

	//开始画DIF曲线
	CClientDC dc6(this);
	CPen difPen(PS_DOT, 1, RGB(255, 255, 255));
	dc6.SelectObject(difPen);
	for (int i=1; i<pDoc->stockDoc->KDay; i=i+1)
	{
		dc6.MoveTo(pDoc->stockDoc->vMacdDifPoint[i].x, pDoc->stockDoc->vMacdDifPoint[i].y);
		dc6.LineTo(pDoc->stockDoc->vMacdDifPoint[i-1].x, pDoc->stockDoc->vMacdDifPoint[i-1].y);
	}

	//开始画DIF曲线
	CClientDC dc7(this);
	CPen deaPen(PS_DOT, 1, RGB(255, 255, 0));
	dc7.SelectObject(deaPen);
	for (int i=1; i<pDoc->stockDoc->KDay; i=i+1)
	{
		dc7.MoveTo(pDoc->stockDoc->vMacdDeaPoint[i].x, pDoc->stockDoc->vMacdDeaPoint[i].y);
		dc7.LineTo(pDoc->stockDoc->vMacdDeaPoint[i-1].x, pDoc->stockDoc->vMacdDeaPoint[i-1].y);
	}
	return;
}

/******************************** 分时线部分 **************************************************/

void CWnStockView::DrowMinLineUI(CDC* pDC)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//设置背景颜色
	CRect rect;	
	GetClientRect (&rect);
	pDC->SetBkMode(OPAQUE);
	pDC->SetBkColor(RGB(0,0,0));
	pDC->SelectStockObject(BLACK_BRUSH);
	pDC->Rectangle(&rect);

	CPen      redpen(PS_SOLID,2,RGB(153,0,0));
	CPen      whitepen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen      redDotpen(PS_DOT, 1, RGB(153,0,0));
	CPen      redThinPen(PS_SOLID, 1, RGB(153, 0, 0));
	char      chName[300];	


	pDC->SelectObject(&redpen);

	//画左上角公共的边框
	pDC->MoveTo(2, 2);
	pDC->LineTo(rect.right - 200, 2);
	pDC->LineTo(rect.right - 200, rect.bottom - 50);
	pDC->LineTo(2, rect.bottom - 50);
	pDC->LineTo(2, 2);

	//画最右面的边框
	pDC->MoveTo(rect.right - 192, 35);
	pDC->LineTo(rect.right - 192, rect.bottom - 50);
	pDC->LineTo(rect.right - 2, rect.bottom - 50);
	pDC->LineTo(rect.right -2, 35);
	pDC->LineTo(rect.right - 192, 35);

	//画右边框中的分割行
	pDC->MoveTo(rect.right - 192, 60);
	pDC->LineTo(rect.right - 2, 60);
	pDC->MoveTo(rect.right - 192, 170);
	pDC->LineTo(rect.right - 2, 170);
	pDC->MoveTo(rect.right - 192, 280);
	pDC->LineTo(rect.right - 2, 280);

	//画最下面的边框
	pDC->SelectObject(&whitepen);
	pDC->MoveTo(2, rect.bottom - 42);
	pDC->LineTo(rect.right - 2, rect.bottom - 42);
	pDC->LineTo(rect.right - 2, rect.bottom - 2);
	pDC->LineTo(2, rect.bottom - 2);
	pDC->LineTo(2, rect.bottom - 42);



	//写最右面方框的文字
	pDC->SetTextColor(RGB(255,255,255));
	pDC->TextOut(rect.right - 188, 40,  "委  比", lstrlen("委  比"));

	pDC->TextOut(rect.right - 188, 65,  "卖  五", lstrlen("卖  四"));
	pDC->TextOut(rect.right - 188, 85,  "卖  四", lstrlen("卖  三"));
	pDC->TextOut(rect.right - 188, 105, "卖  三", lstrlen("卖  二"));
	pDC->TextOut(rect.right - 188, 125, "卖  二", lstrlen("卖  一"));
	pDC->TextOut(rect.right - 188, 145, "卖  一", lstrlen("卖  一"));

	pDC->TextOut(rect.right - 188, 175, "买  一", lstrlen("买  一"));
	pDC->TextOut(rect.right - 188, 195, "买  二", lstrlen("买  二"));
	pDC->TextOut(rect.right - 188, 215, "买  三", lstrlen("买  三"));
	pDC->TextOut(rect.right - 188, 235, "买  四", lstrlen("买  四"));
	pDC->TextOut(rect.right - 188, 255, "买  五", lstrlen("买  五"));

	pDC->TextOut(rect.right - 188, 285, "当  前", lstrlen("当  前"));
	pDC->TextOut(rect.right - 100, 285, "开 盘", lstrlen("开 盘"));
	pDC->TextOut(rect.right - 188, 305, "涨  幅", lstrlen("涨  幅"));
	pDC->TextOut(rect.right - 100, 305, "最 高", lstrlen("最 高"));
	pDC->TextOut(rect.right - 188, 325, "昨  收", lstrlen("昨  收"));
	pDC->TextOut(rect.right - 100, 325, "最 低", lstrlen("最 低"));

	//写最下面边框的文字
	pDC->SetTextColor(RGB(255,255,255));
	sprintf(chName, "%8s", "◆蜗牛201作品： QQ:879268496            email:wangpengfei.201@163.com");
	pDC->TextOut(15, rect.bottom - 28, chName, lstrlen(chName));

	//写右上角的文字
	pDC->SetTextColor(RGB(255,255,0));
	//pDC->TextOut(rect.right-192, 7, "*******",lstrlen("*******"));
	pDC->TextOut(rect.right-192, 7, m_stockName,lstrlen(m_stockName));
	pDC->SetTextColor(RGB(116,245,240));
	//pDC->TextOut(rect.right-100, 7, "600000",lstrlen("600000"));    
	pDC->TextOut(rect.right-100, 7, m_stockCode,lstrlen(m_stockCode));    

	pDC->SelectObject(&redpen);		
	m_interMinH = (rect.bottom - 110)/18;  
	m_interMinW = (rect.right - 322)/4;

	//以下是画那个外面的方框，为保证每份均等，所以要求interH和interW
	pDC->MoveTo(62, 35);
	pDC->LineTo(62+m_interMinW*4, 35);	
	pDC->LineTo(62+m_interMinW*4, 35+m_interMinH*18);
	pDC->LineTo(62, 35+m_interMinH*18);
	pDC->LineTo(62, 35); 	

	//以下是画横着的那些虚线
	pDC->SelectObject(&redDotpen);
	for(int h = 35+m_interMinH; h < 35+m_interMinH*18; h += m_interMinH)
	{
		pDC->MoveTo(62, h);
		pDC->LineTo(62+m_interMinW*4, h);
	}

	//以下是画第六条粗实线
	pDC->SelectObject(&redpen);	
	pDC->MoveTo(62, 35+m_interMinH*6);
	pDC->LineTo(62+m_interMinW*4, 35+m_interMinH*6);

	//以下是画第十二条细实线
	pDC->SelectObject(&redThinPen);
	pDC->MoveTo(62, 35+m_interMinH*12);
	pDC->LineTo(62+m_interMinW*4, 35+m_interMinH*12);      

	//以下是画竖着的的那三条细实线
	for(int w = 62+m_interMinW; w < 62+m_interMinW*4; w += m_interMinW)
	{
		pDC->MoveTo(w, 35);
		pDC->LineTo(w, 35+m_interMinH*18);
	}

	//以下是写时间
	pDC->SetTextColor(RGB(255, 255, 255));	
	pDC->TextOut(44, 37+m_interMinH*18, "09:30", lstrlen("09:30"));
	pDC->TextOut(44+m_interMinW, 37+m_interMinH*18, "10:30", lstrlen("10:30")); 
	pDC->TextOut(44+m_interMinW*2, 37+m_interMinH*18, "13:00", lstrlen("13:00"));
	pDC->TextOut(44+m_interMinW*3, 37+m_interMinH*18, "14:00", lstrlen("14:00"));
	pDC->TextOut(44+m_interMinW*4, 37+m_interMinH*18, "15:00", lstrlen("15:00"));

	//pDoc->GetMinData(pDoc->stockCode);
// 	pDoc->GetMinPoint();
// 	ShowMinData();
// 	DrawMinLine();

	pDoc->stockDoc->GetDayMinPoint(m_interMinH, m_interMinW);
	DrowMinLine();
	ShowMinData();

}

void CWnStockView::ShowMinData()
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CClientDC dcRed(this);
	CClientDC dcGreen(this);
	CClientDC dcWhite(this);

	CRect  rect;
	GetClientRect(&rect);

	char chName[300] = {0};

	dcRed.SetBkMode(OPAQUE);
	dcRed.SetBkColor(RGB(0,0,0));
	dcRed.SetTextAlign (TA_LEFT| TA_TOP);
	dcRed.SetTextColor(RGB(153,0,0));

	dcGreen.SetBkMode(OPAQUE);
	dcGreen.SetBkColor(RGB(0,0,0));
	dcGreen.SetTextAlign (TA_LEFT| TA_TOP);
	dcGreen.SetTextColor(RGB(0,255,255));

	dcWhite.SetBkMode(OPAQUE);
	dcWhite.SetBkColor(RGB(0,0,0));
	dcWhite.SetTextAlign (TA_LEFT| TA_TOP);
	dcWhite.SetTextColor(RGB(255,255,255));


	int n = pDoc->stockDoc->vMinData.size();
	if (n==0)
	{
		return;
	}
	if (pDoc->stockDoc->vMinData[n-1]->fNow >= pDoc->stockDoc->vMinData[n-1]->fPassClose)
	{
		dcRed.TextOut(rect.right-130, 65, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale5Price);
		dcRed.TextOut(rect.right-130, 65, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale5)/100);
		dcRed.TextOut(rect.right-60, 65, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 85, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale4Price);
		dcRed.TextOut(rect.right-130, 85, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale4)/100);
		dcRed.TextOut(rect.right-60, 85, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 105, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale3Price);
		dcRed.TextOut(rect.right-130, 105, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale3)/100);
		dcRed.TextOut(rect.right-60, 105, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 125, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale2Price);
		dcRed.TextOut(rect.right-130, 125, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale2)/100);
		dcRed.TextOut(rect.right-60, 125, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 145, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale1Price);
		dcRed.TextOut(rect.right-130, 145, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale1)/100);
		dcRed.TextOut(rect.right-60, 145, chName, lstrlen(chName)-1);

		dcRed.TextOut(rect.right-130, 175, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy1Price);
		dcRed.TextOut(rect.right-130, 175, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy1)/100);
		dcRed.TextOut(rect.right-60, 175, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 195, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy2Price);
		dcRed.TextOut(rect.right-130, 195, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy2)/100);
		dcRed.TextOut(rect.right-60, 195, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 215, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy3Price);
		dcRed.TextOut(rect.right-130, 215, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy3)/100);
		dcRed.TextOut(rect.right-60, 215, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 235, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy4Price);
		dcRed.TextOut(rect.right-130, 235, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy4)/100);
		dcRed.TextOut(rect.right-60, 235, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-130, 255, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy5Price);
		dcRed.TextOut(rect.right-130, 255, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy5)/100);
		dcRed.TextOut(rect.right-60, 255, chName, lstrlen(chName)-1);

		dcRed.TextOut(rect.right-140, 285, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fNow);
		dcRed.TextOut(rect.right-140, 285, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-140, 305, "      ", strlen("      ")-1);
		float ratio = (-pDoc->stockDoc->vMinData[n-1]->fPassClose + pDoc->stockDoc->vMinData[n-1]->fNow) / pDoc->stockDoc->vMinData[n-1]->fPassClose;
		sprintf(chName, "+%5.2f\n", ratio*100);
		dcRed.TextOut(rect.right-140, 305, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-140, 325, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fPassClose);
		dcRed.TextOut(rect.right-140, 325, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-50, 285, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fOpen);
		dcRed.TextOut(rect.right-50, 285, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-50, 305, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fHigh);
		dcRed.TextOut(rect.right-50, 305, chName, lstrlen(chName)-1);
		dcRed.TextOut(rect.right-50, 325, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fLow);
		dcRed.TextOut(rect.right-50, 325, chName, lstrlen(chName)-1);
	}
	else
	{
		dcGreen.TextOut(rect.right-130, 65, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale5Price);
		dcGreen.TextOut(rect.right-130, 65, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale5)/100);
		dcGreen.TextOut(rect.right-60, 65, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 85, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale4Price);
		dcGreen.TextOut(rect.right-130, 85, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale4)/100);
		dcGreen.TextOut(rect.right-60, 85, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 105, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale3Price);
		dcGreen.TextOut(rect.right-130, 105, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale3)/100);
		dcGreen.TextOut(rect.right-60, 105, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 125, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale2Price);
		dcGreen.TextOut(rect.right-130, 125, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale2)/100);
		dcGreen.TextOut(rect.right-60, 125, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 145, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fSale1Price);
		dcGreen.TextOut(rect.right-130, 145, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iSale1)/100);
		dcGreen.TextOut(rect.right-60, 145, chName, lstrlen(chName)-1);

		dcGreen.TextOut(rect.right-130, 175, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy1Price);
		dcGreen.TextOut(rect.right-130, 175, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy1)/100);
		dcGreen.TextOut(rect.right-60, 175, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 195, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy2Price);
		dcGreen.TextOut(rect.right-130, 195, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy2)/100);
		dcGreen.TextOut(rect.right-60, 195, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 215, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy3Price);
		dcGreen.TextOut(rect.right-130, 215, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy3)/100);
		dcGreen.TextOut(rect.right-60, 215, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 235, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy4Price);
		dcGreen.TextOut(rect.right-130, 235, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy4)/100);
		dcGreen.TextOut(rect.right-60, 235, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-130, 255, "                                      ", strlen("                                      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fBuy5Price);
		dcGreen.TextOut(rect.right-130, 255, chName, lstrlen(chName)-1);
		sprintf(chName, "%d\n", (pDoc->stockDoc->vMinData[n-1]->iBuy5)/100);
		dcGreen.TextOut(rect.right-60, 255, chName, lstrlen(chName)-1);

		dcGreen.TextOut(rect.right-140, 285, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fNow);
		dcGreen.TextOut(rect.right-140, 285, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-140, 305, "      ", strlen("      ")-1);
		float ratio = (pDoc->stockDoc->vMinData[n-1]->fPassClose - pDoc->stockDoc->vMinData[n-1]->fNow) / pDoc->stockDoc->vMinData[n-1]->fPassClose;
		sprintf(chName, "-%5.2f\n", ratio*100);
		dcGreen.TextOut(rect.right-140, 305, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-140, 325, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fPassClose);
		dcGreen.TextOut(rect.right-140, 325, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-50, 285, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fOpen);
		dcGreen.TextOut(rect.right-50, 285, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-50, 305, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fHigh);
		dcGreen.TextOut(rect.right-50, 305, chName, lstrlen(chName)-1);
		dcGreen.TextOut(rect.right-50, 325, "      ", strlen("      ")-1);
		sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[n-1]->fLow);
		dcGreen.TextOut(rect.right-50, 325, chName, lstrlen(chName)-1);
	}

	//分时线的最高，最低，昨收盘价格
	float diffPrice = 0; //每个interH对应的价格差
	float fPass = pDoc->stockDoc->vMinData[0]->fPassClose;
	if (pDoc->stockDoc->fLimitPrice > (fPass)) // 峰值在上
	{
		diffPrice = (pDoc->stockDoc->fLimitPrice - fPass)/6;
	}
	else//峰值在下
	{
		diffPrice = (fPass - pDoc->stockDoc->fLimitPrice)/6;
	}

	float upPrice1 = pDoc->stockDoc->vMinData[0]->fPassClose+diffPrice;
	float upPrice2 = pDoc->stockDoc->vMinData[0]->fPassClose+2*diffPrice;
	float upPrice3 = pDoc->stockDoc->vMinData[0]->fPassClose+3*diffPrice;
	float upPrice4 = pDoc->stockDoc->vMinData[0]->fPassClose+4*diffPrice;
	float upPrice5 = pDoc->stockDoc->vMinData[0]->fPassClose+5*diffPrice;
	float upPrice6 = pDoc->stockDoc->vMinData[0]->fPassClose+6*diffPrice;

	float downPrice1 = pDoc->stockDoc->vMinData[0]->fPassClose-diffPrice;
	float downPrice2 = pDoc->stockDoc->vMinData[0]->fPassClose-2*diffPrice;
	float downPrice3 = pDoc->stockDoc->vMinData[0]->fPassClose-3*diffPrice;
	float downPrice4 = pDoc->stockDoc->vMinData[0]->fPassClose-4*diffPrice;
	float downPrice5 = pDoc->stockDoc->vMinData[0]->fPassClose-5*diffPrice;
	float downPrice6 = pDoc->stockDoc->vMinData[0]->fPassClose-6*diffPrice;

	//(62, 35)
	dcRed.TextOut(20, 30+5*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", upPrice1);
	dcRed.TextOut(20, 30+5*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(20, 30+4*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", upPrice2);
	dcRed.TextOut(20, 30+4*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(20, 30+3*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", upPrice3);
	dcRed.TextOut(20, 30+3*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(20, 30+2*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", upPrice4);
	dcRed.TextOut(20, 30+2*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(20, 30+m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", upPrice5);
	dcRed.TextOut(20, 30+m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(20, 30, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", upPrice6);
	dcRed.TextOut(20, 30, chName, lstrlen(chName)-1);

	dcWhite.TextOut(20, 30+6*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", pDoc->stockDoc->vMinData[0]->fPassClose);
	dcWhite.TextOut(20, 30+6*m_interMinH, chName, lstrlen(chName)-1);

	dcGreen.TextOut(20, 30+7*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", downPrice1);
	dcGreen.TextOut(20, 30+7*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(20, 30+8*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", downPrice2);
	dcGreen.TextOut(20, 30+8*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(20, 30+9*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", downPrice3);
	dcGreen.TextOut(20, 30+9*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(20, 30+110*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", downPrice4);
	dcGreen.TextOut(20, 30+10*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(20, 30+11*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", downPrice5);
	dcGreen.TextOut(20, 30+11*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(20, 30+12*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f\n", downPrice6);
	dcGreen.TextOut(20, 30+12*m_interMinH, chName, lstrlen(chName)-1);

	//分时线的涨跌各个线的涨跌比例
	float upRatio1 = ((upPrice1- fPass)/fPass)*100;
	float upRatio2 = ((upPrice2- fPass)/fPass)*100;
	float upRatio3 = ((upPrice3- fPass)/fPass)*100;
	float upRatio4 = ((upPrice4- fPass)/fPass)*100;
	float upRatio5 = ((upPrice5- fPass)/fPass)*100;
	float upRatio6 = ((upPrice6- fPass)/fPass)*100;

	float downRatio1 = ((fPass- downPrice1)/fPass)*100;
	float downRatio2 = ((fPass- downPrice2)/fPass)*100;
	float downRatio3 = ((fPass- downPrice3)/fPass)*100;
	float downRatio4 = ((fPass- downPrice4)/fPass)*100;
	float downRatio5 = ((fPass- downPrice5)/fPass)*100;
	float downRatio6 = ((fPass- downPrice6)/fPass)*100;

	//涨幅比
	dcRed.TextOut(70+4*m_interMinW, 30+5*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", upRatio1);
	dcRed.TextOut(70+4*m_interMinW, 30+5*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(70+4*m_interMinW, 30+4*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", upRatio2);
	dcRed.TextOut(70+4*m_interMinW, 30+4*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(70+4*m_interMinW, 30+3*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", upRatio3);
	dcRed.TextOut(70+4*m_interMinW, 30+3*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(70+4*m_interMinW, 30+2*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", upRatio4);
	dcRed.TextOut(70+4*m_interMinW, 30+2*m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(70+4*m_interMinW, 30+m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", upRatio5);
	dcRed.TextOut(70+4*m_interMinW, 30+m_interMinH, chName, lstrlen(chName)-1);
	dcRed.TextOut(70+4*m_interMinW, 30, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", upRatio6);
	dcRed.TextOut(70+4*m_interMinW, 30, chName, lstrlen(chName)-1);

	//涨跌幅中间基准
	dcWhite.TextOut(70+4*m_interMinW, 30+6*m_interMinH, "      ", strlen("      ")-1);
	dcWhite.TextOut(70+4*m_interMinW, 30+6*m_interMinH, "0.00%%", lstrlen("0.00%%")-1);

	//跌幅比
	dcGreen.TextOut(70+4*m_interMinW, 30+7*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", downRatio1);
	dcGreen.TextOut(70+4*m_interMinW, 30+7*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(70+4*m_interMinW, 30+8*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", downRatio2);
	dcGreen.TextOut(70+4*m_interMinW, 30+8*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(70+4*m_interMinW, 30+9*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", downRatio3);
	dcGreen.TextOut(70+4*m_interMinW, 30+9*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(70+4*m_interMinW, 30+10*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", downRatio4);
	dcGreen.TextOut(70+4*m_interMinW, 30+10*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(70+4*m_interMinW, 30+11*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", downRatio5);
	dcGreen.TextOut(70+4*m_interMinW, 30+11*m_interMinH, chName, lstrlen(chName)-1);
	dcGreen.TextOut(70+4*m_interMinW, 30+12*m_interMinH, "      ", strlen("      ")-1);
	sprintf(chName, "%5.2f%%\n", downRatio6);
	dcGreen.TextOut(70+4*m_interMinW, 30+12*m_interMinH, chName, lstrlen(chName)-1);

	return;
}

void CWnStockView::DrowMinLine()
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);


	CClientDC dcRed(this);
	CClientDC dcYellow(this);

	CPen  redPen(PS_SOLID,1,RGB(255, 255, 255));
	CPen  yellowPen(PS_SOLID,1,RGB(255, 255, 0));

	dcRed.SelectObject(&redPen);
	dcYellow.SelectObject(&yellowPen);

	int n = pDoc->stockDoc->vMinPricePoint.size();
	if (n == 0)
	{
		return;
	}
	//画价格线
	dcRed.MoveTo(pDoc->stockDoc->vMinPricePoint[0].x ,pDoc->stockDoc->vMinPricePoint[0].y);
	for(int i=1; i<n; i++)
	{
		dcRed.LineTo(pDoc->stockDoc->vMinPricePoint[i].x ,pDoc->stockDoc->vMinPricePoint[i].y);
	}

	//画平均价格线

	int len = pDoc->stockDoc->vMinAvgPricePoint.size();
	dcYellow.MoveTo(pDoc->stockDoc->vMinAvgPricePoint[0].x ,pDoc->stockDoc->vMinAvgPricePoint[0].y);
	for(int i=1; i<n; i++)
	{
		dcYellow.LineTo(pDoc->stockDoc->vMinAvgPricePoint[i].x ,pDoc->stockDoc->vMinAvgPricePoint[i].y);
	}

	//画成交量线
	for(int i=1; i<n; i++)
	{
		dcYellow.MoveTo(pDoc->stockDoc->vMinVolumePoint[i].x ,35+18*m_interMinH);
		dcYellow.LineTo(pDoc->stockDoc->vMinVolumePoint[i].x ,pDoc->stockDoc->vMinVolumePoint[i].y);
	}
	return ;
}

UINT CWnStockView::ThreadGetMinData(LPVOID lParam)
{
	CWnStockView* pThis = (CWnStockView*)lParam;
	CWnStockDoc* pDoc = (CWnStockDoc*)pThis->GetDocument();
	StockTime	stockTime;	

	while(1)
	{
		if ((stockTime.bExchange()) && (pThis->m_drawStatus == 2))
		{
			pDoc->stockDoc->GetDayMinData(pThis->m_stockCode.GetBuffer(pThis->m_stockCode.GetLength()));
			OutputDebugString("获得数据成功\n");
			pThis->Invalidate();
		}
		Sleep(10*1000);
	}
}
/********************************* 三界面公用函数部分 ******************************************/
int CWnStockView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(1, 10000, NULL);
	//AfxBeginThread(ThreadGetMytockData, this);
	
	AfxBeginThread(ThreadGetMinData, this);
	return 0;
}


void CWnStockView::OnTimer(UINT_PTR nIDEvent)
{
	CWnStockDoc* pDoc = GetDocument();
	if (m_drawStatus == 0)
	{
		StockTime	stockTime;	
		if (stockTime.bExchange())
		{
			pDoc->GetMytockData();
			Invalidate();
		}
	}
	CScrollView::OnTimer(nIDEvent);
}

CPoint oldpointx1,oldpointx2;
CPoint oldpointy1,oldpointy2;
void CWnStockView::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	CRect rect;	
	GetClientRect (&rect);

	POINT point1;
	GetCursorPos(&point1);
	ScreenToClient(&point1);
	int x = point1.x;
	int y = point1.y;

	//判断鼠标所在的行
	if (m_drawStatus == 0)
	{
		int m_pos = GetScrollPos(SB_VERT);

		//鼠标在滚动条区域中的位置
		int yy = y + m_pos;
		for (int i=0; i<m_totalNum; i++)
		{
			if ((yy>(30+20*i)) && (yy<(30+20*(i+1))))
			{
				//bMyStockline = TRUE;
				m_mouseLine = i+1;
				break;
			}
		}

		//判断鼠标在第一行的位置(名称栏的位置)
		if ((m_pos==0) && (y>0) && (y<30) && (x>(m_width/8 + 3*m_width)) && (x<(m_width/8 + 3*m_width+20)))
		{
			m_hHandCursor1=LoadCursor (NULL,IDC_HAND);
			SetCursor(m_hHandCursor1);
			eMousePos = ratioPos;
		}
		else
		{
			eMousePos = dataPos;
		}
	}
	if (m_drawStatus == 1)
	{
		CDC* pDC = GetDC();
		pDC->SetROP2(R2_NOT);

		if (bCrossLine)
		{
			//画十字星线
			if (!bFirstCrossLine)
			{
				//清楚上次画线的痕迹
				pDC->MoveTo(oldpointx1);
				pDC->LineTo(oldpointy1);
				pDC->MoveTo(oldpointx2);
				pDC->LineTo(oldpointy2);
			}
			if ((x>=62) && (x<=(rect.right-200))  && (y>=17) && (y<=(9.5*m_interKH+17)))
			{
				bFirstCrossLine = FALSE;
				oldpointx1.x = 62;
				oldpointx1.y = y;
				oldpointy1.x = rect.right-200;
				oldpointy1.y = y;
				oldpointx2.x = x;
				oldpointx2.y =17;
				oldpointy2.x = x;
				oldpointy2.y = /*9.5*interY+17*/rect.bottom-50;

				//画线
				pDC->MoveTo(62,y);
				pDC->LineTo(rect.right-200,y);
				pDC->MoveTo(x,17);
				pDC->LineTo(x,/*9.5*interY+17*/rect.bottom - 50);

				//判断鼠标位置对应的kDataGroup中的位置
				double lw = (rect.right - 200 -62)/pDoc->stockDoc->KDay;
				for (int i=0;i<pDoc->stockDoc->KDay;i++)
				{
					//if ((x>(i*lw+62+0.5*(i+1))) && (x<((i+1)*lw)+62+0.5*(i+1)))
					if ((x>(i*lw+62)) && (x<((i+1)*lw)+62))
					{
						ShowKData(i);
						DrawMacd(i);
						break;
					}
				}
			}
			else
			{
				bFirstCrossLine = TRUE;
			}
		}
		ReleaseDC(pDC);
	}
	CScrollView::OnMouseMove(nFlags, point);
}


void CWnStockView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnStockDoc* pDoc = GetDocument();
	if (m_drawStatus == 0)
	{
		m_selectLine = m_mouseLine;
		Invalidate();

		m_stockCode = pDoc->vMystockRealTimeData[m_selectLine-1]->strStockNum;
		m_stockName = pDoc->vMystockRealTimeData[m_selectLine-1]->strStockName;
		OutputDebugString(m_stockCode);
		OutputDebugString(m_stockName + "\r\n");
	}
	if (m_drawStatus == 1)
	{
		bCrossLine = TRUE;
	}

	CScrollView::OnLButtonDown(nFlags, point);
}


void CWnStockView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnStockDoc* pDoc = GetDocument();
	int kDay0 = 0;
	int kBegin0;
	int pos;
	bFirstCrossLine = TRUE;
	bCrossLine = FALSE;
	switch(nChar)
	{
	case VK_UP:
		{
			if (m_drawStatus==0)
			{
				if(m_selectLine>1)
				{
					m_selectLine--;
					Invalidate();
				}
			}
			else if(m_drawStatus==1) //K线
			{
				kDay0 =  pDoc->stockDoc->KDay;
				kDay0 -= 5;
				if (kDay0 >0)
				{
					//重绘时候恢复开始参数设置
					pDoc->stockDoc->KDay = kDay0;
					Invalidate();
				}
			}
			return;
		}
	case VK_DOWN:
		{
			if (m_drawStatus==0)
			{
				if(m_selectLine< (m_totalNum-1))
				{
					m_selectLine++;
					//UpdateWindow();
					Invalidate();
				}
			}
			else if (m_drawStatus==1) //K线
			{
				kDay0 =  pDoc->stockDoc->KDay;
				kDay0 += 5;
				if (kDay0 <=(pDoc->stockDoc->vKData.size() + pDoc->stockDoc->KPos))
				{
					//重绘时候恢复开始参数设置
					pDoc->stockDoc->KDay = kDay0;
					Invalidate();
				}
				else //剩余天数小于5天
				{
					kDay0 += 1;
					if (kDay0 < (pDoc->stockDoc->vKData.size() + pDoc->stockDoc->KPos))
					{
						//重绘时候恢复开始参数设置
						pDoc->stockDoc->KDay = kDay0;
						Invalidate();
					}
				}
			}
			return;
		}
	case VK_LEFT:
		{
			if (m_drawStatus==1)//显示K线
			{
				int maxPos = pDoc->stockDoc->vKData.size()- pDoc->stockDoc->KDay;
				pos = pDoc->stockDoc->KPos -5;
				if ((-pos) <= maxPos)
				{
					pDoc->stockDoc->KPos = pos;
					Invalidate();
				}
				else //剩余未显示天数小于5
				{
					pos = pDoc->stockDoc->KPos -1;
					if ((-pos) <= maxPos)
					{
						pDoc->stockDoc->KPos = pos;
						Invalidate();
					}
				}
			}
		}
		break;
	case VK_RIGHT:
		{
			int maxPos = pDoc->stockDoc->vKData.size() - pDoc->stockDoc->KDay;
			pos = pDoc->stockDoc->KPos + 5;
			if (pos <=0)
			{
				pDoc->stockDoc->KPos = pos;
				Invalidate();
			}
			else//剩余未显示天数小于5
			{
				pos = pDoc->stockDoc->KPos +1;
				if (pos <=0)
				{
					pDoc->stockDoc->KPos = pos;
					Invalidate();
				}
			}
			Invalidate();
		}
		break;
	case VK_RETURN:
		{
			AfxMessageBox("该键尚无功能");
			return;
		}
	case VK_ESCAPE://自选区显示
		{
			//显示滑动条
			bScroll = TRUE;
			OnInitialUpdate();

			m_drawStatus = 0;
			m_stockCode = "";
			m_stockName = "";
			Invalidate();
			return;
		}
	default:
		{
// 			CSearch cSearch;
// 			cSearch.DoModal();
			return;
		}
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CWnStockView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (m_drawStatus == 0)
	{
		bScroll = false;
		OnInitialUpdate();
		m_drawStatus = 2;
		pDoc->stockDoc->GetDayMinData(m_stockCode.GetBuffer(m_stockCode.GetLength()));
		
		Invalidate();
	}
	else if (m_drawStatus == 2)
	{
		bScroll = false;
		OnInitialUpdate();
		m_drawStatus = 1;
		pDoc->stockDoc->KDay = 60;
		pDoc->stockDoc->GetDayKData(m_stockCode.GetBuffer(m_stockCode.GetLength()));
		Invalidate();
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}
