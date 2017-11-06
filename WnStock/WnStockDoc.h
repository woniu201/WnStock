
// WnStockDoc.h : CWnStockDoc 类的接口
//


#pragma once


class CWnStockDoc : public CDocument
{
protected: // 仅从序列化创建
	CWnStockDoc();
	DECLARE_DYNCREATE(CWnStockDoc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CWnStockDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

	
public:
	StockDoc* stockDoc;
	/****************自定义变量和函数******************/
	
//自选股容器，变量和函数
	vector<string>	vMystock;
	void			ReadMystock();					//获取自选股股票代码
	void			WriteMystock();					//保存自选股到本地数据库(sqlite)
	void			GetMytockData();				//获取所有自选股的数据
	vector<RealTimeStock*> vMystockRealTimeData;	//自选股实时数据容器

	//其他函数
	void AddMystock(char* stockCode);				//右键弹出框添加自选股使用
	void DelMystock(char* stockCode);				//右键弹出框删除自选股使用
};
