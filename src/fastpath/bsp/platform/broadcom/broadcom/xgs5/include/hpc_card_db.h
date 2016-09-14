 /*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  hpc_card_db.h
*
* @purpose   Initialize storage for use in hpc card database.
*
* @component hpc
*
* @create    04/29/2008
*
* @author    bradyr 
* @end
*
*********************************************************************/

#ifndef HPC_CARD_DB_H
#define HPC_CARD_DB_H
/*******************************************************************************
*
* @Data    hpc_card_descriptor_db
*
* @purpose An array of descriptors with a descriptor entry for each card type
*          supported.
*
* @end
*
*******************************************************************************/
/* main card descriptor table */
/* HPC_CARD_DESCRIPTOR_t hpc_card_descriptor_db[] = 
{ */
/* per card type entries */
   /* In order to make preconfiguration easier it is best to 
   ** list all supported physical cards in sequence starting 
   ** as the first card in the list. This arrangement produces
   ** consecutive card indexes that start with 1.
   */
{
  {
    /* Base CPU Card */
    CARD_CPU_BMW_REV_1_ID,      /* cardtypeID */     
    "LVL7 Base CPU Card",        /* cardModel  */
    "LVL7 Base CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_CPU,        /* type       */
    0,                           /* numOfNimPorts */

    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                  /* portInfo, refers to table declared above for each card type */
    &dapiBroadBaseCardEntry       /* dapiCardInfo */
  }
}
,
{
  {
    /* GTO CPU Card */
    CARD_CPU_GTO_REV_1_ID,      /* cardtypeID */     
    "LVL7 Base CPU Card",        /* cardModel  */
    "LVL7 Base CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_CPU,        /* type       */
    0,                           /* numOfNimPorts */

    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                  /* portInfo, refers to table declared above for each card type */
    &dapiBroadBaseCardEntry       /* dapiCardInfo */
  }
}
,
{
  {
    /* Keystone CPU Card */
    CARD_CPU_KEYSTONE_REV_1_ID,      /* cardtypeID */
    "LVL7 Base CPU Card",        /* cardModel  */
    "LVL7 Base CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_CPU,        /* type       */
    0,                           /* numOfNimPorts */

    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                  /* portInfo, refers to table declared above for each card type */
    &dapiBroadBaseCardEntry       /* dapiCardInfo */
  }
}

