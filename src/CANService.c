#include "CANService.h"

void* CAN_loop_func(void* arg);

int can_fd[CAN_CH_NUM];
struct sockaddr_can can_addr[CAN_CH_NUM];
struct ifreq can_ifr[CAN_CH_NUM];
pthread_t can_rx_thrd[CAN_CH_NUM];
pthread_rwlock_t rwlock;

CAN_RXBUFF_T CAN_Buff;

int CAN_Init(CAN_CHANNEL_E can_ch)
{
	int res;
	static CAN_CHANNEL_E can_ch_static[CAN_CH_NUM];

	can_ch_static[can_ch] = can_ch;
	
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
	sprintf(can_name, "can%d", can_ch);	
	strcpy(can_ifr[can_ch].ifr_name, can_name);

	ioctl(can_fd[can_ch], SIOCGIFINDEX, &can_ifr[can_ch]);
	can_addr[can_ch].can_family = AF_CAN;
	can_addr[can_ch].can_ifindex = can_ifr[can_ch].ifr_ifindex;
	
	bind(can_fd[can_ch], (struct sockaddr *)&can_addr[can_ch], sizeof(can_addr[can_ch]));

	res = pthread_create(&can_rx_thrd[can_ch], NULL, CAN_loop_func, (void*)(&can_ch_static[can_ch]));
	if (0 != res)
	{
		printf("Create CAN%d Rx thread error: %s (errno: %d)\r\n", can_ch, strerror(errno), errno);
		return -1;
	}
	pthread_detach(can_rx_thrd[can_ch]);
	printf("Create CAN%d Rx thread success\r\n", can_ch);

	return 0;
}

int CAN_DeInit(CAN_CHANNEL_E can_ch)
{
	pthread_cancel(can_rx_thrd[can_ch]);
	pthread_join(can_rx_thrd[can_ch], NULL);
	CAN_SetMode(can_ch, 0);
	close(can_fd[can_ch]);
	printf("CAN%d Thread Cancel\r\n", can_ch);
	return 0;
}

int CAN_Rwlock_Init(void)
{
	int res;
	res = pthread_rwlock_init(&rwlock, NULL);
	if (0 != res)
	{
		printf("Create CAN rwlock error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("Create CAN rwlock success\r\n");

	return 0;
}

int CAN_Rwlock_DeInit(void)
{
	pthread_rwlock_destroy(&rwlock);
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

int CAN_SendPacket(CAN_CHANNEL_E can_ch, uint16_t can_id, uint8_t can_dlc, uint8_t* data)
{
	uint8_t i;
	int nbytes;
	struct can_frame can_tx_frame;
	uint32_t timestamp;

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
	timestamp = Get_Timestamp();
	printf("(%06d):\t%s\ttx\t%03X\t[%d]\t", timestamp, can_ifr[can_ch].ifr_name, can_tx_frame.can_id, can_tx_frame.can_dlc);
	for (i = 0; i < can_tx_frame.can_dlc; i++)
	{
		printf("%02X ", can_tx_frame.data[i]);
	}
	printf("\r\n");
	return 0;
}

int CAN_RecvPacket(CAN_CHANNEL_E can_ch, uint16_t* can_id, uint8_t* can_dlc, uint8_t* data)
{
	pthread_rwlock_rdlock(&rwlock);

	// Read CAN frame

	pthread_rwlock_unlock(&rwlock);
	return 0;
}

void* CAN_loop_func(void* arg)
{
	CAN_CHANNEL_E can_ch = *( (CAN_CHANNEL_E*)arg );
	CAN_FRAME_T frame_info;
	struct can_frame can_rx_frame;
	socklen_t len;
	uint8_t i;

	printf("CAN%d Receive Thread is starting\r\n", can_ch);

	while(1)
	{
		memset(&can_rx_frame, 0x00, sizeof(struct can_frame));
		recvfrom(can_fd[can_ch], &can_rx_frame, sizeof(struct can_frame), 0, (struct sockaddr *)&can_addr[can_ch], &len);

		frame_info.can_ch = can_ch;
		frame_info.timestamp = Get_Timestamp();
		frame_info.frame = can_rx_frame;

		CAN_Write_Buff(frame_info);

		printf("(%06d):\t%s\trx\t%03X\t[%d]\t",frame_info.timestamp, can_ifr[can_ch].ifr_name, can_rx_frame.can_id, can_rx_frame.can_dlc);
		for (i = 0; i < can_rx_frame.can_dlc; i++)
		{
			printf("%02X ", can_rx_frame.data[i]);
		}
		printf("\r\n");
	}
	pthread_exit((void*)0);
	return 0;
}

void CAN_Buff_Init(void)
{
	memset(&CAN_Buff, 0x00, sizeof(CAN_RXBUFF_T));
}

int CAN_Read_Buff(CAN_FRAME_T* frame)
{
	int res = 0;
	pthread_rwlock_wrlock(&rwlock);

	if (0 == CAN_Buff.len)
	{
		res = -1;
	}
	else
	{
		*frame = CAN_Buff.buff[CAN_Buff.head];
		CAN_Buff.head = (CAN_Buff.head + 1) % CAN_RX_BUFFSIZE;
		CAN_Buff.len--;
	}

	pthread_rwlock_unlock(&rwlock);

	return res;
}

int CAN_Write_Buff(CAN_FRAME_T frame)
{
	int res = 0;
	pthread_rwlock_wrlock(&rwlock);

	if (CAN_RX_BUFFSIZE == CAN_Buff.len)
	{
		res = -1;
	}
	else
	{
		CAN_Buff.buff[CAN_Buff.tail] = frame;
		CAN_Buff.tail = (CAN_Buff.tail + 1) % CAN_RX_BUFFSIZE;
		CAN_Buff.len++;
	}

	pthread_rwlock_unlock(&rwlock);

	return res;
}

uint16_t GetBuffLength(void)
{
	uint16_t len = 0;
	pthread_rwlock_rdlock(&rwlock);
	len = CAN_Buff.len;
	pthread_rwlock_unlock(&rwlock);
	return len;
}
