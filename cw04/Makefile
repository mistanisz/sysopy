compile = gcc $(1)/$(1).c -o c$(1) -Wall
clean = rm -f $(1)

make: czad1 czad2 czad3

czad1:
	$(call compile,zad1)

czad2:
	$(call compile,zad2)

czad3:
	$(call compile,zad3)

clean: clean_zad1 clean_zad2 clean_zad3

clean_zad1:
	$(call clean, czad1)

clean_zad2:
	$(call clean, czad2)

clean_zad3:
	$(call clean, czad3)

# zestaw jest kompletny
