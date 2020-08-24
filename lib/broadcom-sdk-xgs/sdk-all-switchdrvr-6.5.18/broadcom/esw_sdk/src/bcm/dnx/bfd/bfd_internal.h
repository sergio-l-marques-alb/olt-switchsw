/** \file bfd_internal.h
 * General BFD - contains internal functions and definitions for
 * BFD feature support 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BFD_INTERNAL_H_INCLUDED
/*
 * {
 */
#define BFD_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include "include/bcm/bfd.h"

/** 
 *  Number of less significant bits that are not considered
 *  for discriminator range
 */
#define DISCR_RANGE_BIT_SHIFT 18

/** 
 *  When the BFD endpoint has one of these tunneling types,
 *  incoming packets are classified by discriminator
 */
#define BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(type) ((type == bcmBFDTunnelTypeUdp) || (type == bcmBFDTunnelTypeMpls))

/** Macro to differentiate the "simpler" BFD endpoints */
#define BFD_TYPE_NOT_PWE(x) ((x == bcmBFDTunnelTypeUdp) || (x == bcmBFDTunnelTypeMpls))

/**
 * Constant value greater than 0x1fff (max value possible for an OAM MEP name) to indicate
 * it is BFD. 
 *
 */
#define DNX_BFD_RMEP_SW_INFO_NAME_CONST 0xffff

/*
 * Global and Static
 */

/* Callbacks are not supported by warmboot. Need to re-register after warmboot */
bcm_bfd_event_cb _g_bfd_event_cb[BCM_MAX_NUM_UNITS][bcmBFDEventCount];

/*User Data*/
void *_g_bfd_event_ud[BCM_MAX_NUM_UNITS][bcmBFDEventCount];

/*
 * }
 */

#endif /* INTERNAL_OAM_INCLUDED */
