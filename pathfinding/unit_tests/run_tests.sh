rm -rf bin/
rm -rf results/
mkdir bin
mkdir results
gcc -Wall -g -o  ./bin/tests ../../utils/util.c ../../graph/graph.c ../../heap/heap.c ../../hash_table/hash_table.c ../../queue/queue.c ../../stack/stack.c ../seq_a_star.c ../seq_djikstra.c ../../utils/hash_function.c ../../msg_queue/msg_queue.c pathfindingTest.c ../par_a_star.c -lm -lpthread 2> bin/build_errors.log 1> bin/build_logs.log

if [ -f "bin/tests" ]; then
    bin/tests $1 $2 $3 $4 > results/results.log 2>&1
fi

# ./run_tests.sh 1 ../../benchmarks/maze-maps/proc/maze512-32-0.txt 248427 16417
# Result: Found path with cost = 2173 and length = 2174