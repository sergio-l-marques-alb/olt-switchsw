import os

lib_dir = ""
if 'LIBDIR' in os.environ:
    lib_dir = os.environ['LIBDIR']

bcm_libs = []
if 'BCM_LIBS' in os.environ:
    bcm_libs_tmp = os.environ['BCM_LIBS'].split(" ")
    bcm_libs_tmp = filter(None, bcm_libs_tmp)
    for lib in bcm_libs_tmp:
        bcm_libs.append(":" + lib)
bcm_libs.append("rt")
bcm_libs.append("m")
bcm_libs.append("pthread")
bcm_libs.append("nsl")

cfgflags = ""
if 'CFGFLAGS' in os.environ:
    cfgflags = os.environ['CFGFLAGS']
if 'CFLAGS' in os.environ:
    cfgflags += os.environ['CFLAGS']

cfgflags.replace("-Werror", "")

objects = ""
if 'OTH_OBJ' in os.environ:
    objects = os.environ['OTH_OBJ']
