#!/bin/bash

file="MultiServer"
pidFile="/var/lock/ikaros.pid"

function ApplicationRunning
{
    ex=$(pidof $file)
    declare -i pid=0
    if [ -n "$ex" ]; then
        pid=($ex-0)
    fi
    echo $pid
}

case $1 in
    "start")
        if [ -f $pidFile ]; then
            echo 'Application already run'
        else
            ./$file
        fi
        ;;
    "stop")
        if [ -f $pidFile ]; then
            kill $(cat $pidFile)
            echo 'Terminating application'
            rm $pidFile
        fi
        ;;
    "restart")
        if [ -f $pidFile ]; then
            kill $(cat $pidFile)
            echo 'Terminating old instance of application'
            rm $pidFile
        fi
        echo 'Executing application'
        ./$file
        ;;
    "kill")
        if [ -f $pidFile ]; then
            kill -s KILL $(cat $pidFile)
            echo 'Killing application'
            rm $pidFile
        fi
        ;;
    "status")
        run=$(ApplicationRunning)
        if [ $run -gt 0 ]; then
            echo 'Application is running. PID from pidof: '$run
        else
            echo 'Application is not running'
        fi
        if [ -f $pidFile ]; then
            echo 'PID from file: '$(cat $pidFile)
        fi
        ;;
    *)
        echo 'Usage: '$0' <action>'
        echo '<action>: '
        echo -e "\t start"
        echo -e "\t stop"
        echo -e "\t restart"
        echo -e "\t kill"
        echo -e "\t status"
        ;;
esac
