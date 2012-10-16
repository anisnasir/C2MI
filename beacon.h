#ifndef BEACON_H_
#define BEACON_H_

#include "sender.h"
#include "thread_msgs.h"
#include "node.h"
#include "GUI.h"

int type_in_inter(char n[])
{
	char name[20];
	strcpy(name,n);
	int count=0;
	int i;
	for (i=0;i<strlen(name);i++)
	{
		if(name[i] == '.')
			count++;
		if(count == 3)
			name[i] = '\0';
	}
	
	for(i=0;i<node_table.itCount;i++)
	{
		char temp[20] = "";
		strcpy(temp,node_table.interface_table[i].ip);
	
	int j;
	count =0;
		for (j=0;j<strlen(temp);j++)
		{
			if(temp[j] == '.')
				count++;
			if(count == 3)
				temp[j] = '\0';
		}
		if(strcmp(name,temp) == 0)
		{
			if(strcmp(node_table.interface_table[i].name,"eth0") == 0)
				return 2;
			else
				return 1;
		}
	}
}

int isIP(char a[] )
{
	int x =0;
	char array[30];
	strcpy(array,a);
	char *token = strtok(array,".");
 	while(token!=NULL)
 	{
 		x++;
 		token = strtok(NULL,".");
 	}
 
 	if(x==4)
 		return 1;
 	else
 		return 0;	
}
void * beacon_broadcast(void * a)
{
	while(1)
	{
	sleep(1);
	char  beacon_ms[30] = "0,";
	char * beacon_msg = strcat(beacon_ms, MYID);
	
    system("sudo ifconfig |grep 'inet addr'| grep 'Bcast' |awk '{print $3}' >temp.txt");
 	FILE *fp;
 	fp = fopen("temp.txt","r");
 	char array[30];
 	while(!feof(fp))
 	{
 		fgets(array,30,fp);
 		if(strlen(array) < 2)
 			continue;
 		char *token = strtok(array,":");
 		token = strtok(NULL,"\n");
 		strcpy(array,"");
 		if(isIP(token) == 1)
    		send_to_ip(token,beacon_msg,0);
 	}
 	fclose(fp);
	if(dialup_fd>0)
		send_to_dialup(beacon_msg);
	}
    return ( void * ) 0;
}

void reply_beacon(char * src_addr)
{
	char  beacon_ms[30] = "0,";
	//printf("i am sending %s\n",beacon_ms);
   	char * beacon_msg = strcat(beacon_ms, MYID);
	printf("i am replying to beacon : %s\n",beacon_msg);
    		
    if(strcmp(src_addr,"dialup") == 0 && dialup_fd>0)
    	send_to_dialup(beacon_msg);
    else
		send_to_ip(src_addr,beacon_msg,1);
		
}

void * handle_beacon(void * a)
{
	struct handle_th_msg *p = (struct handle_th_msg *)a;
	char * src_addr = p->src_addr;
	char * msg = p->fullmsg;
	
	strtok(msg,","); //	MSGID
    char *sender_id = strtok(NULL,",");
  
//	printf("i got a beacon from	%s,%s\n",sender_id, src_addr);
	
	pthread_mutex_lock (&mutex_neighbor);
	
	
	
	if(strcmp(sender_id,MYID) == 0)
		{
			pthread_mutex_unlock (&mutex_neighbor);			
			return ( void * ) 0;	
		}
	
	//printf("\n entries = %d\n",node_table.ntCount);
	int entryAt=0;
	
		entryAt = find_in_nt(src_addr,sender_id);
		
	if ( entryAt == -1 )
	{
		
			strcpy(node_table.neighbor_table[node_table.ntCount].name , sender_id);
			strcpy(node_table.neighbor_table[node_table.ntCount].ip , src_addr);
			if(strcmp(src_addr,"dialup") == 0)
			{
				node_table.neighbor_table[node_table.ntCount].type = 4;
				node_table.neighbor_table[node_table.ntCount].cost = 50;
			}
			else
			{
				
				node_table.neighbor_table[node_table.ntCount].type = type_in_inter(src_addr);
				node_table.neighbor_table[node_table.ntCount].cost = 25;
			}	
			struct timeval tv;
			struct timezone tz;
			gettimeofday(&tv,&tz);
			struct tm *lts = localtime(&tv.tv_sec);
			node_table.neighbor_table[node_table.ntCount].timestamp = (lts->tm_hour*3600)+(lts->tm_min*60)+(lts->tm_sec);
			node_table.ntCount++;
			reply_beacon(src_addr);
			
			pthread_t thread1;
			int guint;
			guint = pthread_create( &thread1, NULL, gui_send_nt, NULL);
			//send rreq of all destinations from request table
	display_neighbor_table();
	}
	else if ( entryAt >= 0 )
	{
			struct timeval tv;
			struct timezone tz;
			gettimeofday(&tv,&tz);
			struct tm *lts = localtime(&tv.tv_sec);
			node_table.neighbor_table[entryAt].timestamp = (lts->tm_hour*3600)+(lts->tm_min*60)+(lts->tm_sec);
			
	}
	pthread_mutex_unlock (&mutex_neighbor);	
	return ( void * ) 0;	
}
#endif /*BEACON_H_*/
