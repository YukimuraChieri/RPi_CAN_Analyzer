#include "CANService.h"

void* CAN_loop_func(void* arg);

int can_fd[CAN_CH_NUM];
struct sockaddr_can can_addr[CAN_CH_NUM];
struct can_frame can_rx_frame[CAN_CH_NUM];
struct ifreq ifr[CAN_CH_NUM];
uint8_t can_rx_flag[CAN_CH_NUM]; 
pthread_t can_rx_thrd[CAN_CH_NUM];
pthread_rwlock_t rwlock[CAN_CH_NUM];

int CAN_Init(CAN_CHANNEL_E can_ch)
{
	int res;

	res = CAN_SetMode(can_ch, 0);
	if (-1 == res)
	{
		printf("CAN%d Down Failed.\r\n", can_ch);
		return -1;
	}
	res = CAN_SetBaudrate(can_ch, 500000);
	if (-1 == res)
	{
		printf("CAN%d Baudrate Setting Failed.\r\n", can_ch);
		return -1;
	}
	res = CAN_SetMode(can_ch, 1);
	if (-1 == res)
	{
		printf("CAN%d Up Failed.\r\n", can_ch);
		return -1;
	}

	can_fd[can_ch] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (-1 == can_fd[can_ch])
	{
		printf("Create CAN%d socket error: %s (errno: %d)\r\n", can_ch, strerror(errno), errno);
		return -1;
	}

	char can_name[8] = {0};
	struct hwtstamp_config hwconfig;

	sprintf(can_name, "can%d", can_ch);	
	strcpy(ifr[can_ch].ifr_name, can_name);
	ifr[can_ch].tx_type = HWTSTAMP_TX_ON;
	ifr[can_ch].rx_filter = HWTSTAMP_FILTER_ALL;
	ifr[can_ch].ifr_data = (void*)&hwconfig;

	// ioctl(can_fd[can_ch], SIOCGIFINDEX, &ifr[can_ch]);
	ioctl(can_fd[can_ch], SIOCSHWTSTAMP, &ifr[can_ch]);
	can_addr[can_ch].can_family = AF_CAN;
	can_addr[can_ch].can_ifindex = ifr[can_ch].ifr_ifindex;
	
	bind(can_fd[can_ch], (struct sockaddr *)&can_addr[can_ch], sizeof(can_addr[can_ch]));

	res = pthread_create(&can_rx_thrd[can_ch], NULL, CAN_loop_func, (void*)can_ch);
	if (0 != res)
	{
		printf("Create CAN%d Rx thread error: %s (errno: %d)\r\n", can_ch, strerror(errno), errno);
		return -1;
	}
	printf("Create CAN%d Rx thread success\r\n", can_ch);

	res = pthread_rwlock_init(&rwlock[can_ch], NULL);
	if (0 != res)
	{
		printf("Create CAN%d Rx rwlock error: %s (errno: %d)\r\n", can_ch, strerror(errno), errno);
		return -1;
	}
	printf("Create CAN%d Rx rwlock success\r\n", can_ch);

	return 0;
}

int CAN0_DeInit(CAN_CHANNEL_E can_ch)
{
	CAN_SetMode(can_ch, 0);
	close(can_fd[can_ch]);
	pthread_rwlock_destroy(&rwlock[can_ch]);
	return 0;
}

int CAN_SetBaudrate(CAN_CHANNEL_E can_ch, int baud)
{
	char cmd_str[64];
	sprintf(cmd_str, "ip link set can%d type can bitrate %d", can_ch, baud);
	return system(cmd_str); 
}

int CAN_SetMode(CAN_CHANNEL_E can_ch, uint8_t mode)
{
	char cmd_str[64];
	switch (mode)
	{
		case 0: sprintf(cmd_str, "ifconfig can%d down", can_ch); break;
		case 1: sprintf(cmd_str, "ifconfig can%d up", can_ch); break;
	}
	return system(cmd_str);
}

int CAN_SendPacket(CAN_CHANNEL_E can_ch, uint32_t can_id, uint8_t can_dlc, uint8_t* data)
{
	uint8_t i;
	int nbytes;
	struct can_frame can_tx_frame;

	memset(&can_tx_frame, 0x00, sizeof(struct can_frame));
	can_tx_frame.can_id = can_id;
	can_tx_frame.can_dlc = can_dlc;
	memcpy(can_tx_frame.data, data, can_dlc);

	nbytes = write(can_fd[can_ch], &can_tx_frame, sizeof(struct can_frame));
	if (nbytes != sizeof(can_tx_frame))
	{
		printf("Send Error CAN%d frame\r\n", can_ch);
		return -1;
	}
	printf("%s\ttx\t%03X\t[%d]\t", ifr[can_ch].ifr_name, can_tx_frame.can_id, can_tx_frame.can_dlc);
	for (i = 0; i < can_tx_frame.can_dlc; i++)
	{
		printf("%02X ", can_tx_frame.data[i]);
	}
	printf("\r\n");
	return 0;
}

int CAN_RecvPacket(CAN_CHANNEL_E can_ch, uint32_t* can_id, uint8_t* can_dlc, uint8_t* data)
{
	pthread_rwlock_rdlock(&rwlock[can_ch]);

	can_rx_flag[can_ch] = 0;
	*can_id = can_rx_frame[can_ch].can_id;
	*can_dlc = can_rx_frame[can_ch].can_dlc;
	memcpy(data, can_rx_frame[can_ch].data, *can_dlc);

	pthread_rwlock_unlock(&rwlock[can_ch]);
	return 0;
}

int CAN_GetStatus(CAN_CHANNEL_E can_ch)
{
	if (1 == can_rx_flag[can_ch])
	{
		return 1;
	}
	return 0;
}

void* CAN_loop_func(void* arg)
{
	CAN_CHANNEL_E can_ch = (CAN_CHANNEL_E)arg;
	socklen_t len;
	uint8_t i;

	printf("CAN%d Receive Thread is startting\r\n", can_ch);

	while(1)
	{
		memset(&can_rx_frame[can_ch], 0x00, sizeof(struct can_frame));
		recvfrom(can_fd[can_ch], &can_rx_frame[can_ch], sizeof(struct can_frame), 0, (struct sockaddr *)&can_addr[can_ch], &len);
		pthread_rwlock_rdlock(&rwlock[can_ch]);
		can_rx_flag[can_ch] = 1;
		pthread_rwlock_unlock(&rwlock[can_ch]);
		printf("%s\trx\t%03X\t[%d]\t", ifr[can_ch].ifr_name, can_rx_frame[can_ch].can_id, can_rx_frame[can_ch].can_dlc);
		for (i = 0; i < can_rx_frame[can_ch].can_dlc; i++)
		{
			printf("%02X ", can_rx_frame[can_ch].data[i]);
		}
		printf("\r\n");
	}
	pthread_exit(NULL);
}



