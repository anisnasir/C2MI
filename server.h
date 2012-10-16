#ifndef SERVER_H_
#define SERVER_H_

#include "node.h"
#include "constants.h"
#include "beacon.h"
#include "RREQ.h"
#include "RREP.h"
#include "RUP.h"
#include "thread_msgs.h"


void* handle_message(void * a)
{
	int thcount=0;
	pthread_t thread[100];
	struct handle_th_msg msg[100];
	
	struct handle_th_msg * temp = (struct handle_th_msg*)a;
	char buf[MAXBUFLEN];
	char src_addr[20];
	strcpy(buf,temp->fullmsg);
	strcpy(src_addr,temp->src_addr);
	
	switch (buf[0]-48)
    {
    	case 0:					//beacon message type
    		{
    			//"MSG_ID,SRC_ID" 
    			//eg "0,Aadi"
    			
	    		strcpy( msg[thcount].fullmsg, buf);
	    		strcpy(msg[thcount].src_addr,src_addr);
	    		pthread_create( &thread[thcount], NULL, handle_beacon, (void*) &msg[thcount]);
	    		thcount++;
	    		thcount%=100;
	    		
    		}
	    		break;
    		
    	case 1:					//RREQ message type
    		{	
    			//"MSG_ID,DST_ID,SRC_ID,COST,TimeStamp" 
    			//"1,X,S,5,1192"
	    		strcpy( msg[thcount].fullmsg, buf);
	    		strcpy(msg[thcount].src_addr,src_addr);
	    		pthread_create( &thread[thcount], NULL, handle_rreq, (void*) &msg[thcount]);
	    		thcount++;
	    		thcount%=100;
    		}
    		break;
		case 2:				//RREP message
    		{
    			//"2,S,X,5,timestamp"
    			strcpy( msg[thcount].fullmsg, buf);
	    		strcpy(msg[thcount].src_addr,src_addr);
	    		pthread_create( &thread[thcount], NULL, handle_rrep, (void*) &msg[thcount]);
	    		thcount++;
	    		thcount%=100;
	    		
    		}
    		break;
    	case 3:				//RUP message
    		{
    			//"3,X,5"
    			strcpy( msg[thcount].fullmsg, buf);
	    		strcpy(msg[thcount].src_addr,src_addr);
	    		pthread_create( &thread[thcount], NULL, handle_rup_msg, (void*) &msg[thcount]);
	    		thcount++;
	    		thcount%=100;
	    		
    		}
    		case 8:				//RREP message
    		{
    			//"8,routin,****"
    			send_data_to_dst(buf,"G");	    		
    		}
    		break;
    		case 9:				//RREP message
    		{
    			//"9,NODENAME,****"
    			send_data_to_dst(buf,"G");	    		
    		}
    		break;
    	default:
    		printf("shughal");
    	
    }
 
return (void *) 0;   
}



void *dialup_server(void *arg)
{
    char rcv;
    char buf[MAXBUFLEN];
    int i,flag,cap,count; 
 	flag=0;
	cap=0;
	count=0;
	int len =0;
	
        
	while(1){
                i = read(dialup_fd, &rcv, 1);
                if(i<1) 
                	continue;
   
                if(rcv == '$')
                {
                	struct handle_th_msg temp;
                	buf[len] = '\0';
    				strcpy(temp.fullmsg,buf);
    				len = 0;
     				strcpy(temp.src_addr,"dialup");
     
     				pthread_t thread1;
     				int retth; 
     				retth = pthread_create( &thread1, NULL, handle_message, (void*) &temp);
	
                }
                else
                {
                	buf[len] = rcv;
                	len++;
                }	
        }
        return (void*) 0;
}  
		
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void * create_udpserver(void *a)
{
	char buf[MAXBUFLEN];
	//struct ipport_th_msg *ipp = (struct ipport_th_msg *)a;
	
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    
    size_t addr_len;
    char s[INET6_ADDRSTRLEN];


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
	
    if ((rv = getaddrinfo(NULL, SERVERPORT_str, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return ( void * ) 0;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            char * s = strcat("listener: bind"  , (char * )p->ai_addr);
           // perror("listener: bind ");
            perror(s);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return ( void * ) 0;
    }

    freeaddrinfo(servinfo);

	    		
    printf("udp listener: waiting to recvfrom...\n");
	while(1)
	{
	    addr_len = sizeof their_addr;
	    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
	        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
	        perror("recvfrom");
	        exit(1);
	    }
	
		char  src_addr[20] ;
		strcpy( src_addr, inet_ntop(their_addr.ss_family,
	            get_in_addr((struct sockaddr *)&their_addr),s, sizeof s));
	    buf[numbytes] = '\0';    
	    printf("udp listener: got %d bytes packet from %s contains \"%s\"\n", numbytes,src_addr, buf );
	     // display_neighbor_table();
	     
	     struct handle_th_msg temp;
	     strncpy(temp.fullmsg,buf,numbytes);
	     strcpy(temp.src_addr,src_addr);
	     
	     pthread_t thread1;
	     int retth; 
	     retth = pthread_create( &thread1, NULL, handle_message, (void*) &temp);
	}
    close(sockfd);

    return ( void * ) 0;

}

#endif /*SERVER_H_*/
