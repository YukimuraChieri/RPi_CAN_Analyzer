#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "UDPService.h"
#include "CANService.h"
#include "timestamp.h"


#define msleep(tm) usleep(1000*tm)

uint8_t send_buff[8] = {0};

int main(int argc, char const *argv[])
{
	uint32_t timestamp = 0;

	// UDP_Init();
	CAN_Init(CAN0_CH);
	CAN_Init(CAN1_CH);

	printf("Link Start!\r\n");

	reset_timestamp();

	while(1)
	{
		timestamp = get_timestamp();
		printf("time stamp: %dms\r\n", timestamp);
		memcpy(send_buff, (uint8_t *)&timestamp, 4);
		CAN_SendPacket(CAN0_CH, 0x201, 8, send_buff);
		msleep(1000);
	}

	return 0;
}



