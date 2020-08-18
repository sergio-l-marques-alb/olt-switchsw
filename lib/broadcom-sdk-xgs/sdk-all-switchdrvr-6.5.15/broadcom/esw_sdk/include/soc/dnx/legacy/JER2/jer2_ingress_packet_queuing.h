/** \file jer2_ingress_packet_queuing.h
 *
 * Functions for handling Ingress Packet Queuing (IPQ).
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __JER2_INGRESS_PACKET_QUEUING_INCLUDED__
/* { */
#define __JER2_INGRESS_PACKET_QUEUING_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_packet_queuing.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/**
 * \brief
 *   Initialization of IPQ related tables and registers.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Called as part of the initialization sequence.
 * \see
 *   None.
 */
int
  jer2_ipq_init(
    DNX_SAND_IN int unit);

/* } */

/* } __JER2_INGRESS_PACKET_QUEUING_INCLUDED__*/
#endif

