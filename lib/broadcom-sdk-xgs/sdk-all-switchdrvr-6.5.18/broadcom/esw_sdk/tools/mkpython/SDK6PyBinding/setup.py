###############################################################################
#
# Utility to get python bindings for BCM SDK6 C APIs
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

from setuptools import setup, find_packages
from setuptools.extension import Extension
from codecs import open
from os import path, system, environ, walk
import fnmatch
import subprocess

here = path.abspath(path.dirname(__file__))

with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

sdk = environ['SDK']
version_info = ""
with open(path.join(sdk, "RELEASE")) as f:
    version_info = f.read()
print("Building python binding for SDK release %s\n" % version_info)
version = version_info.strip('sdk-')

lib_dir = subprocess.check_output("make -f sdk6.mk libdir", shell=True).decode("utf-8").split()
extra_flags = subprocess.check_output("make -f sdk6.mk cflags", shell=True).decode("utf-8").split()

obj_files = subprocess.check_output("make -f sdk6.mk objs", shell=True).decode("utf-8").split()
includes = path.join(sdk, 'include')

# Get all BCM shared libraries.
libs = []
libs_temp = subprocess.check_output("make -f sdk6.mk libs", shell=True).decode("utf-8").split()

ignore_libs = []

for lib in libs_temp:
    if  lib in ignore_libs:
        continue
    libs.append(":"+lib)

# Add standard libraries used in SDK
libs.append("rt")
libs.append("m")
libs.append("pthread")
libs.append("nsl")


ext_modules = []
py_modules = []

def mod_extension(dir,name):
    wfile = path.join(dir,name)
    part = name.split('.')
    py_module = part[-2].replace('_wrap','')

    binding_module = Extension(
                        name = "_" + py_module,
                        include_dirs=[includes],
                        libraries=libs,
                        library_dirs=lib_dir,
                        runtime_library_dirs=lib_dir,
                        extra_compile_args=extra_flags,
                        extra_link_args=extra_flags,
                        extra_objects=obj_files,
                        sources=[wfile])

    ext_modules.append(binding_module)
    py_modules.append(py_module)

# Make extensions out of all the SWIG wrappers

for root, dirs, files in walk('.'):
    for name in fnmatch.filter(files, '*_wrap.c'):
        if "./build" not in root:
            mod_extension(root,name)


setup(
    name = "SDK6PyBinding",
    version = version,
    packages = find_packages(),

    # Project uses reStructuredText
    # Optional: docutil for reading.
    # install_requires = ['docutils>=0.3'],

    author = "Broadcom Ltd.",
    description = "This utility is used to get python bindings for SDK6 C APIs.",
    long_description=long_description,
    ext_modules = ext_modules,
)
