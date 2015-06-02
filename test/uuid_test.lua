
local uuid = require 'uuid'

uuid.randomseed(os.time())

while true do
	print(os.time(), uuid())
end