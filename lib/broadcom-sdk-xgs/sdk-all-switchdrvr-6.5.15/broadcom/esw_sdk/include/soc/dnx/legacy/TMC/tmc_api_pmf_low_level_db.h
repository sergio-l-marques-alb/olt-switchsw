/* $Id: jer2_jer2_jer2_tmc_api_pmf_low_level_db.h,v 1.10 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/jer2_jer2_jer2_tmc/include/soc_jer2_jer2_jer2_tmcapi_pmf_low_level.h
*
* MODULE PREFIX:  soc_jer2_jer2_jer2_tmcpmf
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

#ifndef __SOC_DNX_API_PMF_LOW_LEVEL_DB_INCLUDED__
/* { */
#define __SOC_DNX_API_PMF_LOW_LEVEL_DB_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <soc/dnx/legacy/TMC/tmc_api_pmf_low_level_ce.h>
#include <soc/dnx/legacy/TMC/tmc_api_tcam.h>
#include <soc/dnx/legacy/TMC/tmc_api_tcam_key.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of banks per TCAM.                       */
#define  SOC_DNX_PMF_TCAM_NOF_BANKS (SOC_DNX_TCAM_NOF_BANKS)

/*     Number of final TCAM lookup results.                    */
#define  SOC_DNX_PMF_TCAM_NOF_LKP_RESULTS (4)

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
   *  Key A [9:0]
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_A_9_0 = 0,
  /*
   *  Key A [19:10]
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_A_19_10 = 1,
  /*
   *  Key A [29:20]
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_A_29_20 = 2,
  /*
   *  Key A [39:30].
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_A_39_30 = 3,
  /*
   *  Key B [9:0]
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_B_9_0 = 4,
  /*
   *  Key B [19:10]
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_B_19_10 = 5,
  /*
   *  Key B [29:20]
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_B_29_20 = 6,
  /*
   *  Key B [39:30].
   */
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_B_39_30 = 7,
  /*
   *  Number of types in SOC_DNX_PMF_DIRECT_TBL_KEY_SRC
   */
  SOC_DNX_NOF_PMF_DIRECT_TBL_KEY_SRCS = 8
}SOC_DNX_PMF_DIRECT_TBL_KEY_SRC;


typedef struct
{
  /*
   *  TCAM Database-ID. Range: 0 - 16K-1. (Soc_petra-B)
   *  In an explicit address mode, bank-id. Range: 0 - 3.
   */
  uint32 db_id;
  /*
   *  Entry index in the TCAM Database. Range: 0 - 16K-1.
   *  In an explicit address mode, line-id of the TCAM bank. Range: 0 - 511.
   */
  uint32 entry_id;

} SOC_DNX_PMF_TCAM_ENTRY_ID;

typedef struct
{
  /*
   *  Lookup-Profile-Id. Range: 0 - 7. (Soc_petra-B)
   */
  uint32 id;
  /*
   *  Cycle index in which the access occurs. Range: 0 - 1.
   *  (Soc_petra-B)
   */
  uint32 cycle_ndx;

} SOC_DNX_PMF_LKP_PROFILE;

typedef struct
{
  /*
   *  If True, then this bank is considered during the key
   *  lookup to get the TCAM lookup result.
   */
  uint8 bank_id_enabled[SOC_DNX_PMF_TCAM_NOF_BANKS];

} SOC_DNX_PMF_TCAM_BANK_SELECTION;

typedef struct
{
  /*
   *  Select how to get lookup result: which banks to compare.
   *  The first lookup result can compare between all the
   *  banks, the second between two banks at the most, after
   *  that the lookup result must equal a single bank result.
   */
  SOC_DNX_PMF_TCAM_BANK_SELECTION bank_sel[SOC_DNX_PMF_TCAM_NOF_LKP_RESULTS];

} SOC_DNX_PMF_TCAM_RESULT_INFO;

typedef struct
{
  /*
   *  If True, the Lookup-Profile points to this Database.
   */
  uint8 is_valid;
  /*
   *  Key source of the key entering the TCAM.
   */
  SOC_DNX_PMF_TCAM_KEY_SRC key_src;
  /*
   *  Bank selection per TCAM lookup result: set the validity
   *  and priority of each TCAM bank.
   */
  SOC_DNX_PMF_TCAM_RESULT_INFO bank_prio;

} SOC_DNX_PMF_TCAM_KEY_SELECTION_INFO;




typedef struct
{
  /*
   *  TCAM key with raw bitmap presentation.
   */
  SOC_DNX_TCAM_KEY key;
  /*
   *  Entry priority in the TCAM Database. Range: 0 - 16K-1.
   */
  uint32 priority;
  /*
   *  TCAM output value. For Soc_petra-B, it is encoded in 32
   *  bits.
   */
  SOC_DNX_TCAM_OUTPUT output;

} SOC_DNX_PMF_TCAM_DATA;

typedef struct
{
  /*
   *  Value of the direct table output. Range: 0 - 2^20-1.
   */
  uint32 val;

} SOC_DNX_PMF_DIRECT_TBL_DATA;