,
{
  {
    /* NSX CPU Card */
    CARD_CPU_CFM_REV_1_ID,      /* cardtypeID */     
    "LVL7 Base CPU Card",        /* cardModel  */
    "LVL7 Base CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_CPU,        /* type       */
    0,                           /* numOfNimPorts */

    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                  /* portInfo, refers to table declared above for each card type */
    &dapiBroadBaseCardEntry       /* dapiCardInfo */
  }
}
,
{
  {
    /* Base CPU Card */
    CARD_CPU_LINE_REV_1_ID,      /* cardtypeID */                   
    "LVL7 Base CPU Card",        /* cardModel  */
    "LVL7 Base CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_CPU,        /* type       */
    0,                           /* numOfNimPorts */

    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                  /* portInfo, refers to table declared above for each card type */
    &dapiBroadBaseCardEntry       /* dapiCardInfo */
  }
}
,
{
  {
    /* Base CPU Card */
    CARD_CPU_GTX_REV_1_ID,       /* cardtypeID */                   
    "LVL7 Base CPU Card",        /* cardModel  */
    "LVL7 Base CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_CPU,        /* type       */
    0,                           /* numOfNimPorts */

    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                  /* portInfo, refers to table declared above for each card type */
    &dapiBroadBaseCardEntry       /* dapiCardInfo */
  }
}
,
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1_ID,   /* cardtypeID */
    "BCM56624-48GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56624 - 48 Port 4Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    52,                               /* numOfNiPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1 /* dapiCardInfo */
  }
}
,
{
  {
    CARD_BROAD_24_GIG_4_TENGIG_56680_REV_1_ID,   /* cardtypeID */
    "BCM56680-24GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56680 - 24 Port 4Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    28,                               /* numOfNiPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1 /* dapiCardInfo */
  }
}
,
/* PTin added: new switch 56689 (Valkyrie2) */
{
  {
    CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID,   /* cardtypeID */
    "BCM56689-24GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56689 - 24 Port GbE + 4 Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    28,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1 /* dapiCardInfo */
  }
}
,
/* PTin added: new switch 5664x (Triumph3) SF */
/* Board 56640 */
{
  {
    CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1_ID,   /* cardtypeID */
    "BCM56640-4x10GE-4x40GE-1GIG",              /* cardModel  */
    "Broadcom BCM56640 - 4x10G + 4x40G + 1GIG Ethernet Switch-Fabric",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    17,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1 /* dapiCardInfo */
  }
}
,
#if (PTIN_BOARD == PTIN_BOARD_TG4G)
/* PTin added: new switch 5664x (Triumph3) GPON */
/* Board 56643 */
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID,    /* cardtypeID */
    "BCM56643-4TENGE-4FORTYGE",                   /* cardModel  */
    "Broadcom BCM56643 - 4 Ten-Gigabit + 4 40GbE Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    8,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1 /* dapiCardInfo */
  }
}
,
#else
/* PTin added: new switch 5664x (Triumph3) */
/* Board 56643 */
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID,   /* cardtypeID */
    "BCM56643-48GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56643 - 48 Port GbE + 4 Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    52,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1 /* dapiCardInfo */
  }
}
,
#endif
/* PTin added: new switch 56340 (Helix4) */
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_56340_REV_1_ID,   /* cardtypeID */
    "BCM56340-12GIG-4TENGE-1GS",              /* cardModel  */
    "Broadcom BCM56340 - 12 Port GbE + 4 Ten-Gigabit + 1 GS Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    18,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1 /* dapiCardInfo */
  }
}
,
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0F)
/* PTin added: new switch 56450 (Katana2) - OLT1T0 */
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_56450_REV_1_ID,   /* cardtypeID */
    "BCM56450-8PON-4GIG-4TENGE-1GS",              /* cardModel  */
    "Broadcom BCM56450 - 8 PON + 4 GbE + 4 Ten-Gigabit + 1 GS Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    17,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1 /* dapiCardInfo */
  }
}
,
#elif (PTIN_BOARD == PTIN_BOARD_TG16GF)
/* PTin added: new switch 56450 (Katana2) - TG16GF */
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_56450_REV_1_ID,   /* cardtypeID */
    "BCM56450-16GIG-4TENGE-1GS",              /* cardModel  */
    "Broadcom BCM56450 - 16 GbE + 4 Ten-Gigabit + 1 GS Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    21,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1 /* dapiCardInfo */
  }
}
,
#endif
/* PTin added: new switch 56843 (Trident) */
{
  {
    CARD_BROAD_40_TENGIG_56843_REV_1_ID,   /* cardtypeID */
    "BCM56843-40TENGE",              /* cardModel  */
    "Broadcom BCM56843 - 40 Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    41,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_40_TENGIG_56843_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_40_TENGIG_56843_REV_1 /* dapiCardInfo */
  }
}
,
/* PTin added: new switch 56846 (Trident-plus) */
{
  {
    CARD_BROAD_64_TENGIG_56846_REV_1_ID,   /* cardtypeID */
    "BCM56846-64TENGE",              /* cardModel  */
    "Broadcom BCM56846 - 64 Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    64, /* XXXX */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_64_TENGIG_56846_REV_1 /* dapiCardInfo */
  }
}
,
/* PTin end */
{
  {
    CARD_BROAD_24_TENGIG_4_GIG_56820_REV_1_ID,   /* cardtypeID */
    "BCM56820-24TENGE-4GE",              /* cardModel  */
    "Broadcom BCM56820 - 24 Port 10GB + 4 Port 1GB Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    28,                               /* numOfNiPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_24_TENGIG_4_GIG_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1 /* dapiCardInfo */
  }
}
,
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1_ID,   /* cardtypeID */
    "BCM56634-48GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56634 - 48 Port 4 Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    52,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1 /* dapiCardInfo */
  }
}
,
{
  {
    CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1_ID,   /* cardtypeID */     
    "BCM56524-24GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56524 - 24 Port 4Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    28,                               /* numOfNiPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1,  
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1 /* dapiCardInfo */
  }
}
,
{
  {
    CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1_ID,   /* cardtypeID */
    "BCM56636-25GIG-6TENGE",              /* cardModel  */
    "Broadcom BCM56636 - 25 Port 6Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    31,                               /* numOfNiPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1 /* dapiCardInfo */
  }
}
,
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_56538_REV_1_ID,   /* cardtypeID */
    "BCM56538-48GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56538 - 48 Port 4 Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    52,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_REV_1,
    /* portInfo, refers to table declared above for each card type */
    /* BCM56538 reference platform has identical mapping as of 56634. Reuse */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1 /* dapiCardInfo */
  }
}
,
{
  {
    CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1_ID,   /* cardtypeID */     
    "BCM56334-24GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56334- 24 Port 4Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    28,                               /* numOfNiPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_REV_1,  
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1 /* dapiCardInfo */
  }
}
,
{ 
 /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_CPU_24_GIG_4_TENGIG_REV_1_ID,       /* cardtypeID */     
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),                               
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1  /* dapiCardInfo */
  }
}
,
{
  /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_4_40GIG_REV_1_ID,       /* cardtypeID */
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),          
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1  /* dapiCardInfo */
  }
}
,
{
  /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID,       /* cardtypeID */
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),          
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1  /* dapiCardInfo */
  }
}
,
{
  /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_CPU_20_TENGIG_REV_1_ID,       /* cardtypeID */
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),                              
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_20_TENGIG_REV_1  /* dapiCardInfo */
  }
}
,
{
  /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_SCORPION_24_TENGIG_4_GIG_REV_1_ID,       /* cardtypeID */
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1  /* dapiCardInfo */
  }
}
,
/* PTin added: new switch 56843 (Trident) */
{
  /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_CPU_40_TENGIG_REV_1_ID,       /* cardtypeID */
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_40_TENGIG_REV_1  /* dapiCardInfo */
  }
}
,
/* PTin added: new switch 56846 (Trident-plus) */
{
  /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_CPU_64_TENGIG_REV_1_ID,       /* cardtypeID */
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_64_TENGIG_REV_1  /* dapiCardInfo */
  }
}
,
/* PTin end */

/* }; */

#endif /* HPC_CARD_DB_H */
