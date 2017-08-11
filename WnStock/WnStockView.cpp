
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
	sprintf(chName, "%8s", "◆蜗牛201作品： QQ:879268496            email:wangpengfei.201@163.com");
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
	pDoc->GetKPoint();
	pDoc->GetKVolumePoint();
	DrawKLine();
	
	//MA线
	pDoc->GetMaPrice();
	pDoc->GetMaPoint();
	DrawMaLine();

	//K线展示数据
	ShowKData(pDoc->KDay-1);

	//MACD线
	pDoc->GetMacd();
	pDoc->GetMacdPoint();
	DrawMacd(pDoc->KDay-1);
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
	//pDoc->ExchangeK();

	CRect rect, kRect, kaRect;
	GetClientRect(&rect);
	float lw = (rect.right - 200 -62)/pDoc->KDay;//每个K线所占的宽度
	int n = pDoc->vKPoint.size();  //获得保存坐标个数
	if (n == 0)
	{
		return;
	}
	//显示最新的数据
	if(n < pDoc->KDay - pDoc->KBegin)
		j = 0+ pDoc->KPos;
	else
		j = n - (pDoc->KDay - pDoc->KBegin) + pDoc->KPos;

	int i;
	int pos = pDoc->KPos;
	for(/*int*/ i = j ; i < n; i++)
	{
		if((pDoc->vKPoint[i]->close.y) <= (pDoc->vKPoint[i]->open.y)) //收盘价大于开盘价为阳线
		{
			dc.SelectObject(&redPen);
			dc.SelectObject(&blackBrush);
		}
		else
		{
			dc.SelectObject(&cyanPen);
			dc.SelectObject(&cyanBrush);
		}
		/*		if(62 + (i-j)*lw + pDoc->Get_kPointArray(i).high.x + pDoc->Get_kBegine()*(rect.right - 200 - 62)/pDoc->Get_kDay() > 62 && 62 + (i-j)*lw + pDoc->Get_kPointArray(i).high.x + pDoc->Get_kBegine()*(rect.right - 200 - 62)/pDoc->Get_kDay() < rect.right - 200)*/
		//判断坐标是否在绘制K线的区域
		if (62 + (i-j)*lw + pDoc->vKPoint[i]->high.x + pDoc->KBegin*lw > 62 && 62 + (i-j)*lw + pDoc->vKPoint[i]->high.x + pDoc->KBegin*lw < rect.right - 200)
		{
			dc.MoveTo(62 + (i-j)*lw + pDoc->vKPoint[i]->high.x + pDoc->KBegin*lw, pDoc->vKPoint[i]->high.y);
			dc.LineTo(62 + (i-j)*lw + pDoc->vKPoint[i]->low.x + pDoc->KBegin*lw, pDoc->vKPoint[i]->low.y);
			kRect.left =62 + (i-j)*lw + pDoc->vKPoint[i]->open.x + pDoc->KBegin*lw;
			kRect.top = pDoc->vKPoint[i]->open.y;
			kRect.right = 62 + (i-j)*lw +pDoc->vKPoint[i]->close.x + pDoc->KBegin*lw;
			kRect.bottom = pDoc->vKPoint[i]->close.y;
			dc.Rectangle(&kRect);     //画矩形

			//@20150506解决开盘板没有K线问题,和开盘价等于收盘价没有横线只有竖线问题
			if (pDoc->vKPoint[i]->open.y == pDoc->vKPoint[i]->close.y)

			{
				dc.MoveTo(62 + (i-j)*lw + pDoc->vKPoint[i]->open.x + pDoc->KBegin*lw,pDoc->vKPoint[i]->open.y);
				dc.LineTo(62 + (i-j)*lw +pDoc->vKPoint[i]->close.x + pDoc->KBegin*lw,pDoc->vKPoint[i]->close.y);
			}

			//画K线总量坐标
			kaRect.left = 62 + (i-j)*lw + pDoc->vVolumePoint[i]->top.x + pDoc->KBegin*lw;
			kaRect.top = pDoc->vVolumePoint[i]->top.y;
			kaRect.right = 62 + (i-j)*lw +pDoc->vVolumePoint[i]->bottom.x + pDoc->KBegin*lw;
			kaRect.bottom = pDoc->vVolumePoint[i]->bottom.y;
			dc.Rectangle(&kaRect);
		}
	}

	//画K线价格坐标值
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(240, 0, 0));
	dc.SetTextAlign(TA_RIGHT);
	float KM = pDoc->KHighMax - pDoc->KLowMin;
	for(i = 0; i < 9; i++)
	{
		sprintf(chName, "%10.2f", pDoc->KHighMax - (KM/9.0)*i);
		dc.TextOut(55, 17 + 0.5 * m_interKH + i * m_interKH - 10, chName, lstrlen(chName));
	}
	sprintf(chName, "%10.2f", pDoc->KHighMax - (KM/9.0)*9);
	dc.TextOut(55, 17 + 9.5 * m_interKH - 20, chName, lstrlen(chName));


	//画K线成交金量坐标值
	dc.SetTextColor(RGB(116, 245, 240));
	sprintf(chName, "%8.0d万", pDoc->KMaxVolume/10000);
	dc.TextOut(55, rect.bottom - 50 - 6*m_interKH - 8, chName, lstrlen(chName));

	sprintf(chName, "%8.0d万", pDoc->KMaxVolume/10000/2);
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
	float lw = b/pDoc->KDay; 


	//画MA5
	int n = pDoc->vMa5Price.size();
	if (n == 0)
	{
		return;
	}
	for (int i=1;i<pDoc->KDay;i=i+1) //没三天画一直线
	{
		if((n - pDoc->KDay +i + pDoc->KPos) >=5) //前4天不画MA5
		{
			ma5Pen.MoveTo(pDoc->vMa5Point[i].x + pDoc->KBegin*lw, pDoc->vMa5Point[i].y);
			ma5Pen.LineTo(pDoc->vMa5Point[i-1].x + pDoc->KBegin*lw,pDoc->vMa5Point[i-1].y);
		}
	}

	//画MA10
	n = pDoc->vMa10Price.size();
	for (int i=1;i<pDoc->KDay;i=i+1) //没三天画一直线
	{
		if((n - pDoc->KDay +i + pDoc->KPos) >=10) //前9天不画MA10
		{
			ma10Pen.MoveTo(pDoc->vMa10Point[i].x + pDoc->KBegin*lw, pDoc->vMa10Point[i].y);
			ma10Pen.LineTo(pDoc->vMa10Point[i-1].x + pDoc->KBegin*lw,pDoc->vMa10Point[i-1].y);
		}
	}

	//画MA5
	n = pDoc->vMa20Price.size();
	for (int i=1;i<pDoc->KDay;i=i+1) //没三天画一直线
	{
		if((n - pDoc->KDay +i + pDoc->KPos) >=20) //前19天不画MA20
		{
			ma20Pen.MoveTo(pDoc->vMa20Point[i].x + pDoc->KBegin*lw, pDoc->vMa20Point[i].y);
			ma20Pen.LineTo(pDoc->vMa20Point[i-1].x + pDoc->KBegin*lw,pDoc->vMa20Point[i-1].y);
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
	int n = pDoc->vKData.size();
	if (n == 0)
	{
		return;
	}

	dc.TextOut(rect.right-110, 44, "                        ", strlen("                        ")-1);
	CString str = (pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->strDate);
	dc.TextOut(rect.right-110, 44,  str,  strlen(str));

	dc.TextOut(rect.right-110, 74, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fOpen);
	dc.TextOut(rect.right-110, 74, chName, lstrlen(chName)-1);

	dc.TextOut(rect.right-110, 104, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fClose);
	dc.TextOut(rect.right-110, 104, chName, lstrlen(chName)-1);

	dc.TextOut(rect.right-110, 132, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fHigh);
	dc.TextOut(rect.right-110, 132, chName, lstrlen(chName)-1);

	dc.TextOut(rect.right-110, 162, "                        ", strlen("                        ")-1);
	sprintf(chName, "%10.2f\n", pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fLow);
	dc.TextOut(rect.right-110, 162, chName, lstrlen(chName)-1);

	//画涨幅数据
	if (pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fPassClose <= pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fClose)
	{
		dc.TextOut(rect.right-110, 192, "                        ", strlen("                        ")-1);//清楚上次显示留下的痕迹
		float grow = pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fClose - pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fPassClose;
		float ratio = grow /(pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fPassClose)*100;
		sprintf(chName, "%10.2f\n", grow);
		dc.TextOut(rect.right-110, 192, chName, lstrlen(chName)-1);
		dc.TextOut(rect.right-110, 222, "                        ", strlen("                        ")-1);
		sprintf(chName, "      %.2f %%\n", ratio);
		dc.TextOut(rect.right-110, 222, chName, lstrlen(chName)-1);
	}
	else
	{
		dc.TextOut(rect.right-110, 192, "                        ", strlen("                        ")-1);//清楚上次显示留下的痕迹
		float grow = ((pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fPassClose) - (pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fClose));
		float ratio = grow/(pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->fPassClose);
		sprintf(chName, "%10.2f\n", grow);
		dc1.TextOut(rect.right-110, 192, chName, lstrlen(chName)-1);
		dc.TextOut(rect.right-110, 222, "                        ", strlen("                        ")-1);
		sprintf(chName, "     - %.2f %%\n", ratio*100);
		dc1.TextOut(rect.right-110, 222, chName, lstrlen(chName)-1);
	}

	//画成交量
	dc.TextOut(rect.right-110, 252, "                        ", lstrlen("                        ")-1);
	sprintf(chName, "%llu\n", pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->Volume); //%llu 64位无符号整形
	dc.TextOut(rect.right-110, 252, chName, lstrlen(chName)-1);


	//计算量比并显示
	//计算日量比
	if (n-pDoc->KDay+i + pDoc->KPos > 6)
	{
		// 		CFunction function;
		// 		CString str;
		// 		function.GetThan(n-pDoc->KDay+i + pDoc->KPos,str);
		// 		AfxMessageBox(str);
		long long nowVolume = pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->Volume;
		long long passVolume = 0;
		float fThan = 0;
		for (int j = n-pDoc->KDay+i + pDoc->KPos-1; j>n-pDoc->KDay+i + pDoc->KPos-6; j--)
		{
			passVolume += pDoc->vKData[j]->Volume/5;
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
	//sprintf(chName, "%10.2f\n", (float)pDoc->m_Stock.Pbuy3);
	dc.TextOut(rect.right-110, 312, "                        ", lstrlen("                        ")-1);
	sprintf(chName, "%llu\n", pDoc->vKData[n-pDoc->KDay+i + pDoc->KPos]->Amount); //%llu 64位无符号整形
	dc.TextOut(rect.right-110, 312, chName, lstrlen(chName)-1);


	//左上角显示MA5,MA10,MA20的数据
	n = pDoc->vMa5Price.size();
	if (pDoc->vMa5Price.size() > 5)
	{
		sprintf(chName, "MA5=%.2f\n", pDoc->vMa5Price[n-pDoc->KDay+i + pDoc->KPos]);
		dcWhite.TextOut(64, 17+0.05*m_interKH, chName, lstrlen(chName) -1);
	}
	n = pDoc->vMa10Price.size();
	if (pDoc->vMa10Price.size() > 10)
	{
		sprintf(chName, "MA10=%.2f\n", pDoc->vMa10Price[n-pDoc->KDay+i + pDoc->KPos]);
		dcYellow.TextOut(130, 17+0.05*m_interKH, chName, lstrlen(chName) -1);
	}
	n = pDoc->vMa20Price.size();
	if (pDoc->vMa20Price.size() > 10)
	{
		sprintf(chName, "MA20=%.2f\n", pDoc->vMa20Price[n-pDoc->KDay+i + pDoc->KPos]);
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

	int n = pDoc->vMacd.size();
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
	for (int j=0; j<pDoc->KDay; j=j+1)
	{

		if (pDoc->vMacd[n-pDoc->KDay + j + pDoc->KPos] >= 0)
		{
			dc1.MoveTo(pDoc->vMacdPoint[j].x, 14*m_interKH+17);
			dc1.LineTo(pDoc->vMacdPoint[j].x, pDoc->vMacdPoint[j].y);
		}
		else
		{
			dc2.MoveTo(pDoc->vMacdPoint[j].x, 14*m_interKH+17);
			dc2.LineTo(pDoc->vMacdPoint[j].x, pDoc->vMacdPoint[j].y);
		}
	}

	//画MACD的坐标值
	dc1.SetBkMode(OPAQUE);
	dc1.SetBkColor(RGB(0,0,0));
	dc1.SetTextAlign (TA_LEFT| TA_TOP);
	dc1.SetTextColor(RGB(240,0,0));
	char chName[1024] = {0};

	sprintf(chName, "%.2f", pDoc->maxMACD);
	dc1.TextOut(30, rect.bottom - 50 - 3*m_interKH - 8, chName, lstrlen(chName));
	sprintf(chName, "%d", 0);
	dc1.TextOut(40, rect.bottom - 50 - 2*m_interKH - 8, chName, lstrlen(chName));
	sprintf(chName, "%.2f", pDoc->minMACD);
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
	sprintf(chName,"DIF:%4.2f", pDoc->vMacdDif[n-pDoc->KDay+i + pDoc->KPos]);
	dc3.TextOut(64, rect.bottom-(24/5*m_interKH)-50, chName, lstrlen(chName));
	sprintf(chName," DEA:%4.2f", pDoc->vMacdDea[n-pDoc->KDay+i + pDoc->KPos]);
	dc4.TextOut(124, rect.bottom-(24/5*m_interKH)-50, chName, lstrlen(chName));
	sprintf(chName," MACD:%4.2f", pDoc->vMacd[n-pDoc->KDay+i + pDoc->KPos]);
	dc5.TextOut(194, rect.bottom-(24/5*m_interKH)-50, chName, lstrlen(chName));

	//开始画DIF曲线
	CClientDC dc6(this);
	CPen difPen(PS_DOT, 1, RGB(255, 255, 255));
	dc6.SelectObject(difPen);
	for (int i=1; i<pDoc->KDay; i=i+1)
	{
		dc6.MoveTo(pDoc->vMacdDifPoint[i].x, pDoc->vMacdDifPoint[i].y);
		dc6.LineTo(pDoc->vMacdDifPoint[i-1].x, pDoc->vMacdDifPoint[i-1].y);
	}

	//开始画DIF曲线
	CClientDC dc7(this);
	CPen deaPen(PS_DOT, 1, RGB(255, 255, 0));
	dc7.SelectObject(deaPen);
	for (int i=1; i<pDoc->KDay; i=i+1)
	{
		dc7.MoveTo(pDoc->vMacdDeaPoint[i].x, pDoc->vMacdDeaPoint[i].y);
		dc7.LineTo(pDoc->vMacdDeaPoint[i-1].x, pDoc->vMacdDeaPoint[i-1].y);
	}
	return;
}
/********************************* 三界面公用函数部分 ******************************************/
int CWnStockView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(1, 10000, NULL);
	//AfxBeginThread(ThreadGetMytockData, this);
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
				double lw = (rect.right - 200 -62)/pDoc->KDay;
				for (int i=0;i<pDoc->KDay;i++)
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
				kDay0 =  pDoc->KDay;
				kDay0 -= 5;
				if (kDay0 >0)
				{
					//重绘时候恢复开始参数设置
					pDoc->KDay = kDay0;
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
				kDay0 =  pDoc->KDay;
				kDay0 += 5;
				if (kDay0 <=(pDoc->vKData.size() + pDoc->KPos))
				{
					//重绘时候恢复开始参数设置
					pDoc->KDay = kDay0;
					Invalidate();
				}
				else //剩余天数小于5天
				{
					kDay0 += 1;
					if (kDay0 < (pDoc->vKData.size() + pDoc->KPos))
					{
						//重绘时候恢复开始参数设置
						pDoc->KDay = kDay0;
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
				int maxPos = pDoc->vKData.size()- pDoc->KDay;
				pos = pDoc->KPos -5;
				if ((-pos) <= maxPos)
				{
					pDoc->KPos = pos;
					Invalidate();
				}
				else //剩余未显示天数小于5
				{
					pos = pDoc->KPos -1;
					if ((-pos) <= maxPos)
					{
						pDoc->KPos = pos;
						Invalidate();
					}
				}
			}
		}
		break;
	case VK_RIGHT:
		{
			int maxPos = pDoc->vKData.size() - pDoc->KDay;
			pos = pDoc->KPos + 5;
			if (pos <=0)
			{
				pDoc->KPos = pos;
				Invalidate();
			}
			else//剩余未显示天数小于5
			{
				pos = pDoc->KPos +1;
				if (pos <=0)
				{
					pDoc->KPos = pos;
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
		m_drawStatus = 1;
		pDoc->KDay = 60;
		pDoc->GetDayKData(m_stockCode.GetBuffer(m_stockCode.GetLength()));
		Invalidate();
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}
