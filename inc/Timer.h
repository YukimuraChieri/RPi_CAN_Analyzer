#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "CANService.h"
#include "UDPService.h"
#include "crcLib.h"


void Reset_Timestamp(void);
uint32_t Get_Timestamp(void);
int Task10ms_Init(void);
int Task10ms_Start(void);
int Task10ms_Cancel(void);
void Task10ms(int sig);

#endif

