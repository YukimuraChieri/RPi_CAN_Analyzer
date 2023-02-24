#ifndef __UDP_SERVICES_H
#define __UDP_SERVICES_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/sockios.h>
#include <linux/wireless.h>
#include "UDP_Buffer.h"

int UDP_Init(void);
int UDP_DeInit(void);
void UDP_SendPacket(uint8_t *data, uint16_t len);
void UDP_RecvPacket(void);

void Print_All_Net_Info(void);
int Get_Wireless_If_Status(char* ath);
in_addr_t Get_Wireless_If_Addr(char* ath);

#endif


