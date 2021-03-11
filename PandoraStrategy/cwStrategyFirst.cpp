#include "cwStrategyFirst.h"

#define ACCTION_TICKS  10 //�۸��жϱ䶯��ticks��
#define QUICK_POINTS 6 //ACCTION_TICKS��tick�ڼ۸�䶯6����

cwStrategyFirst::cwStrategyFirst()
{
	tickCnt = 0;
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

	if (tickCnt == 0)
	{
		tickBasePrice = pPriceData->ClosePrice;
	}
	
	tickCnt += 1;

	if (tickCnt == ACCTION_TICKS)
	{
		tickCurPrice = pPriceData->ClosePrice;

		if (tickCurPrice >= tickBasePrice + QUICK_POINTS)
		{
			EasyInputOrder(pPriceData->InstrumentID, 1, pPriceData->BidPrice1);
		}

		if (tickCurPrice <= tickBasePrice + QUICK_POINTS)
		{
			EasyInputOrder(pPriceData->InstrumentID, -1, pPriceData->AskPrice1);
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
		int iPos = PosIt->second->GetLongTotalPosition(); //��ȡ�������
		if (iPos > 0)
		{
			//�ж��
			bool bHasWaitOrder = false;
			//������йҵ�
			for (WaitOrderIt = WaitOrderList.begin(); WaitOrderIt != WaitOrderList.end(); WaitOrderIt++)
			{
				//ȷ������ҵ��������Լ��
				if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID)
				{
					//�൥��ȥ
					if (WaitOrderIt->second->Direction == CW_FTDC_D_Buy)
					{
						CancelOrder(WaitOrderIt->second);
					}
					else
					{
						//��ʶ�йҵ�
						bHasWaitOrder = true;

						//�ҵ��۸�������¼�+2������
						if (WaitOrderIt->second->LimitPrice > pPriceData->LastPrice + 2)
						{
							CancelOrder(WaitOrderIt->second);
						}
					}
				}
			}
			//û�йҵ����͹ҵ�ƽ��
			if (!bHasWaitOrder)
			{
				//��ί���޼۵���
				EasyInputOrder(pPriceData->InstrumentID, -1 * iPos, pPriceData->BidPrice1);
			}
		}


		iPos = PosIt->second->GetShortTotalPosition();//��ȡ�ղ�����
		if (iPos > 0)
		{
			//�пղ�
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
					if (WaitOrderIt->second->Direction == CW_FTDC_D_Sell)
					{
						CancelOrder(WaitOrderIt->second);
					}
					else
					{
						//�йҵ�
						bHasWaitOrder = true;

						//�ҵ��۸�С�����¼�-2������
						if (WaitOrderIt->second->LimitPrice < pPriceData->LastPrice - 2)
						{
							CancelOrder(WaitOrderIt->second);
						}
					}
				}
			}

			//û�йҵ����͹ҵ�ƽ��
			if (!bHasWaitOrder)
			{
				EasyInputOrder(pPriceData->InstrumentID, iPos, pPriceData->AskPrice1);
			}
		}
		if (PosIt->second->GetLongTotalPosition() + PosIt->second->GetShortTotalPosition() == 0)
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
					if (WaitOrderIt->second->LimitPrice < pPriceData->LastPrice - 2)
					{
						CancelOrder(WaitOrderIt->second);
					}
				}
			}

			//û�йҵ��ͱ�ί�е�������࣬�۸���һ��
			if (!bHasWaitOrder)
			{
				EasyInputOrder(pPriceData->InstrumentID, 1, pPriceData->BidPrice1);
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
			if ((std::string)pPriceData->InstrumentID == (std::string)WaitOrderIt->second->InstrumentID
				&& (WaitOrderIt->second->Direction == CW_FTDC_D_Buy))
			{
				bHasWaitOrder = true;

				//�ҵ��۸�С�����¼�-2������
				if (WaitOrderIt->second->LimitPrice < pPriceData->LastPrice - 2)
				{
					CancelOrder(WaitOrderIt->second);
				}
			}
		}

		//û�йҵ��ͱ�ί�е�������࣬�۸���һ��
		if (!bHasWaitOrder)
		{
			EasyInputOrder(pPriceData->InstrumentID, 1, pPriceData->BidPrice1);
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
