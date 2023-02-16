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
#include "Timer.h"


#define msleep(tm) usleep(1000*tm)

int main(int argc, char const *argv[])
{
	uint8_t send_buff[8] = {0};
	uint8_t i;

	CAN_Rwlock_Init();
	CAN_Buff_Init();
	CAN_Init(CAN0_CH);
	CAN_Init(CAN1_CH);

	UDP_Init();
	
	printf("Link Start!\r\n");

	Reset_Timestamp();

	Task10ms_Init();

	while(1)
	{
		send_buff[0]++;
		for (i = 1; i < 8; i++)
		{
			send_buff[i] = send_buff[0] << i;
		}
		CAN_SendPacket(CAN1_CH, 0x201, 8, send_buff);
		printf("[main loop]:%ld\r\n", time(NULL));
		msleep(3000);
	}

	return 0;
}



