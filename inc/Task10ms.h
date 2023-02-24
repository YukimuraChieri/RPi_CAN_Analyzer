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
#include "Timestamp.h"
#include "CAN_Input.h"
#include "CAN_Output.h"
#include "UDP_Input.h"
#include "UDP_Output.h"

int Task10ms_Init(void);
int Task10ms_Start(void);
int Task10ms_Cancel(void);
void Task10ms(int sig);

#endif

