#ifndef RREQ_H_
#define RREQ_H_

#include "thread_msgs.h"
#include "node.h"
#include "GUI.h"

void* send_RREQ(void *p)
{
	struct send_rreq_thmsg * tmsg = ( struct send_rreq_thmsg * ) p;
	char msg[20] = "1,";
	strcat(msg,tmsg->desName);
	strcat(msg,",");
	strcat(msg,MYID);
	strcat(msg,",0,");
	
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	struct tm *lts = localtime(&tv.tv_sec);
	unsigned long timestamp = (lts->tm_hour*3600)+(lts->tm_min*60)+(lts->tm_sec);
	
	strcat(msg,int_to_str(timestamp));
	
	int i;
	pthread_mutex_lock (&mutex_neighbor);
	for(i=0;i<node_table.ntCount;i++)
	{
		if(strcmp("dialup",node_table.neighbor_table[i].ip) == 0 && dialup_fd >0)
				send_to_dialup(msg);
		else
				send_to_ip(node_table.neighbor_table[i].ip,msg,1);
	}
	pthread_mutex_unlock (&mutex_neighbor);
		
	printf("sending rreq %s\n",msg);
	return (void *)0;
}

void * handle_rreq(void * a)
{
	struct handle_th_msg*temp = (struct handle_th_msg*)a;
	char buf[MAXBUFLEN];
	strcpy(buf,temp->fullmsg);
	
	char srcName[20];
	char desName[20];
	char lastHop[20];
	int cost;
	int tstamp;
	 
	//extracting info from msg
	char*token = strtok(buf,",");
	token = strtok(NULL,",");
	strcpy(desName,token);
	token = strtok(NULL,",");
	strcpy(srcName,token);
	//if src gets the request pck, ignore
	if(strcmp(srcName,MYID) == 0)
		return ( void * ) 0;
	token = strtok(NULL,",");
	cost = atoi(token);
	strcpy(lastHop,temp->src_addr);
	token = strtok(NULL,",");
	tstamp = atol(token);
	    		
	//get cost of neighbor from NT 
	int i;
	int neighborCost=0;
	//l1 n
	pthread_mutex_lock (&mutex_neighbor);
	for(i=0;i<node_table.ntCount;i++)
	{
		if(strcmp(lastHop,node_table.neighbor_table[i].ip) == 0)
			neighborCost = node_table.neighbor_table[i].cost;
	}
	//ul1 n
	pthread_mutex_unlock (&mutex_neighbor);
	int totalCost = neighborCost+cost;
	
	//add/update path to src 
	int exist = 0;
	
	//l2 b
	pthread_mutex_lock (&mutex_backup);
		for(i =0;i<node_table.btCount;i++)
		{
			if((strcmp(node_table.backup_table[i].dst_name,srcName) == 0) && (strcmp(node_table.backup_table[i].next_hop,lastHop)==0))
				{
					exist = 1;
					if(node_table.backup_table[i].cost > totalCost)
					{
						node_table.backup_table[i].cost = totalCost;
						node_table.backup_table[i].timestamp = tstamp;
					}
				}
		}
		if(exist == 0)
		{
			
			strcpy(node_table.backup_table[node_table.btCount].dst_name, srcName);
			node_table.backup_table[node_table.btCount].cost = totalCost;
			strcpy(node_table.backup_table[node_table.btCount].next_hop, lastHop);
			node_table.backup_table[node_table.btCount].timestamp = tstamp;
			node_table.btCount++;
		}
		
	//ul2 b
	pthread_mutex_unlock (&mutex_backup);
	
	
	
		int entryAt;

		//Check whether source exist in Routing Table
	//l3 r
	pthread_mutex_lock (&mutex_routing);
		entryAt = find_in_rt(srcName);
		//if Source is present in RT with greater cost replace cost
		if(entryAt > -1)
		{
			if(node_table.routing_table[entryAt].cost > totalCost)
				node_table.routing_table[entryAt].cost = totalCost;
		}
		else // if Source is not present in RT add Source
		{
			strcpy(node_table.routing_table[node_table.rtCount].dst_name,srcName);
			node_table.routing_table[node_table.rtCount].cost = totalCost;
			strcpy(node_table.routing_table[node_table.rtCount].next_hop,lastHop);
			node_table.routing_table[node_table.rtCount].timestamp = tstamp;
			node_table.rtCount++;
		}

	pthread_t thread1;
	pthread_create( &thread1, NULL, gui_send_rt, NULL);
	
		//If Current node is destination or RT contains Destination send RREP
		
		entryAt = find_in_rt(desName);
		display_routing_table();
		if((strcmp(MYID,desName) == 0) || 	(entryAt>-1))
		{
			//2,desName,srcName,cost,timestamp
			char rrep_msg[30];
			strcat(rrep_msg,"2,");
			strcat(rrep_msg,desName);
			strcat(rrep_msg,",");
			strcat(rrep_msg,srcName);
			strcat(rrep_msg,",");
			if(entryAt>-1 && (strcmp(MYID,desName) != 0))
				strcat(rrep_msg,int_to_str(node_table.routing_table[entryAt].cost));
			else
				strcat(rrep_msg,int_to_str(0));
	//ul3 r
	pthread_mutex_unlock (&mutex_routing);
			
			strcat(rrep_msg,",");
			
			struct timeval tv;
			struct timezone tz;
			gettimeofday(&tv,&tz);
			struct tm *lts = localtime(&tv.tv_sec);
			unsigned long timestamp = (lts->tm_hour*3600)+(lts->tm_min*60)+(lts->tm_sec);
			strcat(rrep_msg,int_to_str(timestamp));
			
			if(strcmp(lastHop,"dialup") == 0)
				send_to_dialup(rrep_msg);
			else
				send_to_ip(lastHop,rrep_msg);
		}else
		{
			//ul3 r
			pthread_mutex_unlock (&mutex_routing);
			//l4 rq
			pthread_mutex_lock (&mutex_request);
			entryAt = find_in_re(desName);
			
			if(entryAt == -1) //entry not found in request table; first time request
			{
				//l5 n
				pthread_mutex_lock (&mutex_neighbor);
				for(i=0;i<node_table.ntCount;i++)
				{
					if(strcmp(node_table.neighbor_table[i].ip,lastHop) != 0)
					{
						char rreq_msg[30];
						strcat(rreq_msg,"1,");
						strcat(rreq_msg,desName);
						strcat(rreq_msg,",");
						strcat(rreq_msg,srcName);
						strcat(rreq_msg,",");
						strcat(rreq_msg,int_to_str(totalCost));
						strcat(rreq_msg,",");
						strcat(rreq_msg,int_to_str(tstamp));
						if(dialup_fd>0 && strcmp(node_table.neighbor_table[i].ip,"dialup") == 0)
							send_to_dialup(rreq_msg);
						else
							send_to_ip(node_table.neighbor_table[i].ip,rreq_msg);
						
					}

				}
				//ul 5 n
				pthread_mutex_unlock (&mutex_neighbor);
				
				strcpy(node_table.request_table[node_table.reCount].from_hop,lastHop);
				strcpy(node_table.request_table[node_table.reCount].dst_name,desName);
				node_table.request_table[node_table.reCount].mark = 0;
				node_table.request_table[node_table.reCount].repCost = -1;
				node_table.request_table[node_table.reCount].timestamp = tstamp;
				node_table.reCount++;
			}
			else	//destination found in request table
			{
				
				if(node_table.request_table[entryAt].mark == 1) // third and further rreqs
				{
					int i;
					for (i= entryAt; i< node_table.reCount; i++ )
					{
						if( strcmp(node_table.request_table[i].from_hop,lastHop) == 0 )
						{
							if (node_table.request_table[i].timestamp < tstamp)
						 		node_table.request_table[i].timestamp = tstamp;
						 	return (void *) 0;	
						}
					}
					strcpy(node_table.request_table[node_table.reCount].from_hop,lastHop);
					strcpy(node_table.request_table[node_table.reCount].dst_name,desName);
					node_table.request_table[node_table.reCount].mark = 1;
					node_table.request_table[node_table.reCount].repCost = -1;
					node_table.request_table[node_table.reCount].timestamp = tstamp;
					node_table.reCount++;
					display_request_table();
					return ( void * ) 0;
				}
				else	//only one entry found in ReqT, send RREQ on port from which first RREQ arrived
				{
					char rreq_msg[30];
					strcat(rreq_msg,"1,");
					strcat(rreq_msg,desName);
					strcat(rreq_msg,",");
					strcat(rreq_msg,srcName);
					strcat(rreq_msg,",");
					strcat(rreq_msg,int_to_str(totalCost));
					strcat(rreq_msg,",");
					strcat(rreq_msg,int_to_str(tstamp));
				
					if(strcmp(node_table.request_table[entryAt].from_hop,"dialup") == 0)
						send_to_dialup(rreq_msg);
					else
						send_to_ip(node_table.request_table[entryAt].from_hop ,rreq_msg);
					node_table.request_table[entryAt].mark = 1;
					
					strcpy(node_table.request_table[node_table.reCount].from_hop,lastHop);
					strcpy(node_table.request_table[node_table.reCount].dst_name,desName);
					node_table.request_table[node_table.reCount].mark = 1;
					node_table.request_table[node_table.reCount].repCost = -1;
					node_table.request_table[node_table.reCount].timestamp = tstamp;
					node_table.reCount++;
					display_request_table();
				}
			}
			//ul 4 rq
			pthread_mutex_unlock (&mutex_request);
		}
	return ( void * ) 0;	
}
#endif /*RREQ_H_*/
