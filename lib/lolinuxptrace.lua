
local lolinuxptrace = { }

local ffi = require 'ffi'

ffi.cdef[[

struct user_regs_struct {
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long rbp;
	unsigned long rbx;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long rax;
	unsigned long rcx;
	unsigned long rdx;
	unsigned long rsi;
	unsigned long rdi;
	unsigned long orig_rax;
	unsigned long rip;
	unsigned long cs;
	unsigned long eflags;
	unsigned long rsp;
	unsigned long ss;
	unsigned long fs_base;
	unsigned long gs_base;
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;
};

long int ptrace(int request, pid_t pid, void *addr, void *data);

]]

lolinuxptrace.PTRACE_TRACEME = 0
lolinuxptrace.PTRACE_PEEKTEXT = 1
lolinuxptrace.PTRACE_PEEKDATA = 2
lolinuxptrace.PTRACE_PEEKUSER = 3
lolinuxptrace.PTRACE_POKETEXT = 4
lolinuxptrace.PTRACE_POKEDATA = 4
lolinuxptrace.PTRACE_POKEUSER = 6
lolinuxptrace.PTRACE_CONT = 7
lolinuxptrace.PTRACE_KILL = 8
lolinuxptrace.PTRACE_SINGLESTEP = 9
lolinuxptrace.PTRACE_GETREGS = 12
lolinuxptrace.PTRACE_ATTACH = 16
lolinuxptrace.PTRACE_DETACH = 17
lolinuxptrace.PTRACE_SYSCALL = 24

return lolinuxptrace
