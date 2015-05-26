
local pid = iojxx.fork(function ()
	print("I'm the forked one.")
	print('My PID: ', iojx.util.getpid())

	iojx.sandbox.reslimit(0, 1)
	-- iojx.sandbox.ixut_reslimit(9, 1024*1024)
	-- iojx.sandbox.ixut_reslimit(2, 1024*1024)
	iojx.util.freopen("a.txt", "w", iojx.util.stdout())
	iojx.util.freopen("a.txt", "w", iojx.util.stderr())

	-- iojx.util.exec('./testrlimit')
	iojx.util.exec('./testloop')
end).pid

iojxx.child_watcher(iojx.current_context(), function (watcher)
	print('process ended.')
	local status = watcher:get_status()
	print(status.status)
	print(status.exit_cause)
	print(status.termsig)
end):start(pid)
