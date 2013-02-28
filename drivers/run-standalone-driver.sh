#!/bin/bash

#
#  run-standalone-driver.sh SERVER-THREAD-NUM LOGDIR
#

if [ $# -lt 2 ]; then
    echo "Error: lack of arguments"
    echo "Usage: `basename $0` SERVER-THREAD-NUM LOGDIR"
    exit 1
fi

serv_thr=$1
log_dir=$2

this_file=`readlink -f "$0"`
this_dir=`dirname "$this_file"`

timestamp=`date +%y%m%d%H%M%S`

examine_one() {
    local method=$1
    local query_num=$2
    local cli_thr=$3
    local sample=${4:-10}

    logfile_basename="log-SA-${timestamp}-cli_thr-${cli_thr},${method},serv_thr-${serv_thr}.txt"
    logfile="$log_dir/$logfile_basename"

    echo $logfile_basename
    "$this_dir"/run-standalone.sh $method $query_num $cli_thr $sample > $logfile 2>&1
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
