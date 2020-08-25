/* $Id: jer2_jer2_jer2_tmc_api_tdm.h,v 1.5 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_jer2_jer2_tmc/include/soc_jer2_jer2_jer2_tmcapi_tdm.h
*
* MODULE PREFIX:  soc_jer2_jer2_jer2_tmctdm
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_DNX_API_TDM_INCLUDED__
/* { */
#define __SOC_DNX_API_TDM_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/SAND/Utils/sand_u64.h>

#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <soc/dnx/legacy/TMC/tmc_api_general.h>

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

typedef enum
{
  /*
   *  Add an FTMH header to all the received TDM cells at this
   *  ITM-Port. If set, the FTMH fields must be configured via
   *  the 'ftmh' parameter and are kept during the cell
   *  switching.
   */
  SOC_DNX_TDM_ING_ACTION_ADD = 0,
  /*
   *  Do not change the FTMH header to all the received TDM
   *  cells at this ITM-Port - the FMTH must be already there.
   */
  SOC_DNX_TDM_ING_ACTION_NO_CHANGE = 1,
  /*
   *  Embed an external customer header in an added Standard
   *  FTMH Header. Relevant only when the FTMH format is
   *  Standard. Must be used for interoperability with a PMC
   *  HyPhy PMM header.
   */
  SOC_DNX_TDM_ING_ACTION_CUSTOMER_EMBED = 2,
  /*
   *  Number of types in SOC_DNX_TDM_ING_ACTION
   */
  SOC_DNX_TDM_NOF_ING_ACTIONS = 3
}SOC_DNX_TDM_ING_ACTION;

typedef enum
{
  /*
   *  Remove the FTMH header to all the transmitted TDM cells
   *  at this OFP-Port.
   */
  SOC_DNX_TDM_EG_ACTION_REMOVE = 0,
  /*
   *  Do not change the FTMH header to all the transmitted TDM
   *  cells at this OFP-Port.
   */
  SOC_DNX_TDM_EG_ACTION_NO_CHANGE = 1,
  /*
   *  Extract an external customer overhead from a removed
   *  Standard FTMH Header. Relevant only when the FTMH format
   *  is Standard. Must be used for interoperability with a
   *  PMC HyPhy PMM header.
   */
  SOC_DNX_TDM_EG_ACTION_CUSTOMER_EXTRACT = 2,
  /*
   *  Number of types in SOC_DNX_TDM_EG_ACTION
   */
  SOC_DNX_TDM_NOF_EG_ACTIONS = 3
}SOC_DNX_TDM_EG_ACTION;

typedef struct
{
  /*
   *  Destination interface: the egress interface with its
   *  channel. Range: 0 - 31.
   */
  uint32 dest_if;
  /*
   *  Destination FAP Id. Range: 0 - 1K-1.
   */
  uint32 dest_fap_id;

} SOC_DNX_TDM_FTMH_OPT_UC;

typedef struct
{
  /*
   *  Multicast Id. Range: 0 - 16K-1.
   */
  uint32 mc_id;

} SOC_DNX_TDM_FTMH_OPT_MC;

typedef struct
{
  /*
   *  The user-defined fields (i.e., 32 bits Customer Header
   *  to embed). Are embedded to the bits 47:32, 23:20, 19:17
   *  and 15:14 in the header according to a static mapping.
   */
  uint32 user_def;  
  /*
   *  Destination system physical port. Range: 0 - 4K-1.
   *  Invalid for JER2_ARAD.
   */
  uint32 sys_phy_port;
  /* 
   *  Destination FAP ID. Range: 0-2047.
   *  Invalid for Soc_petra-B.
   */
  uint32 dest_fap_id;
  /* 
   *  Destination FAP Port. Range: 0-255.
   *  Invalid for Soc_petra-B.
   */
  uint32 dest_fap_port;
  /* 
   *  The user-define fields for bits 33-SOC_DNX_TDM_MAX_USER_DEFINE_FIELDS.
   *  Are embedded to the bits 57:41 in the header according to a static
   *  mapping. Valid only for JER2_ARAD FTMH (i.e. without Soc_petra-B in system).
   *  Invalid for Soc_petra-B. 
   */
  uint32 user_def_2;

} SOC_DNX_TDM_FTMH_STANDARD_UC;

typedef struct
{
  /*
   *  The user-defined fields (i.e., 32 bits Customer Header
   *  to embed). Are embedded to the bits 47:32, 23:20, 19:17
   *  and 15:14 in the header according to a static mapping.
   */
  uint32 user_def;
  /*
   *  Multicast Id. Range: 0 - 16K-1.
   */
  uint32 mc_id;

} SOC_DNX_TDM_FTMH_STANDARD_MC;

