#!/bin/sh

scp ipl/switchdrvr ipl/devshell_symbols.gz asantos@10.112.15.92:/home/asantos
scp target/*.ko asantos@10.112.15.92:/home/asantos
scp ../../../../fastpath.cli/bin/fastpath.cli asantos@10.112.15.92:/home/asantos
scp ../../../../fastpath.shell/bin/fastpath.shell asantos@10.112.15.92:/home/asantos
#ssh asantos@10.112.15.92 'sudo su; cp switchdrvr devshell_symbols.gz ../olt/images_generator/CXP360G/CXP360G_matrix_fs/usr/local/ptin/sbin/.'
