#ifndef RREP_H_
#define RREP_H_

#include "node.h"
void * handle_rrep(void * a)
{
	struct handle_th_msg*temp = (struct handle_th_msg*)a;
	char buf[30];
	strcpy(buf,temp->fullmsg);
	
	char desName[20];
	char srcName[20];
	int cost;
	int tstamp;
	char lastHop[20];
	
	char*token = strtok(buf,",");
	token = strtok(NULL,",");
	strcpy(desName,token);
	if(strcmp(desName,MYID) == 0)
		return (void*)0; 
	token = strtok(NULL,",");
	strcpy(srcName,token);
	token = strtok(NULL,",");
	cost = atoi(token);
	strcpy(lastHop,temp->src_addr);
	token = strtok(NULL,",");
	tstamp = atol(token);
	    		
	//get cost of the arrived port from the nt
	int neighborCost=0;
	pthread_mutex_lock (&mutex_neighbor);
	int i;
	for(i=0;i<node_table.ntCount;i++)
	{
		if(strcmp(lastHop,node_table.neighbor_table[i].ip) == 0)
			neighborCost = node_table.neighbor_table[i].cost;
	}
	pthread_mutex_unlock (&mutex_neighbor);
	int totalCost = neighborCost+cost;
	
	//Add destination in BT
	int exist = 0;
	pthread_mutex_lock (&mutex_backup);
		for(i =0;i<node_table.btCount;i++)
		{
			if((strcmp(node_table.backup_table[i].dst_name,desName) == 0) && (strcmp(node_table.backup_table[i].next_hop,lastHop)==0))
				{
					exist = 1;
				if(node_table.backup_table[i].cost > totalCost)
						node_table.backup_table[i].cost = totalCost;
				}
		}
		if(exist == 0)
		{
			strcpy(node_table.backup_table[node_table.btCount].dst_name, desName);
			node_table.backup_table[node_table.btCount].cost = totalCost;
			strcpy(node_table.backup_table[node_table.btCount].next_hop, lastHop);
			node_table.backup_table[node_table.btCount].timestamp = tstamp;
			node_table.btCount++;
		}
	pthread_mutex_unlock (&mutex_backup);
	
	pthread_mutex_lock (&mutex_routing);
	int entryAt;
	
	
	entryAt = find_in_rt(desName);
	if(entryAt == -1)
	{
		strcpy(node_table.routing_table[node_table.rtCount].dst_name,desName);
		node_table.routing_table[node_table.rtCount].cost = totalCost;
		strcpy(node_table.routing_table[node_table.rtCount].next_hop,lastHop);
		node_table.routing_table[node_table.rtCount].timestamp = tstamp;
		node_table.rtCount++;
	}
	else if(node_table.routing_table[entryAt].cost > totalCost)
	{
		strcpy(node_table.routing_table[entryAt].next_hop ,lastHop);
		node_table.routing_table[entryAt].cost = totalCost;
	}
	
	//sending routing table to GUI
	pthread_t thread1;
	pthread_create( &thread1, NULL, gui_send_rt, NULL);
	
	pthread_mutex_unlock (&mutex_routing);
	pthread_mutex_lock (&mutex_request);
	if(strcmp(MYID,srcName)!= 0)
		{
			for(i=0;i<node_table.reCount;i++)
			{
				if((strcmp(node_table.request_table[i].dst_name,desName) == 0) && ((totalCost < node_table.request_table[i].repCost) || (node_table.request_table[i].repCost == -1)))
				{
					node_table.request_table[i].repCost = totalCost;
					char rrep_msg[30];
					strcat(rrep_msg,"2,");
					strcat(rrep_msg,desName);
					strcat(rrep_msg,",");
					strcat(rrep_msg,srcName);
					strcat(rrep_msg,",");
					strcat(rrep_msg,int_to_str(totalCost));
					strcat(rrep_msg,",");		
					strcat(rrep_msg,int_to_str(tstamp));
					if(dialup_fd>0 && strcmp(node_table.request_table[i].from_hop,"dialup") == 0)
						send_to_dialup(rrep_msg);
					else
						send_to_ip(node_table.request_table[i].from_hop,rrep_msg,1);
				}
			}
		}
	pthread_mutex_unlock (&mutex_request);
	
	
		

	return ( void * ) 0;	
}

#endif /*RREP_H_*/

