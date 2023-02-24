#ifndef __UDP_BUFFER_H
#define __UDP_BUFFER_H

/* 可变长数据包缓冲器 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


#define DATABUFFSIZE 2048   /* 数据BUFF长度 */
#define POINTBUFFSIZE 256   /* 指针BUFF长度 */


/* 定义缓冲区域数据结构 */
typedef struct
{
    uint8_t data[DATABUFFSIZE];
    size_t length;
    uint8_t* head;
    uint8_t* tail;
    struct {
        uint8_t* data[POINTBUFFSIZE];
        size_t length;
        uint8_t** head;
        uint8_t** tail;
    }p_fifo;	/* 指针fifo buff */
    pthread_rwlock_t rwlock;
}UDP_Buff_T;

/* UDP缓冲接口函数 */
int UDP_Buff_Init(UDP_Buff_T* buff);
int UDP_Buff_DeInit(UDP_Buff_T* buff);
int UDP_Buff_ReadData(UDP_Buff_T* buff, uint8_t* data, size_t* len);
int UDP_Buff_WriteData(UDP_Buff_T* buff, uint8_t* data, size_t len);
size_t UDP_GetBuffLength(UDP_Buff_T* buff);
size_t UDP_GetPacketNum(UDP_Buff_T* buff);

#endif

