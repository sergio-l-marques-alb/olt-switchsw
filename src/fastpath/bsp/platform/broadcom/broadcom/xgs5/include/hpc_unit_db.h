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
{*/
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
     3,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
     6,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#else
    5,                                                                                                      /* PTin modified: virtual ports */
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
       ,
       {L7_VLAN_PORT_SLOT_NUM, L7_LOGICAL_CARD_VLAN_PORT_INTF_ID}  /* PTin added: virtual ports */
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
/* PTin added: new switch 5664x (Triumph3) SF */
{
  {     
     UNIT_BROAD_4_10G_3_40G_1_GIG_56640_REV_1_ID,     /* unitTypeID */
     "BCM-56640-00",                                  /* unitModel */
     "Broadcom Triumph3 56640 Development System - 1 GE, 4 TENGIG, 4 FORTYGIG",    /* unitDescription */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_DEFAULT_ID},
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
         {CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1_ID}
       }
     },
     3,      /* four stacking ports */
     {
         /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 5,  /* slot/port for the stacking port */
          {'0', '/', '5'},  /* Port identifier */
          40,       /* 40 Gb/s */
          0, 5,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 6,  /* slot/port for the stacking port */
          {'0', '/', '6'},  /* Port identifier */
          40,       /* 40 Gb/s */
          0, 6,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 7,  /* slot/port for the stacking port */
          {'0', '/', '7'},  /* Port identifier */
          40,       /* 1 Gb/s */
          0, 7,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
       }
   },
   hpc_data_UNIT_BROAD_4_10G_3_40G_1_GIG_56640_REV_1        /* pointer to hpc platform specific data */
},
#if (PTIN_BOARD == PTIN_BOARD_TG4G)
/* PTin added: new switch 5664x (Triumph3) GPON */
{
  {     
     UNIT_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID,      /* unitTypeID */
     "BCM-56643-01",                                  /* unitModel */
     "Broadcom Triumph3 56643 Development System - 4 TENGIG, 4 40GIG",    /* unitDescription */
     L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
     "1.3.6.1.4.1.4413",                             /* systemOID */
     1,                                              /* numPowerSupply */
     4,                                              /* numFans */
     0,                                              /* poeSupport */
     L7_TRUE,                                        /* nsfSupport */
     1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
     3,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
     6,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#else
    5,                                                                                                      /* PTin modified: virtual ports */
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
     UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
     UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
     {
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_4_40GIG_REV_1_ID},
       {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_ROUTING_PACKAGE == 1
       ,
       {L7_VLAN_SLOT_NUM, L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID},
       {L7_LOOPBACK_SLOT_NUM, L7_LOGICAL_CARD_LOOPBACK_INTF_ID},
#if L7_RLIM_PACKAGE == 1
       {L7_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_TUNNEL_INTF_ID}
#endif
#endif
       ,
       {L7_VLAN_PORT_SLOT_NUM, L7_LOGICAL_CARD_VLAN_PORT_INTF_ID}  /* PTin added: virtual ports */
     },
     {
      {
         0,   /* Slot Number */
         L7_FALSE,  /* Slot supports pluggable cards */
         L7_FALSE, /* Slot Can't be powered down */
         1,    /* This slot supports one card types */

         /* Supported card types.
         */
         {CARD_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
         /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 5,  /* slot/port for the stacking port */
          {'0', '/', '5'},  /* Port identifier */
          40,       /* 40 Gb/s */
          0, 54, 0, 0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 6,  /* slot/port for the stacking port */
          {'0', '/', '6'},  /* Port identifier */
          40,       /* 10 Gb/s */
          0, 55, 0, 0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 7,  /* slot/port for the stacking port */
          {'0', '/', '7'},  /* Port identifier */
          40,       /* 1 Gb/s */
          0, 56, 0, 0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 8,  /* slot/port for the stacking port */
          {'0', '/', '8'},  /* Port identifier */
          40,       /* 1 Gb/s */
          0, 57, 0, 0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_4_TENGIG_4_40GIG_56643_REV_1        /* pointer to hpc platform specific data */
},
#else
/* PTin added: new switch 5664x (Triumph3) */
{
  {     
     UNIT_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID,      /* unitTypeID */
     "BCM-56643-00",                                  /* unitModel */
     "Broadcom Triumph3 56643 Development System - 48 GE, 4 TENGIG",    /* unitDescription */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_4_40GIG_REV_1_ID},
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
         {CARD_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID}
       }
     },
     4,      /* four stacking ports */
     {
         /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 49,  /* slot/port for the stacking port */
          {'0', '/', '4', '9'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0, 48,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 50,  /* slot/port for the stacking port */
          {'0', '/', '5', '0'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0, 49,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 51,  /* slot/port for the stacking port */
          {'0', '/', '5', '1'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 50,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 52,  /* slot/port for the stacking port */
          {'0', '/', '5', '2'},  /* Port identifier */
          10,       /* 1 Gb/s */
          0, 51,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56643_REV_1        /* pointer to hpc platform specific data */
},
#endif
/* PTin added: new switch 56340 (Helix4) */
{
  {     
     UNIT_BROAD_48_GIG_4_TENGIG_56340_REV_1_ID,       /* unitTypeID */
     "BCM-56340-00",                                  /* unitModel */
     "Broadcom Helix4 56340 Development System - 12 GE, 4 TENGIG, 1 GS",  /* unitDescription */
     L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
     "1.3.6.1.4.1.4413",                             /* systemOID */
     1,                                              /* numPowerSupply */
     4,                                              /* numFans */
     0,                                              /* poeSupport */
     L7_TRUE,                                        /* nsfSupport */
     1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
     3,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
     6,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#else
    5,                                                                                                      /* PTin modified: virtual ports */
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
     UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
     UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
     {
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_DEFAULT_ID},
       {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_ROUTING_PACKAGE == 1
       ,
       {L7_VLAN_SLOT_NUM, L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID},
       {L7_LOOPBACK_SLOT_NUM, L7_LOGICAL_CARD_LOOPBACK_INTF_ID},
#if L7_RLIM_PACKAGE == 1
       {L7_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_TUNNEL_INTF_ID}
#endif
#endif
       ,
       {L7_VLAN_PORT_SLOT_NUM, L7_LOGICAL_CARD_VLAN_PORT_INTF_ID}  /* PTin added: virtual ports */
     },
     {
      {
         0,   /* Slot Number */
         L7_FALSE,  /* Slot supports pluggable cards */
         L7_FALSE, /* Slot Can't be powered down */
         1,    /* This slot supports one card types */

         /* Supported card types.
         */
         {CARD_BROAD_48_GIG_4_TENGIG_56340_REV_1_ID}
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
          0,50,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 14,  /* slot/port for the stacking port */
          {'0', '/', '1', '4'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,51,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 15,  /* slot/port for the stacking port */
          {'0', '/', '1', '5'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,52,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 16,  /* slot/port for the stacking port */
          {'0', '/', '1', '6'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,53,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1        /* pointer to hpc platform specific data */
},
/* PTin added: new switch 56450 (Katana2) */
{
  {     
     UNIT_BROAD_48_GIG_4_TENGIG_56450_REV_1_ID,       /* unitTypeID */
     "BCM-56450-00",                                  /* unitModel */
     "Broadcom Katana2 56450 Development System - 12 GE, 4 TENGIG, 1 GS",  /* unitDescription */
     L7_UNITMGR_MGMTFUNC_UNASSIGNED,                /* managementPreference */
     "1.3.6.1.4.1.4413",                             /* systemOID */
     1,                                              /* numPowerSupply */
     4,                                              /* numFans */
     0,                                              /* poeSupport */
     L7_TRUE,                                        /* nsfSupport */
     1,                                             /* number of physical slots */
/* do not specify the Logical Routing card as a non-removable card in switching builds */
#if L7_SWITCHING_PACKAGE == 1
     3,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#elif L7_ROUTING_PACKAGE == 1
#if L7_RLIM_PACKAGE == 1
     6,                                             /* number of static card-slot mapping table entries */  /* PTin modified: virtual ports */
#else
    5,                                                                                                      /* PTin modified: virtual ports */
#endif
#else
#error __FILE__, __LINE__: Neither L7_SWITCHING_PACKAGE nor L7_ROUTING_PACKAGE defined.
#endif
     UNIT_BRXGS_CODE_TARGET_DEVICE, /* codeLoadTargetId - What code to load on this device */
     UNIT_BRXGS_CONFIG_TARGET_DEVICE, /* configLoadTargetId - What configuration to load on this device */
     {
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_DEFAULT_ID},
       {L7_LAG_SLOT_NUM, L7_LOGICAL_CARD_LAG_ID}
#if L7_ROUTING_PACKAGE == 1
       ,
       {L7_VLAN_SLOT_NUM, L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID},
       {L7_LOOPBACK_SLOT_NUM, L7_LOGICAL_CARD_LOOPBACK_INTF_ID},
#if L7_RLIM_PACKAGE == 1
       {L7_TUNNEL_SLOT_NUM, L7_LOGICAL_CARD_TUNNEL_INTF_ID}
#endif
#endif
       ,
       {L7_VLAN_PORT_SLOT_NUM, L7_LOGICAL_CARD_VLAN_PORT_INTF_ID}  /* PTin added: virtual ports */
     },
     {
      {
         0,   /* Slot Number */
         L7_FALSE,  /* Slot supports pluggable cards */
         L7_FALSE, /* Slot Can't be powered down */
         1,    /* This slot supports one card types */

         /* Supported card types.
         */
         {CARD_BROAD_48_GIG_4_TENGIG_56450_REV_1_ID}
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
          0,27,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 14,  /* slot/port for the stacking port */
          {'0', '/', '1', '4'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,25,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
             /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 15,  /* slot/port for the stacking port */
          {'0', '/', '1', '5'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,33,0,0  /* Bcm unit, Bcm port, unused, unused */
         },

         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 16,  /* slot/port for the stacking port */
          {'0', '/', '1', '6'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,36,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1        /* pointer to hpc platform specific data */
},
/* PTin added: new switch 56170 (Hurricane3/Greyhound2) */
{
  {     
     UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1_ID, /* unitTypeID */
     "BCM-56170-00",                                  /* unitModel */
     "Broadcom Hurricane3-MG 56170 Development System - 24GE+24TENGIG, 4TENGIG",    /* unitDescription */
     L7_UNITMGR_MGMTFUNC_UNASSIGNED,                 /* managementPreference */
     "1.3.6.1.4.1.4413",                             /* systemOID */
     1,                                              /* numPowerSupply */
     4,                                              /* numFans */
     0,                                              /* poeSupport */
     L7_TRUE,                                        /* nsfSupport */
     1,                                              /* number of physical slots */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_DEFAULT_ID},
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
         {CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1_ID}
       }
     },
     16,     /* 16 stacking ports */
     {
         /* First Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 48,  /* slot/port for the stacking port */
          {'0', '/', '4', '8'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,62,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* Second Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 49,  /* slot/port for the stacking port */
          {'0', '/', '4', '9'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,63,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* third Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 50,  /* slot/port for the stacking port */
          {'0', '/', '5', '0'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,64,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* fourth Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 51,  /* slot/port for the stacking port */
          {'0', '/', '5', '1'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,65,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 5th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 52,  /* slot/port for the stacking port */
          {'0', '/', '5', '2'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,56,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 6th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 53,  /* slot/port for the stacking port */
          {'0', '/', '5', '3'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,57,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 7th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 54,  /* slot/port for the stacking port */
          {'0', '/', '5', '4'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,52,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 8th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 55,  /* slot/port for the stacking port */
          {'0', '/', '5', '5'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,53,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 9th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 56,  /* slot/port for the stacking port */
          {'0', '/', '5', '6'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,58,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 10th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 57,  /* slot/port for the stacking port */
          {'0', '/', '5', '7'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,59,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 11th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 58,  /* slot/port for the stacking port */
          {'0', '/', '5', '8'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,60,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 12th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 59,  /* slot/port for the stacking port */
          {'0', '/', '5', '9'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,61,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 13th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 60,  /* slot/port for the stacking port */
          {'0', '/', '6', '0'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,54,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 14th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 61,  /* slot/port for the stacking port */
          {'0', '/', '6', '1'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,55,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 15th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 62,  /* slot/port for the stacking port */
          {'0', '/', '6', '2'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,50,0,0  /* Bcm unit, Bcm port, unused, unused */
         },
         /* 16th Stacking Port Descriptor */
         {
          1,     /* Front Panel Stacking stacking port */
          0, 63,  /* slot/port for the stacking port */
          {'0', '/', '6', '3'},  /* Port identifier */
          10,       /* 10 Gb/s */
          0,51,0,0  /* Bcm unit, Bcm port, unused, unused */
         }
       }
   },
   hpc_data_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1        /* pointer to hpc platform specific data */
},
/* PTin added: new switch 56843 (Trident) */
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
       {L7_CPU_SLOT_NUM, L7_LOGICAL_CARD_CPU_DEFAULT_ID},
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
/* PTin added: new switch 56846 (Trident-plus) */
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

/* }; */

#endif /*  HPC_UNIT_DB_H */
