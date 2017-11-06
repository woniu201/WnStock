
// WnStockView.h : CWnStockView 类的接口
//
#include "../WnStockDll/WnStockDll.h"

#pragma once

//鼠标所在位置，第一行名称栏的具体位置，还是滚动条区域
enum	EMousePos{
	ratioPos,
	dataPos
};		
class CWnStockView : public CScrollView
{
protected: // 仅从序列化创建
	CWnStockView();
	DECLARE_DYNCREATE(CWnStockView)

// 特性
public:
	CWnStockDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CWnStockView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

/*******************自定义变量*****************************/
public:
	//CRect rect;
	bool		bScroll;		//是否显示滚动条
	CString		m_stockCode;	//当前股票代码
	CString		m_stockName;	//当前股票名称
    
	//自选股UI所用变量
	int			m_drawStatus;	//0:股票展示，1:画分时线，2:画K线
	int			m_selectLine;   //表示自选股中当前选中的行
	int			m_mouseLine;	//表示自选股中鼠标所在的行
	int			m_totalNum;		//My Stock的数量
	int			m_width;		//自选股区每一行显示数据的增量
	int			m_height;		//自选股区每一列显示数据的增量
	int			m_posY;			//滚动条下滑距离顶部高度，固定自选界面首行使用
	HCURSOR		m_hHandCursor1; //手型鼠标
	EMousePos	eMousePos;

	//K线UI所用变量
	double		m_interKH;		//K线的间隔高度
	double		m_interKW;		//K先的间隔宽度

	//十字星线变量
	BOOL		bFirstCrossLine;//是否第一次画十字线
	BOOL		bCrossLine;		//是否交叉

	//分时线
	double		m_interMinH;	//分时线间隔高度
	double		m_interMinW;	//分时线间隔宽度

/******************自定义函数******************************/
protected:
	//自选股UI所用函数
	void		DrowMyStockUI(CDC* pDc);			//画自选股部分
	static UINT ThreadGetMytockData(LPVOID lParam);	//获取My Stock数据线程
	
	//K线UI所用函数
	void		DrowKLineUI(CDC* pDC);				//画K线框架
	void		DrawKLine();						//画K线
	void		DrawMaLine();						//画MA线
	void		ShowKData(int i);					//K线展示数据
	void		DrawMacd(int i);					//画MACD

	//分时线UI所有函数
	void		DrowMinLineUI(CDC* pDC);			//画分时线框架
	void		ShowMinData();						//数据显示
	void		DrowMinLine();						//画分时线
	static UINT ThreadGetMinData(LPVOID lParam);	//获取分时数据线程

public:
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAddMystock();
	afx_msg void OnDelMystock();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // WnStockView.cpp 中的调试版本
inline CWnStockDoc* CWnStockView::GetDocument() const
   { return reinterpret_cast<CWnStockDoc*>(m_pDocument); }
#endif

