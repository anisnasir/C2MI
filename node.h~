#ifndef NODE_H_
#define NODE_H_

#include "constants.h"

int dialup_fd;
///////////////////////////////// TABLES //////////////////////////////
struct interface_info{
	char  name[20];
	char  ip[20];
};

struct neighbor{
	char name[20];
	char ip[20];
	int cost;
	int type;
	unsigned long timestamp;
	
};

struct routing_entry{
	char dst_name[20];
	char next_hop[20];
	int cost;
	unsigned long timestamp;
};


struct request_entry{
	char dst_name[20];
	char from_hop[20];
	int mark;
	int repCost;
	unsigned long timestamp;
};


struct node{	
	struct interface_info interface_table[10];
	int itCount;
	
	struct neighbor neighbor_table[100];
	int ntCount;
	
	struct routing_entry routing_table[100];
	int rtCount;
	

	struct routing_entry backup_table[100];
	int btCount;
	
	struct request_entry request_table[20];
  	int reCount;
	
};

//////////////////////////// VARIABLES AND INITIALIZATION ///////////////////////
struct node node_table;
pthread_mutex_t mutex_neighbor;
pthread_mutex_t mutex_interface;
pthread_mutex_t mutex_routing;
pthread_mutex_t mutex_backup;
pthread_mutex_t mutex_request;

void initialize_node_table()
{
	pthread_mutex_init(&mutex_neighbor, NULL);
	pthread_mutex_init(&mutex_interface, NULL);
	pthread_mutex_init(&mutex_routing, NULL);
	pthread_mutex_init(&mutex_backup, NULL);
	pthread_mutex_init(&mutex_request, NULL);
	
	node_table.ntCount = 0;
	node_table.itCount = 0;
	node_table.rtCount = 0;
	node_table.btCount = 0;
	node_table.reCount = 0;
	
	strcpy(node_table.routing_table[node_table.rtCount].dst_name,"G");
	node_table.routing_table[node_table.rtCount].cost = 5;
	strcpy(node_table.routing_table[node_table.rtCount].next_hop,"192.168.0.100");
	node_table.routing_table[node_table.rtCount].timestamp = 786;
	node_table.rtCount++;
	
	strcpy(node_table.backup_table[node_table.btCount].dst_name,"G");
	node_table.backup_table[node_table.btCount].cost = 5;
	strcpy(node_table.backup_table[node_table.btCount].next_hop,"192.168.0.100");
	node_table.backup_table[node_table.btCount].timestamp = 786;
	node_table.btCount++;
}
/////////////////////////// NEIGHBOR ///////////////////////
void display_neighbor_table()
{
	int i;
	printf("\n NEIGHBOR TABLE\n================\n");
	
	printf("type \t name \t ip \t cost \t TimeStamp \n-----------------------------------------------\n");
	for (i =0 ; i<node_table.ntCount; i++ )
	{ 	
		printf("%d \t %s \t %s \t %d \t %ld \n",node_table.neighbor_table[i].type,node_table.neighbor_table[i].name,
			node_table.neighbor_table[i].ip,node_table.neighbor_table[i].cost,
			node_table.neighbor_table[i].timestamp);
	}
	printf("-----------------------------------------------\n");
	
}

int find_in_nt(char* ip,char * name)
{
	int i;
	for (i=0; i<node_table.ntCount; i++)
	{
		if( strcmp(ip, node_table.neighbor_table[i].ip) == 0 )	
		{
			//printf("ip %s found in neighbor table at entry %d \n",ip,i);
			return i;	
		}
	}
	return -1;
}


int find_in_rt(char name[])
{
	int i;
	for (i=0; i<node_table.rtCount; i++)
	{
		if (strcmp(node_table.routing_table[i].dst_name,name) == 0)
			return i;
	}
	return -1;
}


int find_in_bt(char  name[])
{
	int i;
	for (i=0; i<node_table.btCount; i++)
	{
		if( strcmp(name, node_table.backup_table[i].dst_name) == 0 )	
		{
			//printf("ip %s found in neighbor table at entry %d \n",ip,i);
			return i;	
		}
	}
	return -1;
}

int find_in_re(char name[])
{
	int i;
	for(i=0;i<node_table.reCount;i++)
		if(strcmp(node_table.request_table[i].dst_name,name) == 0)
			return i;
	return -1;
}

//////////////////////////////// INTERFACES //////////////////////////
void display_interface_table()
{
	int i;
	printf("\n INTERFACE TABLE\n================\n");
	
	printf("Name \t ip \n-----------------------------------------------\n");
	for (i =0 ; i<node_table.itCount; i++ )
	{ 	
		printf("%s \t %s \n",node_table.interface_table[i].name,node_table.interface_table[i].ip);
	}
	printf("-----------------------------------------------\n");
}


///////////////////////////////////////////////////////
void display_backup_table()
{
	int i;
	printf("\n BACKUP TABLE\n================\n");
	
	printf("Des \t nextHop \t Cost \t TimeStamp \n-----------------------------------------------\n");
	for (i =0 ; i<node_table.btCount; i++ )
	{ 	
		printf("%s \t %s \t %d \t %ld \n",node_table.backup_table[i].dst_name
		,node_table.backup_table[i].next_hop,
		node_table.backup_table[i].cost,node_table.backup_table[i].timestamp);
	}
	printf("-----------------------------------------------\n");
}

/////////////////////////////////////////////////////////////
void display_routing_table()
{
	int i;
	printf("\n ROUTING TABLE\n================\n");
	
	printf("Des \t nextHop \t Cost \t TimeStamp \n-----------------------------------------------\n");
	for (i =0 ; i<node_table.rtCount; i++ )
	{ 	
		printf("%s \t %s \t %d \t %ld \n",node_table.routing_table[i].dst_name
		,node_table.routing_table[i].next_hop,
		node_table.routing_table[i].cost,node_table.routing_table[i].timestamp);
	}
	printf("-----------------------------------------------\n");
}

/////////////////////////////////////////////////////////////
void display_request_table()
{
	int i;
	printf("\n REQUEST TABLE\n================\n");
	
	printf("Des \t nextHop \t mark \t repCost \t TimeStamp \n---------------------------------------------------\n");
	for (i =0 ; i<node_table.reCount; i++ )
	{ 	
		printf("%s \t %s \t %d \t %d \t %ld \n",node_table.request_table[i].dst_name,
		node_table.request_table[i].from_hop,
		node_table.request_table[i].mark,
		node_table.request_table[i].repCost,
		node_table.request_table[i].timestamp);
	}
	printf("---------------------------------------------------\n");
}


#endif /*NODE_H_*/
