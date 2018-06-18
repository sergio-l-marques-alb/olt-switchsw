/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename              rfc1213.h
 *
 * @purpose
 *
 * @component              Routing MIB Component
 *
 * @comments
 *
 * @create
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef rfc1213_h
#define rfc1213_h

#include "rfc1573.h"


/* Interface types from rfc-1573 */
typedef enum tag_ifType
{
     ifType__other = 1
   , ifType__regular1822 = 2
   , ifType__hdh1822 = 3
   , ifType__ddn_x25 = 4
   , ifType__rfc877_x25 = 5
   , ifType__ethernet_csmacd = 6
   , ifType__iso88023_csmacd = 7
   , ifType__iso88024_tokenBus = 8
   , ifType__iso88025_tokenRing = 9
   , ifType__iso88026_an = 10
   , ifType__starLan = 11
   , ifType__proteon_10Mbit = 12
   , ifType__proteon_80Mbit = 13
   , ifType__hyperchannel = 14
   , ifType__fddi = 15
   , ifType__lapb = 16
   , ifType__sdlc = 17
   , ifType__ds1 = 18
   , ifType__e1 = 19
   , ifType__basicISDN = 20
   , ifType__primaryISDN = 21
   , ifType__propPointToPointSerial = 22
   , ifType__ppp = 23
   , ifType__softwareLoopback = 24
   , ifType__eon = 25
   , ifType__ethernet_3Mbit = 26
   , ifType__nsip = 27
   , ifType__slip = 28
   , ifType__ultra = 29
   , ifType__ds3 = 30
   , ifType__sip = 31
   , ifType__frame_relay = 32
   , ifType__rs232 = 33
   , ifType__para = 34
   , ifType__arcnet = 35
   , ifType__arcnetPlus = 36
   , ifType__atm = 37
   , ifType__miox25 = 38
   , ifType__sonet = 39
   , ifType__x25ple = 40
   , ifType__iso88022llc = 41
   , ifType__localTalk = 42
   , ifType__smdsDxi = 43
   , ifType__frameRelayService = 44
   , ifType__v35 = 45
   , ifType__hssi = 46
   , ifType__hippi = 47
   , ifType__modem = 48
   , ifType__aal5 = 49
   , ifType__sonetPath = 50
   , ifType__sonetVT = 51
   , ifType__smdsIcip = 52
   , ifType__propVirtual = 53
   , ifType__propMultiplexor = 54
   , ifType__ds0 = 81
   , ifType__ds0bundle = 82
} e_ifType;


typedef enum tag_ifAdminStatus
{
     ifStatus__up = 1
   , ifStatus__down = 2
   , ifStatus__testing = 3
} e_ifAdminStatus;

typedef e_ifAdminStatus e_ifOperStatus;


#define MAX_PHYSADDRESS   20

/*
 * interface table
 */
typedef struct tag_ifTable
{
   ulng            ifIndex;
   char           *ifDescr;
   e_ifType        ifType;
   ulng            ifMtu;
   ulng            ifSpeed;
   byte            ifPhysAddress[MAX_PHYSADDRESS];
   word            ifPhysAddrLen;
   e_ifAdminStatus ifAdminStatus;
   e_ifOperStatus  ifOperStatus;
   ulng            ifLastChange;
   ulng            ifInOctets;
   ulng            ifInUcastPkts;
   ulng            ifInNUcastPkts;
   ulng            ifInDiscards;
   ulng            ifInErrors;
   ulng            ifInUnknownProtos;
   ulng            ifOutOctets;
   ulng            ifOutUcastPkts;
   ulng            ifOutDiscards;
   ulng            ifOutErrors;
   ulng            ifOutQLen;
   ulng            ifSpecific;
} t_ifTable;


/*
 * prototype for interface routine that retrieves the
 * ifTable data structure in an interface object
 */
typedef t_ifTable * (F_GetIfTable)(t_Handle);


/********** extensions to ifTable **********/

#define IFEXT_NAME_LENGTH  4

typedef struct tag_ifExt
{
   char          ifName[IFEXT_NAME_LENGTH];
   ulng          ifInMulticastPkts;
   ulng          ifInBroadcastPkts;
   ulng          ifOutMulticastPkts;
   ulng          ifOutBroadcastPkts;
   double        ifHCInOctets;
   double        ifHCInUcastPkts;
   double        ifHCInMulticastPkts;
   double        ifHCInBroadcastPkts;
   double        ifHCOutOctets;
   double        ifHCOutUcastPkts;
   double        ifHCOutMulticastPkts;
   double        ifHCOutBroadcastPkts;
   word          ifLinkUpDownTrapEnable;
   ulng          ifHighSpeed;
   e_TruthValue  ifPromiscuousMode;
   e_TruthValue  ifConnectorPresent;
} t_ifExt;


/* definition of system objects group */
extern ulng sys_timeSystemUp;

#define SYSDESCR        "Broadcom OLTSWITCH"
#define SYSOBJECTID     {{1, 3, 6, 1, 4, 1, 353, 1, 5}, 9 }
#define SYSUPTIME       (TICKS_TO_MILLI(TIMER_SysTime() - sys_timeSystemUp) / 10)
#define SYSCONTACT      "PTInovacao"
#define SYSNAME         "www.ptinovacao.pt"
#define SYSLOCATION     "Aveiro, PORTUGAL"
#define SYSSERVICES     72

#endif

/* --- end of file rfc1213.h --- */
