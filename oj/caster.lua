local lcaster = { }

local languages = {
	C = 0,
	CXX = 1,
	Python = 2,
	Java = 3,
	JavaScript = 4,
	Lua = 5
}

lcaster.languages = languages

lcaster.needscompile = {
	[languages.C] = true,
	[languages.CXX] = true,
	[languages.Java] = true,
	[languages.Python] = false,
	[languages.JavaScript] = false,
	[languages.Lua] = false
}

lcaster.compiler_tag = {
	[languages.C] = 'c',
	[languages.CXX] = 'cxx',
	[languages.Java] = 'javac',
	[languages.Python] = 'dummy',
	[languages.JavaScript] = 'dummy',
	[languages.Lua] = 'dummy'
}

lcaster.runtime_tag = {
	[languages.C] = 'dummy',
	[languages.CXX] = 'dummy',
	[languages.Java] = 'java',
	[languages.Python] = 'py',
	[languages.JavaScript] = 'js',
	[languages.Lua] = 'lua'
}

lcaster.syscall_whitelist = {
	[languages.C] = { 'read', 'write', 'mmap', 'mprotect', 'munmap', 'exit_group', 'open', 'close',
			'arch_prctl', 'fstat', 'access', 'brk' },
	[languages.CXX] = { 'read', 'write', 'mmap', 'mprotect', 'munmap', 'exit_group', 'open', 'close',
			'arch_prctl', 'fstat', 'access', 'brk' },
	[languages.Java] = { },
	[languages.Python] = { 'read', 'write', 'mmap', 'munmap', 'mprotect', 'exit_group', 'open', 
			'close', 'brk', 'fstat', 'lstat', 'rt_sigaction', 'lseek', 'ioctl', 'fcntl', 'readlink', 'getrlimit',
			'rt_sigprocmask', 'stat', 'set_robust_list', 'futex', 'access', 'set_tid_address',
			'arch_prctl', 'getuid', 'getgid', 'geteuid', 'getegid', 'getdents' },
	[languages.JavaScript] = { },
	[languages.Lua] = { }
}

-- AC, TLE, RE, CE, WA, MLE
lcaster.status_code = {
	[0] = 'UNKNOWN',
	[200] = 'AC',
	[504] = 'TLE',
	[404] = 'WA',
	[413] = 'MLE', -- Memory Limit Exceeded
	[500] = 'RE', -- Runtime Error
	[400] = 'CE' -- Compilation Error
}

lcaster.status = { }
for i, v in pairs(lcaster.status_code) do
	lcaster.status[v] = i
end

return lcaster
