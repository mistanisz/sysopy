COMP	?= -Wall

make:
	gcc list_dir.c -o list_dir ${COMP}
	gcc batch_interpreter.c -o batch_interpreter ${COMP}
	gcc infinite_loop.c -o infinite_loop ${COMP}
	gcc memory_leak.c -o memory_leak ${COMP}

run:
	./batch_interpreter batch_files/example 2 2047
	./batch_interpreter batch_files/mem 2 2047
	./batch_interpreter batch_files/cpu 2 2047

list:
	./list_dir .. "<" "2018-12-31"

clean:
	rm batch_interpreter infinite_loop list_dir memory_leak


# zestaw jest kompletny
