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
/* Scorpion BCM56820 */
{
  {
    UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1_ID,      /* unitTypeID */
    "BCM-56820",                                  /* unitModel */
    "Broadcom Scorpion 56820 Development System - 24 TENGIG, 4 GE",    /* unitDescription */
    L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
    "1.3.6.1.4.1.4413",                            /* systemOID */
    1,                                             /* numPowerSupply */
    0,                                             /* numFans */
    0,                                             /* poeSupport */
    L7_TRUE,                                       /* nsfSupport */
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
      {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_SCORPION_24_TENGIG_4_GIG_REV_1_ID},
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
    /* Physical Slot Database
*/
    {
      {
        0,   /* Slot Number */
        L7_FALSE,  /* Slot supports pluggable cards */
        L7_FALSE, /* Slot Can't be powered down */
        1,    /* This slot supports one card types */

        /* Supported card types.
        */
        {CARD_BROAD_24_TENGIG_4_GIG_56820_REV_1_ID}
      }
    },
     4,      /* four stacking ports */
       {
           /* First Stacking Port Descriptor */
       {
        1,     /* Front Panel Stacking stacking port */
        0, 21,  /* slot/port for the stacking port */
        {'0', '/', '2', '1'},  /* Port identifier */
        10,       /* 1 Gb/s */
        0, 21,0,0  /* Bcm unit, Bcm port, unused, unused */
       },

       /* Second Stacking Port Descriptor */
       {
        1,     /* Front Panel Stacking stacking port */
        0, 22,  /* slot/port for the stacking port */
        {'0', '/', '2', '2'},  /* Port identifier */
        10,       /* 1 Gb/s */
        0, 22,0,0  /* Bcm unit, Bcm port, unused, unused */
       },
           /* third Stacking Port Descriptor */
       {
        1,     /* Front Panel Stacking stacking port */
        0, 23,  /* slot/port for the stacking port */
        {'0', '/', '2', '3'},  /* Port identifier */
        10,       /* 1 Gb/s */
        0, 23,0,0  /* Bcm unit, Bcm port, unused, unused */
       },

       /* fourth Stacking Port Descriptor */
       {
        1,     /* Front Panel Stacking stacking port */
        0, 24,  /* slot/port for the stacking port */
        {'0', '/', '2', '4'},  /* Port identifier */
        10,       /* 1 Gb/s */
        0, 24,0,0  /* Bcm unit, Bcm port, unused, unused */
       }

     }
 },
 hpc_data_UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1        /* pointer to hpc platform specific data */
},
{
   {
     UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1_ID,      /* unitTypeID */
     "BCM-56624-48",                                  /* unitModel */
     "Broadcom Triumph 56624 Development System - 48 GE, 4 TENGIG",    /* unitDescription */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID},
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
         {CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
             /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 49,  /* slot/port for the stacking port */
          {'0', '/', '4', '9'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 50,  /* slot/port for the stacking port */
          {'0', '/', '5', '0'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 51,  /* slot/port for the stacking port */
          {'0', '/', '5', '1'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 30,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 52,  /* slot/port for the stacking port */
          {'0', '/', '5', '2'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 31,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1        /* pointer to hpc platform specific data */
},
{
   {
     UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1_ID,      /* unitTypeID */
     "BCM-56680-24",                                  /* unitModel */
     "Broadcom Valkyrie 56680 Development System - 24 GE, 4 TENGIG",    /* unitDescription */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID},
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
         {CARD_BROAD_24_GIG_4_TENGIG_56680_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
             /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 25,  /* slot/port for the stacking port */
          {'0', '/', '2', '5'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 26,  /* slot/port for the stacking port */
          {'0', '/', '2', '6'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 27,  /* slot/port for the stacking port */
          {'0', '/', '2', '7'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 30,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 28,  /* slot/port for the stacking port */
          {'0', '/', '2', '8'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 31,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1        /* pointer to hpc platform specific data */
},
/* PTin added: new switch 56689 (Valkyrie2) */
{
  {     
     UNIT_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID,      /* unitTypeID */
     "BCM-56689-24",                                  /* unitModel */
     "Broadcom Valkyrie2 56689 Development System - 24 GE, 4 TENGIG",    /* unitDescription */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_24_GIG_4_TENGIG_REV_1_ID},
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
         {CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
             /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 25,  /* slot/port for the stacking port */
          {'0', '/', '2', '5'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 26,  /* slot/port for the stacking port */
          {'0', '/', '2', '6'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 27,  /* slot/port for the stacking port */
          {'0', '/', '2', '7'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 30,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 28,  /* slot/port for the stacking port */
          {'0', '/', '2', '8'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 31,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1        /* pointer to hpc platform specific data */
},
/* PTin BCM56843 */
{
  {     
     UNIT_BROAD_40_TENGIG_56843_REV_1_ID,             /* unitTypeID */
     "BCM-56843-40",                                  /* unitModel */
     "Broadcom Trident 56843 Development System - 40 TENGIG",    /* unitDescription */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_40_TENGIG_REV_1_ID},
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
         {CARD_BROAD_40_TENGIG_56843_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
             /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 25,  /* slot/port for the stacking port */
          {'0', '/', '2', '5'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 26,  /* slot/port for the stacking port */
          {'0', '/', '2', '6'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 27,  /* slot/port for the stacking port */
          {'0', '/', '2', '7'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 30,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 28,  /* slot/port for the stacking port */
          {'0', '/', '2', '8'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 31,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_40_TENGIG_56843_REV_1        /* pointer to hpc platform specific data */
},
/* PTin BCM56846 */
{
  {     
     UNIT_BROAD_64_TENGIG_56846_REV_1_ID,             /* unitTypeID */
     "BCM-56846-64",                                  /* unitModel */
     "Broadcom Trident 56846 Development System - 64 TENGIG",    /* unitDescription */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_64_TENGIG_REV_1_ID},
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
         {CARD_BROAD_64_TENGIG_56846_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
             /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 25,  /* slot/port for the stacking port */
          {'0', '/', '2', '5'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 26,  /* slot/port for the stacking port */
          {'0', '/', '2', '6'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 27,  /* slot/port for the stacking port */
          {'0', '/', '2', '7'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 30,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 28,  /* slot/port for the stacking port */
          {'0', '/', '2', '8'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 31,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_64_TENGIG_56846_REV_1        /* pointer to hpc platform specific data */
},
/* PTin end */
{
  {     
     UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1_ID,      /* unitTypeID */
     "BCM-56634-48",                                  /* unitModel */
     "Broadcom Triumph2 56634 Development System - 48 GE, 4 TENGIG",    /* unitDescription */
     L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
     "1.3.6.1.4.1.4413",                             /* systemOID */
     1,                                              /* numPowerSupply */
     4,                                              /* numFans */
     0,                                              /* poeSupport */
     L7_TRUE,                                        /* nsfSupport */
     1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      3,                 /* number of static card-slot mapping table entries */
  #else
      2,                 /* number of static card-slot mapping table entries */
  #endif
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      6,                 /* number of static card-slot mapping table entries */
  #else
      5,                 /* number of static card-slot mapping table entries */
  #endif
#else
  #if L7_WIRELESS_PACKAGE == 1
      5,                 /* number of static card-slot mapping table entries */
  #else
      4,                 /* number of static card-slot mapping table entries */
  #endif
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
     UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
     UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
     {
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID},
       {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_WIRELESS_PACKAGE == 1
       ,
       {L7_CAPWAP_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID}
#endif
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
         {CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
             /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 49,  /* slot/port for the stacking port */
          {'0', '/', '4', '9'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 26,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 50,  /* slot/port for the stacking port */
          {'0', '/', '5', '0'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 27,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 51,  /* slot/port for the stacking port */
          {'0', '/', '5', '1'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 52,  /* slot/port for the stacking port */
          {'0', '/', '5', '2'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1        /* pointer to hpc platform specific data */
},
{
  {
    UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1_ID,      /* unitTypeID */
    "BCM-56524-24",                                  /* unitModel */
    "Broadcom APOLLO 56524 Development System - 24 GE, 4 TENGIG",    /* unitDescription */
    L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
    "1.3.6.1.4.1.4413",                            /* systemOID */
    1,                                             /* numPowerSupply */
    4,                                             /* numFans */
    0,                                             /* poeSupport */
    L7_TRUE,                                        /* nsfSupport */
    1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      3,                 /* number of static card-slot mapping table entries */
  #else
      2,                 /* number of static card-slot mapping table entries */
  #endif
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      6,                 /* number of static card-slot mapping table entries */
  #else
      5,                 /* number of static card-slot mapping table entries */
  #endif
#else
  #if L7_WIRELESS_PACKAGE == 1
      5,                 /* number of static card-slot mapping table entries */
  #else
      4,                 /* number of static card-slot mapping table entries */
  #endif
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
    UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
    UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
    {
      {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID},
      {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_WIRELESS_PACKAGE == 1
       ,
       {L7_CAPWAP_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID}
#endif
#if L7_ROUTING_PACKAGE == 1
      ,
      {L7_VLAN_SLOT_NUM, L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID},
      {L7_LOOPBACK_SLOT_NUM, L7_LOGICAL_CARD_LOOPBACK_INTF_ID},
#if L7_RLIM_PACKAGE == 1
      {L7_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_TUNNEL_INTF_ID}
#endif
#endif
    },
    /* Physical Slot Database
*/
    {
      {
        0,   /* Slot Number */
        L7_FALSE,  /* Slot supports pluggable cards */
        L7_FALSE, /* Slot Can't be powered down */
        1,    /* This slot supports one card types */

        /* Supported card types.
        */
        {CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1_ID}
      }
    },
     4,      /* four stacking ports */
	{
            /* First Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 25,  /* slot/port for the stacking port */
         {'0', '/', '2', '5'},  /* Port identifier */
         16,       /* 1 Gb/s */
         0, 26,0,0  /* Bcm unit, Bcm port, unused, unused */
        },

        /* Second Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 26,  /* slot/port for the stacking port */
         {'0', '/', '2', '6'},  /* Port identifier */
         16,       /* 1 Gb/s */
         0, 27,0,0  /* Bcm unit, Bcm port, unused, unused */
        },
            /* third Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 27,  /* slot/port for the stacking port */
         {'0', '/', '2', '7'},  /* Port identifier */
         16,       /* 1 Gb/s */
         0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
        },

        /* fourth Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 28,  /* slot/port for the stacking port */
         {'0', '/', '2', '8'},  /* Port identifier */
         16,       /* 1 Gb/s */
         0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
        }

      }
  },
  hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1        /* pointer to hpc platform specific data */
},
{
  {
    UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1_ID,      /* unitTypeID */
    "BCM-56636-25",                                  /* unitModel */
    "Broadcom Triumph2 56636 Development System - 25 GE, 6 TENGIG",    /* unitDescription */
    L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
    "1.3.6.1.4.1.4413",                            /* systemOID */
    1,                                             /* numPowerSupply */
    4,                                             /* numFans */
    0,                                             /* poeSupport */
    L7_TRUE,                                        /* nsfSupport */
    1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      3,                 /* number of static card-slot mapping table entries */
  #else
      2,                 /* number of static card-slot mapping table entries */
  #endif
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      6,                 /* number of static card-slot mapping table entries */
  #else
      5,                 /* number of static card-slot mapping table entries */
  #endif
#else
  #if L7_WIRELESS_PACKAGE == 1
      5,                 /* number of static card-slot mapping table entries */
  #else
      4,                 /* number of static card-slot mapping table entries */
  #endif
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
    UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
    UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
    {
      {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID},
      {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_WIRELESS_PACKAGE == 1
       ,
       {L7_CAPWAP_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID}
#endif
#if L7_ROUTING_PACKAGE == 1
      ,
      {L7_VLAN_SLOT_NUM, L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID},
      {L7_LOOPBACK_SLOT_NUM, L7_LOGICAL_CARD_LOOPBACK_INTF_ID},
#if L7_RLIM_PACKAGE == 1
      {L7_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_TUNNEL_INTF_ID}
#endif
#endif
    },
    /* Physical Slot Database
*/
    {
      {
        0,   /* Slot Number */
        L7_FALSE,  /* Slot supports pluggable cards */
        L7_FALSE, /* Slot Can't be powered down */
        1,    /* This slot supports one card types */

        /* Supported card types.
        */
        {CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1_ID}
      }
    },
     4,      /* four stacking ports */
	{
            /* First Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 28,  /* slot/port for the stacking port */
         {'0', '/', '2', '8'},  /* Port identifier */
         16,       /* 16 Gb/s */
         0, 26,0,0  /* Bcm unit, Bcm port, unused, unused */
        },

        /* Second Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 29,  /* slot/port for the stacking port */
         {'0', '/', '2', '9'},  /* Port identifier */
         16,       /* 16 Gb/s */
         0, 27,0,0  /* Bcm unit, Bcm port, unused, unused */
        },
            /* third Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 30,  /* slot/port for the stacking port */
         {'0', '/', '3', '0'},  /* Port identifier */
         16,       /* 16 Gb/s */
         0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
        },

        /* fourth Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 31,  /* slot/port for the stacking port */
         {'0', '/', '3', '1'},  /* Port identifier */
         16,       /* 16 Gb/s */
         0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
        }

      }
  },
  hpc_data_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1        /* pointer to hpc platform specific data */
},

{
  {     
UNIT_BROAD_48_GIG_4_TENGIG_56538_REV_1_ID,      /* unitTypeID */
     "BCM-56538-48",                                  /* unitModel */
     "Broadcom Firebolt-3 56538 Development System - 48 GE, 4 TENGIG",    /* unitDescription */
     L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
     "1.3.6.1.4.1.4413",                             /* systemOID */
     1,                                              /* numPowerSupply */
     4,                                              /* numFans */
     0,                                              /* poeSupport */
     L7_TRUE,                                        /* nsfSupport */
     1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      3,                 /* number of static card-slot mapping table entries */
  #else
      2,                 /* number of static card-slot mapping table entries */
  #endif
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      6,                 /* number of static card-slot mapping table entries */
  #else
      5,                 /* number of static card-slot mapping table entries */
  #endif
#else
  #if L7_WIRELESS_PACKAGE == 1
      5,                 /* number of static card-slot mapping table entries */
  #else
      4,                 /* number of static card-slot mapping table entries */
  #endif
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
     UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
     UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
     {
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID},
       {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_WIRELESS_PACKAGE == 1
       ,
       {L7_CAPWAP_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID}
#endif
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

         /* Supported card types. */
         {CARD_BROAD_48_GIG_4_TENGIG_56538_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
             /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 49,  /* slot/port for the stacking port */
          {'0', '/', '4', '9'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 26,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 50,  /* slot/port for the stacking port */
          {'0', '/', '5', '0'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 27,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 51,  /* slot/port for the stacking port */
          {'0', '/', '5', '1'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 52,  /* slot/port for the stacking port */
          {'0', '/', '5', '2'},  /* Port identifier */
          16,       /* 1 Gb/s */
          0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },

   /* This unit has same card/slot mapping as unit 56634, reuse */
   hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1        /* pointer to hpc platform specific data */
},
{
  {
    UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1_ID,      /* unitTypeID */
    "BCM-56334-24",                                  /* unitModel */
    "Broadcom Enduro 56334 Development System - 24 GE, 4 TENGIG",    /* unitDescription */
    L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
    "1.3.6.1.4.1.4413",                            /* systemOID */
    1,                                             /* numPowerSupply */
    4,                                             /* numFans */
    0,                                             /* poeSupport */
    L7_TRUE,                                        /* nsfSupport */
    1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      3,                 /* number of static card-slot mapping table entries */
  #else
      2,                 /* number of static card-slot mapping table entries */
  #endif
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
  #if L7_WIRELESS_PACKAGE == 1
      6,                 /* number of static card-slot mapping table entries */
  #else
      5,                 /* number of static card-slot mapping table entries */
  #endif
#else
  #if L7_WIRELESS_PACKAGE == 1
      5,                 /* number of static card-slot mapping table entries */
  #else
      4,                 /* number of static card-slot mapping table entries */
  #endif
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
    UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
    UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
    {
      {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID},
      {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_WIRELESS_PACKAGE == 1
       ,
       {L7_CAPWAP_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID}
#endif
#if L7_ROUTING_PACKAGE == 1
      ,
      {L7_VLAN_SLOT_NUM, L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID},
      {L7_LOOPBACK_SLOT_NUM, L7_LOGICAL_CARD_LOOPBACK_INTF_ID},
#if L7_RLIM_PACKAGE == 1
      {L7_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_TUNNEL_INTF_ID}
#endif
#endif
    },
    /* Physical Slot Database
*/
    {
      {
        0,   /* Slot Number */
        L7_FALSE,  /* Slot supports pluggable cards */
        L7_FALSE, /* Slot Can't be powered down */
        1,    /* This slot supports one card types */

        /* Supported card types.
        */
        {CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1_ID}
      }
    },
     4,      /* four stacking ports */
	{
            /* First Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 25,  /* slot/port for the stacking port */
         {'0', '/', '2', '5'},  /* Port identifier */
         13,       /* 13 Gb/s */
         0, 26,0,0  /* Bcm unit, Bcm port, unused, unused */
        },

        /* Second Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 26,  /* slot/port for the stacking port */
         {'0', '/', '2', '6'},  /* Port identifier */
         13,       /* 13 Gb/s */
         0, 27,0,0  /* Bcm unit, Bcm port, unused, unused */
        },
            /* third Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 27,  /* slot/port for the stacking port */
         {'0', '/', '2', '7'},  /* Port identifier */
         13,       /* 13 Gb/s */
         0, 28,0,0  /* Bcm unit, Bcm port, unused, unused */
        },

        /* fourth Stacking Port Descriptor */
        {
         1,     /* Front Panel Stacking stacking port */
         0, 28,  /* slot/port for the stacking port */
         {'0', '/', '2', '8'},  /* Port identifier */
         13,       /* 13 Gb/s */
         0, 29,0,0  /* Bcm unit, Bcm port, unused, unused */
        }

      }
  },
  hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1        /* pointer to hpc platform specific data */
}

/* }; */

#endif /*  HPC_UNIT_DB_H */
