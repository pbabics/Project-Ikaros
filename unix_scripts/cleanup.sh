#!/bin/bash
list=`/usr/bin/find ../ -name *.cpp -o -name *.h`

for i in $list; do
echo "Tab2Spaces and whitespace cleanup on "$i
perl -wpi -e "s/\t/    /g" $i
perl -wpi -e "s/ +$//g" $i
done
