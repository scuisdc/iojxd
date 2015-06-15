
local lfs = require 'lfs'
local uuid = require 'uuid'
local ffi = require 'ffi'

local lplatform = require 'lplatform'
local config_parse = require 'config_parse'
local config = require 'config'

local lutil = require 'lutil'
local caster = require 'caster'

local lo = require 'lo'
local lounix = require 'lounix'
local laoj = require 'laoj'

local ptrace = nil
if lplatform.is_linux() then
	ptrace = require 'lolinuxptrace'
end

function delayed_gc(delay)
	if delay == nil then delay = 2 end
	iojxx.timer(iojx.current_context(), function ()
		print('iojxd - delayed_gc: collecting garbage')
		collectgarbage()
	end):start(delay)
end

function readall(filename)
	local f = io.open(filename, 'r')
	local ret = f:read("*all")
	f:close()
	return ret
end

local iojxd_init = function ()
	uuid.randomseed(os.time())

	config = config_parse.parse(config, config_parse.defaults_conf)
	config.tmp_path = lutil.add_slash(config.tmp_path)

	local attr_tmp, msg = lfs.attributes(config.tmp_path)
	if attr_tmp == nil then
		lfs.mkdir(config.tmp_path)
	end
end

function write_status(args, status, time, mem, log)
	local out = io.open(args.outfile, 'w')
	print('writing to ' .. status)

	-- robustness: a permission denied - 150623 EVE
	if out == nil then
		print('iojxd - ERROR: failed to open outfile ' .. args.outfile)
	end

	out:write(tostring(caster.status[status]) .. '\n')
	out:write(tostring(time) .. ' ' .. tostring(mem) .. '\n')
	if log ~= nil then out:write(log) end
	out:close()
end

-- run before 'exec' to client program
-- set up stuffs like:
--	* forced time limit
--	* ptrace (Linux)
--	* process STDIO
--
function evaluate_child_setup (args, outputpath)
	local u0, u1 = math.floor(args.time / 1000), (args.time % 1000) * 1000

	laoj.freopen(args.data, 'r', laoj.stdin())
	laoj.freopen(outputpath, 'w', laoj.stdout())

	if lplatform.is_linux() then
		-- ffi.C.setuid(args.sbuid)
		-- ffi.C.setgid(args.sbgid)

		-- for stdin, stdout, stderr
		-- but programs do not always keep these descs
		-- they can be closed, so it makes almose no difference
		-- iojx.sandbox.reslimit(lo.RLIMIT_NOFILE, 4)
		-- iojx.sandbox.reslimit(lo.RLIMIT_AS, tonumber(args.mem))
		lutil.ptrace(ptrace.PTRACE_TRACEME, 0)
	end

	local val = ffi.new('struct itimerval')
	val.it_interval.tv_sec, val.it_interval.tv_usec = 0, 0
	val.it_value.tv_sec, val.it_value.tv_usec = u0, u1 -- we just passed raw time here
	ffi.C.setitimer(lo.ITIMER_PROF, val, nil)
end

