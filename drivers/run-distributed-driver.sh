#!/bin/bash

#
#  run-distributed-driver.sh SERVER-PROC-NUM SERVER-THREAD-NUM LOGDIR
#

if [ $# -lt 3 ]; then
    echo "Error: lack of arguments"
    echo "Usage: `basename $0` SERVER-PROC-NUM SERVER-THREAD-NUM LOGDIR"
    exit 1
fi

serv_proc=$1
serv_thr=$2
log_dir=$3

this_file=`readlink -f "$0"`
this_dir=`dirname "$this_file"`

timestamp=`date +%y%m%d%H%M%S`

examine_one() {
    local method=$1
    local query_num=$2
    local cli_thr=$3

    logfile_basename="log-ZK-${timestamp}-cli_thr-${cli_thr},${method},serv_thr-${serv_proc}-${serv_thr}.txt"
    logfile="$log_dir/$logfile_basename"

    echo $logfile_basename
    "$this_dir"/run-distributed.sh $method $query_num $cli_thr > $logfile 2>&1
}

# method=query_cht, query_num=10000, thread_num=1, 2,4,8,16
examine_one query_cht 10000 1
examine_one query_cht 10000 2
examine_one query_cht 10000 4
examine_one query_cht 10000 8
examine_one query_cht 10000 16

# method=query_cht, query_num=10000, thread_num=1, 2,4,8,16
examine_one query_cht_nolock 10000 1
examine_one query_cht_nolock 10000 2
examine_one query_cht_nolock 10000 4
examine_one query_cht_nolock 10000 8
examine_one query_cht_nolock 10000 16
