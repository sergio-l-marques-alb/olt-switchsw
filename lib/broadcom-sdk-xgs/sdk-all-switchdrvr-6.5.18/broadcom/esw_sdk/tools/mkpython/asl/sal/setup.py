#!/tools/bin/python
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

"""
setup.py file for SDK6 SAL
"""

from distutils.core import setup, Extension
import os, sys
import argparse
import flags

parser = argparse.ArgumentParser(description='Bind py and C files')
parser.add_argument('-d', '--directory', help="Python files directory",
        required=True)
args = parser.parse_args(sys.argv[1:])
pydir = args.directory

extra_compile_args = flags.cfgflags
extra_compile_args = extra_compile_args.split()

cwd = os.getcwd()
os.chdir(pydir)

obj_files = flags.objects
obj_files = obj_files.split()

sal_module = Extension(
    '_sal',
    include_dirs = [os.environ['SDK'] + "/include"],
    libraries = flags.bcm_libs,
    library_dirs = [flags.lib_dir],
    runtime_library_dirs = [flags.lib_dir],
    extra_compile_args = extra_compile_args,
    extra_objects=obj_files,
    sources=['sal_wrap.c'],
)

setup(
    name = 'sal',
    version = '0.1',
    author      = 'Broadcom Ltd.',
    description = 'SDK6 SAL',
    ext_modules = [sal_module],
    py_modules = ['sal'],
    script_args=["build_ext", "--inplace", "--force"]
)
os.chdir(cwd)
