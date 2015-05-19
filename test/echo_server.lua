
local sock_server = iojx.sock.create(iojx.current_context())
iojx.sock.tcp.bind_ip(sock_server, '127.0.0.1', 6999)
iojx.sock.set_read_callback(sock_server, function (ctx, data, len)
	print(data)
	iojx.sock.write(ctx, data, len)
end)
iojx.sock.tcp.listen(sock_server)
