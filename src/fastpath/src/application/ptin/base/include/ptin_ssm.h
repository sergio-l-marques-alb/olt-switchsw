
/*********************************************************************
*
* (C) Copyright PT Inovacao 2012
*
**********************************************************************
*
* @filename    ptin_ssm.h
* @purpose     SSM main functions
* @component   SSM
* @comments    none
* @create      26/11/2012
* @author      mruas
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#ifndef INCLUDE_PTIN_SSM_H
#define INCLUDE_PTIN_SSM_H

#include "nimapi.h"
#include "l7_cnfgr_api.h"
#include "ptin_globaldefs.h"
#include "sysnet_api.h"

#ifdef SYNC_SSM_IS_SUPPORTED

#ifndef DEFINES_SYNC_SSM
  #define DEFINES_SYNC_SSM

  #define SSM_SLOW_PROTOCOL_TYPE      0x8809 /* Tbl 43B-2 */
  #define SSM_ILLEGAL_SUBTYPE         0x00   /* Tbl 43B-3 */
  #define SSM_ILLEGAL_SUBTYPE_MIN     0x0B   /* Tbl 43B-3 */

  #define SSM_PROTOCOL_SUBTYPE        0x0A
  #define SSM_ITU_OUI                 0x0019A7UL
  #define SSM_ITU_SUBTYPE             0x0001

  #define SSM_VERSION                 0x10  /* Version=1, event flag=0 */
  #define SSM_L4_TYPE                 0x01
  #define SSM_L4_LENGTH               0x04

  #define SSM_DMAC_OFFSET             0
  #define SSM_SMAC_OFFSET             6
  #define SSM_ETHTYPE_OFFSET          12
  #define SSM_SUBTYPE_OFFSET          14
  #define SSM_ITU_OUI_OFFSET          15
  #define SSM_ITU_SUBTYPE_OFFSET      18
  #define SSM_VERSION_OFFSET          20

  #define SSM_L4_TYPE_OFFSET          24
  #define SSM_L4_LENGTH_OFFSET        25
  #define SSM_L4_CODE_OFFSET          27

  #define SSM_PDU_DATALENGTH          60
#endif /*DEFINES_SYNC_SSM*/


/*********************************************************************
* @purpose  structure which will be used as an overlay on the received SSM messages
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef struct ssm_pdu_s
{
  L7_uchar8         dmac[L7_ENET_MAC_ADDR_LEN]; /* 6 bytes */
  L7_uchar8         smac[L7_ENET_MAC_ADDR_LEN]; /* 6 bytes */

  L7_uint16         etherType;              /* 2 bytes */
  L7_uchar8         subType;                /* 1 byte  */
  L7_uchar8         itu_oui[3];             /* 3 bytes */
  L7_uint16         itut_subtype;           /* 2 bytes */
  L7_uchar8         version;                /* 1 byte  */
  L7_uchar8         reserved[3];            /* 3 bytes */

  L7_uchar8         ssm_type;               /* 1 byte  */
  L7_uint16         ssm_length;             /* 2 bytes */
  L7_uchar8         ssm_code;               /* 4 bits  */

  L7_uchar8         zero_padded[32];

  #if 0
  L7_uchar8         fcs[4];                 /* 4 bytes */
  #endif

} __attribute__((packed)) ssm_pdu_t;

/*********************************************************************
* @purpose  structure which will be used for the Queue messages
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef struct
{
  L7_uint32 ssm;  /* to be determined */
} ssm_t;

typedef struct
{
  L7_uint32 status;
  L7_BOOL   updateConfig;
} ssm_AdminMode_t;

typedef struct
{
  L7_uint32 event;
  L7_uint32 intf;
  union
  {
    ssm_t             ssm;
    L7_uint32         p;    /* or key or priority */
    ssm_AdminMode_t   adminMode;
    L7_enetMacAddr_t  mac;
    L7_uchar8         state;
    void              *bufHandle;
    NIM_EVENT_COMPLETE_INFO_t nimInfo;
    L7_CNFGR_CMD_DATA_t cnfgrInfo;
  } intfData;
} ssmMsg_t;


#ifdef SYNC_SSM_STATS
static L7_uint32 ssm_counters_port[] =
{
  L7_PLATFORM_CTR_SSM_PDUS_RX,
  L7_PLATFORM_CTR_SSM_PDUS_TX
};
#endif

#endif

/**************************************************************************
*
* @purpose   Allocate all necessary SSM resources
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Initialization routine
*         
*
* @end
*
*************************************************************************/
L7_RC_t ssm_init(void);

/**************************************************************************
*
* @purpose   Remove all related SSM resources
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Deinitialization routine
*         
*
* @end
*
*************************************************************************/
L7_RC_t ssm_fini(void);

/*********************************************************************
* @purpose  Receives an SSM PDU
*
* @param    intf            interface number received on
* @param    bufHandle       handle to the PDU location in DTL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if port not found
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ssmPDUReceive(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

/*********************************************************************
* @purpose  Decodes and services an incoming request from the stats
*           manager for a SSM statistic
*
* @param    c           Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Only called by stats manager
*       
* @end
*********************************************************************/
void ssmStatGet(void *statHandle);

/*********************************************************************
* @purpose  Clears the port statistics. 
*          
* @param    intIfNum @b{(input)} Internal Interface Number of the port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Clears all the ssm PDU stats associated with this port. 
*
* @end
*********************************************************************/
L7_RC_t ssmPortStatsClear(L7_uint32 intIfNum);

#endif /* INCLUDE_PTIN_SSM_H */
