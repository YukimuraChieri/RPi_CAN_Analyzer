#include "timestamp.h"

struct timeval start_tv;
struct timeval now_tv;

void reset_timestamp(void)
{
	gettimeofday(&start_tv, NULL);
}

uint32_t get_timestamp(void)
{
	uint32_t timestamp = 0;

	gettimeofday(&now_tv, NULL);
	timestamp = (now_tv.tv_sec - start_tv.tv_sec)*1000 + (now_tv.tv_usec - start_tv.tv_usec)/1000;
	return timestamp;
}


