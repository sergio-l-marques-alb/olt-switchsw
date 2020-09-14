/**
 * \file bcm_int/dnx/instru/instru_sflow.h
 * Internal DNX INSTRU APIs
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef INSTRU_SFLOW_H_INCLUDED
/*
 * {
 */
#define INSTRU_SFLOW_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/**
 * \brief
 * SFLOW - Mapping of system-port to Interface is UNKNOWN
 * Note:
 * According SFLOW SPEC V5 - 0 means "interface is not known"
 */
#define SFLOW_PACKET_INTERFACE_UNKNOWN  (0)
/*
 * }
 */

/*
  * Internal functions.
  * {
  */

/*
 * }
 */
#endif /* INSTRU_SFLOW_H_INCLUDED */
