/* $Id: multicast.h,v  $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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


#define _ARAD_TEST_MCDS


/* get the number of mcds asserts that have occurred, return it as the return value */
uint32 dpp_mcds_get_nof_asserts(void);
uint8 dpp_mcds_get_mc_asserts_enabled(void);
void dpp_mcds_set_mc_asserts_enabled(uint8 enabled);
uint8 dpp_mcds_get_mc_asserts_real(void);
void dpp_mcds_set_mc_asserts_real(uint8 real);


/*
 * Functions called from outside the dpp multicast code.
 */

/*
 * Initialize the multicast part of the software database.
 * Do not fill the data from hardware yet.
 * dpp_mcds_multicast_init2() will be called to do so when we can access the MCDB using DMA.
 */
uint32 dpp_mcds_multicast_init(
    SOC_SAND_IN int      unit
);

/*
 * Initialize the multicast part of the software database.
 * Must be run after dpp_mcds_multicast_init() was called successfully, and when DMA is up.
 * fills the multicast data from hardware.
 */
uint32 dpp_mcds_multicast_init2(
    SOC_SAND_IN int      unit
);

/*
 * free allocations done in the multicat init
 */
uint32 dpp_mcds_multicast_terminate(
    SOC_SAND_IN int unit
);

/*
 * Initialization of the Arad blocks configured in this module.
 * Called as part of the initialization sequence.
 */
uint32 dpp_mc_init(
    SOC_SAND_IN  int                 unit
);

/*
 * This function checks if the multicast group is open (possibly empty).
 * returns TRUE if the group is open (start of a group), otherwise FALSE.
 */
uint32 dpp_mult_does_group_exist_ext( 
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_MULT_ID mcid,      /* MC ID of the group */
    SOC_SAND_IN  int             is_egress, /* is the MC group an egress group */
    SOC_SAND_OUT uint8           *is_open   /* returns FALSE if not open */
);


/*
 * Ingress Multicast functions called from outside the dpp multicast code.
 */

/*
 * This API sets the ingress group to the given replications,
 * configuring its linked list; and creates the group if it did not exist.
 */
uint32 dpp_mult_ing_group_open(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *mc_group,        /* group replications to set */
    SOC_SAND_IN  uint32                 mc_group_size,    /* number of group replications (size of mc_group) */
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          /* return possible errors that the caller may want to ignore: insufficient memory or duplicate replications */
);
 
/*
 * Closes the ingress muticast group, freeing its linked list.
 * Do nothing if the group is not open.
 */
uint32 dpp_mult_ing_group_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t      multicast_id_ndx /* group mcid to close */
);

/*
 * This API sets the ingress group to the given replications, configuring its linked list.
 * The group must exist.
 */
uint32 dpp_mult_ing_group_update(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  dpp_mc_id_t          multicast_id_ndx,    /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY   *mc_group,           /* group replications to set */
    SOC_SAND_IN  uint32                mc_group_size,       /* number of group replications (size of mc_group) */
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err             /* return possible errors that the caller may want to ignore: insufficient memory or duplicate replications */
);

uint32 dpp_mult_ing_traffic_class_map_set(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_TMC_MULT_ING_TR_CLS_MAP *map
);
uint32 dpp_mult_ing_traffic_class_map_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT SOC_TMC_MULT_ING_TR_CLS_MAP *map
);

/*
 * Adds the given replication to the ingress multicast group.
 * It is an error if the group is not open.
 */
uint32 dpp_mult_ing_destination_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *replication,     /* replication to add */
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          /* return possible errors that the caller may want to ignore : insufficient memory or duplicate replication */
);

/*
 * Removes the given replication from the ingress multicast group.
 * It is an error if the group is not open or does not contain the replication.
 */
uint32 dpp_mult_ing_destination_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *entry,           /* replication to remove */
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          /* return possible errors that the caller may want to ignore : replication does not exist */
);

/*********************************************************************
*     Returns the size of the multicast group with the
*     specified multicast id.
*********************************************************************/
uint32 dpp_mult_ing_group_size_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx,
    SOC_SAND_OUT uint32      *mc_group_size
);

/*
 * Gets the ingress multicast group with the specified multicast id.
 * will return up to mc_group_size replications, and the exact
 * The group's replication number is returned in exact_mc_group_size.
 * The number of replications returned in the output arrays is
 * min{mc_group_size, exact_mc_group_size}.
 * It is not an error if the group is not open.
 */
uint32 dpp_mult_ing_get_group(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t group_mcid,           /* group id */
    SOC_SAND_IN  uint32      mc_group_size,        /* maximum replications to return */
    SOC_SAND_OUT soc_gport_t *ports,               /* output ports (array of size mc_group_size) */
    SOC_SAND_OUT soc_if_t    *cuds,                /* output ports (array of size mc_group_size) */
    SOC_SAND_OUT uint32      *exact_mc_group_size, /* the number of replications in the group will be returned here */
    SOC_SAND_OUT uint8       *is_open              /* will return if the group is open (false or true) */
);

/*********************************************************************
*     Closes all opened ingress multicast groups.
*********************************************************************/
uint32 dpp_mult_ing_all_groups_close(
    SOC_SAND_IN  int unit
);


