#pragma once
#include "cwBasicStrategy.h"
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
	int tickBasePrice;
	int tickCurPrice;

};

