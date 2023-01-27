#include "UDPService.h"

#define BUFF_SIZE 256
#define SERVER_PORT 10001
#define CLIENT_IP "192.168.2.26"

void* UDP_loop_func(void *arg);

uint8_t buff[BUFF_SIZE];
struct sockaddr_in servaddr;
socklen_t sock_len = sizeof(servaddr);
int udpfd;

int UDP_Init(void)
{
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	udpfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (-1 == udpfd)
	{
		printf("Create UDP socket error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}

	bind(udpfd, (struct sockaddr*)&servaddr, sock_len);	

	pthread_t udp_rx_thrd;
	int res;

	res = pthread_create(&udp_rx_thrd, NULL, UDP_loop_func, (void*)0);
	if (0 != res)
	{
		printf("Create thread error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("Create thread success\r\n");

	return 0;
}


void UDP_SendPacket(uint8_t *data, uint16_t len)
{
	sendto(udpfd, data, len, 0, (struct sockaddr*)&servaddr, sock_len);
}

void* UDP_loop_func(void *arg)
{
	printf("Udp Receive Thread is startting\r\n");

	while(1)
	{
		memset(buff, 0x00, BUFF_SIZE);
		recvfrom(udpfd, buff, BUFF_SIZE, 0, NULL, NULL);
		printf("buff:%02X %02X\r\n", buff[0], buff[1]);
	}
	pthread_exit(NULL);
}


