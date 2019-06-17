/** \file jer2_multicast_fabric.h
 *
 * Functions for handling fabric multicast.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __JER2_MULTICAST_FABRIC_INCLUDED__
/* { */
#define __JER2_MULTICAST_FABRIC_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/TMC/tmc_api_multicast_fabric.h>

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
 *   Set the Fabric Multicast credit generator configuration.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core to set the FMC configuration upon.
 * \param [in] info -
 *   FMC configurations info.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Set whether credits come directly or through scheduling scheme.
 *   Set shapers on the FMC classes.
 * \see
 *   None.
 */
int
  jer2_mult_fabric_credit_source_set(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  int                        core,
    DNX_SAND_IN  SOC_DNX_MULT_FABRIC_INFO      *info);

/**
 * \brief
 *   Get the Fabric Multicast credit generator configuration.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core to set the FMC configuration upon.
 * \param [out] info -
 *   FMC configurations info.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Get whether credits come directly or through scheduling scheme.
 *   Get shapers on the FMC classes.
 * \see
 *   None.
 */
int
  jer2_mult_fabric_credit_source_get(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  int                        core,
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_INFO      *info);

/**
 * \brief
 *   Set FMQs range in enhance mode.
 * \param [in] unit -
 *   The unit number.
 * \param [in] queue_range -
 *   FMQs queues range (min queue and max queue).
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Min queue should be 0.
 *   Max queue should be in ((multiples of region size) - 1).
 *   The credits to these queues are comming according to a
 *   scheduling scheme.
 * \see
 *   None.
 */
int
  jer2_mult_fabric_enhanced_set(
    DNX_SAND_IN  int                                 unit,
    DNX_SAND_IN  DNX_SAND_U32_RANGE                  *queue_range);

/**
 * \brief
 *   Get FMQs range in enhance mode.
 * \param [in] unit -
 *   The unit number.
 * \param [out] queue_range -
 *   FMQs queues range (min queue and max queue).
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Min queue should be 0.
 *   Max queue should be in ((multiples of region size) - 1).
 *   The credits to these queues are comming according to a
 *   scheduling scheme.
 * \see
 *   None.
 */
int
  jer2_mult_fabric_enhanced_get(
    DNX_SAND_IN  int                                 unit,
    DNX_SAND_INOUT DNX_SAND_U32_RANGE                *queue_range);

/* } */

/* } __JER2_MULTICAST_FABRIC_INCLUDED__*/
#endif
