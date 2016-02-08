#! /bin/sh
#
# run this script through /bin/sh
# scriptname < rfcxxxx.txt > mibxxxx.my
#
cat $* > /tmp/$$.my
(fgrep 'DEFINITIONS ::= BEGIN' /tmp/$$.my; \
        sed -e '1,/DEFINITIONS ::= BEGIN/d' /tmp/$$.my | sed -e '/END$/,$d'; \
        echo END) | \
    sed -e 's/--\(.*\[Page [1-9][0-9]*\]\)$/--\1 /g' | \
    sed -e '/\[Page [1-9][0-9]*\]$/d' | \
    sed -e '/$/d' | \
    sed -e 's/--\(.*199[0-9]\)$/--\1 /g' | \
    sed -e '/199[0-9]$/d'

rm -f /tmp/$$.my

