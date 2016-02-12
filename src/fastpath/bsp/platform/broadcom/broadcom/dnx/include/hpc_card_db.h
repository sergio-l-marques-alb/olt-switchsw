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
/* PTin added: new switch ARAD 12+3 */
{
  {
    CARD_BROAD_12_ETH_4_BCK_88360_REV_1_ID,   /* cardtypeID */
    "DNX-ARAD-12ETH-4BCK",                    /* cardModel  */
    "Broadcom DNX-ARAD - 12 Port ETH + 3 BCK Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    15,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_12_ETH_4_BCK_88650_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_12_ETH_4_BCK_88650_REV_1 /* dapiCardInfo */
  }
}
,
/* PTin added: new switch ARAD 8+3 */
{
  {
    CARD_BROAD_8_ETH_3_BCK_88360_REV_1_ID,   /* cardtypeID */
    "DNX-ARAD-8ETH-3BCK",                    /* cardModel  */
    "Broadcom DNX-ARAD - 8 Port ETH + 3 BCK Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    11,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_8_ETH_3_BCK_88650_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_8_ETH_3_BCK_88650_REV_1 /* dapiCardInfo */
  }
}
,
/* PTin added: new switch ARAD 1+3 */
{
  {
    CARD_BROAD_1_ETH_3_BCK_88360_REV_1_ID,   /* cardtypeID */
    "DNX-ARAD-1ETH-3BCK",                    /* cardModel  */
    "Broadcom DNX-ARAD - 1 Port ETH + 3 BCK Line Card",  /* cardDescription  */
    SYSAPI_CARD_TYPE_LINE,            /* type       */
    4,
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_BROAD_1_ETH_3_BCK_88650_REV_1,
    /* portInfo, refers to table declared above for each card type */
    &dapiBroadPhysicalCardEntry_CARD_BROAD_1_ETH_3_BCK_88650_REV_1 /* dapiCardInfo */
  }
}
,
{ 
 /* Logical CPU Card */
  {
    L7_LOGICAL_CARD_CPU_GENERIC_REV_1_ID,       /* cardtypeID */     
    "LVL7 Logical CPU Card",        /* cardModel  */
    "LVL7 Logical CPU Card",        /* cardDescription  */
    SYSAPI_CARD_TYPE_LOGICAL_CPU, /* type       */
    (sizeof(hpcPortInfoTable_CARD_CPU)/sizeof(SYSAPI_HPC_PORT_DESCRIPTOR_t)),                               
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    /* numOfNimPorts (computed from number of SYSAPI_PORT_DESCRIPTOR_t entries in table */
    hpcPortInfoTable_CARD_CPU,    /* portInfo, refers to table declared above for each card type */
    &dapiBroadCpuCardEntry_CARD_BROAD_GENERIC_REV_1  /* dapiCardInfo */
  }
}
,
/* }; */

#endif /* HPC_CARD_DB_H */