-- forked from child process
-- stuffs like ptrace its system call
function evaluate_tracer(args, pid, paths)
	-- the first syscall would be 'exec' family and we'll ignore it
	local inited = false
	local status = { name = nil }
	while true do
		local ret, status_final, rusage = lutil.wait4(pid)
		local exitstatus, termsig = laoj.WEXITSTATUS(status_final), laoj.WTERMSIG(status_final)

		local time_ms = lutil.rusage_time(rusage) * 1000
		local mem = tonumber(rusage.ru_idrss)

		if laoj.WIFEXITED(status_final) then
			iojx.util.init_loop()
			print(status_final, exitstatus, termsig)
			laoj.fork(function () laoj.freopen(paths.diff, 'w', laoj.stdout()) end
				, 'diff', paths.output, args.result, function (status)
					if lfs.attributes(paths.diff).size == 0 then
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
			print('iojxd - target program termed by signal')
			print(status_final, exitstatus, termsig)
			if termsig == lounix.SIGSEGV then -- a common kind of Runtime Error
				write_status(args, 'RE', time_ms, mem)
				break
			end
			if termsig == lounix.SIGPROF then -- triggered by setitimer, a TLE
				write_status(args, 'TLE', time_ms, mem) end
			if termsig == lounix.SIGKILL then -- MLE, setrlimit RLIMIT_AS, or ptrace
				if status.name == 'overflow' then
					write_status(args, 'RE', time_ms, mem, 
						"Detected invalid system call '" .. status.syscall_name .. "'!")
				else
					write_status(args, 'MLE', time_ms, mem)
				end
			end
			break
		elseif laoj.WIFSTOPPED(status_final) then

			termsig = laoj.WSTOPSIG(status_final)
			if termsig == lounix.SIGPROF then
				write_status(args, 'TLE', time_ms, mem)
				break
			end

			local regs = ffi.new('struct user_regs_struct')
			ffi.C.ptrace(ptrace.PTRACE_GETREGS, pid, nil, regs)
			local syscall_name = laoj.syscall_name(tonumber(regs.orig_rax))
			io.write(syscall_name .. ' ')

			if not inited then
				inited = true
				lutil.ptrace(ptrace.PTRACE_SYSCALL, pid)
			else
				if not lutil.inside(args.syscall_whitelist, syscall_name) then
					print('\ndetected invalid syscall ' .. tostring(tonumber(regs.orig_rax)) .. ' ' .. syscall_name)
					status.name = 'overflow'
					status.syscall_name = syscall_name
					lutil.ptrace(ptrace.PTRACE_KILL, pid)
					-- lutil.ptrace(ptrace.PTRACE_SYSCALL, pid)
				else
					lutil.ptrace(ptrace.PTRACE_SYSCALL, pid)
				end
			end

		end
	end
end

function evaluate (args, paths)

	iojxx.fork(function ()
		local pid_in = ffi.C.fork()
		if pid_in == 0 then
			evaluate_child_setup(args, paths.output)
			if #args[caster.runtime_tag[args.language]] == 0 then
				iojx.util.exec(paths.exec)
			else
				local execpath
				if caster.needscompile[args.language] then
					execpath = paths.exec
				else execpath = args.source[1] end
				iojx.util.exec(args[caster.runtime_tag[args.language]], execpath)
			end
		else evaluate_tracer(args, pid_in, paths) end
	end)

end

function judge(args)

	if args == nil then args = { } end
	args = config_parse.parse(args, config_parse.defaults_args)
	if args.syscall_whitelist == nil then
		args.syscall_whitelist = caster.syscall_whitelist[args.language] end

	-- generate session ID
	local session_uuid = uuid()
	print('iojxd - session uuid ' .. session_uuid)
	-- URI for temp files
	local session_dir = config.tmp_path .. session_uuid .. '/'
	lfs.mkdir(session_dir)
	local paths = {
		cclog = session_dir .. 'compiler.log',
		exec = session_dir .. 'a.out',
		output = session_dir .. 'tmp.out',
		diff = session_dir .. 'diff.diff'
	}

	if caster.needscompile[args.language] then
		-- extra work to invoke a compiler
		local compiler = args[caster.compiler_tag[args.language]]
		print(compiler, args.source[1])
		local pid_compiler = iojxx.fork(function ()
			laoj.freopen(paths.cclog, "w", laoj.stderr()) -- redirect compiler to log
			iojx.util.exec(compiler, unpack(args.source), '-o', paths.exec)
		end).pid
		iojxx.child_watcher(iojx.current_context(), function (watcher)
			local status = watcher:get_status()
			if status.status ~= 0 then
				write_status(args, 'CE', 0, 0, readall(paths.cclog))
			else evaluate(args, paths) end
		end):start(pid_compiler)
	else evaluate(args, paths) end
end

local tcp_connect = iojxx.ixlbx_tcp_base(iojx.current_context())

function execute_cmd(cmd, ctx)
	print('iojxd - executing command ', cmd)

	local commands = {
		AUTH = function ()
			ctx:write('1')
			ctx.data = { authed = true, context = {
						execute = judge } }
		end,
		EXIT = function ()
			ctx:write('1')
			print('exiting ...')
			ctx:close()
			tcp_connect:close()
		end
	}

	commands[cmd]()
end

iojxd_init()

tcp_connect:on_accept(function ()
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
				print('iojxd - loadstring failed: ', msg)
				ctx:write('0\n' .. msg)
			else
				setfenv(f, ctx.data.context)
				local succeeded, result = pcall(f)
				if succeeded then ctx:write('1')
				else ctx:write('0\n' .. result) end
			end

		else
			ctx:write('0\nyou are not authed.')
			delayed_gc()
			ctx:close()
		end
	end
end):start(config.listen_address, config.listen_port)
