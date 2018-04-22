PREF	?= run_

compile = gcc $(1)/$(2).c -o ${PREF}$(1)_$(2) -Wall
clean = rm -f ${PREF}$(1)_$(2)


make: czad1 czad2

czad1: zad1_server zad1_client

czad2: zad2_server zad2_client

zad1_server:
	$(call compile,zad1,server)

zad1_client:
	$(call compile,zad1,client)

zad2_server:
	$(call compile,zad2,server)

zad2_client:
	$(call compile,zad2,client)




clean: clean_zad1 clean_zad2

clean_zad1:
	$(call clean,zad1,server)
	$(call clean,zad1,client)

clean_zad2:
	$(call clean,zad2,server)
	$(call clean,zad2,client)
