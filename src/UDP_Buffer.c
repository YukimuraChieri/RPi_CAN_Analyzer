/* 可变长数据包缓冲器 */
#include "UDP_Buffer.h"


int P_Buff_ReadPoint(UDP_Buff_T* buff, uint8_t** pp_data)
{
	if (buff->p_fifo.length == 0)
	{
		return -1;
	}

	*pp_data = *buff->p_fifo.head;
	buff->p_fifo.head = ((buff->p_fifo.head - buff->p_fifo.data + 1) % POINTBUFFSIZE) + buff->p_fifo.data;
	buff->p_fifo.length--;

	return 0;
}

int P_Buff_WritePoint(UDP_Buff_T* buff, uint8_t* p_data)
{
	if (buff->p_fifo.length == POINTBUFFSIZE)
	{
		return -1;
	}

	*buff->p_fifo.tail = p_data;
	buff->p_fifo.tail = ((buff->p_fifo.tail - buff->p_fifo.data + 1) % POINTBUFFSIZE) + buff->p_fifo.data;
	buff->p_fifo.length ++;

	return 0;
}

/* 初始化缓冲区域 */
int UDP_Buff_Init(UDP_Buff_T* buff)
{
	buff->head = buff->data;
	buff->tail = buff->data;
    buff->length = 0;
	buff->p_fifo.tail = buff->p_fifo.data;
	buff->p_fifo.head = buff->p_fifo.data;
	buff->p_fifo.length = 0;	
    int res = pthread_rwlock_init(&buff->rwlock, NULL);
	if (0 != res)
	{
		printf("Create Buff rwlock error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("Create Buff rwlock success\r\n");
	return 0;
}

int UDP_Buff_DeInit(UDP_Buff_T* buff)
{
	buff->head = buff->data;
	buff->tail = buff->data;
    buff->length = 0;
	buff->p_fifo.tail = buff->p_fifo.data;
	buff->p_fifo.head = buff->p_fifo.data;
	buff->p_fifo.length = 0;	
    
	pthread_rwlock_destroy(&buff->rwlock);
    return 0;
}

/* 读取缓冲数据 */
int UDP_Buff_ReadData(UDP_Buff_T* buff, uint8_t* data, size_t* len)
{
	size_t data_len = 0;
	uint8_t* next_tail = NULL;
    int res = 0;

    pthread_rwlock_wrlock(&buff->rwlock);

	if (buff->length == 0)
	{
		res = -1;
	}
    else
    {
        P_Buff_ReadPoint(buff, &next_tail);   /* 读取缓存的tail指针 */

        if (buff->head <= next_tail)     /* 当tail指针大于等于head时 */
        {
            data_len = next_tail - buff->head; /* 计算数据长度 */
            memcpy(data, buff->head, data_len);
        }
        else    /* tail指针小于head时 */
        {
            size_t tail_len = 0;
            size_t over_len = 0;
            tail_len = buff->data + DATABUFFSIZE - buff->head;
            over_len = next_tail - buff->data; /* 计算超出长度 */
            data_len = tail_len + over_len;
            memcpy(data, buff->head, tail_len);    /* 分段copy数据 */
            memcpy(data+tail_len, buff->data, over_len);
        }

	    *len = data_len;
	    buff->head = next_tail;
	    buff->length -= data_len;
    }
    pthread_rwlock_unlock(&buff->rwlock);

	return res;
}

/* 写入缓冲数据 */
int UDP_Buff_WriteData(UDP_Buff_T* buff, uint8_t* data, size_t len)
{
    int res;
    pthread_rwlock_wrlock(&buff->rwlock);

	if (DATABUFFSIZE < (buff->length+len))  /* 判断缓冲区域能否存下数据 */
	{
		res = -1;
	}
    else
    {
        if (buff->head <= buff->tail)     /* 当tail指针大于等于head时 */
        {
            if ((buff->tail + len) <= (buff->data + DATABUFFSIZE))  /* tail到buff数组结尾的长度大于等于写入长度 */
            {
                memcpy(buff->tail, data, len);     /* 此时可以直接在tail后copy数据 */
            }
            else    /* tail到buff数组结尾的长度小于写入长度，此时需要将数据分片 */
            {
                size_t over_len = (buff->tail + len) - (buff->data + DATABUFFSIZE);   /* 计算超出长度 */
                memcpy(buff->tail, data, len - over_len);  /* 写入前段数据 */
                memcpy(buff->data, data+(len - over_len), over_len);  /* 写入后段数据 */
            }
        }
        else    /* tail指针小于head时 */
        {
            memcpy(buff->tail, data, len); /* 因为已经判断过不会超出buff，所以此时可以直接在tail后copy数据 */
        }
	    buff->tail = ((buff->tail - buff->data + len) % DATABUFFSIZE) + buff->data;
	    P_Buff_WritePoint(buff, buff->tail); /* 将tail指针加入到缓冲区中 */
	    buff->length += len;
    }
    pthread_rwlock_unlock(&buff->rwlock);

	return res;
}

