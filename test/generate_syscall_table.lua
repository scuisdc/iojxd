
print('{')

for i = 0, 512 do
	local name = iojx.platform.syscall_name(i)
	if name ~= 'unknown' then
		print('\t[' .. i .. '] = ' .. "'" .. name .. "'" .. ',')
	end
end

print('}')