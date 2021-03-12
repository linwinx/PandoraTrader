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

		//����
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

        //ƽ��
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
	//����map�����ڱ���ֲ���Ϣ 
	std::map<std::string, cwPositionPtr> CurrentPosMap;

	std::map<std::string, cwOrderPtr>::iterator WaitOrderIt;
	//����map�����ڱ���ҵ���Ϣ 
	std::map<std::string, cwOrderPtr> WaitOrderList;
	//��ȡ�ҵ���  ��ǰ�ֲ���Ϣ
	GetPositionsAndActiveOrders(CurrentPosMap, WaitOrderList);

	//�ҳ���ǰ��Լ�ĳֲ�
	std::map<std::string, cwPositionPtr>::iterator PosIt; //�ֲ�map�ĵ�����
	PosIt = CurrentPosMap.find(pPriceData->InstrumentID);
	if (PosIt != CurrentPosMap.end())
	{
		int iPosLong = PosIt->second->GetLongTotalPosition(); //��ȡ�������
		int iPosShort = PosIt->second->GetShortTotalPosition();//��ȡ�ղ�����
		if (iPosLong > 0 || iPosShort > 0)
		{
			//�в�
			std::map<std::string, cwOrderPtr>::iterator WaitOrderIt;
			//����map�����ڱ���ҵ���Ϣ 
			std::map<std::string, cwOrderPtr> WaitOrderList;
			//��ȡ�ҵ���Ϣ
			GetActiveOrders(WaitOrderList);

			bool bHasWaitOrder = false;
			for (WaitOrderIt = WaitOrderList.begin();
				WaitOrderIt != WaitOrderList.end(); WaitOrderIt++)
			{
				if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID)
				{
					//��ʶ�йҵ�
					bHasWaitOrder = true;

					//�ҵ��۸�������¼�+2������
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
			//����map�����ڱ���ҵ���Ϣ 
			std::map<std::string, cwOrderPtr> WaitOrderList;
			//��ȡ�ҵ���Ϣ
			GetActiveOrders(WaitOrderList);

			//������û�йҵ�
			bool bHasWaitOrder = false;
			for (WaitOrderIt = WaitOrderList.begin();
				WaitOrderIt != WaitOrderList.end(); WaitOrderIt++)
			{
				if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID)
				{
					//�йҵ�
					bHasWaitOrder = true;

					//�ҵ��۸�С�����¼�-2������
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
		//û�ҵ��ֲ���Ϣ

		std::map<std::string, cwOrderPtr>::iterator WaitOrderIt;
		//����map�����ڱ���ҵ���Ϣ 
		std::map<std::string, cwOrderPtr> WaitOrderList;
		//��ȡ�ҵ���Ϣ
		GetActiveOrders(WaitOrderList);

		//������û�йҵ�
		bool bHasWaitOrder = false;
		for (WaitOrderIt = WaitOrderList.begin(); WaitOrderIt != WaitOrderList.end(); WaitOrderIt++)
		{
			if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID)
			{
				bHasWaitOrder = true;

				//�ҵ��۸�С�����¼�-2������
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