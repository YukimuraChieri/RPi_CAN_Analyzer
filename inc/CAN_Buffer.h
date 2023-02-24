#ifndef __CAN_BUFFER_H
#define __CAN_BUFFER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <errno.h>
#include <pthread.h>

#define CAN_RX_BUFFSIZE 128     /* CAN接收缓冲大小，单位：CAN帧个数 */
#define CAN_TX_BUFFSIZE 128     /* CAN发送缓冲大小，单位：CAN帧个数 */

/* 定义CAN通道枚举类型 */
typedef enum CAN_CHANNEL_E
{
	CAN0_CH = 0,
	CAN1_CH = 1,
	CAN_CH_NUM
}CAN_CHANNEL_E;

/* 定义接收CAN帧结构体 */
typedef struct
{
	uint32_t timestamp;
	CAN_CHANNEL_E can_ch;
	struct can_frame frame;
}CAN_RxFrame_T;

/* 定义发送CAN帧结构体 */
typedef struct
{
	CAN_CHANNEL_E can_ch;
	struct can_frame frame;
}CAN_TxFrame_T;

/* 定义CAN接收缓冲结构体 */
typedef struct
{
	uint16_t len;
	uint16_t head;
	uint16_t tail;
	CAN_RxFrame_T data[CAN_RX_BUFFSIZE];
    pthread_rwlock_t rwlock;
}CAN_RxBuff_T;

/* 定义CAN发送缓冲结构体 */
typedef struct
{
	uint16_t len;
	uint16_t head;
	uint16_t tail;
	CAN_TxFrame_T data[CAN_TX_BUFFSIZE];
    pthread_rwlock_t rwlock;
}CAN_TxBuff_T;

/* CAN接收缓冲接口函数 */
int CAN_RxBuff_Init(void);
int CAN_RxBuff_DeInit(void);
int CAN_Read_RxBuff(CAN_RxFrame_T* frame);
int CAN_Write_RxBuff(CAN_RxFrame_T frame);
uint16_t CAN_GetRxBuffLength(void);

/* CAN发送缓冲接口函数 */
int CAN_TxBuff_Init(void);
int CAN_TxBuff_DeInit(void);
int CAN_Read_TxBuff(CAN_TxFrame_T* frame);
int CAN_Write_TxBuff(CAN_TxFrame_T frame);
uint16_t CAN_GetTxBuffLength(void);

#endif

