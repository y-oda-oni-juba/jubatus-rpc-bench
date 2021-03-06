#!/bin/bash

#
# nullalgo_keeper-zk-start.sh ZK-HOST
#

if [ $# -lt 1 ]; then
    echo "Error: lack of arguments"
    echo "Usage: `basename $0` ZK-HOST"
    exit 1
fi

zk_host=$1
port=9000

this_file=`readlink -f "$0"`
this_dir=`dirname "$this_file"`

exec "$this_dir"/build/nullalgo_keeper -p $port -z $zk_host -e 2
