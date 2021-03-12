#include "cwStrategyFirst.h"


cwStrategyFirst::cwStrategyFirst()
{
	tickCnt = 0;
	isLongOrShort = 0;
	tickPipeIndex = 0;
	isCanStart = false;
}


cwStrategyFirst::~cwStrategyFirst()
{
}


std::string  cwStrategyFirst::GetStrategyName()
{
	return "FirstStrategy"; 
}

void cwStrategyFirst::PriceUpdate(cwMarketDataPtr pPriceData)
{
	if (pPriceData.get() == NULL)
	{
		return;
	}
	m_strCurrentUpdateTime = pPriceData->UpdateTime;
	tickCurPrice = pPriceData->ClosePrice;
	FillPipePrice(tickCurPrice);

	if (!isCanStart)
	{
		tickCnt += 1;
		if (tickCnt > ACCTION_TICKS + 12)
		{
			isCanStart = true;
			tickCnt = 0;
		}
	}
	else
	{
		tickStartPrice = tickPipePrice[0];
		tickEndPrice = tickPipePrice[ACCTION_TICKS - 1];

		//开仓
		if (tickEndPrice >= tickStartPrice + QUICK_POINTS && isLongOrShort == 0)
		{
			EasyInputOrder(pPriceData->InstrumentID, 1, pPriceData->BidPrice1);
			isLongOrShort = 1;
		}

		if (tickEndPrice <= tickStartPrice - QUICK_POINTS && isLongOrShort == 0)
		{
			EasyInputOrder(pPriceData->InstrumentID, -1, pPriceData->AskPrice1);
			isLongOrShort = -1;
		}

        //平仓
		if (isLongOrShort == 1 && (tickCurPrice > tickEndPrice + PROFIT_POINTS || tickCurPrice < tickEndPrice - LOSS_POINTS))
		{
			EasyInputOrder(pPriceData->InstrumentID, -1, pPriceData->AskPrice1);
			isLongOrShort = 0;
		}

		if (isLongOrShort == -1 && (tickCurPrice > tickEndPrice + LOSS_POINTS || tickCurPrice < tickEndPrice - PROFIT_POINTS))
		{
			EasyInputOrder(pPriceData->InstrumentID, 1, pPriceData->AskPrice1);
			isLongOrShort = 0;
		}

	}

	////////////////////////////////////////////////////////
	//定义map，用于保存持仓信息 
	std::map<std::string, cwPositionPtr> CurrentPosMap;

	std::map<std::string, cwOrderPtr>::iterator WaitOrderIt;
	//定义map，用于保存挂单信息 
	std::map<std::string, cwOrderPtr> WaitOrderList;
	//获取挂单信  当前持仓信息
	GetPositionsAndActiveOrders(CurrentPosMap, WaitOrderList);

	//找出当前合约的持仓
	std::map<std::string, cwPositionPtr>::iterator PosIt; //持仓map的迭代器
	PosIt = CurrentPosMap.find(pPriceData->InstrumentID);
	if (PosIt != CurrentPosMap.end())
	{
		int iPosLong = PosIt->second->GetLongTotalPosition(); //获取多仓数量
		int iPosShort = PosIt->second->GetShortTotalPosition();//获取空仓数量
		if (iPosLong > 0 || iPosShort > 0)
		{
			//有仓
			std::map<std::string, cwOrderPtr>::iterator WaitOrderIt;
			//定义map，用于保存挂单信息 
			std::map<std::string, cwOrderPtr> WaitOrderList;
			//获取挂单信息
			GetActiveOrders(WaitOrderList);

			bool bHasWaitOrder = false;
			for (WaitOrderIt = WaitOrderList.begin();
				WaitOrderIt != WaitOrderList.end(); WaitOrderIt++)
			{
				if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID)
				{
					//标识有挂单
					bHasWaitOrder = true;

					//挂单价格大于最新价+2，撤单
					if (fabs(WaitOrderIt->second->LimitPrice - pPriceData->LastPrice) > 4.0)
					{
						CancelOrder(WaitOrderIt->second);
					}
				}
			}
		}
		else if (PosIt->second->GetLongTotalPosition() + PosIt->second->GetShortTotalPosition() == 0)
		{

			std::map<std::string, cwOrderPtr>::iterator WaitOrderIt;
			//定义map，用于保存挂单信息 
			std::map<std::string, cwOrderPtr> WaitOrderList;
			//获取挂单信息
			GetActiveOrders(WaitOrderList);

			//看看有没有挂单
			bool bHasWaitOrder = false;
			for (WaitOrderIt = WaitOrderList.begin();
				WaitOrderIt != WaitOrderList.end(); WaitOrderIt++)
			{
				if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID)
				{
					//有挂单
					bHasWaitOrder = true;

					//挂单价格小于最新价-2，撤单
					if (fabs(WaitOrderIt->second->LimitPrice - pPriceData->LastPrice) > 4.0)
					{
						CancelOrder(WaitOrderIt->second);
					}
				}
			}
		}
	}
	else
	{
		//没找到持仓信息

		std::map<std::string, cwOrderPtr>::iterator WaitOrderIt;
		//定义map，用于保存挂单信息 
		std::map<std::string, cwOrderPtr> WaitOrderList;
		//获取挂单信息
		GetActiveOrders(WaitOrderList);

		//看看有没有挂单
		bool bHasWaitOrder = false;
		for (WaitOrderIt = WaitOrderList.begin(); WaitOrderIt != WaitOrderList.end(); WaitOrderIt++)
		{
			if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID)
			{
				bHasWaitOrder = true;

				//挂单价格小于最新价-2，撤单
				if (fabs(WaitOrderIt->second->LimitPrice - pPriceData->LastPrice) > 4.0)
				{
					CancelOrder(WaitOrderIt->second);
				}
			}
		}
	}



}

void cwStrategyFirst::OnRtnTrade(cwTradePtr pTrade)
{
}

void cwStrategyFirst::OnRtnOrder(cwOrderPtr pOrder, cwOrderPtr pOriginOrder)
{
}

void cwStrategyFirst::OnOrderCanceled(cwOrderPtr pOrder)
{
}


void cwStrategyFirst::FillPipePrice(int price)
{
	tickPipePrice[tickPipeIndex++] = price;
	if (tickPipeIndex == ACCTION_TICKS)
	{
		tickPipeIndex = 0;
	}
}