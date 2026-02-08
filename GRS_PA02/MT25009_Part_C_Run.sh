#!/bin/bash
# MT25009

echo "Compiling..."
make

OUT="results.csv"
echo "version,threads,size,cycles,cache_misses,l1_misses,context_switches" > $OUT

VERSIONS=("A1" "A2" "A3")
THREADS=(1 2 4 8)
SIZES=(512 1024 4096 16384 65536)

DURATION=2

run_one () {

    ver=$1
    th=$2
    sz=$3

    SERVER=./${ver}S
    CLIENT=./${ver}C

    echo "▶ Running $ver | threads=$th | size=$sz"

    $SERVER >/dev/null 2>&1 &
    SPID=$!

    sleep 1

    PERF=$(timeout 10s perf stat -e cycles,cache-misses,L1-dcache-load-misses,context-switches \
        $CLIENT $th $sz $DURATION 2>&1)

    kill -9 $SPID 2>/dev/null

    cycles=$(echo "$PERF" | grep "cycles" | awk '{print $1}' | tr -d ,)
    cache=$(echo "$PERF" | grep "cache-misses" | awk '{print $1}' | tr -d ,)
    l1=$(echo "$PERF" | grep "L1-dcache-load-misses" | awk '{print $1}' | tr -d ,)
    ctx=$(echo "$PERF" | grep "context-switches" | awk '{print $1}' | tr -d ,)

    echo "$ver,$th,$sz,$cycles,$cache,$l1,$ctx" >> $OUT
}

for v in "${VERSIONS[@]}"; do
    for t in "${THREADS[@]}"; do
        for s in "${SIZES[@]}"; do
            run_one $v $t $s
        done
    done
done

echo "✅ Done. Results saved to results.csv"
