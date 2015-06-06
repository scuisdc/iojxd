
local lolinux = { }

local ffi = require 'ffi'

ffi.cdef[[

	typedef long time_t;
	typedef long suseconds_t;
	typedef int pid_t;
	typedef unsigned int uid_t;
	typedef unsigned int gid_t;

	struct timeval {
		time_t tv_sec;	        /* seconds */
		suseconds_t tv_usec;        /* and microseconds */
	};

	struct rusage {
		struct timeval ru_utime;	/* user time used (PL) */
		struct timeval ru_stime;	/* system time used (PL) */
		long	ru_maxrss;		/* max resident set size (PL) */
		long	ru_ixrss;		/* integral shared memory size (NU) */
		long	ru_idrss;		/* integral unshared data (NU)  */
		long	ru_isrss;		/* integral unshared stack (NU) */
		long	ru_minflt;		/* page reclaims (NU) */
		long	ru_majflt;		/* page faults (NU) */
		long	ru_nswap;		/* swaps (NU) */
		long	ru_inblock;		/* block input operations (atomic) */
		long	ru_oublock;		/* block output operations (atomic) */
		long	ru_msgsnd;		/* messages sent (atomic) */
		long	ru_msgrcv;		/* messages received (atomic) */
		long	ru_nsignals;		/* signals received (atomic) */
		long	ru_nvcsw;		/* voluntary context switches (atomic) */
		long	ru_nivcsw;		/* involuntary " */
	};

]]

lolinux.ITIMER_REAL = 0
lolinux.ITIMER_VIRTUAL = 1
lolinux.ITIMER_PROF = 2

lolinux.RLIMIT_CPU = 0
lolinux.RLIMIT_FSIZE = 1
lolinux.RLIMIT_DATA = 2
lolinux.RLIMIT_STACK = 3
lolinux.RLIMIT_CORE = 4
lolinux.RLIMIT_AS = 9
lolinux.RLIMIT_RTTIME = 15

local lsyscallid = require 'lsyscallid'
lolinux.id_syscall = lsyscallid.id_syscall_linux
lolinux.syscall_id = lsyscallid.syscall_id_linux

return lolinux
