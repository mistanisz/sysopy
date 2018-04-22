PREF	?= run_

compile = gcc $(1)/$(2).c -o ${PREF}$(1)_$(2) -Wall -lrt
clean = rm -f ${PREF}$(1)_$(2)


make: czad1 czad2a czad2b

czad1:
	$(call compile,zad1,zad1)

czad2a:
	$(call compile,zad2a,zad2a)

czad2b:
	$(call compile,zad2b,zad2b)


clean: clean_zad1 clean_zad2a clean_zad2b

clean_zad1:
	$(call clean,zad1,zad1)

clean_zad2a:
	$(call clean,zad2a,zad2a)

clean_zad2b:
	$(call clean,zad2b,zad2b)
