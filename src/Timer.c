#include "Timer.h"

struct timeval start_time;
pthread_t task_10ms_thrd;

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

void* Task_Create_thrd(void* arg)
{
	struct itimerval* point_it;

	point_it = (struct itimerval*)arg;

	if (0 != setitimer(ITIMER_REAL, point_it, NULL))
	{
		perror("setitimer");
		pthread_exit((void*)-1);
	}

	if (SIG_ERR == signal(SIGALRM, Task10ms))
	{
		perror("signal");
		pthread_exit((void*)-1);
	}
	
	while(1)
	{
		pause();
	}
	
	pthread_exit((void*)0);
	return 0;
}


int Task10ms_Init(void)
{
	int res;
	static struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 250000;
	it.it_interval = it.it_value;
	
	res = pthread_create(&task_10ms_thrd, NULL, Task_Create_thrd, (void*)(&it));
	if (0 != res)
	{
		printf("Create task 10ms thread error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("Create task 10ms thread success\r\n");
	pthread_detach(task_10ms_thrd);

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	return 0;
}

int Task10ms_Start(void)
{
	struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 250000;
	it.it_interval = it.it_value;
	return setitimer(ITIMER_REAL, &it, NULL);
}

int Task10ms_Cancel(void)
{
	struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	it.it_interval = it.it_value;
	return setitimer(ITIMER_REAL, &it, NULL);
}

void Task10ms(int sig)
{
	static uint8_t tx_datagram[4096] = {0};
	uint16_t can_frame_num = GetBuffLength();
	uint16_t send_max_num = (sizeof(tx_datagram)-8)/16;
	uint16_t index = 4, crc = 0;
	uint16_t temp_u16;
	uint32_t temp_u32;
	CAN_FRAME_T can_frame_info;
	printf("[Task10ms]:%06d\r\n", Get_Timestamp());

	if (can_frame_num > send_max_num)
	{
		can_frame_num = send_max_num;
	}

	// head
	tx_datagram[0] = 0xA5;
	tx_datagram[1] = 0xC3;
	// number of can framesi, Little Endian Mode
	memcpy(&tx_datagram[2], &can_frame_num, 2);

	for (int n = 0 ; n < can_frame_num; n++)
	{
		CAN_Read_Buff(&can_frame_info);

		temp_u32 = htonl(can_frame_info.timestamp);
		memcpy(&tx_datagram[index], &temp_u32, 4);
		index += 4;

		tx_datagram[index++] = can_frame_info.can_ch;
		
		temp_u16 = htons(can_frame_info.frame.can_id);
		memcpy(&tx_datagram[index], &temp_u16, 2);
		index += 2;

		tx_datagram[index++] = can_frame_info.frame.can_dlc;

		memcpy(&tx_datagram[index], &can_frame_info.frame.data, 8);
		index += 8;
	}

	// CRC
	uint16_t len = can_frame_num * 16;
	crc = crc16_ibm(tx_datagram+4, len);
	memcpy(&tx_datagram[index], &crc, 2);
	index += 2;
	// tail
	tx_datagram[index++] = 0x5A;
	tx_datagram[index++] = 0x3C;

	UDP_SendPacket(tx_datagram, index);
}
