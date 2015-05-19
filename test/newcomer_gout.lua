
local first_client = nil

local sock_server = iojx.sock.create(iojx.current_context())
iojx.sock.tcp.bind_ip(sock_server, '127.0.0.1', 6999)

iojx.sock.set_read_callback(sock_server, function (ctx, data, len)
	if first_client == nil then
		first_client = ctx
		iojx.sock.write(ctx, 'Welcome!', #'Welcome!')
	elseif ctx.fd ~= first_client.fd then
		iojx.sock.write(ctx, 'Go Away!', #'Go Away!')
	end
end)

iojx.sock.tcp.listen(sock_server)