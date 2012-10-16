#ifndef GUI_H_
#define GUI_H_

#include "node.h"
#include "constants.h"
#include "sender.h"

void * gui_send_nt(void * p)
{
	char a[10] = "*",
		 b[10] = "*",
		 c[10] = "*",
		 d[10] = "*";
	char gui_msg[30] = "9,";
	strcat(gui_msg,MYID);
	strcat(gui_msg,",");
	
	pthread_mutex_lock (&mutex_neighbor);
	int i;
	for (i=0; i<node_table.ntCount; i++)
	{
		if(node_table.neighbor_table[i].type == 1)
		{
			strcpy(a,node_table.neighbor_table[i].name);
		}if(node_table.neighbor_table[i].type == 2)
		{
			strcpy(b,node_table.neighbor_table[i].name);
		}else if(node_table.neighbor_table[i].type == 3)
		{
			strcpy(c,node_table.neighbor_table[i].name);
		}else if(node_table.neighbor_table[i].type == 4)
		{
			strcpy(d,node_table.neighbor_table[i].name);
		}
			
	}
	pthread_mutex_unlock (&mutex_neighbor);
	
	strcat(gui_msg,a);
	strcat(gui_msg,b);
	strcat(gui_msg,c);
	strcat(gui_msg,d);
		
	send_data_to_dst(gui_msg,"G");
	return (void *) 0;
}

void * gui_send_rt(void * p)
{
	char gui_msg[300] = "8,";
	strcat(gui_msg,MYID);
	strcat(gui_msg,",");
		
	pthread_mutex_lock (&mutex_routing);
	int i;
	for (i=0; i<node_table.rtCount; i++)
	{
		strcat(gui_msg,node_table.routing_table[i].dst_name);
		strcat(gui_msg,",");
		strcat(gui_msg,node_table.routing_table[i].next_hop);
		strcat(gui_msg,",");
		strcat(gui_msg,int_to_str(node_table.routing_table[i].cost));
		strcat(gui_msg,",");
		strcat(gui_msg,int_to_str(node_table.routing_table[i].timestamp));
		strcat(gui_msg,",");
	}
	pthread_mutex_unlock (&mutex_routing);
	send_data_to_dst(gui_msg,"G");
	return (void *) 0;
}
#endif /*GUI_H_*/
