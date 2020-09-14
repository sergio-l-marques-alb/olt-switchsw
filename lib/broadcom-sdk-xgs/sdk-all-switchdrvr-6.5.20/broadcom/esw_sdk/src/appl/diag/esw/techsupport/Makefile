# $Id: Makefile Broadcom SDK $
# $Copyright: (c) 2020 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
LOCALDIR = src/appl/diag/esw/techsupport

include ${SDK}/make/Make.config

subdirs := basic \
           l3uc \
           l3mc \
           mpls \
           mmu \
           oam \
           niv \
           riot \
           vlan \
           ifp \
           vfp \
           efp \
           cos \
           loadbalance \
           vxlan \
           dgm \
           tcb \
           pstat \
           fflow \
           phy \
           ledup \
           cmic \
           visibility\
           warmboot
subdirs += etrap
subdirs += int
subdirs += histogram
subdirs += alpm
subdirs += cancun
subdirs += xflow-macsec
subdirs += flowtracker
subdirs += cpri
subdirs += rsvd4

include ${SDK}/make/Make.subdirs

lib = libtechsupport

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
