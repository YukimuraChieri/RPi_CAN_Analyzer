#include "StateMachine.h"


void State_Machine(void)
{
	static NET_STATUS_E wirelessState = Wireless_Default;
    static CLI_STATUS_E clientState = Client_Default;
	static uint16_t timer_u16;

	switch(wirelessState)
	{
		case Wireless_Disconnect: {
			if (0 == Get_Wireless_If_Status("wlan0"))
			{
				wirelessState = Wireless_Connect;
			}
		}
		break;

		case Wireless_Connect: {
			if (0 != Get_Wireless_If_Status("wlan0"))
			{
				wirelessState = Wireless_Disconnect;
                UDP_DeInit();
			}
            switch(clientState)
            {
                case Client_Start: {
                    if (0)
                    {
                        clientState = Client_Stop;
                        CAN_DeInit(CAN0_CH);
                        CAN_DeInit(CAN1_CH);
	                    CAN_RxBuff_DeInit();
	                    CAN_TxBuff_DeInit();
                    }
                }
                break;

                case Client_Stop: {
                    if (0)
                    {
                        clientState = Client_Start;
	                    CAN_RxBuff_Init();
	                    CAN_TxBuff_Init();
                        CAN_Init(CAN0_CH);
                        CAN_Init(CAN1_CH);
	                    Reset_Timestamp();
                    }
                }
                break;

                default: {
                    clientState = Client_Stop;
                }
                break;
            }
		}
		break;
		
        default: {
            if (0 == Get_Wireless_If_Status("wlan0"))
            {
			    wirelessState = Wireless_Connect;
                UDP_Init();
            }
            else
            {
			    wirelessState = Wireless_Disconnect;
            }
		}
		break;
	}
}
