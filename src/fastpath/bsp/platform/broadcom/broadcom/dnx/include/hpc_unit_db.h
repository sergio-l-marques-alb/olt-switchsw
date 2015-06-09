 /*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  hpc_unit_db.h
*
* @purpose   Initialize storage for use in hpc unit database.
*
* @component hpc
*
* @create    04/29/2008
*
* @author    bradyr 
* @end
*
*********************************************************************/

#ifndef HPC_UNIT_DB_H
#define HPC_UNIT_DB_H

#include "broad_ids.h"
/*******************************************************************************
*
* @Data    hpc_unit_descriptor_db
*
* @purpose An array of descriptors with an entry for each unit type
*          supported.
*
* @end
*
*******************************************************************************/
/* main unit descriptor table */
/* HPC_UNIT_DESCRIPTOR_t hpc_unit_descriptor_db[] =
{ */

/* PTin added: new switch ARAD */
{
  {     
     UNIT_BROAD_12_ETH_4_BCK_88360_REV_1_ID,       /* unitTypeID */
     "BCM-88360-00",                                  /* unitModel */
     "Broadcom DNX-ARAD Development System - 12 ETH, 4 BCK",  /* unitDescription */
     L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
     "1.3.6.1.4.1.4413",                             /* systemOID */
     1,                                              /* numPowerSupply */
     4,                                              /* numFans */
     0,                                              /* poeSupport */
     L7_TRUE,                                        /* nsfSupport */
     1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
     2,                                             /* number of static card-slot mapping table entries */
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
     5,                                             /* number of static card-slot mapping table entries */
#else
    4,
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
     UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
     UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
     {
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_12_ETH_4_BCK_REV_1_ID},
       {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_ROUTING_PACKAGE == 1
       ,
       {L7_VLAN_SLOT_NUM, L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID},
       {L7_LOOPBACK_SLOT_NUM, L7_LOGICAL_CARD_LOOPBACK_INTF_ID},
#if L7_RLIM_PACKAGE == 1
       {L7_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_TUNNEL_INTF_ID}
#endif
#endif
     },
     {
      {
         0,   /* Slot Number */
         L7_FALSE,  /* Slot supports pluggable cards */
         L7_FALSE, /* Slot Can't be powered down */
         1,    /* This slot supports one card types */

         /* Supported card types.
         */
         {CARD_BROAD_12_ETH_4_BCK_88360_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
         /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 13,  /* slot/port for the stacking port */
          {'0', '/', '1', '3'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,13,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 14,  /* slot/port for the stacking port */
          {'0', '/', '1', '4'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,14,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 15,  /* slot/port for the stacking port */
          {'0', '/', '1', '5'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,15,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 16,  /* slot/port for the stacking port */
          {'0', '/', '1', '6'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,16,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_12_ETH_4_BCK_88650_REV_1        /* pointer to hpc platform specific data */
},

/* }; */

#endif /*  HPC_UNIT_DB_H */
