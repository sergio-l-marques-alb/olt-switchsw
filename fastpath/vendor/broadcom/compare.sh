#!/bin/sh

echo 'Different files:'
diff -rq $1 $2 | grep 'Files' | awk '{print $2}'

echo ''
echo 'Files only present in '$1
diff -rq $1 $2 | grep 'Only in '$1 | awk '{print $3 $4}' | sed 's/:/\//g'

echo ''
echo 'Files only present in '$2
diff -rq $1 $2 | grep 'Only in '$2 | awk '{print $3 $4}' | sed 's/:/\//g'