/*
 * Egress Multicast functions called from outside the dpp multicast code.
 */

/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List). Only the max bitmap ID is configurable.
*********************************************************************/
uint32 dpp_mult_eg_vlan_membership_group_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
);

/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 dpp_mult_eg_vlan_membership_group_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
);

/*
 * This function opens the bitmap group, ans sets it to replicate to the given ports.
 */
uint32 dpp_mult_eg_vlan_membership_group_open(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  dpp_mc_id_t                           multicast_id_ndx, /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group            /* group replications (members) */
);

/*********************************************************************
*     This API updates the egress-multicast-replication
*     definitions for the specific multicast-id, and creates
*     in the device the needed link-list/bitmap. The user only
*     specifies the multicast-id and copies. All inner
*     link-list nodes and bitmap are allocated and handled by
*     the driver.
*********************************************************************/
uint32 dpp_mult_eg_vlan_membership_group_update(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  dpp_mc_id_t                        multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP  *group
);

/*
 * Gets the egress replications (ports) of the given bitmap.
 * If the bitmap is a vlan egress group, it does not have to be open/created.
 */
uint32 dpp_mult_eg_vlan_membership_group_get(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  dpp_mc_id_t                           bitmap_id, /* ID of the bitmap */
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group     /* output port bitmap */
);

/*********************************************************************
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
*********************************************************************/
uint32 dpp_mult_eg_vlan_membership_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      /* return possible errors that the caller may want to ignore */
);

/*********************************************************************
*     Removes a port member of the egress multicast.
*********************************************************************/
uint32 dpp_mult_eg_vlan_membership_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      /* return possible errors that the caller may want to ignore */
);

/*********************************************************************
*     Closes all opened egress multicast groups in range of
*     vlan membership.
*********************************************************************/
uint32 dpp_mult_eg_vlan_membership_all_groups_close(
    SOC_SAND_IN  int                 unit
);

/*********************************************************************
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
*********************************************************************/
uint32 dpp_mult_eg_group_close(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx
);

/*********************************************************************
*     Closes all opened egress multicast groups.
*********************************************************************/
uint32 dpp_mult_eg_all_groups_close(
    SOC_SAND_IN  int                 unit
);

/*
 * Returns the size of the multicast group with the specified multicast id.
 * Not needed for bcm APIs, so not tested.
 * returns 0 for non open groups.
 */
uint32 dpp_mult_eg_group_size_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t           multicast_id_ndx,
    SOC_SAND_OUT uint32                 *mc_group_size
);

/*
 * This API sets the egress group to the given replications,
 * configuring its linked list.
 * If the group does not exist, it will be created or an error will be returned based on allow_create.
 * Creation may involve relocating the mcdb entry which will be the start
 * of the group, and possibly other consecutive entries.
 *
 * We always want to create entries with pointers from port+outlif couples and from bitmaps.
 * We need to leave one entry with a pointer for the start of the group.
 * every block of entries with no pointers ends with an entry pointer, except for the end of the group.
 */
uint32 dpp_mult_eg_group_set(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t   mcid,    /* the group mcid */
    SOC_SAND_IN  uint8          allow_create,        /* if non zero, will create the group if it does not exist */
    SOC_SAND_IN  uint32         group_size,          /* size of ports and cuds to read group replication data from */
    SOC_SAND_IN  uint32         *ports,              /* array containing ports/destinations */
    SOC_SAND_IN  soc_if_t       *cuds,               /* array containing encapsulations/CUDs/outlifs */
    SOC_SAND_OUT SOC_TMC_ERROR  *out_err             /* return possible errors that the caller may want to ignore */
);

/*
 * Adds the given replication to the non bitmap egress multicast group.
 * It is an error if the group is not open.
 */
uint32 dpp_mult_eg_port_add(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  dpp_mc_id_t        group_mcid,          /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY  *replication,        /* replication to add */
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err             /* return possible errors that the caller may want to ignore */
);

/*
 * Removes the given replication from the non bitmap egress multicast group.
 * It is an error if the group is not open or does not contain the replication.
 */
uint32 dpp_mult_eg_port_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid,   /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY *replication, /* replication to remove */
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err      /* return possible errors that the caller may want to ignore */
);

/*
 * Set the outlif (cud) to (local) port mapping from the given cud to the given port.
 */
uint32 dpp_mult_cud_to_port_map_set(
    SOC_SAND_IN int                 unit, /* input device */
    SOC_SAND_IN uint32              cud,  /* input cud/outlif */
    SOC_SAND_IN SOC_TMC_FAP_PORT_ID port  /* input (local egress) port */
);

/*
 * Get the outlif (cud) to (local) port mapping from the given cud.
 */
uint32 dpp_mult_cud_to_port_map_get(
    SOC_SAND_IN  int                 unit, /* input device */
    SOC_SAND_IN  uint32              cud,  /* input cud/outlif */
    SOC_SAND_OUT SOC_TMC_FAP_PORT_ID *port /* output (local egress) port */
);

#endif /* __DPP_MULTICAST_H__ */

