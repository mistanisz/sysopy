PREF	?= run_

compile = gcc $(1)/$(2).c -o ${PREF}$(1)_$(2) -Wall -lrt -lpthread
clean = rm -f ${PREF}$(1)_$(2)


make: czad1 czad2

czad1: zad1_barber zad1_client

czad2: zad2_barber zad2_client

zad1_barber:
	$(call compile,zad1,barber)

zad1_client:
	$(call compile,zad1,client)

zad2_barber:
	$(call compile,zad2,barber)

zad2_client:
	$(call compile,zad2,client)




clean: clean_zad1 clean_zad2

clean_zad1:
	$(call clean,zad1,barber)
	$(call clean,zad1,client)

clean_zad2:
	$(call clean,zad2,barber)
	$(call clean,zad2,client)
