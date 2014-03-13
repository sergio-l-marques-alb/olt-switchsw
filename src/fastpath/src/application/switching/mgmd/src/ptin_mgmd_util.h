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

#include "ptin_mgmd_core.h"
#include "ptin_mgmd_defs.h"
#include "ptin_mgmd_cfg.h"
#include "ptin_mgmd_inet_defs.h"

#define PTIN_MGMD_CLEAR_ARRAY(array)         memset((array),0x00,sizeof(array))

/* PTIN_MGMD_IS_MASKBITSET returns 0 if the id is not set in mask array */
#define PTIN_MGMD_PORT_IS_MASKBITSET(array, id)                                  \
        (array[((id-1)/(8*sizeof(uint8)))]                          \
                         & ( 1 << ((id-1) % (8*sizeof(uint8)))) )


/* PTIN_MGMD_SET_MASKBIT turns on bit index # id in mask array. */
#define PTIN_MGMD_PORT_SET_MASKBIT(array, id)                                    \
            (array[((id-1)/(8*sizeof(uint8)))]                      \
                         |= 1 << ((id-1) % (8*sizeof(uint8))))

/* PTIN_MGMD_UNSET_MASKBIT turns off bit index # id in mask array. */
#define PTIN_MGMD_PORT_UNSET_MASKBIT(array, id)                                  \
           (array[((id-1)/(8*sizeof(uint8)))]                       \
                        &= ~(1 << ((id-1) % (8*sizeof(uint8)))))

#define PTIN_MGMD_CLIENT_IS_MASKBITSET(array,idx) ((array[(idx)/(sizeof(uint8)*8)] >> ((idx)%(sizeof(uint8)*8))) & 1)
#define PTIN_MGMD_CLIENT_SET_MASKBIT(array,idx)   { array[(idx)/(sizeof(uint8)*8)] |=   (uint8) 1 << ((idx)%(sizeof(uint8)*8)) ; }
#define PTIN_MGMD_CLIENT_UNSET_MASKBIT(array,idx) { array[(idx)/(sizeof(uint8)*8)] &= ~((uint8) 1 << ((idx)%(sizeof(uint8)*8))); }

#define PTIN_MGMD_CLIENT_NONZEROMASK(array, result)                         \
{                                                                           \
    uint32 _i_;                                                             \
    result=-1;                                                              \
    for(_i_ = 0; _i_ < sizeof(array)/sizeof(uint8); ++_i_)                  \
        if(array[_i_] != 0)                                                 \
        {                                                                   \
            result = _i_;                                                   \
            break;                                                          \
        }                                                                   \
}

#define PTIN_MGMD_GET_BYTE(val, cp) ((val) = *(uchar8 *)(cp)++)

#define PTIN_MGMD_GET_LONG(val, cp) \
  do { \
       memcpy(&(val),(cp), sizeof(uint32));\
       (val) = ntohl((val));\
       (cp) += sizeof(uint32);\
     } while (0)

#define PTIN_MGMD_GET_SHORT(val, cp) \
  do { \
       memcpy(&(val),(cp), sizeof(ushort16));\
       (val) = ntohs((val));\
       (cp) += sizeof(ushort16);\
     } while (0)

#define PTIN_MGMD_GET_ADDR(addr, cp) \
  do { \
       memcpy((addr),(cp), sizeof(uint32));\
       (*addr) = ntohl((*addr));\
       (cp) += sizeof(uint32);\
     } while (0)

#define PTIN_MGMD_GET_ADDR6(addr, cp)\
  do { \
       register uchar8 *Xap; \
       register int i; \
       Xap = (uchar8 *)(addr); \
       for (i = 0; i < 16; i++) \
         *Xap++ = *(cp)++; \
     } while (0)

#define PTIN_MGMD_PUT_BYTE(val, cp) (*(cp)++ = (uchar8)(val))

#define PTIN_MGMD_PUT_SHORT(val, cp) \
  do { \
       (val) = htons((val));\
       memcpy((cp), &(val), sizeof(ushort16));\
       (cp) += sizeof(ushort16); \
     } while (0)

#define PTIN_MGMD_PUT_ADDR(addr, cp) \
  do { \
       (addr) = htonl((addr));\
       memcpy((cp), &(addr), sizeof(uint32));\
       (cp) += sizeof(uint32); \
     } while (0)

#define PTIN_MGMD_PUT_DATA(data, len, cp) (memcpy (cp, data, len), (cp) += (len))

#define PTIN_MGMD_UNUSED_PARAM(x) ((void)(x))
#define PTIN_MGMD_INTERVAL_ROUND(x, y) (((x) % (y) != 0) ? ((x)/(y))+1 : (x)/(y))


RC_t      ptinMgmdQuerySchedule(uint16 serviceId, ptin_mgmd_inet_addr_t* groupAddr, BOOL sFlag, ptin_mgmd_inet_addr_t *sources, uint8 sourcesCnt);
RC_t      ptinMgmdScheduleReportMessage(uint32 serviceId, ptin_mgmd_inet_addr_t* groupAddr, uint8  reportType,uint32 timeOut, BOOL isInterface,uint32 noOfRecords, void* ptr);
RC_t      mgmdBuildIgmpv2CSR(uint32 serviceId,uint32 maxResponseTime);
int32     ptinMgmd_generate_random_response_delay (int32 maxResponseTime);

RC_t      ptinMgmdPacketSend(struct mgmdSnoopControlPkt_s *mcastPacket, uchar8 igmp_type, uchar8 portType);
RC_t      ptinMgmdPacketPortSend(ptinMgmdControlPkt_t *mcastPacket, uint8 igmp_type, uint16 portId);

ushort16  ptinMgmdCheckSum(ushort16 *addr, ushort16 len, ushort16 csum);

uint8     ptinMgmdPacketType2IGMPStatField(uint8 packetType,uint8 fieldType);

void      ptinMgmdDumpL3AvlTree(void);
void      ptinMgmdCleanAllGroupAvlTree(void);
void      ptinMgmdDumpGroupRecordAvlTree(void);
RC_t      ptinMgmdCleanUpGroupRecordAvlTree(uint32 serviceId);
void      ptinMgmdCleanAllGroupRecordAvlTree(void);

void      ptinMgmdDumpGeneralQuery(void);
void      ptinMgmdCleanAllGeneralQuery(void);
void      ptinMgmdStartAllGeneralQuery(void);
void      ptinMgmdStopAllGeneralQuery(void);

void      ptinMgmdCleanAllGroupSpecificQueriesAvlTree(void);
void      ptinMgmdMcastgroupPrint(int32 serviceId,uint32 groupAddrText);
void      ptinMgmdGroupRecordPrint(uint32 serviceId,uint32 groupAddrText,uint8 recordType);

RC_t      ptinMgmdServiceRemove(uint32 serviceId);

#endif /* SNOOPING_UTIL_H */
