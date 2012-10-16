#ifndef THREAD_MSGS_H_
#define THREAD_MSGS_H_

struct handle_th_msg
{
	char  fullmsg[MAXBUFLEN];
	char  src_addr[20];
};

struct send_rreq_thmsg
{
	char desName[20];
};
#endif /*THREAD_MSGS_H_*/
