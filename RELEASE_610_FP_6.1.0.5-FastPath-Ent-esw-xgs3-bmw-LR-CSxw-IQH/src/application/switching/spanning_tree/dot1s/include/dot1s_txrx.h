/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_txrx.h
*
* @purpose   Multiple Spanning tree Transmit and Receive BPDUs
*
* @component dot1s
*
* @comments 
*
* @create    10/3/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_TXRX_H
#define INCLUDE_DOT1S_TXRX_H


#include "sysnet_api.h"
#include "datatypes.h"

#define DOT1S_MAX_BPDU_SIZE 				(1024 + 102 + 12 +5)/*bytes*/
#define DOT1S_BPDU_PROTOCOL_ID_START		17	/*bytes*/
#define	DOT1S_BPDU_CISTROOTID_START			5	/*bytes*/
#define DOT1S_BPDU_FWD_VER1_START			30	/*bytes*/
#define DOT1S_BPDU_VER1_LENGTH				1	/*bytes*/
#define DOT1S_BPDU_CISTINTROOTPATH_START	53	/*bytes*/
#define	DOT1S_BPDU_MSTI_MSG_START			13	/*bytes*/
#define DOT1S_BPDU_MSTIINTROOTPATH_START	9	/*bytes*/
#define DOT1S_BPDU_MSTI_REMHOPS_END			7	/*bytes*/
#define DOT1S_BPDU_CONFIG_ID_LEN            51

typedef struct 
{
  L7_uchar8 		  mstiFlags;
  L7_uchar8			  Pad1;
  DOT1S_BRIDGEID_t	  mstiRegRootId;
  L7_uchar8			  Pad2[2];                  /*pad field*/
  L7_uint32			  mstiIntRootPathCost;
  L7_uchar8			  mstiBridgePriority;
  L7_uchar8           mstiPortPriority;
  L7_uchar8           mstiRemHops;
  L7_uchar8			  Pad3;
}DOT1S_MSTI_CFG_MSG_t;


typedef struct
{
  /* header information */
  L7_uchar8           destMac[L7_MAC_ADDR_LEN]; /* destination MAC */
  L7_uchar8           srcMac[L7_MAC_ADDR_LEN];  /* source MAC */
  L7_ushort16         packetLength;             /* packet length */
  L7_uchar8           logicalLinkCtrl[3];       /* LLC information */
  L7_uchar8			  Pad1;                     /*pad field*/
  /* bpdu message */
  L7_ushort16         protocolIdentifier;       /* Spanning Tree = 0x0000 */
  L7_uchar8           protocolVersion;          /* Current version = 0x00 */
  L7_uchar8           bdpuType;                 /* Config = 0x00, Topo Change = 0x80 */
  L7_uchar8           flags;                    /* bit 0: topo change, bit 1: topo change ack */
} dot1sBpduHeader_t;

typedef struct 
{
  /* header information */
  L7_uchar8           destMac[L7_MAC_ADDR_LEN]; /* destination MAC */
  L7_uchar8           srcMac[L7_MAC_ADDR_LEN];  /* source MAC */
  L7_ushort16         packetLength;             /* packet length */
  L7_uchar8           logicalLinkCtrl[3];       /* LLC information */
  L7_uchar8			  Pad1;                     /*pad field*/
  /* bpdu message */
  L7_ushort16         protocolIdentifier;       /* Spanning Tree = 0x0000 */
  L7_uchar8           protocolVersion;          /* Current version = 0x00 */
  L7_uchar8           bdpuType;                 /* Config = 0x00, Topo Change = 0x80 */
  L7_uchar8           cistFlags;				
  L7_uchar8			  Pad2; 	                /*pad field*/	
  DOT1S_BRIDGEID_t	  cistRootId;
  L7_uint32			  cistExtRootPathCost;
  DOT1S_BRIDGEID_t	  cistRegRootId;
  L7_ushort16		  cistPortId;
  L7_ushort16		  msgAge;
  L7_ushort16		  maxAge;
  L7_ushort16		  helloTime;
  L7_ushort16		  fwdDelay;
  L7_uchar8			  ver1Len;   
  L7_uchar8           Pad6;                     /*pad field*/
  L7_ushort16		  ver3Len;
  L7_uchar8		      mstConfigId[DOT1S_BPDU_CONFIG_ID_LEN];
  L7_uchar8			  Pad7;                     /*pad field*/
  L7_uint32			  cistIntRootPathCost;
  DOT1S_BRIDGEID_t	  cistBridgeId;
  L7_uchar8		      cistRemHops;
  /*beyond this there may be MST Config messages*/
  /*the ver3Len will identify the number of config msgs*/
  DOT1S_MSTI_CFG_MSG_t mstiMsg[L7_MAX_MSTI];

}DOT1S_MSTP_ENCAPS_t;

