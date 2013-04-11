#!/bin/bash

#
# run-bare-rpc.sh METHOD QUERY_NUM THREAD_NUM [SAMPLE_NUM=3]
#

if [ $# -lt 3 ]; then
    echo "Error: lack of arguments"
    echo "Usage: `basename $0` METHOD QUERY_NUM THREAD_NUM"
    echo "Env: JUBATUS_VERSION (default: msgpack-rpc-only )"
    exit 1
fi

# measurement setting
method=$1
query_num=$2
thread_num=$3
sample_num=${4:-3}

juba_version=${JUBATUS_VERSION:-msgpack-rpc-only}

# load environment
this_file=`readlink -f "$0"`
this_dir=`dirname "$this_file"`

source "${this_dir}/hosts-bare-rpc.sh"

rpc_bench_bin="${this_dir}/../${juba_version}/build/rpc-bench"

# examine
run_test() {
    local tag=$1
    local host=$2
    local port=$3
    local sample_num=$4
    local rpc_bench_bin="$5"
    
    for i in $(seq 1 $sample_num); do
        echo "${tag}: host=<${host}:${port}>, method=${method}, #query=${query_num}, #thread=${thread_num}, sample=$i/${sample_num}"
        ${rpc_bench_bin} --host ${host} --port ${port} --method ${method} --query ${query_num} --thread ${thread_num}
        echo
        sleep 1
    done
}

run_test 'mpio_v041' $mpio_v041_host $mpio_v041_port $sample_num $rpc_bench_bin
# echo '----'
# run_test 'new mpio ' $v040_host $v040_port $sample_num $rpc_bench_bin
echo '===='
