/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#ifndef SNOOPING_UTIL_H
#define SNOOPING_UTIL_H

#include "snooping.h"
#include "ptin_mgmd_defs.h"
#include "ptin_mgmd_cfg.h"
#include "ptin_mgmd_inet_defs.h"
#include "snooping_ptin_defs.h"

#define SNOOP_GET_BYTE(val, cp) ((val) = *(uchar8 *)(cp)++)

#define SNOOP_GET_LONG(val, cp) \
  do { \
       memcpy(&(val),(cp), sizeof(uint32));\
       (val) = ntohl((val));\
       (cp) += sizeof(uint32);\
     } while (0)

#define SNOOP_GET_SHORT(val, cp) \
  do { \
       memcpy(&(val),(cp), sizeof(ushort16));\
       (val) = ntohs((val));\
       (cp) += sizeof(ushort16);\
     } while (0)

#define SNOOP_GET_ADDR(addr, cp) \
  do { \
       memcpy((addr),(cp), sizeof(uint32));\
       (*addr) = ntohl((*addr));\
       (cp) += sizeof(uint32);\
     } while (0)

#define SNOOP_GET_ADDR6(addr, cp)\
  do { \
       register uchar8 *Xap; \
       register int i; \
       Xap = (uchar8 *)(addr); \
       for (i = 0; i < 16; i++) \
         *Xap++ = *(cp)++; \
     } while (0)

#define SNOOP_PUT_BYTE(val, cp) (*(cp)++ = (uchar8)(val))

#define SNOOP_PUT_SHORT(val, cp) \
  do { \
       (val) = htons((val));\
       memcpy((cp), &(val), sizeof(ushort16));\
       (cp) += sizeof(ushort16); \
     } while (0)

#define SNOOP_PUT_ADDR(addr, cp) \
  do { \
       (addr) = htonl((addr));\
       memcpy((cp), &(addr), sizeof(uint32));\
       (cp) += sizeof(uint32); \
     } while (0)

#define SNOOP_PUT_DATA(data, len, cp) (memcpy (cp, data, len), (cp) += (len))

#define SNOOP_UNUSED_PARAM(x) ((void)(x))
#define SNOOP_INTERVAL_ROUND(x, y) (((x) % (y) != 0) ? ((x)/(y))+1 : (x)/(y))


RC_t      ptinMgmdQuerySchedule(uint16 serviceId, ptin_mgmd_inet_addr_t* groupAddr, BOOL sFlag, ptin_mgmd_inet_addr_t *sources, uint8 sourcesCnt);
RC_t      ptinMgmdScheduleReportMessage(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr, uint8  reportType,uint32 timeOut, BOOL isInterface,uint32 noOfRecords, void* ptr);
RC_t      mgmdBuildIgmpv2CSR(uint32 serviceId,uint32 maxResponseTime);
int32     ptinMgmd_generate_random_response_delay (int32 maxResponseTime);

RC_t      ptinMgmdPacketSend(struct mgmdSnoopControlPkt_s *mcastPacket, uchar8 igmp_type, uchar8 portType);
RC_t      ptinMgmdPacketPortSend(mgmdSnoopControlPkt_t *mcastPacket, uint8 igmp_type, uint16 portId);

ushort16  ptinMgmdCheckSum(ushort16 *addr, ushort16 len, ushort16 csum);

uint8     ptinMgmdPacketType2IGMPStatField(uint8 packetType,uint8 fieldType);

void      ptinMgmdDumpL3AvlTree(void);
void      ptinMgmdDumpGroupRecordAvlTree(void);
void      ptinMgmdMcastgroupPrint(int32 serviceId,uint32 groupAddrText);
void      ptinMgmdGroupRecordPrint(uint32 serviceId,uint32 groupAddrText,uint8 recordType);

#endif /* SNOOPING_UTIL_H */
