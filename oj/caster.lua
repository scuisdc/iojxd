local lcaster = { }

local languages = {
	C = 0,
	CXX = 1
}

lcaster.languages = languages

lcaster.needscompile = {
	[languages.C] = true,
	[languages.CXX] = true,
}

lcaster.compiler_tag = {
	[languages.C] = 'c',
	[languages.CXX] = 'cxx'
}

lcaster.syscall_whitelist = {
	[languages.C] = { 'read', 'write', 'mmap', 'mprotect', 'munmap', 'exit_group', 'open', 'close',
	'arch_prctl', 'fstat', 'access', 'brk' },
	[languages.CXX] = { 'read', 'write', 'mmap', 'mprotect', 'munmap', 'exit_group', 'open', 'close',
	'arch_prctl', 'fstat', 'access', 'brk' }
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
