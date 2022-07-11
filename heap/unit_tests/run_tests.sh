rm -rf bin/
rm -rf results/
mkdir bin
mkdir results
gcc -Wall -D DEBUG -g -o ./bin/tests ../../utils/util.c ../heap.c heapTest.c; 
bin/tests > results/results.log;