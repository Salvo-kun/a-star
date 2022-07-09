#
# Makefile
#

target :
	gcc -Wall -g -o aStar main.c


clean :
	rm -f *.o
	rm -rf aStar

distclean : clean
	rm -rf core
	rm -rf *~
