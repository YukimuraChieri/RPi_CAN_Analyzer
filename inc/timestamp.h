#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>


void reset_timestamp(void);
uint32_t get_timestamp(void);

#endif

