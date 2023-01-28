#include "CANService.h"

void* CAN0_loop_func(void* arg);
void* CAN1_loop_func(void* arg);


int can_fd[CAN_CH_NUM];
struct sockaddr_can can_addr[CAN_CH_NUM];
struct can_frame can_rx_frame[CAN_CH_NUM];
struct ifreq ifr[CAN_CH_NUM];
bool can_rx_flag[CAN_CH_NUM]; 

int CAN_Init(CAN_CHANNEL_E can_ch)
{
	switch (can_ch)
	{
		case CAN0_CH: {
				      CAN_DOWN(can0); 
				      SetBaud(can0, 1000000);
				      CAN_UP(can0);
				      break;
			      }
		case CAN1_CH: {
				      CAN_DOWN(can1);
				      SetBaud(can1, 1000000);
				      CAN_UP(can1);
				      break;
			      }
		default:
	}

	can_fd[can_ch] = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	if (-1 == can_fd)
	{
		printf("Create CAN%d socket error: %s (errno: %d)\r\n", can_ch, strerror(errno), errno);
		return -1;
	}

	char can_name[8] = {0};
	sprintf(can_name, "can%d", can_ch);	
	strcpy(ifr[can_ch].ifr_name, can_name);

	ioctl(can_fd[can_ch], SIOCGIFINDEX, &ifr[can_ch]);
	can_addr[can_ch].can_family = AF_CAN;
	can_addr[can_ch].can_ifindex = ifr[can_ch].ifr_ifindex;
	
	bind(can_fd[can_ch], (struct sockaddr *)&can_addr[can_ch], sizeof(can_addr[can_ch]));

	pthread_t can_rx_thrd;
	int res;

	res = pthread_create(&can_rx_thrd, NULL, CAN0_loop_func, (void*)0);
	if (0 != res)
	{
		printf("Create CAN%d Rx thread error: %s (errno: %d)\r\n", can_ch, strerror(errno), errno);
		return -1;
	}
	printf("Create CAN%d Rx thread success\r\n", can_ch);

	return 0;
}


void CAN0_DeInit(CAN_CHANNEL_E can_ch)
{
	switch (can_ch)
	{
	CAN_DOWN(can0);
	close(can0_fd);
	}
}

void CAN_SetBaudrate(int baud)
{
}

void CAN0_SendPacket(uint32_t can_id, uint8_t can_dlc, uint8_t* data)
{
	int nbytes;
	can_frame can0_tx_frame;

	memset(&can0_tx_frame, 0x00, sizeof(can0_tx_frame));
	can0_tx_frame.can_id = can_id;
	can0_tx_frame.can_dlc = can_dlc;
	memcpy(can0_tx_frame.data, data, can_dlc);

	nbytes = write(can0_fd, &can0_tx_frame, sizeof(can0_tx_frame));
	if (nbytes != sizeof(can0_tx_frame))
	{
		printf("Send Error CAN0 frame\r\n");
	}
}

void CAN1_SendPacket(uint32_t can_id, uint8_t can_dlc, uint8_t* data)
{
	int nbytes;
	can_frame can1_tx_frame;

	memset(&can1_tx_frame, 0x00, sizeof(can1_tx_frame));
	can1_tx_frame.can_id = can_id;
	can1_tx_frame.can_dlc = can_dlc;
	memcpy(can1_tx_frame.data, data, can_dlc);

	nbytes = write(can1_fd, &can1_rx_frame, sizeof(can1_tx_frame));
	if (nbytes != sizeof(can1_tx_frame))
	{
		printf("Send Error CAN1 frame\r\n");
	}
}

int CAN0_RecvPacket(uint32_t* can_id, uint8_t* can_dlc, uint8_t* data)
{
	if (true == can0_rx_flag)
	{
		can0_rx_flag = false;
		*can_id = can0_rx_frame.can_id;
		*can_dlc = can0_rx_frame.can_dlc;
		memcpy(data, can0_rx_frame.data, *can_dlc);
		return 1;
	}
	return 0;
}

int CAN1_RecvPacket(uint32_t* can_id, uint8_t* can_dlc, uint8_t* data)
{
	if (true == can1_rx_flag)
	{
		can1_rx_flag = false;
		*can_id = can1_rx_frame.can_id;
		*can_dlc = can1_rx_frame.can_dlc;
		memcpy(data, can1_rx_frame.data, *can_dlc);
		return 1;
	}
	return 0;
}

void* CAN0_loop_func(void* arg)
{
	socklen_t len;

	printf("CAN0 Receive Thread is startting\r\n");

	while(1)
	{
		memset(&can0_rx_frame, 0x00, sizeof(can0_rx_frame));
		recvfrom(can0_fd, &can0_rx_frame, sizeof(can0_rx_frame), 0, (struct sockaddr *)&can0_addr, &len);
		can0_rx_flag = true;
		printf("%s\t%03X\t[%d]\t%02X %02X %02X %02X %02X %02X %02X %02X\r\n", ifr0.ifr_name, can0_rx_frame.can_id, can0_rx_frame.can_dlc, can0_rx_frame.data[0], can0_rx_frame.data[1], can0_rx_frame.data[2], can0_rx_frame.data[3], can0_rx_frame.data[4], can0_rx_frame.data[5], can0_rx_frame.data[6], can0_rx_frame.data[7]);
	}
	pthread_exit(NULL);
}

void* CAN1_loop_func(void* arg)
{
	socklen_t len;

	printf("CAN1 Receive Thread is startting\r\n");

	while(1)
	{
		memset(&can1_rx_frame, 0x00, sizeof(can1_rx_frame));
		recvfrom(can1_fd, &can1_rx_frame, sizeof(can1_rx_frame), 0, (struct sockaddr *)&can1_addr, &len);
		can1_rx_flag = true;
		printf("%s\t%03X\t[%d]\t%02X %02X %02X %02X %02X %02X %02X %02X\r\n", ifr1.ifr_name, can1_rx_frame.can_id, can1_rx_frame.can_dlc, can1_rx_frame.data[0], can1_rx_frame.data[1], can1_rx_frame.data[2], can1_rx_frame.data[3], can1_rx_frame.data[4], can1_rx_frame.data[5], can1_rx_frame.data[6], can1_rx_frame.data[7]);
	}
	pthread_exit(NULL);
}


