###########################################################
# FILE : cputrans.i
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
##########################################################

%module cputrans

%{
#define SWIG_FILE_WITH_INIT
#include <sal/types.h>
#include <appl/cputrans/atptrans_socket.h>
%}

%include "typemaps.i"
%include "cpointer.i"
%include "carrays.i"

void atptrans_socket_show(void);