typedef struct 
{
  L7_uchar8 		  mstiFlags;
  L7_uchar8	  		  mstiRegRootId[8];
  L7_uchar8			  mstiIntRootPathCost[4];
  L7_uchar8			  mstiBridgePriority;
  L7_uchar8           mstiPortPriority;
  L7_uchar8           mstiRemHops;
}DOT1S_BYTE_MSTI_CFG_MSG_t;


typedef struct
{
  /* header information */
  L7_uchar8           destMac[L7_MAC_ADDR_LEN]; /* destination MAC */
  L7_uchar8           srcMac[L7_MAC_ADDR_LEN];  /* source MAC */
  L7_uchar8           packetLength[2];          /* packet length */
  L7_uchar8           logicalLinkCtrl[3];       /* LLC information */
  /* bpdu message */
  L7_uchar8           protocolIdentifier[2];    /* Spanning Tree = 0x0000 */
  L7_uchar8           protocolVersion;          /* Current version = 0x00 */
  L7_uchar8           bdpuType;                 /* Config = 0x00, Topo Change = 0x80 */
  L7_uchar8           flags;                    /* bit 0: topo change, bit 1: topo change ack */
} dot1sByteBpduHeader_t;

typedef struct 
{
  /* header information */
  L7_uchar8           destMac[L7_MAC_ADDR_LEN]; /* destination MAC */
  L7_uchar8           srcMac[L7_MAC_ADDR_LEN];  /* source MAC */
  L7_uchar8           packetLength[2];          /* packet length */
  L7_uchar8           logicalLinkCtrl[3];       /* LLC information */
  /* bpdu message */
  L7_uchar8           protocolIdentifier[2];    /* Spanning Tree = 0x0000 */
  L7_uchar8           protocolVersion;          /* Current version = 0x00 */
  L7_uchar8           bdpuType;                 /* Config = 0x00, Topo Change = 0x80 */
  L7_uchar8           cistFlags;
  L7_uchar8	          cistRootId[8];
  L7_uchar8			  cistExtRootPathCost[4];
  L7_uchar8	          cistRegRootId[8];
  L7_uchar8		      cistPortId[2];
  L7_uchar8		  	  msgAge[2];
  L7_uchar8		  	  maxAge[2];
  L7_uchar8		      helloTime[2];
  L7_uchar8		      fwdDelay[2];
  L7_uchar8			  ver1Len;
  L7_uchar8		      ver3Len[2];
  L7_uchar8		      mstConfigId[51];
  L7_uchar8			  cistIntRootPathCost[4];
  L7_uchar8	  		  cistBridgeId[8];
  L7_uchar8		      cistRemHops ;
  /*beyond this there may be MST Config messages*/
  /*the ver3Len will identify the number of config msgs*/
  DOT1S_BYTE_MSTI_CFG_MSG_t mstiMsg[L7_MAX_MSTI];

}DOT1S_BYTE_MSTP_ENCAPS_t;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
L7_RC_t dot1sBpduTransmit(L7_netBufHandle bufHandle, 
						  L7_uint32 intIfNum);
L7_RC_t dot1sBpduReceive(L7_netBufHandle bufHandle, 
						 sysnet_pdu_info_t *bpduInfo);
L7_RC_t dot1sTcnTx(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sConfigTx(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sMstpTx(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sTxRxInit(void);
L7_RC_t dot1sTxRxHeaderTagRemove(L7_netBufHandle bufHandle);
L7_RC_t dot1sTxRxHostConvert(L7_netBufHandle bufHandle, L7_uchar8 ** buff);
L7_netBufHandle dot1sTxRxNetworkConvert(L7_netBufHandle bufHandle);

#endif /*INCLUDE_DOT1S_TXRX_H*/
