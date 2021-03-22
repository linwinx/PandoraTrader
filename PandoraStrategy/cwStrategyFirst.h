#pragma once
#include "cwBasicStrategy.h"

#define ACCTION_TICKS  10 //价格判断变动的ticks数
#define QUICK_POINTS 6 //ACCTION_TICKS个tick内价格变动6个点
#define PROFIT_POINTS 4 //止盈点数
#define LOSS_POINTS 4 //止损点数


class cwStrategyFirst :
	public cwBasicStrategy
{
public:
	cwStrategyFirst();
	~cwStrategyFirst();

	//设置策略名称
	virtual std::string  GetStrategyName();

	//MarketData SPI
	///行情更新
	virtual void PriceUpdate(cwMarketDataPtr pPriceData);

	//Trade SPI
	///成交回报
	virtual void OnRtnTrade(cwTradePtr pTrade);
	//报单回报
	virtual void OnRtnOrder(cwOrderPtr pOrder, cwOrderPtr pOriginOrder = cwOrderPtr());
	//撤单成功
	virtual void OnOrderCanceled(cwOrderPtr pOrder);

	std::string m_strCurrentUpdateTime;
	int tickCnt;
	std::string tickPipePriceTime[ACCTION_TICKS];
	int tickPipePrice[ACCTION_TICKS];
	int tickPipeIndex;
	bool FillPipePrice(int price, std::string upTime);

	int tickStartPrice;
	std::string tickStartPriceTime;
	int tickEndPrice;
	std::string tickEndPriceTime;

	int tickCurPrice;

	bool isCanStart;

	int isLongOrShort;

};

