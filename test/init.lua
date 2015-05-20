
print('Hellor Worlder!')

iojx.enable_termcb(iojx.current_context())

function spawn_co(file)
	iojx.co.spawn_process(file)
	return coroutine.yield()
end

function spawn_cotest(file)
	print('spawn_cotest: ready')
	spawn_co(file)
	print("spawn_cotest: end")

	-- iojx.disable_termcb(iojx.current_context())
end

-- coroutine.wrap(spawn_cotest)('./test_exec.py')

-- t = function ()
-- 	iojx.spawn_process('./test_exec.py', function () t() end) end
-- t()

local timer_a = iojx.timer.create(iojx.current_context())
iojx.timer.start(timer_a, 3, function ()

	print('timer_a (3) triggered')
	local timer_b = iojx.timer.create(iojx.current_context())

	iojx.timer.start(timer_b, 3, function ()
		print('timer_b (3) triggered, pid', iojx.util.getpid())
		local timer_e = iojx.timer.create(iojx.current_context())
		iojx.timer.start(timer_e, 6, function ()
			print('timer_e (6) triggered, pid', iojx.util.getpid())
		end)
	end)

	print('Parent ID: ', iojx.util.getpid())
	local pid = iojx.util.fork(function ()
		print("I'm the forked one.")
		print('My ID: ', iojx.util.getpid())

		iojx.util.exec('./test_exec.py', 'WTF!', 1)
		iojx.util.init_loop()

		local timer_d = iojx.timer.create(iojx.current_context())
		iojx.timer.start(timer_d, 1, function ()
			print('timer_d (1) triggered, pid', iojx.util.getpid())
		end)

		local timer_c = iojx.timer.create(iojx.current_context())
		iojx.timer.start(timer_c, 5, function ()
			print('timer_c (5) triggered, pid', iojx.util.getpid())
		end)
	end)
	print('Forked ID: ', pid)


end)


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
