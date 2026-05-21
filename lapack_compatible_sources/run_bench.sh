#!/bin/bash

LD_LIBRARY_PATH=/opt/AMD/libflame/mt/lib:/opt/AMD/blis/mt/lib:/opt/AMD/aocl-utils/lib:/opt/AMD/aocc-compiler-5.1.0/lib
export LD_LIBRARY_PATH

OUTPUT=bench_result.csv
echo "n,time,GFLOPS" > ${OUTPUT}

for n in $(seq 10240 2048 102400); do
    ./simple_test.x ${n} | tee -a ${OUTPUT}
done
