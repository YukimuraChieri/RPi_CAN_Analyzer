#include "StateMachine.h"


void NetConnectControl(void)
{
	static NET_STATUS_E currentState = NetState_Disconnect;
	static uint16_t timer_u16;

	switch(currentState)
	{
		case NetState_Disconnect: {
			if (0)
			{
				currentState = NetState_Connect;
			}
		}
		break;
		case NetState_RampingConnect: {
		}
		break;
		case NetState_Connect: {
			if (0)
			{
				currentState = NetState_Disconnect;
			}
		}
		break;
		case NetState_RampingDisconnect: {
		}
		break;
		default: {
			currentState = NetState_Disconnect;
		}
		break;
	}
}
