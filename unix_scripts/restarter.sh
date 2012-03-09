#!/bin/bash

file="./MultiServer"
pidFile="/var/lock/ikaros.pid"

echo '******** WARNING: Do NOT use when Daemon is ENABLED ********'

if [ ! -f $file ]; then
    echo 'MultiServer Not Exists'
    exit
fi

while true; do
    if [ -f $pidFile ]; then
        kill $(cat $pidFile)
        rm $pidFile
    fi
    $file
done
