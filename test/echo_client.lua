
local sock_client = iojx.sock.create(iojx.current_context())
iojx.sock.tcp.connect(sock_client, '127.0.0.1', 6666, function (ctx)
	local msg = 'Do you have foundation?'
	iojx.sock.write(ctx, msg, #msg)
end, function (sock)
	print 'connection failed.'
end)

iojx.sock.set_read_callback(sock_client, function (ctx, data, len)
	print(data)
	iojx.sock.write(ctx, data, len)
end)
