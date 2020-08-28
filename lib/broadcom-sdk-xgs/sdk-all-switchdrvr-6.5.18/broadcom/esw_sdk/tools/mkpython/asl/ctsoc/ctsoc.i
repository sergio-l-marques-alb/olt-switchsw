###########################################################
# FILE : ctsoc.i
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
##########################################################

%module ctsoc

%{
#define SWIG_FILE_WITH_INIT
#include <sal/types.h>
#include <appl/cputrans/atptrans_socket.h>

#include "ctsoc.c"
%}

%include "typemaps.i"
%include "cpointer.i"
%include "carrays.i"

typedef int bcm_error_t;
typedef unsigned int bcm_ip_t;
typedef unsigned int uint32;

bcm_error_t bcm_ctsoc_cpudb_create(const uint32 ukey);
bcm_error_t bcm_ctsoc_client_server_start(void);
bcm_error_t bcm_ctsoc_client_rpc_start(const int retry_ms, const int retries);
bcm_error_t bcm_ctsoc_client_install(uint32 dest_ukey, bcm_ip_t dest_ip);
bcm_error_t bcm_ctsoc_setup();
bcm_error_t bcm_ctsoc_client_attach(const int local_unit,
                                    const int remote_unit,
                                    const uint32 ukey);
bcm_error_t
bcm_ctsoc_socket_config_set(int local_port, int remote_port);

void atptrans_socket_show(void);
void bcm_ctsoc_mem_control(const int use_heap);
