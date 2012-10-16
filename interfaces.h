#ifndef INTERFACES_H_
#define INTERFACES_H_

#include "node.h"

void ethernet_init()
{
		
	int fd;
	struct if_nameindex *curif, *ifs;
	struct ifreq req;
	
	if((fd = socket(PF_INET, SOCK_DGRAM, 0)) != -1) 
	{
		ifs = if_nameindex();
	
		if(ifs)
		{
			for(curif = ifs; curif && curif->if_name; curif++) 
			{
				strncpy(req.ifr_name, curif->if_name, IFNAMSIZ);
				req.ifr_name[IFNAMSIZ] = 0;
				if (ioctl(fd, SIOCGIFADDR, &req) < 0)
				{
		//			perror("ioctl");
				}
				else
				{
					char * ipp = inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr);
					if (strcmp(ipp,"127.0.0.1") == 0 )
					{
						//printf("local loop\n");
						continue;	
					}
				pthread_mutex_lock (&mutex_interface);
	
				strcpy(node_table.interface_table[node_table.itCount].ip , ipp );
				strcpy(node_table.interface_table[node_table.itCount].name , curif->if_name );
				node_table.itCount++;
				pthread_mutex_unlock (&mutex_interface);

					//printf("%s: [%s]\n", curif->if_name,ipp);
							
				}
			}
	
			if_freenameindex(ifs);
			if(close(fd)!=0)
				perror("close");
		}
		else
			perror("if_nameindex");
	}
	else
		perror("socket");
}
void serial_init()
{
	struct termios oldtio, newtio;

   while(1)
   {
        dialup_fd = open(SERIAL_PORT, O_RDWR | O_NONBLOCK);

        if(dialup_fd < 0) 
        {
    			sleep(2);
    			continue;
        }
        else 
        {
                printf("\nSerial Opened...\n");
                fflush(stdout);
                break;
        }
   }
        tcgetattr(dialup_fd, &oldtio);
        bzero(&newtio, sizeof(newtio)); 

        newtio.c_cflag = 0 | BAUDRATE | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        newtio.c_lflag = 0;
        newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN] = 1;   /* blocking read until chars received */

        tcflush(dialup_fd, TCIFLUSH);

        if (tcsetattr(dialup_fd, TCSANOW, &newtio) != 0) {
              //  printf("\nError in com setting...");
                return;
        }
        
	pthread_mutex_lock (&mutex_interface);
		strcpy(node_table.interface_table[node_table.itCount].ip , int_to_str(dialup_fd) );
		strcpy(node_table.interface_table[node_table.itCount].name , "dialup" );
		node_table.itCount++;
	pthread_mutex_unlock (&mutex_interface);
        
}

void * check_interfaces( void * a)
{	
	ethernet_init();
//	serial_init();

	return ( void * ) 0;
}


#endif /*INTERFACES_H_*/
