rm -rf mem-leaks-report.txt
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=mem-leaks-report.txt "$@"
