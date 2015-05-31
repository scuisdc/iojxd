
local lutil = require 'lutil'
local lcaster = require 'lcaster'

local lo = require 'lodarwin'
local lounix = require 'lounix'
local laoj = require 'laoj'

local lfs = require 'lfs'

local ffi = require 'ffi'

local example_args = {
	source = { '/Users/secondatke/Desktop/apb.cpp' },
	language = 1,
	flags = '-O3 -g',
	data = '/Users/secondatke/Desktop/testdata',
	result = '/Users/secondatke/Desktop/testout',
	time = 256, -- ms
	mem = 3, -- byte
	outfile = '/Users/secondatke/Desktop/out.txt',

	-- optional arguments
	c = 'clang',
	cxx = '/usr/bin/clang++',
}

function write_status(args, status)
	local out = io.open(args.outfile, 'a')
	print('writing to ', tostring(lcaster.status[status]))
	out:write(tostring(id) .. ' ' .. tostring(lcaster.status[status]) .. '\n')
	out:close()
	print('------ STATUS ------')
	print(lcaster.status_code[status.status])
	print(status.compiler_log)
end

function write_status_test(id, status, args)
	local out = io.open(args.outfile, 'a')
	print('writing to ', tostring(lcaster.status[status]))
	out:write(tostring(id) .. ' ' .. tostring(lcaster.status[status]) .. '\n')
	out:close()
end

function judge(args)
	print('iojxd - received judge command ..')

	if lcaster.needscompile[args.language] then
		local compiler = args[lcaster.compiler_tag[args.language]]
		print(compiler, args.source[1])
		local pid_compiler = iojxx.fork(function ()
			-- iojx.util.freopen("compiler.log", "w", iojx.util.stdout())
			print('I\'m the compiler')
			iojx.util.freopen("compiler.log", "w", iojx.util.stderr())
			-- ffi.C.sleep(1)

			iojx.util.exec(compiler, unpack(args.source))
		end).pid
		iojxx.child_watcher(iojx.current_context(), function (watcher)
			local status = watcher:get_status()
			if status.status ~= 0 then

				local log = io.open("compiler.log", 'r')
				local log_content = log:read("*all")
				log:close()
				write_status(args, 'CE', log_content)
				
			else

				if string.sub(args.data, #args.data, #args.data) ~= '/' then
					args.data = args.data .. '/'
				end
				if string.sub(args.result, #args.result, #args.result) ~= '/' then
					args.result = args.result .. '/'
				end
				local datagrp = lutil.count_files(args.data)
				print('iojxd - detected ' .. tostring(datagrp) .. ' test cases')

				local run_test = function (cur_id)
					local pid_d = iojxx.fork(function ()
						local pid_in = ffi.C.fork()
						if pid_in == 0 then
							iojx.util.freopen(args.data .. cur_id .. '.txt', 'r', iojx.util.stdin())
							iojx.util.freopen(cur_id .. '_tmp.txt', 'w', iojx.util.stdout())

							-- iojx.sandbox.reslimit(lo.RLIMIT_AS, args.mem)
							-- iojx.sandbox.reslimit(lo.RLIMIT_DATA, args.mem)
							-- iojx.sandbox.reslimit(lo.RLIMIT_RSS, args.mem)
							
							iojx.util.exec('./a.out')
						else
							while true do
								local ret, status_final, rusage = lutil.wait4(pid_in)
								local exitstatus, termsig = laoj.WEXITSTATUS(status_final), laoj.WTERMSIG(status_final)

								if laoj.WIFEXITED(status_final) then
									iojx.util.init_loop()
									print(status_final, exitstatus, termsig)
									laoj.fork(function ()
											iojx.util.freopen(cur_id .. '_diff.txt', 'w', iojx.util.stdout())
										end, 'diff', cur_id .. '_tmp.txt', args.result .. cur_id .. '.txt', function (status)
											if lfs.attributes(cur_id .. '_diff.txt').size == 0 then
												write_status_test(cur_id, 'AC', args)
											else
												write_status_test(cur_id, 'WA', args)
											end
										end)
									iojx.util.run()
									break
								elseif laoj.WIFSIGNALED(status_final) then
									print(status_final, exitstatus, termsig)
									if termsig == 11 then -- SIGSEGV
										write_status_test(cur_id, 'RE', args)
										break
									end
									break
								end
							end

						end
					end).pid
				end

				for i = 1, datagrp do run_test(i) end

			end
		end):start(pid_compiler)
	else

	end
end

function execute_cmd(cmd, ctx)
	print('iojxd - executing command ', cmd)
	if cmd == 'AUTH' then
		ctx:write('1')
		ctx.data = { authed = true, context = {
					execute = judge, example_args = example_args,
					sock = ctx,
					}	
		}
	end
end

iojxx.ixlbx_tcp_base(iojx.current_context()):
on_accept(function ()
	print('iojxd - accepting connection ...')

end):on_close(function (ctx)
	print('iojxd - closing connection ...')

end):on_read(function (ctx, data, len)
	print('iojxd - received ', data)

	-- identified as a system command if first char is '$'
	if len > 1 and string.sub(data, 1, 1) == '$' then
		execute_cmd(string.sub(data, 2, len), ctx)
	else
		-- are you authed?
		if ctx.data ~= nil and ctx.data.authed == true then

			local f, msg = loadstring(data)
			if f == nil then
				ctx:write('0\n' .. msg)
			else
				setfenv(f, ctx.data.context)
				local succeeded, result = pcall(f)
				if succeeded then ctx:write('1')
				else ctx:write('0\n' .. result) end
			end

		else
			ctx:write('0\nyou are not authed.')
			ctx:close()
		end
	end
end):start('127.0.0.1', 23333)
