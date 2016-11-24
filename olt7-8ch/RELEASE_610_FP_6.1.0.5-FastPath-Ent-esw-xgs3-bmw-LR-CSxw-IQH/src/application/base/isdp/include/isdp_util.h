/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_util.h
*
* @purpose   isdp utility file
*
* @component isdp
*
* @comments
*
* @create    23/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#ifndef ISDP_UTIL_H
#define ISDP_UTIL_H

#include "isdp_txrx.h"

typedef enum isdpDeviceIdFormat_s
{
  ISDP_DEVICE_ID_SERIAL = 1,
  ISDP_DEVICE_ID_MAC,
  ISDP_DEVICE_ID_OTHER
} isdpDeviceIdFormat_t;

#define ISDP_CAP_ROUTER_BITMASK               0x00000001
#define ISDP_CAP_TRANS_BRIDGE_BITMASK         0x00000002
#define ISDP_CAP_SOURCE_ROUTE_BRIDGE_BITMASK  0x00000004
#define ISDP_CAP_SWITCH_BITMASK               0x00000008
#define ISDP_CAP_HOST_BITMASK                 0x00000010
#define ISDP_CAP_IGMP_BITMASK                 0x00000020
#define ISDP_CAP_REPEATER_BITMASK             0x00000040

#define ISDP_CAP_ROUTER_ASCII                'R'/* 0x52*/
#define ISDP_CAP_TRANS_BRIDGE_ASCII          'T'/* 0x54*/
#define ISDP_CAP_SOURCE_ROUTE_BRIDGE_ASCII   'B'/* 0x42*/
#define ISDP_CAP_SWITCH_ASCII                'S'/* 0x53*/
#define ISDP_CAP_HOST_ASCII                  'H'/* 0x48*/
#define ISDP_CAP_IGMP_ASCII                  'I'/* 0x49*/
#define ISDP_CAP_REPEATER_ASCII              'r'/* 0x72*/

#define ISDP_CAP_ROUTER_STRING               "Router "
#define ISDP_CAP_TRANS_BRIDGE_STRING         "Trans Bridge "
#define ISDP_CAP_SOURCE_ROUTE_BRIDGE_STRING  "Source Route Bridge "
#define ISDP_CAP_SWITCH_STRING               "Switch "
#define ISDP_CAP_HOST_STRING                 "Host "
#define ISDP_CAP_IGMP_STRING                 "IGMP "
#define ISDP_CAP_REPEATER_STRING             "Repeater "

#define ISDP_ID_FORMAT_CAP_SN_STRING         "Serial Number "
#define ISDP_ID_FORMAT_CAP_MA_STRING         "MAC Address "
#define ISDP_ID_FORMAT_CAP_OTHER_STRING      "Other "

#define ISDP_DEVICE_ID_FORMAT_CAPABILITY_CURRENT_BITMASK L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_SERIAL_BITMASK

typedef struct isdpIntfInfoData_s
{
  AcquiredMask acquiredList;
  L7_BOOL intfAcquired;
  L7_uchar8 active;
  L7_uchar8 version1Received;
} isdpIntfInfoData_t;

typedef struct isdpEntryKey_s
{
  L7_uint32      intIfNum;
  L7_uchar8      deviceId[L7_ISDP_DEVICE_ID_LEN];
}isdpEntryKey_t;

typedef struct isdpEntry_s
{
  isdpEntryKey_t        key;
  L7_uchar8             holdTime;
  L7_uchar8             protoVersion;
  L7_uint32             capabilities;
  L7_uchar8             portId[L7_ISDP_PORT_ID_LEN];
  L7_uchar8             platform[L7_ISDP_PLATFORM_LEN];
  L7_uchar8             version[L7_ISDP_VERSION_LEN];
  L7_uint32             addressNumber;
  isdpIpAddressBuffer_t *ipAddressList;
  L7_uint32             lastChangeTime;

  /* This field is needed by the AVL Library */
  void           *avlNext;
}isdpEntry_t;

