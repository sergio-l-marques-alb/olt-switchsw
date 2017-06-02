
# Do not change this line.
#
include $(FP_SRC)/l7tools/build/make.cfg

# Specify the correct relative path for libsw.a file.
LIBDIR=$(FP_SRC)/lib

# Specify compile flags.
#
installexe: mem_layout.exe

mem_layout.exe: mem_layout.o
	gcc -o mem_layout.exe mem_layout.o

mem_layout.o: mem_layout.c
	gcc -c -I. $(L7PUBLIC_INCLDIR) $(CFLAGSLOCAL)  mem_layout.c

clean::
	rm mem_layout.exe
	rm mem_layout.o

bldlib:

