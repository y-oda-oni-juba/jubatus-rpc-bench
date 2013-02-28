#!/bin/bash

#
#  run-bare-server-driver.sh SERVER-THREAD-NUM LOGDIR [OPTIONAL-TAG]
#

if [ $# -lt 2 ]; then
    echo "Error: lack of arguments"
    echo "Usage: `basename $0` SERVER-THREAD-NUM LOGDIR [OPTIONAL-TAG]"
    echo "Env: QUERY_COUNT ( default: 10000 )"
    exit 1
fi

serv_thr=$1
log_dir=$2
opt_tag=$3
query_count=${QUERY_COUNT:-10000}

this_file=`readlink -f "$0"`
this_dir=`dirname "$this_file"`

timestamp=`date +%y%m%d%H%M%S`

examine_one() {
    local method=$1
    local query_num=$2
    local cli_thr=$3
    local sample=${4:-10}

    if [ "$opt_tag" != "" ]; then
        tag="_${opt_tag}"
    fi

    logfile_basename="log-BS${tag}-${timestamp}-cli_thr-${cli_thr},${method},serv_thr-${serv_thr}.txt"
    logfile="$log_dir/$logfile_basename"

    echo $logfile_basename
    "$this_dir"/run-standalone.sh $method $query_num $cli_thr $sample > $logfile 2>&1
}

# # method=query_cht, query_num=10000, thread_num=1, 2,4,8,16
# examine_one query_cht 10000 1
# examine_one query_cht 10000 2
# examine_one query_cht 10000 4
# examine_one query_cht 10000 8
# examine_one query_cht 10000 16

# method=query_cht, query_num=$query_count, thread_num=1, 2,4,8,16
examine_one query_cht_nolock $query_count 1
examine_one query_cht_nolock $query_count 2
examine_one query_cht_nolock $query_count 4
examine_one query_cht_nolock $query_count 8
examine_one query_cht_nolock $query_count 16
