#!/bin/bash

# nullalgo-zk-start.sh ZK-HOST PROCESS-NUM THREAD-NUM

if [ $# -lt 3 ]; then
    echo "Error: lack of arguments"
    echo "Usage: `basename $0` ZK-HOST PROCESS-NUM THREAD-NUM"
    exit 1
fi

zk_host=$1
process_num=$2
thread_num=$3

port_base=9100

this_file=`readlink -f "$0"`
this_dir=`dirname "$this_file"`
nullalgo_bin="${this_dir}/build/nullalgo"
name="v034"

for i in $(seq 1 $process_num); do
    port=$((port_base + i))
    $nullalgo_bin -p $port -z $zk_host -n $name -c $thread_num \
        -t 10000 -s 1000000 -i 1000000 > /dev/null 2>&1 < /dev/null &
    echo "start @ $port"
done
