#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H

#include <stdio.h>
#include <stdint.h>

typedef enum
{
	NetState_Disconnect = 0,
	NetState_RampingConnect,
	NetState_Connect,
	NetState_RampingDisconnect,
}NET_STATUS_E;

void NetConnectControl(void);

#endif

