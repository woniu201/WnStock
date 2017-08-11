
// WnStockDoc.h : CWnStockDoc 类的接口
//


#pragma once

struct 	KPoint	//K线价格对应坐标结构体
{	
	POINT high;
	POINT low;
	POINT open;
	POINT close;
};

struct KVolumePoint     //K线价格对应的坐标结构体 
{
	POINT top;
	POINT bottom;
};
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
	/****************自定义变量和函数******************/
	
//自选股容器，变量和函数
	vector<string>	vMystock;
	void			ReadMystock();					//获取自选股股票代码
	void			WriteMystock();					//保存自选股到本地数据库(sqlite)
	void			GetMytockData();				//获取所有自选股的数据
	vector<RealTimeStock*> vMystockRealTimeData;	//自选股实时数据容器
//K线容器，变量和函数
	vector<KStock*>		vKData;						//K历史数据
	vector <KPoint *>	vKPoint;					//K线坐标容器
	vector <KVolumePoint *> vVolumePoint;		    //成交量坐标容器 (KDAY,KPOS,KBEGIN变量用double是因为需要和浮点数运算避免了强制转换)
	vector <double>		vMa5Price;					//5日均价数据
	vector <double>		vMa10Price;					//10日均价数据
	vector <double>		vMa20Price;					//20日均价数据
	vector <POINT>		vMa5Point;					//MA5坐标
	vector <POINT>		vMa10Point;					//MA10坐标
	vector <POINT>		vMa20Point;					//MA20坐标
	vector <double>		vMacdDif;					//MACD线中DIF容器
	vector <double>		vMacdDea;					//MACD线中DEA容器
	vector <double>		vMacd;						//MACD线中macd容器
	vector <POINT >		vMacdDifPoint;				//MACD线中DIF坐标容器
	vector <POINT >		vMacdDeaPoint;				//MACD线中DEA坐标容器
	vector <POINT >		vMacdPoint;					//MACD线中macd坐标容器

	double				KDay;						//要画K线的天数
	double				KPos;						//K线左右移动所用到的偏移
	double				KBegin;						//开始显示的K线索引	
	double				KHighMax;					//显示K线范围内的最高价
	double				KLowMin;					//显示K线范围内的最低价
	double				KMaxVolume;					//显示K线范围内的最大成交量
	float maxMACD;            //显示范围内最大macd值
	float minMACD;            //显示范围内最小macd值

	void GetDayKData(char* stockCode);				//获取K线历史数据，保存到"WnStockDll.h"中的vKStockData中
	void GetKPoint();								//获得K线的坐标点
	void GetKVolumePoint();							//获得成交量坐标
	void GetMaPrice();								//获取均线的价格
	void GetMaPoint();								//获得MA5,MA10,MA20坐标
	void GetMacd();									//获得MACD值
	void GetMacdPoint();							//获得MACD的坐标值

//其他函数
	void AddMystock(char* stockCode);				//右键弹出框添加自选股使用
	void DelMystock(char* stockCode);				//右键弹出框删除自选股使用
};