typedef union
{
  /*
   *  The FTMH header fields to configure of a TDM unicast
   *  cell with an Optimized FTMH.
   */
  SOC_DNX_TDM_FTMH_OPT_UC opt_uc;
  /*
   *  The FTMH header fields to configure of a TDM multicast
   *  cell with an Optimized FTMH.
   */
  SOC_DNX_TDM_FTMH_OPT_MC opt_mc;
  /*
   *  The FTMH header fields to configure of a TDM unicast
   *  cell with a Standard FTMH.
   */
  SOC_DNX_TDM_FTMH_STANDARD_UC standard_uc;
  /*
   *  The FTMH header fields to configure of a TDM multicast
   *  cell with a Standard FTMH.
   */
  SOC_DNX_TDM_FTMH_STANDARD_MC standard_mc;

} SOC_DNX_TDM_FTMH;

typedef struct
{
  /*
   *  Action (adding or not) to operate when the TDM cells are
   *  received.
   */
  SOC_DNX_TDM_ING_ACTION action_ing;
  /*
   *  If TRUE, then the destination FTMH to add is of type
   *  multicast. Must be set only if the ingress action is 'ADD'.
   */
  uint8 is_mc;
  /*
   *  FTMH to add to the received TDM cells. Must be set only
   *  if the ingress action is 'ADD'.
   */
  SOC_DNX_TDM_FTMH ftmh;
  /*
   *  Action (removing or not) to operate when the TDM cells
   *  are transmitted.
   */
  SOC_DNX_TDM_EG_ACTION action_eg;
  
} SOC_DNX_TDM_FTMH_INFO;

typedef struct
{
  /*
   *  Bitmap of the fabric links to define the set of links
   *  used for specific MC route cell. For each link (in the
   *  range 0 - 35), if its bit is set, then the respective
   *  link is used for this Multicast route.
   */
  DNX_SAND_U64 link_bitmap;

} SOC_DNX_TDM_MC_STATIC_ROUTE_INFO;

typedef struct
{
  /*
   *  Bitmap of the fabric links to define the set of links
   *  used for direct routing. For each link (in the
   *  range 0 - 35), if its bit is set, then the respective
   *  link is used for this Multicast route.
   */
  DNX_SAND_U64 link_bitmap;

} SOC_DNX_TDM_DIRECT_ROUTING_INFO;


#define SOC_DNX_TDM_PUSH_QUEUE_TYPE                            (SOC_DNX_ITM_QT_NDX_15)

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
  SOC_DNX_TDM_FTMH_OPT_UC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_OPT_UC *info
  );

void
  SOC_DNX_TDM_FTMH_OPT_MC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_OPT_MC *info
  );

void
  SOC_DNX_TDM_FTMH_STANDARD_UC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_STANDARD_UC *info
  );

void
  SOC_DNX_TDM_FTMH_STANDARD_MC_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_STANDARD_MC *info
  );

void
  SOC_DNX_TDM_FTMH_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH *info
  );

void
  SOC_DNX_TDM_FTMH_INFO_clear(
    DNX_SAND_OUT SOC_DNX_TDM_FTMH_INFO *info
  );
void
  SOC_DNX_TDM_MC_STATIC_ROUTE_INFO_clear(
    DNX_SAND_OUT SOC_DNX_TDM_MC_STATIC_ROUTE_INFO *info
  );

void
  SOC_DNX_TDM_DIRECT_ROUTING_INFO_clear(
    DNX_SAND_OUT SOC_DNX_TDM_DIRECT_ROUTING_INFO *info
  );


const char*
  SOC_DNX_TDM_ING_ACTION_to_string(
    DNX_SAND_IN  SOC_DNX_TDM_ING_ACTION enum_val
  );

const char*
  SOC_DNX_TDM_EG_ACTION_to_string(
    DNX_SAND_IN  SOC_DNX_TDM_EG_ACTION enum_val
  );

void
  SOC_DNX_TDM_FTMH_OPT_UC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_OPT_UC *info
  );

void
  SOC_DNX_TDM_FTMH_OPT_MC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_OPT_MC *info
  );

void
  SOC_DNX_TDM_FTMH_STANDARD_UC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_STANDARD_UC *info
  );

void
  SOC_DNX_TDM_FTMH_STANDARD_MC_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_STANDARD_MC *info
  );

void
  SOC_DNX_TDM_FTMH_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH *info
  );

void
  SOC_DNX_TDM_FTMH_INFO_print(
    DNX_SAND_IN  SOC_DNX_TDM_FTMH_INFO *info
  );
void
  SOC_DNX_TDM_MC_STATIC_ROUTE_INFO_print(
    DNX_SAND_IN  SOC_DNX_TDM_MC_STATIC_ROUTE_INFO *info
  );

void
  SOC_DNX_TDM_DIRECT_ROUTING_INFO_print(
    DNX_SAND_IN  SOC_DNX_TDM_DIRECT_ROUTING_INFO *info
  );


/* } */


/* } __SOC_DNX_API_TDM_INCLUDED__*/
#endif
