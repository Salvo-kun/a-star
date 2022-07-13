rm -rf bin/
rm -rf results/
mkdir bin
mkdir results
gcc -Wall -D DEBUG -g -o ./bin/tests ../../utils/util.c ../queue.c queueTest.c 2> bin/build_errors.log 1> bin/build_logs.log

if [ -f "bin/tests" ]; then
    bin/tests > results/results.log 2>&1
fi