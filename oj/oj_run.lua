
laoj = require 'laoj'

laoj.fork(nil, 'clang', 'A-c.c', '-o', 'A-c', function (status)
	-- if you logged something to stdout without flushing
	-- 	you'll get 14 messages ...
	iojx.util.fflush(laoj.stdout())
	for i = 1, 14 do
		iojx.util.fflush(laoj.stdout())
		local outfile = "output" .. i .. ".txt"
		laoj.fork(function ()
			laoj.freopen("A.".. i ..".dat", "r", laoj.stdin())
			laoj.freopen(outfile, "w", laoj.stderr())
			laoj.freopen(outfile, "w", laoj.stdout())
		end, './A-c', function (status)

			laoj.fork(nil, 'diff', outfile, 'A.'.. i ..'.diff')
		end)
	end
end)