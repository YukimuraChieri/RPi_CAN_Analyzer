#include "UDP_Input.h"

static uint8_t rx_datagram[1024];

/* UDP输入处理函数 */
void UDP_Input(void)
{
    size_t data_len = 0;
    size_t packet_num = 0;

    /* 获取UDP接收的数据包数量 */
    packet_num = UDP_GetPacketNum(&UDP_RxBuff);
    printf("[udp input]: %u\r\n", packet_num);

    for (size_t i = 0; i < packet_num; i++)
    {
        UDP_Buff_ReadData(&UDP_RxBuff, rx_datagram, &data_len);
        for (size_t j = 0; j < data_len; j++)
        {
            printf("%02X ", rx_datagram[j]);
        }
        printf("\r\n");
    }
}

