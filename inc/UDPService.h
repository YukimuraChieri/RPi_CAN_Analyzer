#ifndef __UDPSERVICE_H
#define __UDPSERVICE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

int UDP_Init(void);
void UDP_SendPacket(uint8_t *data, uint16_t len);
void UDP_RecvPacket(void);

#endif


