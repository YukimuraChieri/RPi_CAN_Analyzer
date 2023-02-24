#include "UDP_Services.h"
#include "CAN_Services.h"
#include "Task10ms.h"

#define BUFF_SIZE 1024
#define SERVER_PORT 10000
#define CLIENT_PORT 10001
#define SERVER_IP "192.168.43.165"
#define CLIENT_IP "192.168.43.198"

void* UDP_receive(void *arg);

uint8_t buff[BUFF_SIZE];
struct sockaddr_in srv_addr, cli_addr;
socklen_t sock_len = sizeof(srv_addr);
struct ifreq wlan_ifr;
int sock_fd;
pthread_t udp_rx_thrd;

/* 大于所有支持UDP的网卡信息 */
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
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&(p_ifr->ifr_addr))->sin_addr.s_addr), buf, sizeof(buf));
		printf("\tinet: %s ", buf);
		ioctl(sock, SIOCGIFNETMASK, p_ifr);
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&(p_ifr->ifr_netmask))->sin_addr.s_addr), buf, sizeof(buf));
		printf("netmask: %s ", buf);
		ioctl(sock, SIOCGIFBRDADDR, p_ifr);
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&(p_ifr->ifr_broadaddr))->sin_addr.s_addr), buf, sizeof(buf));
		printf("broadcast: %s\r\n", buf);

		printf("\r\n");
	}
	close(sock);
}

/*
获取无线网卡的连接状态
(/proc/net/wireless的status)
return 0:未连接
return 1:已连接
*/
int Get_Wireless_If_Status(char *ath)
{
    int ret = 0;
    struct iwreq iwr;
    struct iw_statistics stats;

    if (NULL == ath)
    {
        printf("ath is NULL\r\n");
        return -1;
    }

    /* make iwreq */
    memset(&iwr, 0, sizeof(iwr));
    memset(&stats, 0, sizeof(stats));
    iwr.u.data.pointer = (caddr_t) &stats; /* result value */
    iwr.u.data.length = sizeof(stats);
    iwr.u.data.flags = 1; /* clear updated flag */

    /* ifname is reqired */
    strncpy(iwr.ifr_ifrn.ifrn_name, ath, IFNAMSIZ - 1);

    /* get SIOCGIWSTATS */
    if (ioctl(sock_fd, SIOCGIWSTATS, &iwr) < 0)
    {
        printf("No Such Device %s\r\n",ath);
        return -1;
    }

    ret = stats.status;

    return ret;
}

in_addr_t Get_Wireless_If_Addr(char *ath)
{
	struct ifreq interfaces[8];
	struct ifconf ifconf = {
		.ifc_len = sizeof(interfaces),
		.ifc_req = interfaces,
	};
    
    if (NULL == ath)
    {
        printf("ath is NULL\r\n");
        return -1;
    }

	ioctl(sock_fd, SIOCGIFCONF, &ifconf);
	int num = ifconf.ifc_len/sizeof(struct ifreq);
	struct ifreq *p_ifr =  interfaces;
	in_addr_t wireless_addr = 0;
	char buf[32];
	for (int i = 0; i < num; i++, p_ifr++)
	{
		if (0 == strcmp(p_ifr->ifr_name, ath))
		{
			ioctl(sock_fd, SIOCGIFADDR, p_ifr);
			wireless_addr = ((struct sockaddr_in *)&(p_ifr->ifr_addr))->sin_addr.s_addr;
			inet_ntop(AF_INET, &wireless_addr, buf, sizeof(buf));
			printf("%s inet: %s\r\n", ath, buf);
		}
	}
	return wireless_addr;
}


int UDP_Init(void)
{
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	srv_addr.sin_family = AF_INET;	// ipv4
	srv_addr.sin_port = htons(SERVER_PORT);
	// srv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    srv_addr.sin_addr.s_addr = Get_Wireless_If_Addr("wlan0");

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

	int res;

	res = pthread_create(&udp_rx_thrd, NULL, UDP_receive, (void*)0);
	if (0 != res)
	{
		printf("Create thread error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
    pthread_detach(udp_rx_thrd);
	printf("Create thread success\r\n");

	return 0;
}

int UDP_DeInit(void)
{
    pthread_cancel(udp_rx_thrd);
    pthread_join(udp_rx_thrd, NULL);
    close(sock_fd);
    printf("UDP DeInit\r\n");
    return 0;
}

void UDP_SendPacket(uint8_t *data, uint16_t len)
{
	sendto(sock_fd, data, len, 0, (struct sockaddr*)&cli_addr, sock_len);
}

void* UDP_receive(void *arg)
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


