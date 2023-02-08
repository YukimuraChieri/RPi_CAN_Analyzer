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


#define msleep(tm) usleep(1000*tm)

uint8_t send_buff[8] = {0};

int main(int argc, char const *argv[])
{
	struct timeval start_tv, now_tv;
	long long delta_ms = 0;

	// UDP_Init();
	CAN_Init(CAN0_CH);
	CAN_Init(CAN1_CH);

	printf("Link Start!\r\n");
	gettimeofday(&start_tv, NULL);

	while(1)
	{
		gettimeofday(&now_tv, NULL);
		delta_ms = (now_tv.tv_sec - start_tv.tv_sec)*1000 + (now_tv.tv_usec - start_tv.tv_usec)/1000;
		printf("time stamp: %lldms\r\n", delta_ms);
		memcpy(send_buff, (uint8_t *)&delta_ms, 8);
		CAN_SendPacket(CAN0_CH, 0x201, 8, send_buff);
		msleep(1000);
	}

	return 0;
}



