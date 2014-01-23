#!/bin/sh
if test -f configure; then
   #defenir compilador e SYS_ROOT_DIR para a UNICOM
   export DESTDIR=$PWD/rfs
   export SYSROOTDIR=$DESTDIR
   export PREFIXDIR=/usr/local/ptin
   export ETCDIR=$PREFIXDIR/etc
   export PKG_CONFIG_PATH=$DESTDIR$PREFIXDIR/lib/pkgconfig
   export TARGET_PPC=ppc_85xxDP
   export CROSS_COMPILE=$TARGET_PPC-
   export COMPILER_DIR=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux/bin
   export COMPILER_PREFIX=powerpc-e300c3-linux-
   export PATH=$PATH:$COMPILER_DIR
   #export INCLUDEDIR="/opt/eldk/usr/bin/usr/include"
   export CROSSOPTS="--host=ppc-linux --build=$MACHTYPE"
   #export CFLAGS="-I$INCLUDEDIR -I$SYSROOTDIR$PREFIXDIR/include"
   #export LIBDIR="/opt/ppc-ptin-4.2.2/$TARGET_PPC/usr/lib"
   export LIBS=
   #export LD_PATH="-L$LIBDIR -L$SYSROOTDIR$PREFIXDIR/lib"
   #export LDFLAGS="$LD_PATH"
   #export CPPFLAGS="-I$INCLUDEDIR -I$SYSROOTDIR$PREFIXDIR/include"   
   export STRIP="$COMPILER_DIR"/"$COMPILER_PREFIX"strip
   export CC="$COMPILER_DIR"/"$COMPILER_PREFIX"gcc
   export CXX="$COMPILER_DIR"/"$COMPILER_PREFIX"g++
   export AR="$COMPILER_DIR"/"$COMPILER_PREFIX"ar
   export LD="$COMPILER_DIR"/"$COMPILER_PREFIX"ld
   export NM="$COMPILER_DIR"/"$COMPILER_PREFIX"nm
   export RANLIB="$COMPILER_DIR"/"$COMPILER_PREFIX"ranlib
   export READELF="$COMPILER_DIR"/"$COMPILER_PREFIX"readelf
   export OBJCOPY="$COMPILER_DIR"/"$COMPILER_PREFIX"objcopy
   export OBJDUMP="$COMPILER_DIR"/"$COMPILER_PREFIX"objdump
   export INSTALL=/usr/bin/install
   export PTIN_MGMD_PLATFORM_MAX_CHANNELS=1024
   export PTIN_MGMD_PLATFORM_MAX_WHITELIST=4096
   export PTIN_MGMD_PLATFORM_MAX_CLIENTS=128
   export PTIN_MGMD_PLATFORM_MAX_PORTS=23
   export PTIN_MGMD_PLATFORM_MAX_SERVICES=8
   #export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=

   rm -rf ./build/export.var
   echo '.EXPORT_ALL_VARIABLES:' > ./build/export.var
   echo "export DESTDIR                           = $DESTDIR"                              >> ./build/export.var
   echo "export SYSROOTDIR                        = $SYSROOTDIR"                           >> ./build/export.var
   echo "export PREFIXDIR                         = $PREFIXDIR"                            >> ./build/export.var
   echo "export ETCDIR                            = $ETCDIR"                               >> ./build/export.var
   echo "export PKG_CONFIG_PATH                   = $PKG_CONFIG_PATH"                      >> ./build/export.var
   echo "export TARGET_PPC                        = $TARGET_PPC"                           >> ./build/export.var
   echo "export CROSS_COMPILE                     = $CROSS_COMPILE"                        >> ./build/export.var
   echo "export COMPILER_DIR                      = $COMPILER_DIR"                         >> ./build/export.var
   echo "export PATH                              = $PATH"                                 >> ./build/export.var
   echo "export COMPILER_PREFIX                   = $COMPILER_PREFIX"                      >> ./build/export.var
   echo "export CROSSOPTS                         = $CROSSOPTS"                            >> ./build/export.var
   echo "export CFLAGS                            = $CFLAGS"                               >> ./build/export.var
   echo "export LIBDIR                            = $LIBDIR"                               >> ./build/export.var
   echo "export LIBS                              = $LIBS"                                 >> ./build/export.var
   echo "export LD_PATH                           = $LD_PATH"                              >> ./build/export.var
   echo "export LDFLAGS                           = $LDFLAGS"                              >> ./build/export.var
   echo "export INCLUDEDIR                        = $INCLUDEDIR"                           >> ./build/export.var
   echo "export CPPFLAGS                          = $CPPFLAGS"                             >> ./build/export.var
   echo "export STRIP                             = $STRIP"                                >> ./build/export.var
   echo "export CC                                = $CC"                                   >> ./build/export.var
   echo "export CXX                               = $CXX"                                  >> ./build/export.var
   echo "export AR                                = $AR"                                   >> ./build/export.var
   echo "export LD                                = $LD"                                   >> ./build/export.var
   echo "export NM                                = $NM"                                   >> ./build/export.var
   echo "export RANLIB                            = $RANLIB"                               >> ./build/export.var
   echo "export READELF                           = $READELF"                              >> ./build/export.var
   echo "export OBJCOPY                           = $OBJCOPY"                              >> ./build/export.var
   echo "export OBJDUMP                           = $OBJDUMP"                              >> ./build/export.var
   echo "export INSTALL                           = $INSTALL"                              >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_STACK_SIZE     = $PTIN_MGMD_PLATFORM_STACK_SIZE"        >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_MAX_CHANNELS   = $PTIN_MGMD_PLATFORM_MAX_CHANNELS"      >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_MAX_WHITELIST  = $PTIN_MGMD_PLATFORM_MAX_WHITELIST"     >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_MAX_CLIENTS    = $PTIN_MGMD_PLATFORM_MAX_CLIENTS"       >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_MAX_PORTS      = $PTIN_MGMD_PLATFORM_MAX_PORTS"         >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_MAX_SERVICES   = $PTIN_MGMD_PLATFORM_MAX_SERVICES"      >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID = $PTIN_MGMD_PLATFORM_MAX_SERVICE_ID"    >> ./build/export.var
   echo "export PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE = $PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE"    >> ./build/export.var
   
   ./configure --prefix=$PREFIXDIR $CROSSOPTS
else
   echo "O script configure não existe!!!!"
   echo "Vá para o directório raiz e faça:"
   echo "sh build/autogen.sh"
   echo "sh build/configure-ud.sh"
   exit 1
fi

