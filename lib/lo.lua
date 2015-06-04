
-- a wrapper package to implement package
-- selection among different platforms

local lo = nil

local lplatform = require 'lplatform'

if lplatform.is_darwin() then lo = require 'lodarwin'
elseif lplatform.is_linux() then lo = require 'lolinux' end

return lo