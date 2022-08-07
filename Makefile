.PHONY : fresh clean distclean

build :
	@if [ ! -d "bin/" ]; then\
		mkdir bin;\
	fi
	@gcc -Wall -g -o bin/aStar main.c utils/util.c

clean :
	@rm -f bin/*.o
	@rm -rf bin/

distclean : clean
	@rm -rf bin/core
	@rm -rf bin/*~

fresh : distclean build
