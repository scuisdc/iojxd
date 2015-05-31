
local server = iojxx.ixlbx_tcp_base(iojx.current_context())

server:on_accept(function (ctx)
	print 'accepting connection ...'
end):on_read(function (ctx, data, len)
	print('Recved: ', data)
	ctx:write(data)
	if (data == 'CLOSE') then
		print('close recv')
		ctx:close()
		print('context closed')
		ctx:sock():close()
		print('exited')
	else
		iojxx.timer(iojx.current_context(), function ()
			print('closing connection')
			print('active', ctx:is_active())
			if ctx:is_active() then
				ctx:close()
			end
			iojxx.timer(iojx.current_context(), function ()
					print('collecting garbage 1')
					collectgarbage()
			end):start(2)
		end):start(4)
	end
end):on_close(function (ctx)
	print('conntection closed')
	iojxx.timer(iojx.current_context(), function ()
		print('collecting garbage 2')
		collectgarbage()
	end):start(2)
end):start('127.0.0.1', 6666)

print('exiting')