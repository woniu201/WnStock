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
#include "include/sqlite3.h"

using namespace std;

/***************自定义类*******************/
/*
*	@ SqliteClient:操作sqlite类	
*	@ woniu201
*	@ 2017/08/03
*/
class WNSTOCKDLL_API SqliteClient{

public:
	SqliteClient();
	~SqliteClient();

	int SqliteClient_Open(char* dbName);
	int SqliteClient_Insert(char* sql1, char* sql2);
	int SqliteClient_Select(char* sql, int& nrow, int& ncolumn, char**& data);
	int SqliteClient_Update(char* sql1, char* sql2);
	int SqliteClient_Delete(char* sql);

private:
	sqlite3* db;
	char*	errMsg;	
};

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

class WNSTOCKDLL_API StockData
{
public:
	vector<string> ReadMystockCode();
	/************My Stock********************/
	bool GetRealTimeStockData(char* stockCode, RealTimeStock* realTimeStock);
	bool GetRealTimeMarketData(char* code, RealTimeMarket* realTimeMarket);
	/*************K线 部分*******************/       
	vector<KStock*>		vKStockData;
	void				SaveKData(char* stock, char* data, int i);
	bool				GetKStockData(char* stockCode);
	bool				GetKStockData1(char* stockCode);
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
};
#endif