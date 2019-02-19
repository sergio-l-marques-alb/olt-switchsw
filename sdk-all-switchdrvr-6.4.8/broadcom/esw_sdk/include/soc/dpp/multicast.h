/* $Id: multicast.h,v  $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/

#ifndef __DPP_MULTICAST_H__
#define __DPP_MULTICAST_H__

/*
 * This file contains joint multicast interfaces and mechanisms between mutiple dpp devices
 */

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/types.h>
#include <soc/dpp/TMC/tmc_api_multicast_ingress.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>
#include <shared/swstate/sw_state.h>
/*#include <soc/dpp/drv.h>*/

/*************
 * GLOBALS   *
 *************/


/*************
 *  MACROS   *
 *************/
/* { */

/* macros accessing device specific multicast functions */
#define MCDS_GET_COMMON_MEMBER(mcds, m) (((dpp_mcds_common_t*)(mcds))->m)

#define MCDS_INGRESS_LINK_END(mcds) MCDS_GET_COMMON_MEMBER(mcds, ingress_link_end)
#define MCDS_GET_MCDB_ENTRY(mcds, index) MCDS_GET_COMMON_MEMBER((mcds), get_mcdb_entry_from_mcds)((mcds), (index))

#define MCDS_GET_MCDB_ENTRY_TYPE(mcds, entry) MCDS_GET_COMMON_MEMBER(mcds, get_mcdb_entry_type)(entry)
#define MCDS_SET_MCDB_ENTRY_TYPE(mcds, entry, type_value) MCDS_GET_COMMON_MEMBER(mcds, set_mcdb_entry_type)((entry), (type_value))
#define MCDS_GET_MCDB_ENTRY_TYPE_FROM_MCDS(mcds, index) MCDS_GET_MCDB_ENTRY_TYPE(mcds, MCDS_GET_MCDB_ENTRY((mcds), (index)))
#define MCDS_SET_MCDB_ENTRY_TYPE_FROM_MCDS(mcds, index, type_value) MCDS_SET_MCDB_ENTRY_TYPE(mcds, MCDS_GET_MCDB_ENTRY((mcds), (index)), (type_value))

#define MCDS_GET_NEXT_POINTER(mcds, unit, entry, entry_type, next_entry) MCDS_GET_COMMON_MEMBER(mcds, get_next_pointer)((unit), (entry), (entry_type), (next_entry))
#define MCDS_SET_NEXT_POINTER(mcds, unit, entry, entry_type, next_entry) MCDS_GET_COMMON_MEMBER(mcds, set_next_pointer)((unit), (entry), (entry_type), (next_entry))


/* get and set the entry type */
#define JER_SWDB_ENTRY_GET_TYPE(entry) ((entry)->word1 >> JER_SWDB_MCDB_TYPE_SHIFT) /* assumes the usage of the msb bits */
#define JER_SWDB_ENTRY_SET_TYPE(entry, type_value) /* assumes the usage of the msb bits */ \
    do {(entry)->word1 = ((entry)->word1 & ~(JER_SWDB_MCDB_TYPE_MASK << JER_SWDB_MCDB_TYPE_SHIFT)) | \
      ((type_value) << JER_SWDB_MCDB_TYPE_SHIFT); } while (0)
#define JER_SWDB_MCDB_GET_TYPE(mcds, index) JER_SWDB_ENTRY_GET_TYPE((mcds)->mcdb + (index)) /* assumes the usage of the msb bits */
#define JER_SWDB_MCDB_SET_TYPE(mcds, index, type_value) JER_SWDB_ENTRY_SET_TYPE((mcds)->mcdb + (index), (type_value)) /* assumes the usage of the msb bits */
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* types of replication information, that can be passed between functions inside the mcds and prevent the need for allocation */

/* definition of virtual types, where per device a real type is used instead */
typedef void dpp_mcds_t;       /* Points to the multicast data structure of a device.
                                  This must be a structure whose first member is dpp_mcds_common_t
                                  to implement the polymorphism / joint interfaces. */
typedef void dpp_mcdb_entry_t; /* points to contains one MCDB entry */

typedef uint32 dpp_mc_id_t;
 
