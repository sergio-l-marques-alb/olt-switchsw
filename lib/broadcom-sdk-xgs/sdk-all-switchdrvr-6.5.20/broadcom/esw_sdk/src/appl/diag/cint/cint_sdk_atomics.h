/*
 * $Id$
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __CINT_SDK_ATOMICS_H__
#define __CINT_SDK_ATOMICS_H__

#include <sdk_config.h>

#ifdef INCLUDE_LIB_CINT

#include <cint_interpreter.h>
#include <cint_types.h>

extern cint_atomic_type_t cint_sdk_atomics[]; 
extern cint_data_t cint_sdk_data; 
#ifdef INCLUDE_XFLOW_MACSEC
extern cint_data_t cint_xflow_macsec_data;
#endif

#endif /* INCLUDE_LIB_CINT */

#endif /* __CINT_SDK_ATOMICS_H__ */
