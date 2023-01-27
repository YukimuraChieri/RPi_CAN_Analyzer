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

int main(int argc, char const *argv[])
{
	struct timeval start_tv, now_tv;
	long long delta_ms = 0;

	// UDP_Init();

	printf("Link Start!\r\n");
	gettimeofday(&start_tv, NULL);

	while(1)
	{
		gettimeofday(&now_tv, NULL);
		delta_ms = (now_tv.tv_sec - start_tv.tv_sec)*1000 + (now_tv.tv_usec - start_tv.tv_usec)/1000;
		printf("microsecond: %lldms\r\n", delta_ms);
		msleep(1000);
	}

	return 0;
}



