#include "Timestamp.h"

struct timeval start_time;

void Reset_Timestamp(void)
{
	gettimeofday(&start_time, NULL);
}

uint32_t Get_Timestamp(void)
{
	uint32_t timestamp = 0;
	struct timeval current_time;

	gettimeofday(&current_time, NULL);
	timestamp = (current_time.tv_sec - start_time.tv_sec)*1000 + (current_time.tv_usec - start_time.tv_usec)/1000;

	return timestamp;
}

