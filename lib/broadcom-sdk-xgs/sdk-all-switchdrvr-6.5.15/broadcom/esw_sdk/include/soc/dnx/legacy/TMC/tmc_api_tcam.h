/* $Id: jer2_jer2_jer2_tmc_api_tcam.h,v 1.14 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/ppc/include/soc_jer2_jer2_jer2_tmcapi_tcam.h
*
* MODULE PREFIX:  soc_ppc_tcam
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

#ifndef __LEGACY_SOC_DNX_API_TCAM_INCLUDED__
/* { */
#define __LEGACY_SOC_DNX_API_TCAM_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <soc/dnx/legacy/TMC/tmc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of TCAM banks                                    */
#define  SOC_DNX_TCAM_NOF_BANKS (4)

/*     Number of TCAM cycles                                   */

#ifndef SOC_DNX_TCAM_NL_88650_MAX_NOF_ENTRIES
#if defined(INCLUDE_KBP)
/* Max number of entries: 1 million entries of 80b for KBP NL88650 */
#define SOC_DNX_TCAM_NL_88650_MAX_NOF_ENTRIES (1 << 20)
#else  /* defined(INCLUDE_KBP) */
/* No external TCAM entries without KBP */
#define SOC_DNX_TCAM_NL_88650_MAX_NOF_ENTRIES (2)
#endif /* defined(INCLUDE_KBP) */
#endif /* ifndef SOC_DNX_TCAM_NL_88650_MAX_NOF_ENTRIES */

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
   *  TCAM bank size of 72 bits
   */
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_72_BITS = 0,
  /*
   *  TCAM bank size of 144 bits
   */
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_144_BITS = 1,
  /*
   *  TCAM bank size of 288 bits
   */
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_288_BITS = 2,
  /* 
   *  TCAM bank size of 80 bits
   *  JER2_ARAD only
   */
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_80_BITS = 0,
  /* 
   *  TCAM bank size of 160 bits
   *  JER2_ARAD only
   */
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_160_BITS = 1,
  /* 
   *  TCAM bank size of 320 bits
   *  JER2_ARAD only
   */
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_320_BITS = 2,

  /*
   *  Number of types in SOC_DNX_TCAM_BANK_ENTRY_SIZE
   */
  SOC_DNX_NOF_TCAM_BANK_ENTRY_SIZES = 3

}SOC_DNX_TCAM_BANK_ENTRY_SIZE;

typedef enum
{
  /*
   *  The TCAM User is the IPv4 multicast forwarding database
   */
  SOC_DNX_TCAM_USER_FWDING_IPV4_MC = 0,
  /*
   *  The TCAM User is the IPv6 unicast forwarding database
   */
  SOC_DNX_TCAM_USER_FWDING_IPV6_UC = 1,
  /*
   *  The TCAM User is the IPv6 multicast forwarding database
   */
  SOC_DNX_TCAM_USER_FWDING_IPV6_MC = 2,
  /*
   *  The TCAM User is the Ingress Field Processor
   */
  SOC_DNX_TCAM_USER_FP = 3,
  /*
   *  The TCAM User is the Egress ACL
   */
  SOC_DNX_TCAM_USER_EGRESS_ACL = 4,
  /*
   *  Number of types in SOC_DNX_TCAM_USER_PB
   */
  SOC_DNX_NOF_TCAM_USERS 
}SOC_DNX_TCAM_USER;

typedef enum
{
  /*
   *  The TCAM Bank owner is the Ingress PMF Cycle 0
   */
  SOC_DNX_TCAM_BANK_OWNER_PMF_0 = 0,

  /*
   *  The TCAM Bank owner is the Ingress PMF Cycle 1
   */
  SOC_DNX_TCAM_BANK_OWNER_PMF_1 = 1,

  /*
   *  The TCAM Bank owner is the FLP
   */
  SOC_DNX_TCAM_BANK_OWNER_FLP_TCAM = 2,

  /*
   *  The TCAM Bank owner is the FLP
   */
  SOC_DNX_TCAM_BANK_OWNER_FLP_TRAPS = 3,

  /* 
   *  The TCAM Bank owner is the VT
   */
  SOC_DNX_TCAM_BANK_OWNER_VT = 4,

  /* 
   *  The TCAM Bank owner is the TT
   */
  SOC_DNX_TCAM_BANK_OWNER_TT = 5,

  /*
   *  The TCAM Bank owner is the Egress ACL
   */
  SOC_DNX_TCAM_BANK_OWNER_EGRESS_ACL = 6,  

  /*
   *  Number of types in JER2_ARAD_TCAM_BANK_OWNER
   */
  SOC_DNX_TCAM_NOF_BANK_OWNERS = 7 
}SOC_DNX_TCAM_BANK_OWNER;

