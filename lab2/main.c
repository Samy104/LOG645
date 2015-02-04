#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int main (int argc, char* argv[])
{
	int thread_count = strtol(argv[1], NULL, 10);
	

	printf("You have %d threads \n", thread_count);
	char bigMessage[1024] = "";
	//omp_set_num_threads(thread_count-1);
	#pragma omp parallel num_threads(thread_count)
	{
		#pragma omg parallel num_threads(3)
		{
			clock_t timestart, timeend;
			int i;
			char message[512] = "";
			char buff[3] = "";
			timestart = clock();
			//num_threads(thread_count);
			for(i = 0; i <= 10; i++)
			{
				sprintf(buff, "%d", i);
				strncat(message,"Spoon # ",strlen("Spoon # "));
				strncat(message,buff,sizeof(message)/sizeof(char));			
			}
			printf("The thread %d is done.\n", omp_get_thread_num());
			char mess[512];
			sprintf(mess, "Message %d: %s \n",omp_get_thread_num(), message);
			#pragma omp critical
			{
				strncat(bigMessage, mess, strlen(mess));
			}
			timeend = clock();
			printf("Process %d has taken %f ms\n", omp_get_thread_num(), (((float)timeend-(float)timestart)/ 1000000.0F)*1000);
		}
			
	}
	printf("Messages : \n%s", bigMessage);
	return 0;
}