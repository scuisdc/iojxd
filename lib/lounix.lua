local lounix = { }

local ffi = require 'ffi'

-- 1 second = 1,000,000 microseconds
ffi.cdef[[

	struct itimerval {
		struct timeval it_interval;	/* timer interval */
		struct timeval it_value;	/* current value */
	};

	int	setitimer(int which, const struct itimerval * value, struct itimerval * ovalue);

	pid_t fork();
	pid_t wait4(pid_t pid, int *stat_loc, int options, struct rusage *rusage);

	int execl(const char *path, const char *, ...);
	int execle(const char *path, const char *, ...);
	int execlp(const char *file, const char *, ...);
	int execv(const char *path, char * const argv[]);
	int execve(const char *file, char * const *, char * const *);
	int execvp(const char *file, char * const argv[]);

	unsigned int sleep(unsigned int interval);

	int	setgid(gid_t);
	int	setpgid(pid_t, pid_t);
	int	setuid(uid_t);

]]

lounix.SIGKILL = 9
lounix.SIGSEGV = 11
lounix.SIGPROF = 27

return lounix