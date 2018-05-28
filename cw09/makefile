PREF ?= run_
compile = gcc $(1)/$(2).c -o ${PREF}$(1) -Wall -lpthread -lm
clean = rm -f ${PREF}$(1)

	
czad1:
	$(call compile,zad1,main)

czad2:
	$(call compile,zad2,main)

make: czad2 czad1

clean: 
	$(call clean,zad1,main)
	$(call clean,zad2,main)
	

example_zad1: czad1
	./run_zad1 100 100 100 pan-tadeusz-czyli-ostatni-zajazd-na-litwie.txt 40 '<' 0 0

example_zad2: czad2
	./run_zad2 100 100 100 pan-tadeusz-czyli-ostatni-zajazd-na-litwie.txt 40 '<' 0 0
