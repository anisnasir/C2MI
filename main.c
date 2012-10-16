#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include<sys/time.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "int_to_str.h"
#include "entry_cleaner.h"
#include "server.h"
#include "beacon.h"
#include "node.h"
#include"constants.h"
#include "thread_msgs.h"
#include "interfaces.h"

int main(void)
{

	pthread_t thread[100];
	int threadcount=0;
   		 
	initialize_node_table();

	 pthread_create( &thread[threadcount], NULL, check_interfaces, NULL);
    threadcount++;
    sleep(1);
    display_interface_table();
	pthread_create( &thread[threadcount], NULL, create_udpserver, NULL);
    threadcount++;
     			
    //int dialupret;
	//dialupret = pthread_create( &thread[threadcount], NULL, dialup_server, NULL);
   // threadcount++;
    
	pthread_create( &thread[threadcount++], NULL, beacon_broadcast, NULL);

	pthread_create( &thread[threadcount++], NULL, entry_cleaner, NULL);
	
	
	     			
int i;
	for(i =0;i<threadcount;i++)
		pthread_join( thread[i], NULL);
		
    return 0;
}

