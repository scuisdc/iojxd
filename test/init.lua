
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

local sock_server = iojx.sock.create(iojx.current_context())
iojx.sock.tcp.bind_ip(sock_server, '127.0.0.1', 6666)

iojx.sock.set_read_callback(sock_server, function (ctx, data, len)
	iojx.spawn_process(data, function ()
		iojx.sock.write(ctx, 'finished', #'finished')
	end)
end)

iojx.sock.tcp.listen(sock_server)