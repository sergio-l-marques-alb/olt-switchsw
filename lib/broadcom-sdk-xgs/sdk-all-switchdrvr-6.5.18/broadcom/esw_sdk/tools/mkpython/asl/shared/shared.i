###########################################################
# FILE : shared.i
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
##########################################################

%module shared

%{
#define SWIG_FILE_WITH_INIT
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/bslext.h>

#include "shared.c"
%}

%include "typemaps.i"
%include "cpointer.i"
%include "carrays.i"

typedef enum bsl_severity_e {
    bslSeverityOff            = 0,
    bslSeverityFatal          = 1,
    bslSeverityError          = 2,
    bslSeverityWarn           = 3,
    bslSeverityInfo           = 4,
    bslSeverityVerbose        = 5,
    bslSeverityDebug          = 6,
    bslSeverityNormal         = bslSeverityInfo,
    bslSeverityCount          = 7
} bsl_severity_t;

void bcm_bsl_setup(bsl_severity_t level);
