.PHONY : fresh clean distclean

build :
	@if [ ! -d "bin/" ]; then\
		mkdir bin;\
	fi
	@gcc -Wall -g -o ./bin/aStar ./main.c ./utils/util.c ./graph/graph.c ./graph/hash.c ./graph/domain.c ./graph/heuristic.c ./heap/heap.c ./hash_table/hash_table.c \
	./queue/queue.c ./stack/stack.c ./pathfinding/seq_a_star.c ./pathfinding/seq_djikstra.c ./msg_queue/msg_queue.c ./pathfinding/par_a_star.c -lm -lpthread 2> bin/build_errors.log 1> bin/build_logs.log

debug :
	@if [ ! -d "dev/" ]; then\
		mkdir dev;\
	fi
	@gcc -Wall -g -D DEBUG_PAR_ASTAR -D DEBUG_ASTAR -o ./dev/aStar ./main.c ./utils/util.c ./graph/graph.c ./graph/hash.c ./graph/domain.c ./graph/heuristic.c ./heap/heap.c ./hash_table/hash_table.c \
	./queue/queue.c ./stack/stack.c ./pathfinding/seq_a_star.c ./pathfinding/seq_djikstra.c ./msg_queue/msg_queue.c ./pathfinding/par_a_star.c -lm -lpthread 2> dev/build_errors.log 1> dev/build_logs.log

clean :
	@rm -f bin/*.o
	@rm -rf bin/
	@rm -f dev/*.o
	@rm -rf dev/

distclean : clean
	@rm -rf bin/core
	@rm -rf bin/*~
	@rm -rf dev/core
	@rm -rf dev/*~

fresh : distclean build
