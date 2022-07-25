rm -rf perf_report.txt perf.data
perf stat "$@"
perf record "$@"
perf report > perf_report.txt
rm -rf perf.data