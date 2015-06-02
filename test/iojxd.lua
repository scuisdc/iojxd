
local lutil = require 'lutil'
local lcaster = require 'lcaster'

local lo = require 'lo'
local lounix = require 'lounix'
local laoj = require 'laoj'

local lfs = require 'lfs'
local uuid = require 'uuid'

local ffi = require 'ffi'

local config_parse = require 'config_parse'
local config = require 'config'

local iojxd_init = function ()
	uuid.randomseed(os.time())

	config = config_parse.parse(config, config_parse.defaults_conf)
	config.tmp_path = lutil.add_slash(config.tmp_path)

	local attr_tmp, msg = lfs.attributes(config.tmp_path)
	if attr_tmp == nil then
		lfs.mkdir(config.tmp_path)
	end
end

function delayed_gc(delay)
	if delay == nil then delay = 2 end
	iojxx.timer(iojx.current_context(), function ()
		print('iojxd - delayed_gc: collecting garbage')
		collectgarbage()
	end):start(delay)
end

function write_status(args, status, time, mem, log)
	local out = io.open(args.outfile, 'w')
	print('writing to ' .. status)
	out:write(tostring(lcaster.status[status]) .. '\n')
	out:write(tostring(time) .. ' ' .. tostring(mem) .. '\n')
	if log ~= nil then out:write(log) end
	out:close()
end

function judge(args)
	print('iojxd - received judge command ..')
	args = config_parse.parse(args, config_parse.defaults_args)

	local session_uuid = uuid()
	print('iojxd - session uuid ' .. session_uuid)

	local session_dir = config.tmp_path .. session_uuid .. '/'
	local compiler_logpath = session_dir .. 'compiler.log'
	local execpath = session_dir .. 'a.out'
	local outputpath = session_dir .. 'tmp.out'
	local diffpath = session_dir .. 'diff.diff'
	lfs.mkdir(session_dir)

	if lcaster.needscompile[args.language] then
		local compiler = 'g++' -- args[lcaster.compiler_tag[args.language]]
		print(compiler, args.source[1])
		local pid_compiler = iojxx.fork(function ()
			iojx.util.freopen(compiler_logpath, "w", iojx.util.stderr())

			iojx.util.exec(compiler, unpack(args.source), '-o', execpath)
		end).pid
		iojxx.child_watcher(iojx.current_context(), function (watcher)
			local status = watcher:get_status()
			if status.status ~= 0 then

				local log = io.open(compiler_logpath, 'r')
				local log_content = log:read("*all")
				log:close()
				write_status(args, 'CE', 0, 0, log_content)

			else

				local pid_d = iojxx.fork(function ()
					local pid_in = ffi.C.fork()
					if pid_in == 0 then
						iojx.util.freopen(args.data, 'r', iojx.util.stdin())
						iojx.util.freopen(outputpath, 'w', iojx.util.stdout())

						local val = ffi.new('struct itimerval')
						val.it_interval.tv_sec, val.it_interval.tv_usec = 0, 0
						-- we just passed raw time here
						val.it_value.tv_sec, val.it_value.tv_usec = math.floor(args.time / 1000), (args.time % 1000) * 1000
						ffi.C.setitimer(lo.ITIMER_PROF, val, nil)

						-- iojx.sandbox.reslimit(lo.RLIMIT_AS, args.mem)
						-- iojx.sandbox.reslimit(lo.RLIMIT_DATA, args.mem)
						-- iojx.sandbox.reslimit(lo.RLIMIT_RSS, args.mem)

						iojx.util.exec(execpath)
					else
						while true do
							local ret, status_final, rusage = lutil.wait4(pid_in)
							local exitstatus, termsig = laoj.WEXITSTATUS(status_final), laoj.WTERMSIG(status_final)

							local time_ms = lutil.rusage_time(rusage) * 1000
							local mem = tonumber(rusage.ru_idrss)

							if laoj.WIFEXITED(status_final) then
								iojx.util.init_loop()
								print(status_final, exitstatus, termsig)
								laoj.fork(function ()
										iojx.util.freopen(diffpath, 'w', iojx.util.stdout())
									end, 'diff', outputpath, args.result, function (status)
										if lfs.attributes(diffpath).size == 0 then
											write_status(args, 'AC', time_ms, mem)
										else write_status(args, 'WA', time_ms, mem) end

										-- that's a bug, it cannot exit from event loop
										-- automatically (maybe there are other watchers alive)
										-- so we need to 'break' explicitly
										--
										-- maybe we need a global watcher list ...
										iojx.util.break_loop(iojx.current_context())
									end)
								iojx.util.run()
								break
							elseif laoj.WIFSIGNALED(status_final) then
								print(status_final, exitstatus, termsig)
								if termsig == lounix.SIGSEGV then -- a common kind of Runtime Error
									write_status(args, 'RE', time_ms, mem)
									break
								end
								if termsig == lounix.SIGPROF then -- triggered by setitimer, a TLE
									write_status(args, 'TLE', time_ms, mem)
								end
								break
							end
						end

					end
				end).pid

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
					execute = judge }
		}
	end
end

iojxd_init()

iojxx.ixlbx_tcp_base(iojx.current_context()):
on_accept(function ()
	print('iojxd - accepting connection ...')
end):on_close(function (ctx)
	print('iojxd - closing connection ...')
	delayed_gc()
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

	delayed_gc()
end):start('127.0.0.1', 23333)
