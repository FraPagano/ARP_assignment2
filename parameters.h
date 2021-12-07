#define SIZE 1000
#define CBUFFER_SIZE 1000
#define MAX 250000
#define BUFFER_NOELEMENT 250
#define SNAME_MUTEX "/sem_mutex"
#define SNAME_NOTFULL "/sem_not_full"
#define SNAME_NOTEMPTY "/sem_not_empty"
#define SHM_NAME "/shm"
#define PIPE_PATH "/tmp/pipe_data"
#define TSTART_PATH "/tmp/pipe_start_time"
#define TEND_PATH "/tmp/pipe_end_time"

/* Defining CHECK() tool. By using this methid the code results ligher and cleaner. */
#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stdout,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

void send_start_time ();
void send_end_time ();