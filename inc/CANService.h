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


typedef enum CAN_CHANNEL_E
{
	CAN0_CH = 0,
	CAN1_CH = 1,
	CAN_CH_NUM
}CAN_CHANNEL_E;


int CAN_Init(CAN_CHANNEL_E can_ch);
void CAN_DeInit(CAN_CHANNEL_E can_ch);
void CAN_SetBaudrate(CAN_CHANNEL_E can_ch, int baud);
void CAN_SendPacket(CAN_CHANNEL_E can_ch, uint32_t can_id, uint8_t can_dlc, uint8_t* data);
int CAN_RecvPacket(CAN_CHANNEL_E can_ch, uint32_t* can_id, uint8_t* can_dlc, uint8_t* data);

#endif

