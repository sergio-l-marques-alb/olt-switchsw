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
/* PTin added: new platform */
{
  {
    CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1_ID,   /* cardtypeID */
    "BCM56643-48GIG-4TENGE",              /* cardModel  */
    "Broadcom BCM56643 - 48 Port GbE + 4 Ten-Gigabit Ethernet Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    52,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1 /* dapiCardInfo */   // PTin modified: new switch
  }
}
,
/* PTin end */
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
    L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_REV_1_ID,       /* cardtypeID */
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
/* PTin added: new switch BCM56843 */
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
/* PTin added: new switch BCM56846 */
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
