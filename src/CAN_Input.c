#include "CAN_Input.h"

#define SEND_MAX_NUM ((sizeof(tx_datagram)-8)/16)

static uint8_t tx_datagram[1024] = {0};

/* CAN输入处理函数 */
void CAN_Input(void)
{
	uint16_t can_frame_num = CAN_GetRxBuffLength();
	uint16_t index = 4, crc = 0;
	uint16_t temp_u16;
	uint32_t temp_u32;
	CAN_RxFrame_T can_frame_info;

    /* 由于UDP传输的长度不能超过MTU，需要限制每次传输的数量 */
	if (can_frame_num > SEND_MAX_NUM)
	{
        can_frame_num = SEND_MAX_NUM;
	}

	// head
	tx_datagram[0] = 0xA5;
	tx_datagram[1] = 0xC3;
	// number of can framesi, Little Endian Mode
	temp_u16 = htons(can_frame_num);
	memcpy(&tx_datagram[2], &temp_u16, 2);

	for (uint16_t n = 0 ; n < can_frame_num; n++)
	{
		CAN_Read_RxBuff(&can_frame_info);

		temp_u32 = htonl(can_frame_info.timestamp);
		memcpy(&tx_datagram[index], &temp_u32, 4);
		index += 4;

		tx_datagram[index++] = can_frame_info.can_ch;
		
		temp_u16 = htons(can_frame_info.frame.can_id);
		memcpy(&tx_datagram[index], &temp_u16, 2);
		index += 2;

		tx_datagram[index++] = can_frame_info.frame.can_dlc;

		memcpy(&tx_datagram[index], &can_frame_info.frame.data, 8);
		index += 8;
	}

	// CRC
	uint16_t len = can_frame_num * 16;
	crc = crc16_ibm(tx_datagram+4, len);
	memcpy(&tx_datagram[index], &crc, 2);
	index += 2;
	// tail
	tx_datagram[index++] = 0x5A;
	tx_datagram[index++] = 0x3C;

    /* 将发送数据写入UDP TX BUFF */
    UDP_Buff_WriteData(&UDP_TxBuff, tx_datagram, index);
}


