#!/bin/bash

EVENTS="cycles,instructions,cache-misses,L1-dcache-load-misses,context-switches"

VERSIONS=("A1" "A2" "A3")
THREADS=(1 2 4 8)
SIZES=(512 1024 4096 16384 65536)
ITER=20

OUT="results.csv"

echo "version,threads,size,cycles,instructions,cache_misses,l1_misses,context_switches" > $OUT

for V in "${VERSIONS[@]}"
do
    SERVER="./${V}S"
    CLIENT="./${V}C"

    echo "Starting $SERVER ..."
    $SERVER &
    PID=$!

    sleep 1

    for T in "${THREADS[@]}"
    do
        for S in "${SIZES[@]}"
        do
            echo "Running $V threads=$T size=$S"

            PERF_OUT=$(perf stat -e $EVENTS $CLIENT 127.0.0.1 $T $S $ITER 2>&1)

            CYCLES=$(echo "$PERF_OUT" | grep cycles | awk '{print $1}')
            INST=$(echo "$PERF_OUT" | grep instructions | awk '{print $1}')
            CMISS=$(echo "$PERF_OUT" | grep cache-misses | awk '{print $1}')
            L1MISS=$(echo "$PERF_OUT" | grep L1-dcache-load-misses | awk '{print $1}')
            CSW=$(echo "$PERF_OUT" | grep context-switches | awk '{print $1}')

            echo "$V,$T,$S,$CYCLES,$INST,$CMISS,$L1MISS,$CSW" >> $OUT
        done
    done

    kill $PID
    wait $PID 2>/dev/null
done

echo "Done. Results saved in $OUT"
