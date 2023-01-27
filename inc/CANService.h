#ifndef __CANSERVICE_H
#define __CANSERVICE_H

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


#define SetBaud(can_ch, baud) system("ip link set "#can_ch" type can bitrate "#baud"")
#define CAN_UP(can_ch) system("ifconfig "#can_ch" up")
#define CAN_DOWN(can_ch) system("ifconfig "#can_ch" down")


void CAN0_Init(void);
void CAN1_Init(void);
void CAN0_DeInit(void);
void CAN1_DeInit(void);
void CAN0_SetBaudrate(int baud);
void CAN1_SetBaudrate(int baud);
void CAN0_SendPacket(uint32_t ID, uint8_t dlc, uint8_t* data);
void CAN1_SendPacket(uint32_t ID, uint8_t dlc, uint8_t* data);
void CAN0_RecvPacket(void);
void CAN1_RecvPacket(void);

#endif

