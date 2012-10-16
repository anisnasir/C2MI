#ifndef ENTRY_CLEANER_H_
#define ENTRY_CLEANER_H_

#include"beacon.h"
#include"node.h"
#include "GUI.h"
#include "RUP.h"

void* entry_cleaner(void *a)
{
	while(1)
	{
		sleep(3);
		
		int i;
		int count = 0;
		pthread_mutex_lock (&mutex_neighbor);
			
		for (i =0 ;i<node_table.ntCount;i++)
		{
			unsigned long currentTimeSec;
			struct tm *cts;
			struct timeval tv;
			struct timezone tz;
			gettimeofday(&tv,&tz);
			cts = localtime(&tv.tv_sec);
			currentTimeSec = (cts->tm_hour*3600)+(cts->tm_min*60)+(cts->tm_sec);
			unsigned long lastTimeSec = node_table.neighbor_table[i].timestamp;
			currentTimeSec%=90060;
			lastTimeSec%=90060;
			if((currentTimeSec-lastTimeSec) > 5)
			{
				pthread_t thread1;
				char entry[20] = "";
				strcat(entry,node_table.neighbor_table[i].ip);
				display_neighbor_table();
				pthread_create( &thread1, NULL, send_RUP, (void*)entry);
				node_table.neighbor_table[i] = node_table.neighbor_table[node_table.ntCount-1-count];
				count++;
				
			}
		}
		
		node_table.ntCount-=count;
		pthread_mutex_unlock (&mutex_neighbor);
		if(count>0)
		{
			display_neighbor_table();
			pthread_t thread2;
			int guint;
			guint = pthread_create( &thread2, NULL, gui_send_nt, NULL);
		}
			
	}
}

#endif /*ENTRY_CLEANER_H_*/

