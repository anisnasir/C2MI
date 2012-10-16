#ifndef SENDER_H_
#define SENDER_H_
#include "constants.h"
#include "node.h"
#include "thread_msgs.h"
#include "RREQ.h"

void send_to_ip(char ip[], char msg[],int flag)
{
	int sockfd;
    struct sockaddr_in their_addr; // connector's address information
    struct hostent *he;
    int numbytes;
    int broadcast = 1;

	if ((he=gethostbyname(ip)) == NULL) {  // get the host info
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

	if (flag == 0)
	{
    	// this call is what allows broadcast packets to be sent:
    	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
	        sizeof broadcast) == -1)
	    {    
	    	perror("setsockopt (SO_BROADCAST)");
	        exit(1);
	    }
	}

    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(SERVERPORT); // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    if ((numbytes=sendto(sockfd, msg, strlen(msg), 0,
             (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
        perror("sendto");
        exit(1);
    }

    printf("sent %d bytes to %s\n", numbytes,
        inet_ntoa(their_addr.sin_addr));

    close(sockfd);

    return;
}

void send_to_dialup(char str[MAXBUFLEN])
{
	if(dialup_fd < 0 )
		return;
	int i;      
	strcat(str, "$");
	for(i=0; str[i]!=0; i++) {     
    	  write(dialup_fd, &str[i], 1); 
	}        
	printf("\nSent on Dial up\n");
}

void send_data_to_dst(char msg[], char dst[])
{
	pthread_mutex_lock (&mutex_routing);
		int entryAt = find_in_rt(dst);
	int x = 0;
	while(  entryAt == -1 )	//entry not found in rt
	{
		if(x == 10 )
			return;
		pthread_mutex_unlock (&mutex_routing);
		//send rreq
		struct send_rreq_thmsg tmsg;
		strcpy(tmsg.desName,dst);
		int rreq;
		pthread_t thread1;
		rreq = pthread_create( &thread1, NULL, send_RREQ, (void * ) &tmsg);
		
			//wait for rrep
			sleep(5);
		pthread_mutex_lock (&mutex_routing);
		entryAt = find_in_rt(dst);
		x++;
	}
	
		//printf("%s\n",node_table.routing_table[entryAt].next_hop);
			
			if( strcmp(node_table.routing_table[entryAt].next_hop,"dialup") == 0 && dialup_fd >0)
				send_to_dialup(msg);
			else
				send_to_ip(node_table.routing_table[entryAt].next_hop, msg,1);	
		pthread_mutex_unlock (&mutex_routing);
	
	
	return;
}
#endif /*SENDER_H_*/
