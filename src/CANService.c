#include "CANService.h"

void* CAN0_loop_func(void* arg);
void* CAN1_loop_func(void* arg);

int can0_fd, can1_fd, nbytes;
struct sockaddr_can can0_addr;
struct sockaddr_can can1_addr;
struct ifreq ifr0;
struct ifreq ifr1;

struct can_frame can0_rx_frame;
struct can_frame can1_rx_frame;

int CAN0_Init(void)
{
	CAN_DOWN(can0);
	SetBaud(can0, 1000000);
	CAN_UP(can0);

	can0_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	if (-1 == can0_fd)
	{
		printf("Create CAN0 socket error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}

	strcpy(ifr0.ifr_name, "can0");
	ioctl(can0_fd, SIOCGIFINDEX, &ifr0);
	can0_addr.can_family = AF_CAN;
	can0_addr.can_ifindex = ifr0.ifr_ifindex;
	
	bind(can0_fd, (struct sockaddr *)&can0_addr, sizeof(can0_addr));

	pthread_t can0_rx_thrd;
	int res;

	res = pthread_create(&can0_rx_thrd, NULL, CAN0_loop_func, (void*)0);
	if (0 != res)
	{
		printf("Create CAN0 Rx thread error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("Create CAN0 Rx thread success\r\n");

	return 0;
}


int CAN1_Init(void)
{
	CAN_DOWN(can1);
	SetBaud(can1, 1000000);
	CAN_UP(can1);

	can1_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	if (-1 == can1_fd)
	{
		printf("Create CAN1 socket error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}

	strcpy(ifr1.ifr_name, "can1");

	ioctl(can1_fd, SIOCGIFINDEX, &ifr1);

	can1_addr.can_family = AF_CAN;
	can1_addr.can_ifindex = ifr1.ifr_ifindex;
	
	bind(can1_fd, (struct sockaddr *)&can1_addr, sizeof(can1_addr));

	pthread_t can1_rx_thrd;
	int res;

	res = pthread_create(&can1_rx_thrd, NULL, CAN1_loop_func, (void*)0);
	if (0 != res)
	{
		printf("Create CAN1 Rx thread error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("Create CAN1 Rx thread success\r\n");

	return 0;
}

void CAN0_DeInit(void)
{
	CAN_DOWN(can0);
	close(can0_fd);
}

void CAN1_DeInit(void)
{
	CAN_DOWN(can1);
	close(can1_fd);
}


void CAN_SetBaudrate(int baud)
{
}

void CAN0_SendPacket(uint32_t ID, uint8_t dlc, uint8_t* data)
{
	memset(&can0_rx_frame, 0x00, sizeof(can0_rx_frame));
	can0_rx_frame.can_id = ID;
	can0_rx_frame.can_dlc = dlc;
	memcpy(can0_rx_frame.data, data, dlc);

	nbytes = write(can0_fd, &can0_rx_frame, sizeof(can0_rx_frame));
	if (nbytes != sizeof(can0_rx_frame))
	{
		printf("Send Error CAN0 frame\r\n");
	}
}

void CAN_RecvPacket(void)
{
}

void* CAN0_loop_func(void* arg)
{
	printf("CAN0 Receive Thread is startting\r\n");

	while(1)
	{
		memset(&can0_rx_frame, 0x00, sizeof(can0_rx_frame));
		recvfrom(can0_fd, &can0_rx_frame, sizeof(can0_rx_frame), 0, can0_addr, NULL);
		printf("%s\t%03X\t[%d]\t%02X %02X %02X %02X %02X %02X %02X %02X\r\n", ifr0.ifr_name, can0_rx_frame.can_id, can0_rx_frame.can_dlc, can0_rx_frame.data[0], can0_rx_frame.data[1], can0_rx_frame.data[2], can0_rx_frame.data[3], can0_rx_frame.data[4], can0_rx_frame.data[5], can0_rx_frame.data[6], can0_rx_frame.data[7]);
	}
	pthread_exit(NULL);
}

void* CAN1_loop_func(void* arg)
{
	printf("CAN1 Receive Thread is startting\r\n");

	while(1)
	{
		memset(&can1_rx_frame, 0x00, sizeof(can1_rx_frame));
		recvfrom(can1_fd, &can1_rx_frame, sizeof(can1_rx_frame), 0, can1_addr, NULL);
		printf("%s\t%03X\t[%d]\t%02X %02X %02X %02X %02X %02X %02X %02X\r\n", ifr1.ifr_name, can1_rx_frame.can_id, can1_rx_frame.can_dlc, can1_rx_frame.data[0], can1_rx_frame.data[1], can1_rx_frame.data[2], can1_rx_frame.data[3], can1_rx_frame.data[4], can1_rx_frame.data[5], can1_rx_frame.data[6], can1_rx_frame.data[7]);
	}
	pthread_exit(NULL);
}


