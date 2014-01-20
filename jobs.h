#define MAX_JOBS 10

extern int job_index;
typedef struct job_t
{
	pid_t pid;
	int index;
};

extern job_t* jobs[MAX_JOBS];
