local ffi = require 'ffi'

local lo = require 'lolinux'
local lounix = require 'lounix'

local ptrace = require 'lolinuxptrace'
local lutil = require 'lutil'
local laoj = require 'laoj'

local whitelist = {
	'read', 'write', 'mmap', 'mprotect', 'munmap', 'exit_group', 'open', 'close',
	'arch_prctl', 'fstat', 'access', 'brk'
}

local pid = iojxx.fork(function ()
	print("I'm the forked one.")
	print('My PID: ', iojx.util.getpid())

	-- iojx.util.freopen("a.txt", "w", iojx.util.stdout())
	-- iojx.util.freopen("a.txt", "w", iojx.util.stderr())
	-- iojx.sandbox.reslimit(0, 1)
	-- iojx.sandbox.reslimit(1, 1024*1024)
	-- iojx.sandbox.reslimit(2, 1024*1024)

	local pid_inside = ffi.C.fork()
	if pid_inside == 0 then
		local val = ffi.new('struct itimerval')
		val.it_interval.tv_sec = 0
		val.it_interval.tv_usec = 0
		val.it_value.tv_sec = 0
		val.it_value.tv_usec = 700000
		ffi.C.setitimer(lo.ITIMER_PROF, val, nil)

		lutil.ptrace(ptrace.PTRACE_TRACEME, 0)

		iojx.util.exec('./testsort')
	else
		local inited = false
		while true do
			local ret, status_final, rusage = lutil.wait4(pid_inside)

			if not laoj.WIFEXITED(status_final) then
				local regs = ffi.new('struct user_regs_struct')
				ffi.C.ptrace(ptrace.PTRACE_GETREGS, pid_inside, nil, regs)
				local syscall_name = laoj.syscall_name(tonumber(regs.orig_rax))
				print("syscall ", syscall_name)
				if not inited then
					inited = true
					lutil.ptrace(ptrace.PTRACE_SYSCALL, pid_inside)
				else
					if not lutil.inside(whitelist, syscall_name) then
						print('detected invalid syscall ' .. syscall_name)
						lutil.ptrace(ptrace.PTRACE_KILL, pid_inside)
					else
						lutil.ptrace(ptrace.PTRACE_SYSCALL, pid_inside)
					end
				end
			else
				print(laoj.WEXITSTATUS(status_final), laoj.WTERMSIG(status_final), time_final)
				break
			end
		end
	end

end).pid

iojxx.child_watcher(iojx.current_context(), function (watcher)
	print('process ended.')
	local status = watcher:get_status()
	print(status.status)
	print(status.exit_cause)
	print(status.termsig)
end):start(pid)
