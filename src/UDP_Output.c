#include "UDP_Output.h"

static uint8_t tx_datagram[1024];

/* UDP发送处理函数 */
void UDP_Output(void)
{   
    size_t data_len;
    size_t packet_num;
    
    packet_num = UDP_GetPacketNum(&UDP_TxBuff); /* 获取发送的数据包个数 */

    /* 发送当前缓冲内的所有数据包 */
    for (size_t i = 0; i < packet_num; i++)
    {
        UDP_Buff_ReadData(&UDP_TxBuff, tx_datagram, &data_len);
        UDP_SendPacket(tx_datagram, data_len);
    }
}
