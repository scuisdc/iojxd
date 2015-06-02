
local defaults_conf = {
	tmp_path = '/tmp/iojxd/',
}

local defaults_args = {
	source = { '/Users/secondatke/Desktop/apb.cpp' },
	language = 1,
	flags = '-O3 -g',
	data = '/Users/secondatke/Desktop/in.in',
	result = '/Users/secondatke/Desktop/out.out',
	time = 5, -- ms
	mem = 3, -- byte
	outfile = '/Users/secondatke/Desktop/out.txt',

	-- optional arguments
	c = 'clang',
	cxx = '/usr/bin/clang++'
}

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