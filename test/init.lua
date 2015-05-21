
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

--local sock_client = iojx.sock.create(iojx.current_context())
--iojx.sock.tcp.connect(sock_client, '127.0.0.1', 6666, function (ctx)
	--timer = iojx.timer.create(iojx.current_context())
	--iojx.timer.setimeout(timer)
	--iojx.timer.start(timer, 5, function (_timer)
		--print('timeout')
	--end)
--end)

--iojx.sock.set_read_callback(sock_client, function (ctx, data, len)
	--iojx.timer.tick(timer)
	--print(data)
	--iojx.sock.write(ctx, data, len)
--end)
