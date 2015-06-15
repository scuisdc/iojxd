
local lplatform = require 'lplatform'

local defaults_conf = {
	tmp_path = '/tmp/iojxd/',
	listen_address = '127.0.0.1',
	listen_port = 23333,
}

local defaults_args = nil

if lplatform.is_darwin() then

defaults_args = {
	source = { '/Users/secondatke/Desktop/apb.cpp' },
	language = 1,
	flags = '-O3 -g',
	data = '/Users/secondatke/Desktop/in.in',
	result = '/Users/secondatke/Desktop/out.out',
	time = 5, -- ms
	mem = 3, -- byte
	outfile = '/Users/secondatke/Desktop/out.txt',

	-- optional arguments
	c = '/usr/bin/clang',
	cxx = '/usr/bin/clang++',
	javac = '',
	java = '',
	py = '/usr/local/bin/python',
	js = 'jsrunner',
	lua = 'lua',

	syscall_whitelist = nil,
	sbuid = 2048,
	sbgid = 2048,

	dummy = ''
}

else

defaults_args = {
	source = { '/home/secondwtq/iojxd_test/apb.cpp' },
	language = 1,
	flags = '-O3 -g',
	data = '/home/secondwtq/iojxd_test/in.in',
	result = '/home/secondwtq/iojxd_test/out.out',
	time = 5, -- ms
	mem = 3, -- byte
	outfile = '/home/secondwtq/iojxd_test/out.txt',

	-- optional arguments
	c = '/usr/bin/gcc',
	cxx = '/usr/bin/g++',
	javac = '',
	java = '',
	py = '/usr/local/bin/python',
	js = 'jsrunner',
	lua = 'lua',

	syscall_whitelist = nil,
	sbuid = 2048,
	sbgid = 2048,

	dummy = ''
}

end

local parse = function (src, defaults)
	for i, v in pairs(defaults) do
		if src[i] == nil then
			src[i] = v
		end
	end

	return src
end

return {
	defaults_conf = defaults_conf,
	defaults_args = defaults_args,
	parse = parse
}
