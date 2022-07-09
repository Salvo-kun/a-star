.PHONY : fresh

build :
	mkdir bin
	gcc -Wall -g -o bin/aStar main.c

clean :
	rm -f bin/*.o
	rm -rf bin/

distclean : clean
	rm -rf bin/core
	rm -rf bin/*~

fresh : distclean build
