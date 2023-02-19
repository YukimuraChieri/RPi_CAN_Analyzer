#include "UDPService.h"
#include "CANService.h"
#include "Timer.h"

#define BUFF_SIZE 1024
#define SERVER_PORT 10000
#define CLIENT_PORT 10001
#define SERVER_IP "192.168.2.16"
#define CLIENT_IP "192.168.2.26"

void* UDP_loop_func(void *arg);

uint8_t buff[BUFF_SIZE];
struct sockaddr_in srv_addr, cli_addr;
socklen_t sock_len = sizeof(srv_addr);
struct ifreq wlan_ifr;
int sock_fd;

void Print_All_Net_Info(void)
{
	struct ifreq interfaces[8];
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifconf ifconf = {
		.ifc_len = sizeof(interfaces),
		.ifc_req = interfaces,
	};
	ioctl(sock, SIOCGIFCONF, &ifconf);
	int num = ifconf.ifc_len/sizeof(struct ifreq);
	printf("interfaces num: %d\r\n", num);
	struct ifreq *p_ifr =  interfaces;
	char buf[64];
	for (int i = 0; i < num; i++, p_ifr++)
	{
		printf("[%d]:%s ", i+1, p_ifr->ifr_name);
		ioctl(sock, SIOCGIFFLAGS, p_ifr);
		printf("flag=%d<> ", p_ifr->ifr_mtu);
		ioctl(sock, SIOCGIFMTU, p_ifr);
		printf("mtu %u\r\n", p_ifr->ifr_ifru.ifru_flags?:65536);

		ioctl(sock, SIOCGIFADDR, p_ifr);
		ioctl(sock, SIOCGIFNETMASK, p_ifr);
		ioctl(sock, SIOCGIFBRDADDR, p_ifr);
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&(p_ifr->ifr_addr))->sin_addr.s_addr), buf, sizeof(buf));
		printf("\tinet: %s ", buf);
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&(p_ifr->ifr_netmask))->sin_addr.s_addr), buf, sizeof(buf));
		printf("netmask: %s ", buf);
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&(p_ifr->ifr_broadaddr))->sin_addr.s_addr), buf, sizeof(buf));
		printf("broadcast: %s\r\n", buf);

		printf("\r\n");
	}
}

in_addr_t Get_WLAN0_Addr(void)
{
	struct ifreq interfaces[8];
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifconf ifconf = {
		.ifc_len = sizeof(interfaces),
		.ifc_req = interfaces,
	};
	ioctl(sock, SIOCGIFCONF, &ifconf);
	int num = ifconf.ifc_len/sizeof(struct ifreq);
	struct ifreq *p_ifr =  interfaces;
	in_addr_t wlan0_addr = 0;
	char buf[32];
	for (int i = 0; i < num; i++, p_ifr++)
	{
		if (0 == strcmp(p_ifr->ifr_name, "wlan0"))
		{
			ioctl(sock, SIOCGIFADDR, p_ifr);
			wlan0_addr = ((struct sockaddr_in *)&(p_ifr->ifr_addr))->sin_addr.s_addr;
			inet_ntop(AF_INET, &wlan0_addr, buf, sizeof(buf));
			printf("WLAN0 inet: %s\r\n", buf);
		}
	}
	return wlan0_addr;
}


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

	memset(&wlan_ifr, 0x00, sizeof(wlan_ifr));
	strcpy(wlan_ifr.ifr_name, "wlan0");
	setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, (char*)&wlan_ifr, sizeof(wlan_ifr));

	printf("WLAN0 Local IP: %s\r\n", inet_ntoa(((struct sockaddr_in*)&(wlan_ifr.ifr_addr))->sin_addr));

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
	printf("Udp Receive Thread is starting\r\n");

	while(1)
	{
		memset(buff, 0x00, BUFF_SIZE);
		recvlen = recvfrom(sock_fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&cli_addr, &socklen);
		// printf("buff:%02X %02X\r\n", buff[0], buff[1]);
		printf("[udp recv len]: %d\r\n", recvlen);

		if (0xEE == buff[0])
		{
			printf("Task10ms OFF\r\n");
			Task10ms_Cancel();
		}
		else if (0xFF == buff[0])
		{
			printf("Task10ms ON\r\n");
			Task10ms_Start();
		}
		else if (0x10 == buff[0])
		{
			printf("CAN1 OFF\r\n");
			CAN_DeInit(CAN1_CH);
		}
		else if (0x11 == buff[0])
		{
			printf("CAN1 ON\r\n");
			CAN_Init(CAN1_CH);
		}
		else if (0x00 == buff[0])
		{
			printf("CAN0 OFF\r\n");
			CAN_DeInit(CAN0_CH);
		}
		else if (0x01 == buff[0])
		{
			printf("CAN0 ON\r\n");
			CAN_Init(CAN0_CH);
		}
	}
	pthread_exit(NULL);
	return 0;
}


