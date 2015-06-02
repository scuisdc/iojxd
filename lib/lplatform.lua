
local ffi = require 'ffi'

local is_darwin = function ()
	return ffi.os == 'OSX' end

local is_linux = function ()
	return ffi.os == 'Linux' end

return { is_darwin = is_darwin, is_linux = is_linux }