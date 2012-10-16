#ifndef RUP_H_
#define RUP_H_

#include "node.h"

void del_lastHop_re(char name[])
{
	int count = 0;
	int i;
	for(i=0;i<node_table.reCount;i++)
	{
		if(strcmp(node_table.request_table[i].from_hop,name) == 0)
		{
			node_table.request_table[i] = node_table.request_table[node_table.reCount-count-1];
			count++;
		}
	}
		node_table.reCount-=count;
}
void* send_RUP(void*p)
{
	char ipName[20] ;
	strcpy(ipName,(char*)p);
	int i;
	int count =0;
	
	pthread_mutex_lock (&mutex_request);
	del_lastHop_re(ipName);
	pthread_mutex_unlock (&mutex_request);
	
	pthread_mutex_lock (&mutex_backup);
	for (i=0; i<node_table.btCount; i++)
	{
		if( strcmp(ipName, node_table.backup_table[i].next_hop) == 0 )		
		{
			char tempDes[20] = "";			
			strcpy(tempDes,node_table.backup_table[i].dst_name);
			
			node_table.backup_table[i] = node_table.backup_table[node_table.btCount-count-1];
			count++;
			
			pthread_mutex_lock (&mutex_routing);
			int entryAt = find_in_rt(tempDes);
			
			if(entryAt> -1 && strcmp(node_table.routing_table[entryAt].next_hop,ipName) ==0)
			{//copy best cost from BT and delete the entry in bt
					
				int j;
				int mincost =9999;
				for (j=0; j<node_table.btCount-count; j++)
				{
					if(strcmp(node_table.backup_table[j].dst_name,tempDes) == 0)
					{
						if(node_table.backup_table[j].cost < mincost)
						{
							mincost = node_table.backup_table[j].cost;
							node_table.routing_table[entryAt] = node_table.backup_table[j];
						}
					}
					
				}
				if(mincost == 9999 )
				{
					node_table.routing_table[entryAt] = node_table.backup_table[node_table.rtCount-1];
					node_table.rtCount--;
				}
				pthread_t thread1;
				pthread_create( &thread1, NULL, gui_send_rt, NULL);
	
				pthread_mutex_unlock (&mutex_routing);		
				//send RUP
				char rup_msg[20] ="";
				strcat(rup_msg,"3,");
				strcat(rup_msg,tempDes);
				strcat(rup_msg,",");
				strcat(rup_msg,int_to_str(mincost));
				
				int k;
				pthread_mutex_lock (&mutex_neighbor);
				for(k=0;k<node_table.ntCount;k++)
				{
					if(strcmp("dialup",node_table.neighbor_table[k].ip) == 0 && dialup_fd >0)
						send_to_dialup(rup_msg);
					else
						send_to_ip(node_table.neighbor_table[k].ip,rup_msg,1);
				}
				pthread_mutex_unlock (&mutex_neighbor);
			}
		}				 
	}
		node_table.btCount-=count;
		pthread_mutex_unlock (&mutex_backup);
	
	display_routing_table();
	display_backup_table();
	display_request_table();
	
	return (void*)0;
}
void* handle_rup_msg(void*p)
{
	struct handle_th_msg*temp = (struct handle_th_msg*)p;
	char buf[30];
	strcpy(buf,temp->fullmsg);
	
	char desName[20];
	int cost;
	char lastHop[20];
	
	char*token = strtok(buf,",");
	token = strtok(NULL,",");
	strcpy(desName,token);
	token = strtok(NULL,",");
	cost = atoi(token);
	strcpy(lastHop,temp->src_addr);
	
	
	int i;
	int neighborCost=0;
	pthread_mutex_lock (&mutex_neighbor);
	for(i=0;i<node_table.ntCount;i++)
	{
		if(strcmp(lastHop,node_table.neighbor_table[i].ip) == 0)
			neighborCost = node_table.neighbor_table[i].cost;
	}
	pthread_mutex_unlock (&mutex_neighbor);
	int totalCost = neighborCost+cost;
	
	pthread_mutex_lock (&mutex_routing);
	int entryAt = find_in_rt(desName);
			
	if(entryAt > -1 && strcmp(node_table.routing_table[entryAt].next_hop,lastHop) == 0 )
	{				
		if(cost > 9998)
		{
			node_table.routing_table[entryAt] = node_table.routing_table[node_table.rtCount-1];
			node_table.rtCount--;
			
			int j;
			pthread_mutex_lock (&mutex_backup);
			for (j=0; j<node_table.btCount; j++)
			{
				if(strcmp(desName,node_table.backup_table[j].dst_name) == 0 && strcmp(lastHop,node_table.backup_table[j].next_hop) == 0) 
				{
					node_table.backup_table[j] = node_table.backup_table[node_table.btCount-1];
					node_table.btCount--;
					break;
				}
			}
			pthread_mutex_unlock (&mutex_backup);
			
			int mincost = 9999;
			int entryAt = -1;
			pthread_mutex_lock (&mutex_backup);
			for (j=0; j<node_table.btCount; j++)
			{
				if(strcmp(desName,node_table.backup_table[j].dst_name) == 0 && node_table.backup_table[j].cost < mincost) 
				{
					mincost = node_table.backup_table[j].cost;
					entryAt = j;
				}
			}
			pthread_mutex_unlock (&mutex_backup);
			
			char rup_msg[20] ="";
			strcat(rup_msg,"3,");
			strcat(rup_msg,desName);
			strcat(rup_msg,",");
			strcat(rup_msg,int_to_str(mincost));
			
			
			int k;
				pthread_mutex_lock (&mutex_neighbor);
				for(k=0;k<node_table.ntCount;k++)
				{
					if(strcmp("dialup",node_table.neighbor_table[k].ip) == 0 && dialup_fd >0)
						send_to_dialup(rup_msg);
					else
						send_to_ip(node_table.neighbor_table[k].ip,rup_msg,1);
				}
				pthread_mutex_unlock (&mutex_neighbor);			
		}else
		{
			int j;
			pthread_mutex_lock (&mutex_backup);
			for (j=0; j<node_table.btCount; j++)
			{
				if(strcmp(desName,node_table.backup_table[j].dst_name) == 0 && strcmp(lastHop,node_table.backup_table[j].next_hop) == 0)
				{
					node_table.backup_table[j].cost = totalCost;
					break;
				}
			}
			int mincost =999;
			int tentryAt =-1;
			for (j=0; j<node_table.btCount; j++)
			{
				if(strcmp(node_table.backup_table[j].dst_name,desName) == 0)
				{
					if(node_table.backup_table[j].cost < mincost)
					{
						mincost = node_table.backup_table[j].cost;
						tentryAt = j;
					}
				}
					
			}
			
			node_table.routing_table[entryAt] = node_table.backup_table[tentryAt];
			pthread_mutex_unlock (&mutex_backup);
			char rup_msg[20] ="";
			strcat(rup_msg,"3,");
			strcat(rup_msg,desName);
			strcat(rup_msg,",");
			strcat(rup_msg,int_to_str(totalCost));
			int k;
			pthread_mutex_lock (&mutex_neighbor);
			for(k=0;k<node_table.ntCount;k++)
			{
				if(strcmp("dialup",node_table.neighbor_table[k].ip) == 0 && dialup_fd >0)
					send_to_dialup(rup_msg);
				else
					send_to_ip(node_table.neighbor_table[k].ip,rup_msg,1);
			}
			pthread_mutex_unlock (&mutex_neighbor);
		}
	}else
	{
		int j;
		pthread_mutex_lock (&mutex_backup);
		for (j=0; j<node_table.btCount; j++)
		{
			if(strcmp(node_table.backup_table[j].dst_name,desName) == 0 &&  strcmp(node_table.backup_table[j].next_hop,lastHop) == 0)
			{
				node_table.backup_table[j] = node_table.backup_table[node_table.btCount];
				node_table.btCount--;
				break;
			}				
		}
		pthread_mutex_unlock (&mutex_backup);
		
	}
	pthread_mutex_unlock (&mutex_routing);
	
	pthread_t thread1;
	pthread_create( &thread1, NULL, gui_send_rt, NULL);
	
	return (void*)0;
}
#endif /*RUP_H_*/