typedef struct
{
  /*
   *  Prefix value. Range: 0 - 15.
   */
  uint32 val;
  /*
   *  Prefix length. Units: Bits. Range: 0 - 3.
   */
  uint32 nof_bits;

} SOC_DNX_TCAM_DB_PREFIX;

typedef struct
{
  /*
   *  Database priority (global). Range: 0 - 16K-1.
   */
  uint32 priority;
  /*
   *  Entry size.
   */
  SOC_DNX_TCAM_BANK_ENTRY_SIZE entry_size;
  /*
   *  Prefix length
   */
  uint32 prefix_length;

} SOC_DNX_TCAM_DB_USER;

typedef struct
{
  /*
   *  If True, then this Database is present in the (Bank,
   *  Cycle). Is valid only if this (Bank, Cycle) is reserved
   *  to this TCAM user.
   */
  uint8 is_present[SOC_DNX_TCAM_NOF_BANKS];
  /*
   *  Database entry prefix per (Bank, Cycle). Valid only if
   *  the Database is present in this (Bank, Cycle).
   */
  SOC_DNX_TCAM_DB_PREFIX prefix[SOC_DNX_TCAM_NOF_BANKS];
  /*
   *  User-Defined Database properties
   */
  SOC_DNX_TCAM_DB_USER user;

} SOC_DNX_TCAM_DB_INFO;

/* JER2_ARAD only defines { */
typedef enum
{
  /*
   *  TCAM action size of the first 20 bits
   *  (24 bits in Jericho)
   */
  SOC_DNX_TCAM_ACTION_SIZE_FIRST_20_BITS = 0x1,
  /*
   *  TCAM action size of the second 20 bits
   *  (24 bits in Jericho)
   */
  SOC_DNX_TCAM_ACTION_SIZE_SECOND_20_BITS = 0x2,
  /*
   *  TCAM action size of the third 20 bits
   *  (24 bits in Jericho).
   *  Relevant only in case bank entry size is 320bits.
   */
  SOC_DNX_TCAM_ACTION_SIZE_THIRD_20_BITS = 0x4,
  /*
   *  TCAM action size of the forth 20 bits
   *  (24 bits in Jericho).
   *  Relevant only in case bank entry size is 320bits.
   */
  SOC_DNX_TCAM_ACTION_SIZE_FORTH_20_BITS = 0x8,
  /* 
   *  TCAM number of action size types
   */
  SOC_DNX_NOF_TCAM_ACTION_SIZES = 4
  
}SOC_DNX_TCAM_ACTION_SIZE;
/* JER2_ARAD only defines } */
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
  SOC_DNX_TCAM_DB_PREFIX_clear(
    DNX_SAND_OUT SOC_DNX_TCAM_DB_PREFIX *info
  );

void
  SOC_DNX_TCAM_DB_USER_clear(
    DNX_SAND_OUT SOC_DNX_TCAM_DB_USER *info
  );

void
  SOC_DNX_TCAM_DB_INFO_clear(
    DNX_SAND_OUT SOC_DNX_TCAM_DB_INFO *info
  );


const char*
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_to_string(
    DNX_SAND_IN  SOC_DNX_TCAM_BANK_ENTRY_SIZE enum_val
  );
const char*
  SOC_DNX_TCAM_BANK_ENTRY_SIZE_JER2_ARAD_to_string(
    DNX_SAND_IN  SOC_DNX_TCAM_BANK_ENTRY_SIZE enum_val
  );
const char*
  SOC_DNX_TCAM_USER_to_string(
    DNX_SAND_IN  SOC_DNX_TCAM_USER enum_val
  );
const char*
  SOC_DNX_TCAM_BANK_OWNER_to_string(
    DNX_SAND_IN  SOC_DNX_TCAM_BANK_OWNER enum_val
  );

void
  SOC_DNX_TCAM_DB_PREFIX_print(
    DNX_SAND_IN  SOC_DNX_TCAM_DB_PREFIX *info
  );

void
  SOC_DNX_TCAM_DB_USER_print(
    DNX_SAND_IN  SOC_DNX_TCAM_DB_USER *info
  );

void
  SOC_DNX_TCAM_DB_INFO_print(
    DNX_SAND_IN  SOC_DNX_TCAM_DB_INFO *info
  );


/* } */


/* } __LEGACY_SOC_DNX_API_TCAM_INCLUDED__*/
#endif

