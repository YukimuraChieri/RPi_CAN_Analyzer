#include "Task10ms.h"

pthread_t task_10ms_thrd;

/* 任务线程创建函数 */
void* Task_Create_thrd(void* arg)
{
	struct itimerval* point_it;

	point_it = (struct itimerval*)arg;

	if (0 != setitimer(ITIMER_REAL, point_it, NULL))
	{
		perror("setitimer");
		pthread_exit((void*)-1);
	}

	if (SIG_ERR == signal(SIGALRM, Task10ms))
	{
		perror("signal");
		pthread_exit((void*)-1);
	}
	
	while(1)
	{
		pause();
	}
	
	pthread_exit(NULL);
	return 0;
}

/* 初始化任务函数 */
int Task10ms_Init(void)
{
	int res;
	static struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 250000;
	it.it_interval = it.it_value;
	
	res = pthread_create(&task_10ms_thrd, NULL, Task_Create_thrd, (void*)(&it));
	if (0 != res)
	{
		printf("Create task 10ms thread error: %s (errno: %d)\r\n", strerror(errno), errno);
		return -1;
	}
	printf("Create task 10ms thread success\r\n");
	pthread_detach(task_10ms_thrd);

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	return 0;
}

/* 任务开始函数 */
int Task10ms_Start(void)
{
	struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 250000;
	it.it_interval = it.it_value;
	return setitimer(ITIMER_REAL, &it, NULL);
}

/* 取消10ms任务函数 */
int Task10ms_Cancel(void)
{
	struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	it.it_interval = it.it_value;
	return setitimer(ITIMER_REAL, &it, NULL);
}

/* 10ms周期任务函数 */
void Task10ms(int sig)
{
	//printf("[Task10ms]:%06d\r\n", Get_Timestamp());
    //CAN_Input();
    //CAN_Output();
    UDP_Input();
    UDP_Output();
}
