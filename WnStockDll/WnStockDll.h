#ifndef _WNSTOCKDLL_H
#define _WNSTOCKDLL_H

#define WNSTOCKDLL_EXPORTS

#ifdef WNSTOCKDLL_EXPORTS
#define WNSTOCKDLL_API __declspec(dllexport)
#else
#define WNSTOCKDLL_API __declspec(dllimport)
#endif

#include <vector>
#include <string>


using namespace std;



/*
*	@ StockData:获取Stock数据类
*	@ woniu201
*	@ 2017/08/03
*/

struct RealTimeStock
{
	char		strStockNum[32];
	char		strStockName[32];
	double		fOpen;
	double		fPassClose;
	double		fNow;
	double		fHigh;  
	double		fLow;
	double		fBuy;
	double		fSale;
	long long	Volume;	
	long long	Amount;

	int			iBuy1;
	double		fBuy1Price;
	int			iBuy2;
	double		fBuy2Price;
	int			iBuy3;
	double		fBuy3Price;
	int			iBuy4;
	double		fBuy4Price;
	int			iBuy5;
	double		fBuy5Price;

	int			iSale1;
	double		fSale1Price;
	int			iSale2;
	double		fSale2Price;
	int			iSale3;
	double		fSale3Price;
	int			iSale4;
	double		fSale4Price;
	int			iSale5;
	double		fSale5Price;

	char		strDate[32];
	char		strTime[32];

	double		fRatio;   
};

struct KStock 
{
	char		strStockCode[32];
	char		strDate[32];
	double		fPassClose; 
	double		fOpen;
	double		fClose;
	double		fLow;
	double		fHigh;
	long long	Volume;
	long long	Amount;
};

struct RealTimeMarket{
	char		marketName[32];//大盘名称
	double		marketIndex;   //大盘指数
	double		marketRose;    //涨幅
	double		marketRatio;   //涨幅比例
	long long	Volume;  	 //成交量
	long long	Amount;    //成交额

};

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


class WNSTOCKDLL_API StockData
{
public:
	vector<string> ReadMystockCode();
	/************My Stock********************/
	bool GetRealTimeStockData(char* stockCode, RealTimeStock* realTimeStock);
	bool GetRealTimeMarketData(char* code, RealTimeMarket* realTimeMarket);
	/*************K线 部分*******************/       
	vector<KStock*>			vKStockData;
	vector<RealTimeStock*>	vMinStockData;
	void					SaveKData(char* stock, char* data, int i);
	bool					GetKStockData(char* stockCode);
	bool					GetKStockData1(char* stockCode);
	bool					GetMinStockData(char* stockCode);
private:
};

/*
*	@ StockTime:获取Stock交易时间
*	@ woniu201
*	@ 2017/08/08
*/
class WNSTOCKDLL_API StockTime
{
public:
	int		CaculateWeekDay(char* pDate);
	int		CaculateDifferenceDay(char* pDate1, char* pDate2);
	BOOL	bSameWeek(char* pDate1, char* pDate2);
	BOOL	bSameMonth(char* pDate1, char* pDate2);
	BOOL	bExchange();
	char*	TimeToSecond(char* time);
};



class WNSTOCKDLL_API StockDoc
{
public:

/******************** K线部分 ****************************/

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

	void GetDayKData(char* stockCode);						//获取K线历史数据，保存到"WnStockDll.h"中的vKStockData中
	void GetKPoint(double m_interKH,double m_interKW);		//获得K线的坐标点
	void GetKVolumePoint(double m_interKH, double m_interKW);//获得成交量坐标
	void GetMaPrice();										//获取均线的价格
	void GetMaPoint(double m_interKH, double m_interKW);	//获得MA5,MA10,MA20坐标
	void GetMacd();											//获得MACD值
	void GetMacdPoint(double m_interKH, double m_interKW);	//获得MACD的坐标值

/********************* 分时线部分 **************************/

	vector<RealTimeStock*>	vMinData;						//分时线的数据
	vector<POINT>			vMinPoint;						//分时线的坐标
	vector<double>			vMinAvgPrice;					//平均价格
	vector<POINT>			vMinPricePoint;					//价格坐标
	vector<POINT>			vMinAvgPricePoint;				//平均价格坐标
	vector<POINT>			vMinVolumePoint;				//成交量坐标

	void GetDayMinData(char* stockCode);					//获取分时线数据，保存到"WnStockDll.h"中的vMinStockData中
	void GetDayMinPoint(double m_interMinH, double m_interMinW);//获取分时线的坐标数据
	double					fLimitPrice;					//该价格为离中间线最远的最低价或最高价

};


#endif