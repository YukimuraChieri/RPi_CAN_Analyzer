#include "Timer.h"

struct timeval start_time;

void Reset_Timestamp(void)
{
	gettimeofday(&start_time, NULL);
}

uint32_t Get_Timestamp(void)
{
	uint32_t timestamp = 0;
	struct timeval current_time;

	gettimeofday(&current_time, NULL);
	timestamp = (current_time.tv_sec - start_time.tv_sec)*1000 + (current_time.tv_usec - start_time.tv_usec)/1000;

	return timestamp;
}


int Task10ms_Init(void)
{
	struct itimerval it;
	int res;
	
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 500000;
	it.it_interval = it.it_value;

	res = setitimer(ITIMER_REAL, &it, NULL);
	
	if (-1 == res)
	{
		perror("setitimer");
		return -1;
	}

	if (SIG_ERR == signal(SIGALRM, Task10ms))
	{
		perror("signal");
		return -1;
	}

	return 0;
}

uint8_t tx_datagram[4096] = {0};

void Task10ms(int sig)
{
	uint16_t can_frame_num = GetBuffLength();
	uint16_t i, index = 4, crc;
	CAN_FRAME_T can_frame_info;

	// head
	tx_datagram[0] = 0xA5;
	tx_datagram[1] = 0xC3;
	// number of can frames
	memcpy(&tx_datagram[2], &can_frame_num, 2);

	for (i = 0 ; i < can_frame_num; i++)
	{
		CAN_Read_Buff(&can_frame_info);

		memcpy(&tx_datagram[index], &can_frame_info.timestamp, 4);
		index += 4;
		tx_datagram[index++] = can_frame_info.can_ch;

		memcpy(&tx_datagram[index], &can_frame_info.frame.can_id, 4);
		index += 4;

		tx_datagram[index++] = can_frame_info.frame.can_dlc;

		memcpy(&tx_datagram[index], &can_frame_info.frame.data, 8);
		index += 8;

	}

	// CRC
	memcpy(&tx_datagram[index], &crc, 2);
	index += 2;
	// tail
	tx_datagram[index++] = 0x5A;
	tx_datagram[index++] = 0x3C;

	UDP_SendPacket(tx_datagram, index);
}
