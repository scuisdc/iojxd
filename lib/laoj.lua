local laoj = { }

local function dummy () end

laoj.dummy = dummy

laoj.freopen = iojx.util.freopen
laoj.fflush = iojx.util.fflush

laoj.stdin = iojx.util.stdin
laoj.stdout = iojx.util.stdout
laoj.stderr = iojx.util.stderr

laoj.fork = function (prerun, program, ...)

	local args = { ... }

	if prerun == nil then prerun = dummy end
	local func_cb = dummy

	if #args >= 1 and type(args[#args]) == 'function' then
		func_cb = args[#args]
		args[#args] = nil
	end

	local pid = iojxx.fork(function ()
		prerun()
		iojx.util.exec(program, unpack(args))
	end).pid
	
	iojxx.child_watcher(iojx.current_context(), function (watcher)
		func_cb(watcher:get_status())
	end):start(pid)

end

laoj.syscall_name = iojx.platform.syscall_name

laoj.WIFEXITED = iojx.util.WIFEXITED
laoj.WEXITSTATUS = iojx.util.WEXITSTATUS
laoj.WTERMSIG = iojx.util.WTERMSIG
laoj.WIFSIGNALED = iojx.util.WIFSIGNALED
laoj.WIFSTOPPED = iojx.util.WIFSTOPPED
laoj.WSTOPSIG = iojx.util.WSTOPSIG

return laoj