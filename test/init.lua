
print('Hellor Worlder!')

iojx.enable_termcb(iojx.current_context())

function spawn_co(file)
	iojx.co.spawn_process(file)
	return coroutine.yield()
end

function spawn_cotest(file)
	print('spawn_cotest: ready')
	spawn_co(file)
	print("spawn_cotest: end")
end

coroutine.wrap(spawn_cotest)('./test_exec.py')

