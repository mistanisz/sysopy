PREF	?= run_

compile = gcc $(1).c -o ${PREF}$(1) -Wall -lrt -lpthread
clean = rm -f ${PREF}$(1)


make: zad2_main zad2_client

zad2_main:
	$(call compile,main)

zad2_client:
	$(call compile,client)




clean: clean_zad2

clean_zad2:
	$(call clean,main)
	$(call clean,client)