typedef enum
{
  /*
   *  Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_ETH = 0,
  /*
   *  MAC-in-MAC
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_ETH_ETH = 1,
  /*
   *  IPv4 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH = 2,
  /*
   *  IPv6 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH = 3,
  /*
   *  MPLS over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH = 5,
  /*
   *  MPLS x 2 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH = 6,
  /*
   *  MPLS x 3 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH = 7,
  /*
   *  FC standard over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_FC_STD_ETH = 8,
  /*
   *  FC with Encap over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH = 9,
  /*
   *  Ethernet over IP over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_ETH_IPV4_ETH = 10,
  /*
   *  Ethenet over TRILL over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH = 12,
  /*
   *  Ethernet over MPLS over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH = 13,
  /*
   *  Ethernet over MPLSx2 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH = 14,
  /*
   *  Ethernet over MPLSx3 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH = 15,
  /*
   *  IPv4 over IPv4 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH = 18,
  /*
   *  IPv4 over IPv6 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV6_ETH = 19,
  /*
   *  IPv4 over MPLS over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH = 21,
  /*
   *  IPv4 over MPLSx2 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH = 22,
  /*
   *  IPv4 over MPLSx3 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH = 23,
  /*
   *  IPv6 over IPv4 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH = 26,
  /*
   *  IPv6 over IPv6 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV6_ETH = 27,
  /*
   *  IPv6 over MPLS over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH = 29,
  /*
   *  IPv6 over MPLSx2 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH = 30,
  /*
   *  IPv6 over MPLSx3 over Ethernet
   */
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH = 31,
  /*
   *  Number of types in SOC_DNX_PARSER_PKT_HDR_STK_TYPE
   */
  SOC_DNX_PARSER_NOF_PKT_HDR_STK_TYPES = 23
}SOC_DNX_PARSER_PKT_HDR_STK_TYPE;


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
  SOC_DNX_PMF_TCAM_ENTRY_ID_clear(
    DNX_SAND_OUT SOC_DNX_PMF_TCAM_ENTRY_ID *info
  );

void
  SOC_DNX_PMF_LKP_PROFILE_clear(
    DNX_SAND_OUT SOC_DNX_PMF_LKP_PROFILE *info
  );

void
  SOC_DNX_PMF_TCAM_BANK_SELECTION_clear(
    DNX_SAND_OUT SOC_DNX_PMF_TCAM_BANK_SELECTION *info
  );

void
  SOC_DNX_PMF_TCAM_DATA_clear(
    DNX_SAND_OUT SOC_DNX_PMF_TCAM_DATA *info
  );


void
  SOC_DNX_PMF_TCAM_RESULT_INFO_clear(
    DNX_SAND_OUT SOC_DNX_PMF_TCAM_RESULT_INFO *info
  );

void
  SOC_DNX_PMF_TCAM_KEY_SELECTION_INFO_clear(
    DNX_SAND_OUT SOC_DNX_PMF_TCAM_KEY_SELECTION_INFO *info
  );

void
  SOC_DNX_PMF_DIRECT_TBL_DATA_clear(
    DNX_SAND_OUT SOC_DNX_PMF_DIRECT_TBL_DATA *info
  );


const char*
  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC_to_string(
    DNX_SAND_IN  SOC_DNX_PMF_DIRECT_TBL_KEY_SRC enum_val
  );

void
  SOC_DNX_PMF_TCAM_ENTRY_ID_print(
    DNX_SAND_IN  SOC_DNX_PMF_TCAM_ENTRY_ID *info
  );

void
  SOC_DNX_PMF_LKP_PROFILE_print(
    DNX_SAND_IN  SOC_DNX_PMF_LKP_PROFILE *info
  );

void
  SOC_DNX_PMF_TCAM_BANK_SELECTION_print(
    DNX_SAND_IN  SOC_DNX_PMF_TCAM_BANK_SELECTION *info
  );

void
  SOC_DNX_PMF_TCAM_RESULT_INFO_print(
    DNX_SAND_IN  SOC_DNX_PMF_TCAM_RESULT_INFO *info
  );

void
  SOC_DNX_PMF_TCAM_KEY_SELECTION_INFO_print(
    DNX_SAND_IN  SOC_DNX_PMF_TCAM_KEY_SELECTION_INFO *info
  );


void
  SOC_DNX_PMF_TCAM_DATA_print(
    DNX_SAND_IN  SOC_DNX_PMF_TCAM_DATA *info
  );

void
  SOC_DNX_PMF_DIRECT_TBL_DATA_print(
    DNX_SAND_IN  SOC_DNX_PMF_DIRECT_TBL_DATA *info
  );

const char*
  SOC_DNX_PARSER_PKT_HDR_STK_TYPE_to_string(
    DNX_SAND_IN  SOC_DNX_PARSER_PKT_HDR_STK_TYPE enum_val
  );


/* } */


/* } __SOC_DNX_API_PMF_LOW_LEVEL_DB_INCLUDED__*/
#endif

