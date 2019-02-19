# -*- Makefile -*-
# $Id: Makefile,v 1.1.2.2 Broadcom SDK $
# $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
# OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
# BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$

#
# This make job requires the following environment variables to be set:
#
# SDK                - path to StrataXGS SDK root directory
#
# Optionally the following environment variables can be set to
# override the default build server configuration:
#
# PPC_TOOLS_DIR      - path to build tools (if not in PATH already)
# PPC_CROSS_COMPILE  - cross compile tools prefix
# LINUX_INCLUDE      - path to Linux kernel include directory
#

override kernel_version=2_6
platform=$(L7_CPU)-$(kernel_version)
LINUX_MAKE_USER=1
LINUX_MAKE_DIR := $(SDK)/systems/linux/user/fastpath_common
include ${SDK}/make/Make.linux

