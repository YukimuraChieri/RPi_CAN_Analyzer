#include "UDPService.h"

#define BUFF_SIZE 1024
#define SERVER_PORT 10000
#define CLIENT_PORT 10001
#define SERVER_IP "192.168.43.165"
#define CLIENT_IP "192.168.43.198"

void* UDP_loop_func(void *arg);

uint8_t buff[BUFF_SIZE];
struct sockaddr_in srv_addr, cli_addr;
socklen_t sock_len = sizeof(srv_addr);
int sock_fd;

int UDP_Init(void)
{
	srv_addr.sin_family = AF_INET;	// ipv4
	srv_addr.sin_port = htons(SERVER_PORT);
	// servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(CLIENT_PORT);
	cli_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (-1 == sock_fd)
	{
		printf("Create UDP socket error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}

	if (bind(sock_fd, (struct sockaddr*)&srv_addr, sock_len))
	{
		perror("bind");
		return -1;
	}

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
	sendto(sock_fd, data, len, 0, (struct sockaddr*)&cli_addr, sock_len);
}

void* UDP_loop_func(void *arg)
{
	socklen_t socklen;
	int recvlen = 0;
	printf("Udp Receive Thread is startting\r\n");

	while(1)
	{
		memset(buff, 0x00, BUFF_SIZE);
		recvlen = recvfrom(sock_fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&cli_addr, &socklen);
		// printf("buff:%02X %02X\r\n", buff[0], buff[1]);
		printf("[udp recv len]: %d\r\n", recvlen);
	}
	pthread_exit(NULL);

	return 0;
}


