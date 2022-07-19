rm -rf bin/
rm -rf results/
mkdir bin
mkdir results
gcc -Wall -g -o ./bin/tests ../../utils/util.c ../../graph/graph.c ../../heap/heap.c ../../hash_table/hash_table.c ../../queue/queue.c ../../stack/stack.c ../seq_a_star.c seqAStarTest.c 2> bin/build_errors.log 1> bin/build_logs.log

if [ -f "bin/tests" ]; then
    bin/tests $1 $2 $3 > results/results.log 2>&1
fi