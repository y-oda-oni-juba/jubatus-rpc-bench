#!/bin/bash

#
# run-distributed.sh METHOD QUERY_NUM THREAD_NUM [SAMPLE_NUM=3]
#

if [ $# -lt 3 ]; then
    echo "Error: lack of arguments"
    echo "Usage: `basename $0` METHOD QUERY_NUM THREAD_NUM"
    exit 1
fi

# measurement setting
method=$1
query_num=$2
thread_num=$3
sample_num=${4:-3}

# load environment
this_file=`readlink -f "$0"`
this_dir=`dirname "$this_file"`

source "${this_dir}/hosts-distributed.sh"

rpc_bench_bin="${this_dir}/../0.4.0/build/rpc-bench"

# examine
run_test() {
    local tag=$1
    local name=$2
    local host=$3
    local port=$4
    local sample_num=$5
    local rpc_bench_bin="$6"
    
    for i in $(seq 1 $sample_num); do
        echo "${tag}: host=<${host}:${port}>, method=${method}, #query=${query_num}, #thread=${thread_num}, sample=$i/${sample_num}"
        ${rpc_bench_bin} --name ${name} --host ${host} --port ${port} --method ${method} --query ${query_num} --thread ${thread_num}
        echo
        sleep 1
    done
}

run_test 'v0.3.4 standalone' $v034_keeper_name $v034_keeper_host $v034_keeper_port $sample_num $rpc_bench_bin
echo '----'
run_test 'v0.4.0 standalone' $v040_keeper_name $v040_keeper_host $v040_keeper_port $sample_num $rpc_bench_bin
echo '===='
