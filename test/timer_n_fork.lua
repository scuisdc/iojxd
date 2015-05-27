
print('Hellor Worlder!')

iojx.enable_termcb(iojx.current_context())

iojxx.timer(iojx.current_context(), function ()

	print('timer_a (3) triggered')

	iojxx.timer(iojx.current_context(), function ()
		print('timer_b (3) triggered, pid', iojx.util.getpid())

		iojxx.timer(iojx.current_context(), function ()
			print('timer_e (6) triggered, pid', iojx.util.getpid())
		end):start(6)
	end):start(3)

	print('Parent ID: ', iojx.util.getpid())
	local pid = iojxx.fork(function ()
		print("I'm the forked one.")
		print('My PID: ', iojx.util.getpid())

		iojx.util.exec('./test_exec.py', 'WTF!', 1)
	end).pid
	print('Forked ID: ', pid)
	iojxx.child_watcher(iojx.current_context(), function ()
		print('process ended.')
	end):start(pid)

end):start(3)

