#ifndef __TASK10MS_H
#define __TASK10MS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "CAN_Services.h"
#include "UDP_Services.h"
#include "crcLib.h"


void Reset_Timestamp(void);
uint32_t Get_Timestamp(void);

int Task10ms_Init(void);
int Task10ms_Start(void);
int Task10ms_Cancel(void);
void Task10ms(int sig);

#endif

