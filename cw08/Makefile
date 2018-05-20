PREF	?= run_
compile = gcc $(1)/$(2).c -o ${PREF}$(2) -Wall -lpthread -lm
THREADS ?= 4
IN		?= images/mountain.ascii.pgm
OUT		?= results/out.pgm
FILTER  ?= filters/filter20

all: compile

compile:
	$(call compile,zad1/src,main)

run: compile
	./run_main ${THREADS} ${IN} ${FILTER} ${OUT} 

clean: 
	rm -rf run_main
