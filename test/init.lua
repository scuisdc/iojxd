
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

local sock_client = iojx.sock.create(iojx.current_context())
iojx.sock.tcp.connect(sock_client, '127.0.0.1', 6666, function (ctx)
	timer = iojx.timer.create(iojx.current_context())
	iojx.timer.setimeout(timer)
	iojx.timer.start(timer, 5, function (_timer)
		print('timeout')
	end)
end)

iojx.sock.set_read_callback(sock_client, function (ctx, data, len)
	iojx.timer.tick(timer)
	print(data)
	iojx.sock.write(ctx, data, len)
end)