#!/tools/bin/python
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

"""
setup.py file for SDK6 CPUDB
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

cpudb_module = Extension(
    '_cpudb',
    include_dirs = [os.environ['SDK'] + "/include"],
    extra_compile_args = extra_compile_args,
    sources = ['cpudb_wrap.c'],
)

setup(
    name = 'cpudb',
    version = '0.1',
    author      = 'Broadcom Ltd.',
    description = 'SDK6 CPUDB',
    ext_modules = [cpudb_module],
    py_modules = ['cpudb'],
    script_args = ["build_ext", "--inplace", "--force"]
)
os.chdir(cwd)
