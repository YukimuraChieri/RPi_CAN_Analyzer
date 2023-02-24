#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H

#include <stdio.h>
#include <stdint.h>
#include "CAN_Services.h"
#include "UDP_Services.h"

typedef enum
{
	Wireless_Disconnect = 0,
	Wireless_Connect,
    Wireless_Default,
}NET_STATUS_E;

typedef enum
{
    Client_Start = 0,
    Client_Stop,
    Client_Default,
}CLI_STATUS_E;


void State_Machine(void);

#endif

