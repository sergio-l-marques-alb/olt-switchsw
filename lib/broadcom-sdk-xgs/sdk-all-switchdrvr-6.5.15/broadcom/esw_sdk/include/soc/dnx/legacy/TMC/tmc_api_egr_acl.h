/* $Id: soc_jer2_jer2_jer2_tmcapi_egr_acl.h,v 1.3 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __SOC_DNX_API_EGR_ACL_INCLUDED__
/* { */
#define __SOC_DNX_API_EGR_ACL_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <soc/dnx/legacy/TMC/tmc_api_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_tcam.h>
#include <soc/dnx/legacy/TMC/tmc_api_tcam_key.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of Drop Precedence values                        */
#define  SOC_DNX_EGR_ACL_NOF_DP_VALUES (2)

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

typedef struct
{
  /*
   *  DP value. Range: 0 - 3.
   */
  uint32 val[SOC_DNX_EGR_ACL_NOF_DP_VALUES];

} SOC_DNX_EGR_ACL_DP_VALUES;

typedef struct
{
  /*
   *  Port ACL profile Range: 0 - 3.
   */
  uint32 profile;
  /*
   *  ACL Data part of the key construction. Range: 0 - 63.
   */
  uint32 acl_data;

} SOC_DNX_EGR_ACL_PORT_INFO;

typedef struct
{
  /*
   *  If True, the packet is trapped according to the Action
   *  profile 'trap_code'.
   */
  uint8 trap_en;
  /*
   *  Trap code: relevant only if the trapping 'trap_en' is
   *  enabled. Range: 0 - 7.
   */
  uint32 trap_code;
  /*
   *  OFP override. If True, the value in 'ofp' parameter is
   *  the Out-TM-Port.
   */
  uint8 ofp_ov;
  /*
   *  OFP (Out-TM-Port) value if an override is allowed.
   *  Range: 0 - 79.
   */
  uint32 ofp;
  /*
   *  Traffic Class and Drop Precedence override. If True, the
   *  values in 'tc' and 'dp' parameters are meaningful.
   */
  uint8 dp_tc_ov;
  /*
   *  Traffic Class value if an override is allowed. Range: 0
   *  - 7.
   */
  uint32 tc;
  /*
   *  Drop Precedence value if an override is allowed. Range:
   *  0 - 1. Set according to soc_pb_egr_acl_dp_values_set
   */
  uint32 dp;
  /*
   *  Copy-Unique-Data value if an override is allowed. A
   *  value equal to '0' disables the overriding.
   */
  uint32 cud;

} SOC_DNX_EGR_ACL_ACTION_VAL;

typedef struct
{
  /*
   *  Entry format: must be identical to its Database type.
   */
  SOC_DNX_EGR_ACL_DB_TYPE type;
  /*
   *  Key value. The key can be built with the use of helper
   *  functions (e.g., soc_pb_tcam_key_masked_val_set).
   */
  SOC_DNX_TCAM_KEY key;
  /*
   *  Entry priority.
   */
  uint16 priority;
  /*
   *  Action values for all the possible actions.
   */
  SOC_DNX_EGR_ACL_ACTION_VAL action_val;

} SOC_DNX_EGR_ACL_ENTRY_INFO;


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

void
  SOC_DNX_EGR_ACL_DP_VALUES_clear(
    DNX_SAND_OUT SOC_DNX_EGR_ACL_DP_VALUES *info
  );

void
  SOC_DNX_EGR_ACL_PORT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_EGR_ACL_PORT_INFO *info
  );

void
  SOC_DNX_EGR_ACL_ACTION_VAL_clear(
    DNX_SAND_OUT SOC_DNX_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_DNX_EGR_ACL_ENTRY_INFO_clear(
    DNX_SAND_OUT SOC_DNX_EGR_ACL_ENTRY_INFO *info
  );


void
  SOC_DNX_EGR_ACL_DP_VALUES_print(
    DNX_SAND_IN  SOC_DNX_EGR_ACL_DP_VALUES *info
  );

void
  SOC_DNX_EGR_ACL_PORT_INFO_print(
    DNX_SAND_IN  SOC_DNX_EGR_ACL_PORT_INFO *info
  );

void
  SOC_DNX_EGR_ACL_ACTION_VAL_print(
    DNX_SAND_IN  SOC_DNX_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_DNX_EGR_ACL_ENTRY_INFO_print(
    DNX_SAND_IN  SOC_DNX_EGR_ACL_ENTRY_INFO *info
  );


/* } */


/* } __SOC_DNX_API_EGR_ACL_INCLUDED__*/
#endif

