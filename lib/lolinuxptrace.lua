
local lolinuxptrace = { }

local ffi = require 'ffi'

ffi.cdef[[

struct user_regs_struct {
	unsigned long long int r15;
	unsigned long long int r14;
	unsigned long long int r13;
	unsigned long long int r12;
	unsigned long long int rbp;
	unsigned long long int rbx;
	unsigned long long int r11;
	unsigned long long int r10;
	unsigned long long int r9;
	unsigned long long int r8;
	unsigned long long int rax;
	unsigned long long int rcx;
	unsigned long long int rdx;
	unsigned long long int rsi;
	unsigned long long int rdi;
	unsigned long long int orig_rax;
	unsigned long long int rip;
	unsigned long long int cs;
	unsigned long long int eflags;
	unsigned long long int rsp;
	unsigned long long int ss;
	unsigned long long int fs_base;
	unsigned long long int gs_base;
	unsigned long long int ds;
	unsigned long long int es;
	unsigned long long int fs;
	unsigned long long int gd;
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