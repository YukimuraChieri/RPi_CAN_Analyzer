#include "CAN_Buffer.h"

CAN_RxBuff_T CAN_RxBuff;    /* CAN接收缓冲 */
CAN_TxBuff_T CAN_TxBuff;    /* CAN发送缓冲 */

/**************************CAN接收缓冲接口函数*********************************/
int CAN_RxBuff_Init(void)
{
	memset(&CAN_RxBuff, 0x00, sizeof(CAN_RxBuff_T));

    int res;
	res = pthread_rwlock_init(&CAN_RxBuff.rwlock, NULL);
	if (0 != res)
	{
		printf("CAN RxBuff Init error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("CAN RxBuff Init success\r\n");

    return 0;
}

int CAN_RxBuff_DeInit(void)
{
	memset(&CAN_RxBuff, 0x00, sizeof(CAN_RxBuff_T));
    pthread_rwlock_destroy(&CAN_RxBuff.rwlock);
    return 0;
}

int CAN_Read_RxBuff(CAN_RxFrame_T* frame)
{
	int res = 0;
	pthread_rwlock_wrlock(&CAN_RxBuff.rwlock);

	if (0 == CAN_RxBuff.len)
	{
		res = -1;
	}
	else
	{
		*frame = CAN_RxBuff.data[CAN_RxBuff.head];
		CAN_RxBuff.head = (CAN_RxBuff.head + 1) % CAN_RX_BUFFSIZE;
		CAN_RxBuff.len--;
	}

	pthread_rwlock_unlock(&CAN_RxBuff.rwlock);

	return res;
}

int CAN_Write_RxBuff(CAN_RxFrame_T frame)
{
	int res = 0;
	pthread_rwlock_wrlock(&CAN_RxBuff.rwlock);

	if (CAN_RX_BUFFSIZE == CAN_RxBuff.len)
	{
		res = -1;
	}
	else
	{
		CAN_RxBuff.data[CAN_RxBuff.tail] = frame;
		CAN_RxBuff.tail = (CAN_RxBuff.tail + 1) % CAN_RX_BUFFSIZE;
		CAN_RxBuff.len++;
	}

	pthread_rwlock_unlock(&CAN_RxBuff.rwlock);

	return res;
}

uint16_t CAN_GetRxBuffLength(void)
{
	uint16_t len = 0;
	pthread_rwlock_rdlock(&CAN_RxBuff.rwlock);
	len = CAN_RxBuff.len;
	pthread_rwlock_unlock(&CAN_RxBuff.rwlock);
	return len;
}

/**************************CAN发送缓冲接口函数*********************************/
int CAN_TxBuff_Init(void)
{
	memset(&CAN_TxBuff, 0x00, sizeof(CAN_TxBuff_T));

    int res;
	res = pthread_rwlock_init(&CAN_TxBuff.rwlock, NULL);
	if (0 != res)
	{
		printf("CAN TxBuff Init error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("CAN TxBuff Init success\r\n");

    return 0;
}

int CAN_TxBuff_DeInit(void)
{
	memset(&CAN_TxBuff, 0x00, sizeof(CAN_TxBuff_T));
    pthread_rwlock_destroy(&CAN_TxBuff.rwlock);
    return 0;
}

int CAN_Read_TxBuff(CAN_TxFrame_T* frame)
{
	int res = 0;
	pthread_rwlock_wrlock(&CAN_TxBuff.rwlock);

	if (0 == CAN_TxBuff.len)
	{
		res = -1;
	}
	else
	{
		*frame = CAN_TxBuff.data[CAN_TxBuff.head];
		CAN_TxBuff.head = (CAN_TxBuff.head + 1) % CAN_TX_BUFFSIZE;
		CAN_TxBuff.len--;
	}

	pthread_rwlock_unlock(&CAN_TxBuff.rwlock);

	return res;
}

int CAN_Write_TxBuff(CAN_TxFrame_T frame)
{
	int res = 0;
	pthread_rwlock_wrlock(&CAN_TxBuff.rwlock);

	if (CAN_TX_BUFFSIZE == CAN_TxBuff.len)
	{
		res = -1;
	}
	else
	{
		CAN_TxBuff.data[CAN_TxBuff.tail] = frame;
		CAN_TxBuff.tail = (CAN_TxBuff.tail + 1) % CAN_TX_BUFFSIZE;
		CAN_TxBuff.len++;
	}

	pthread_rwlock_unlock(&CAN_TxBuff.rwlock);

	return res;
}

uint16_t CAN_GetTxBuffLength(void)
{
	uint16_t len = 0;
	pthread_rwlock_rdlock(&CAN_TxBuff.rwlock);
	len = CAN_TxBuff.len;
	pthread_rwlock_unlock(&CAN_TxBuff.rwlock);
	return len;
}
