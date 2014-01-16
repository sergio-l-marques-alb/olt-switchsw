#!/bin/sh

# The --force option rebuilds the configure script regardless of its timestamp in relation to that of the file configure.ac.
# The option --install copies some missing files to the directory, including the text files COPYING and INSTALL.
#autoreconf --force --install N�O SEI PORQU~E MAS O ZEBOS N�O GOSTA DISTO!!!!!
if test -f configure.ac; then
	touch build/export.var
	libtoolize -fq
	aclocal --force
	autoheader -f
	touch NEWS README AUTHORS ChangeLog
	automake -acf
	autoconf -f
else
	echo "V� para o direct�rio raiz e fa�a: sh build/autogen.sh"
fi
