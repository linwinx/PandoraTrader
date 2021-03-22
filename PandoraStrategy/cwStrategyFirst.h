#pragma once
#include "cwBasicStrategy.h"

#define ACCTION_TICKS  10 //�۸��жϱ䶯��ticks��
#define QUICK_POINTS 6 //ACCTION_TICKS��tick�ڼ۸�䶯6����
#define PROFIT_POINTS 4 //ֹӯ����
#define LOSS_POINTS 4 //ֹ�����


class cwStrategyFirst :
	public cwBasicStrategy
{
public:
	cwStrategyFirst();
	~cwStrategyFirst();

	//���ò�������
	virtual std::string  GetStrategyName();

	//MarketData SPI
	///�������
	virtual void PriceUpdate(cwMarketDataPtr pPriceData);

	//Trade SPI
	///�ɽ��ر�
	virtual void OnRtnTrade(cwTradePtr pTrade);
	//�����ر�
	virtual void OnRtnOrder(cwOrderPtr pOrder, cwOrderPtr pOriginOrder = cwOrderPtr());
	//�����ɹ�
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

