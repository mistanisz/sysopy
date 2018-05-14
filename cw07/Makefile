PREF	?= run_

compile = gcc $(1)/$(2).c -o ${PREF}$(1)_$(2) -Wall -lrt
clean = rm -f ${PREF}$(1)_$(2)


make: czad1

czad1: zad1_main zad1_client


zad1_main:
	$(call compile,zad1,main)

zad1_client:
	$(call compile,zad1,client)




clean: clean_zad1

clean_zad1:
	$(call clean,zad1,main)
	$(call clean,zad1,client)

