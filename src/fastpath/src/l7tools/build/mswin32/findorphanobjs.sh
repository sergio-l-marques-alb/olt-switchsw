#!/bin/sh

for i in $(find $* -type f -name "*.o" -print)
do
	DIRNAME=$(dirname $i)
	if [ ! -f ${DIRNAME}/fp_*.a ]
	then
		echo $i
	fi
done
