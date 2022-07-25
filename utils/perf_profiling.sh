rm -rf perf_report.txt time_report.txt perf.data
perf stat "$@"
perf record "$@"
perf report > perf_report.txt
/usr/bin/time -v "$@" > time_report.txt
rm -rf perf.data