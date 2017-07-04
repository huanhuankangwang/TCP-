//为了实现多用户交互

#define        SERVER_POOL_SIZE			20
#define        READ_BUFFER_SIZE			1024

typedef struct _server_pool_t{
	int  client_fd;
	int  server_fd;

	int  send_fd;

	unsigned char 	read_buf[READ_BUFFER_SIZE];
	unsigned char   write_buf[READ_BUFFER_SIZE];
	pthread_mutex_t write_mutex;
	pthread_mutex_t read_mutex;
}T_Server_Pool,*PT_Server_Pool;

static PT_Server_Pool  server_pool[SERVER_POOL_SIZE];

PT_Server_Pool    server_pool_moalloc()
{
	PT_Server_Pool pool = malloc(sizeof(T_Server_Pool));
	return pool;
}


void init_server_pool(PT_Server_Pool pool,int server_fd,int client_fd)
{
	
}