/* virtual functions - functions that may be implemented differently (different function pointer in dpp_mcds_common_t) per device */

/* Get a pointer to the mcdb entry with the given index in the mcds */
typedef dpp_mcdb_entry_t* (*dpp_get_mcdb_entry_from_mcds_f)(
    SOC_SAND_IN  dpp_mcds_t* mcds,
    SOC_SAND_IN  uint32 mcdb_index
  );
/* Get the type of a MCDB entry */
typedef uint32 (*dpp_get_mcdb_entry_type_f)(
    SOC_SAND_IN  dpp_mcdb_entry_t* entry
  );
/* set the type of a MCDB entry */
typedef void (*dpp_set_mcdb_entry_type_f)(
    SOC_SAND_INOUT  dpp_mcdb_entry_t* entry,
    SOC_SAND_IN     uint32 type_value
  );


/*
 * Get the (pointer to the) next entry from the given entry.
 * The entry type (ingress/egress/egress) TDM is given as an argument.
 */
typedef uint32 (*dpp_get_next_pointer_f)(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry,      /* entry from which to get the next entry pointer */
    SOC_SAND_IN  uint32  entry_type, /* the type of the entry */
    SOC_SAND_OUT uint32  *next_entry /* the output next entry */
  );
  
/*
 * Set the pointer to the next entry in the given entry.
 * The entry type (ingress/egress/egress) TDM is given as an argument.
 * changes both the mcds and hardware.
 */
typedef uint32 (*dpp_set_next_pointer_f)(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry_to_set, /* index of the entry in which to set the pointer */
    SOC_SAND_IN  uint32  entry_type,   /* the type of entry_to_set */
    SOC_SAND_IN  uint32  next_entry    /* the entry that entry_to_set will point to */
  );


typedef struct {
    /* functions to handle various multicast functionality */
    dpp_get_mcdb_entry_from_mcds_f get_mcdb_entry_from_mcds;
    dpp_get_mcdb_entry_type_f get_mcdb_entry_type;
    dpp_set_mcdb_entry_type_f set_mcdb_entry_type;
    dpp_get_next_pointer_f get_next_pointer;
    dpp_set_next_pointer_f set_next_pointer;

    uint32 ingress_link_end; /* link pointer marking the end of an ingress linked list */
    uint32 flags;
} dpp_mcds_common_t;

typedef struct
{
  PARSER_HINT_ARR SHR_BITDCL *egress_groups_open_data; /* replicate the information of is each egress group, in warm boot it is stored to the warm boot file */
  PARSER_HINT_ARR SHR_BITDCL *cud2core;           /* For Jericho dual core, store for each CUD to port mapping, what is the core of the port */
} ARAD_MULTICAST;
/* } */

/* return the mcds of the given unit */
dpp_mcds_t *dpp_get_mcds(
    SOC_SAND_IN  int unit
  );

/* Allocate the multicast data structure of a unit, using a given size */
uint32 dpp_alloc_mcds(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  unsigned   size_of_mcds, /* size of mcds to allocate in bytes */
    SOC_SAND_OUT dpp_mcds_t **mcds_out    /* output: allocated mcds */
);

/* De-allocate the multicast data structure of a unit */
uint32 dpp_dealloc_mcds(
    SOC_SAND_IN  int        unit
);


/* Mutlicast asserts mecahnism */

#ifndef _DPP_NO_MC_ASSERTS
#define DPP_MC_ASSERT(cond) do {if (!(cond)) dpp_perform_mc_assert(__FILE__, __LINE__);} while (0)
void dpp_perform_mc_assert(const char *file_name, unsigned line_number);
EXTERN int arad_mcds_asserts_enabled;
#else
#define DPP_MC_ASSERT(cond)
#endif



/* get the number of mcds asserts that have occurred, return it as the return value */
uint32 dpp_mcds_get_nof_asserts(void);
uint8 dpp_mcds_get_mc_asserts_enabled(void);
void dpp_mcds_set_mc_asserts_enabled(uint8 enabled);
uint8 dpp_mcds_get_mc_asserts_real(void);
void dpp_mcds_set_mc_asserts_real(uint8 real);

#endif /* __DPP_MULTICAST_H__ */

