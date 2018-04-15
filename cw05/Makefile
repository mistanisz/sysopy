PREF	?= run_

compile = gcc $(1)/$(2).c -o ${PREF}$(2) -Wall
clean = rm -f $(1)


make: czad1 czad2

czad1:
	$(call compile,zad1,zad1)

czad2:
	$(call compile,zad2,slave)
	$(call compile,zad2,master)


clean: clean_zad1 clean_zad2

clean_zad1:
	$(call clean, ${PREF}zad1)

clean_zad2:
	$(call clean, ${PREF}master)
	$(call clean, ${PREF}slave)

# zestaw jest kompletny
