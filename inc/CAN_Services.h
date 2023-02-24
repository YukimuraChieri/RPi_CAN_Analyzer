#ifndef __CAN_SERVICES_H
#define __CAN_SERVICES_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "CAN_Buffer.h"
#include "Timestamp.h"

#define SetBaud(can_ch, baud) system("ip link set "#can_ch" type can bitrate "#baud"")
#define CAN_UP(can_ch) system("ifconfig "#can_ch" up")
#define CAN_DOWN(can_ch) system("ifconfig "#can_ch" down")

int CAN_Init(CAN_CHANNEL_E can_ch);
int CAN_DeInit(CAN_CHANNEL_E can_ch);
int CAN_SetBaudrate(CAN_CHANNEL_E can_ch, int baud);
int CAN_SetMode(CAN_CHANNEL_E can_ch, uint8_t mode);
int CAN_SendPacket(CAN_CHANNEL_E can_ch, uint16_t can_id, uint8_t can_dlc, uint8_t* data);
int CAN_RecvPacket(CAN_CHANNEL_E can_ch, uint16_t* can_id, uint8_t* can_dlc, uint8_t* data);

#endif

