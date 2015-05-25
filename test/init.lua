
inside = function (array, element, cmp)
	if cmp == nil then cmp = function (x, y) return x == y end end
	for i, v in ipairs(array) do
		if cmp(v, element) then return true end
	end
	return false
end

sock_write = function (sock, data)
	data = tostring(data)
	iojx.sock.write(sock, data, #data)
end

authed_users = { }

local server = iojx.sock.create(iojx.current_context())

iojx.sock.tcp.bind_ip(server, '127.0.0.1', 6666)
iojx.sock.set_read_callback(server, function (ctx, data, len)
	print('calling read callback ...')
	local message = '500'
	-- ATTENTION: Do not compare two LuaBridge userdatas directly!
	if inside(authed_users, ctx, function (x, y) return x.fd == y.fd end) then
		message = 'You are authed, executing your task ...'
		iojx.sock.write(ctx, message, #message)

		local context = {

		}
		
		local f = loadstring(data)
		if f ~= nil then
			setfenv(f, context)

			local succeeded, result = pcall(f)
			sock_write(ctx, tostring(result))
		end
	else
		if string.sub(data, 1, 5) == '$AUTH' then
			table.insert(authed_users, ctx)
			message = 'You are now authed ...'
			iojx.sock.write(ctx, message, #message)
		else
			message = 'You are not authed to do anything ...'
			iojx.sock.write(ctx, message, #message)
		end
	end
	print(message)
end)

iojx.sock.tcp.listen(server)
