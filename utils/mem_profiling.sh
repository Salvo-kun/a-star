rm -rf massif.* mem_report.txt
valgrind --tool=massif --time-unit=B --detailed-freq=1 --massif-out-file=massif.out "$@"
ms_print massif.out > mem_report.txt
rm -rf massif.*