typedef struct isdpIntfStats_s
{
  L7_uint32            pduRx;
  L7_uint32            v1PduRx;
  L7_uint32            v2PduRx;
  L7_uint32            badHeaderRx;
  L7_uint32            chkSumErrorRx;
  L7_uint32            invalidFormatRx;

  L7_uint32            pduTx;
  L7_uint32            v1PduTx;
  L7_uint32            v2PduTx;
  L7_uint32            failureTx;
} isdpIntfStats_t;

typedef struct isdpStats_s
{
  L7_uint32            lastChangeTime;  /* last insert, delete, or modify of record */
  L7_uint32            insertEntryDrops;   /* # of times neighbor entry not inserted due to lack of resources */
  L7_uint32            insertIpAddressDrops;   /* # of times ip address not inserted due to lack of resources */
  isdpIntfStats_t      intfStats[L7_ISDP_INTF_MAX_COUNT];
  isdpIntfStats_t      globalStats;
} isdpStats_t;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */

isdpEntry_t * isdpEntryFind(isdpEntryKey_t key);
isdpEntry_t * isdpEntryDeviceIdFind(isdpEntryKey_t key, L7_uchar8* deviceId);
L7_RC_t isdpEntryAdd(isdpEntry_t * pEntry);
L7_RC_t isdpEntryDelete(isdpEntryKey_t key);
L7_RC_t isdpIntfAllEntryDelete(L7_uint32 intIfNum);
L7_RC_t isdpAllEntryDelete(void);
L7_uint32 isdpDataCountEntryTree(void);
isdpEntry_t * isdpEntryGetNext(isdpEntryKey_t key);
L7_RC_t isdpTreeDelete  (void);

isdpIpAddressBuffer_t *isdpIpAddrBufferGet(isdpIpAddressBuffer_t **pIpAddrList);

void isdpIpAddrListRelease(isdpIpAddressBuffer_t **pIpAddrlist);

L7_RC_t isdpIpAddrListIndexGet(isdpIpAddressBuffer_t *ipAddrList,
                               L7_uint32 index,
                               L7_uint32 *address);
L7_RC_t isdpIpAddrPutList(isdpIpAddressBuffer_t **pIpAddressList,
                          L7_uint32 ipAddress);

L7_RC_t isdpFillEntry(isdpEntry_t * pIsdpEntry, isdpPacket_t *pdu,
    L7_uint32 intIfNum);

void isdpTrafficPduReceivedInc(L7_uint32 intIfNum);
void isdpTrafficV1PduReceivedInc(L7_uint32 intIfNum);
void isdpTrafficV2PduReceivedInc(L7_uint32 intIfNum);
void isdpTrafficBadHeaderPduReceivedInc(L7_uint32 intIfNum);
void isdpTrafficChkSumErrorPduReceivedInc(L7_uint32 intIfNum);
void isdpTrafficInvalidFormatPduReceivedInc(L7_uint32 intIfNum);
void isdpTrafficPduTransmitInc(L7_uint32 intIfNum);
void isdpTrafficV1PduTransmitInc(L7_uint32 intIfNum);
void isdpTrafficV2PduTransmitInc(L7_uint32 intIfNum);
void isdpTrafficFailurePduTransmitInc(L7_uint32 intIfNum);
void isdpInsertEntryDropsInc(void);
void isdpInsertIpAddressDropsInc(void);
void isdpStatsClear(void);
L7_RC_t isdpNeighborsLastChangeTimeUpdate();

/* handle configuration mapping */
L7_BOOL isdpIntfIsConfigurable(L7_uint32 intIfNum, isdpIntfCfgData_t **pCfg);
L7_BOOL isdpIntfConfigEntryGet(L7_uint32 intIfNum, isdpIntfCfgData_t **pCfg);

L7_RC_t isdpIntfAddrGet(L7_uint32 intIfNum, L7_uint32 addrType,
    L7_uchar8 *macAddr);

L7_RC_t isdpIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
    NIM_CORRELATOR_t correlator);

L7_uint32 isdpIntfChangeProcess(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status);

L7_BOOL isdpIsValidIntf(L7_uint32 intIfNum);

L7_BOOL isdpIsValidIntfType(L7_uint32 sysIntfType);

L7_RC_t isdpModeApply(L7_uint32 mode);

L7_RC_t isdpIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode);

#endif /*ISDP_UTIL_H*/

