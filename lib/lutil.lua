local lutil = { }

local lfs = require 'lfs'

local inside = function (array, element, cmp)
	if cmp == nil then cmp = function (x, y) return x == y end end
	for i, v in ipairs(array) do
		if cmp(v, element) then return true end
	end
	return false
end

-- input: rusage tpye
-- output: usertime + system time in second
local rusage_time = function (rusage)
	local sec = tonumber(rusage.ru_utime.tv_sec) + tonumber(rusage.ru_stime.tv_sec)
	local usec = tonumber(rusage.ru_utime.tv_usec) + tonumber(rusage.ru_stime.tv_usec)
	return (sec + usec / 1000000.0)
end

lutil.inside = inside
lutil.rusage_time = rusage_time

local ffi = require 'ffi'

-- a helper function to simplify code like:
-- 	ffi.C.ptrace(ptrace.PTRACE_SYSCALL, pid_inside, nil, nil)
lutil.ptrace = function (request, pid)
	ffi.C.ptrace(request, pid, nil, nil)
end

-- helper function to simplify boilerplate code for wait4()
-- output: pid, status, rusage
lutil.wait4 = function (pid)
	local rusage = ffi.new('struct rusage')
	local status = ffi.new('int[1]')
	local ret = ffi.C.wait4(pid, status, 0, rusage)
	return ret, tonumber(status[0]), rusage
end

-- too simple
lutil.count_files = function (dir)
	local ret = 0
	for file in lfs.dir(dir) do
		if file ~= '.' and file ~= '..' then
			ret = ret + 1
		end
	end
	return ret
end

return lutil