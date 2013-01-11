/*
 *
 * Copyright (C) 1996-2001 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/*
 * number of fields in /proc/net/dev can be different in different Linux
 * version. 
 *
 */


#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "sitedefs.h"
#include "trap.h"

#include "snmpsupp.h"

#include "l7_common.h"
#include "dtlapi.h"
#include "usmdb_1213_api.h"
#include "usmdb_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <net/if.h>
/* #include <linux/netdevice.h> */
#include <net/if_arp.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <unistd.h> 
#include <linux/sockios.h> 
#include <arpa/inet.h> 
#include <net/route.h>
#include <errno.h> 
#include <sys/select.h>

#ifndef ATF_PERM
#define ATF_PERM 0x04
#endif /* ATF_PERM */

#ifndef ATF_COM
#define ATF_COM 0x02
#endif /* ATF_COM */


#define MAX_LINE_LEN 512
#define MAX_DEV_NUM 20
#define DEV_NAME_LEN 32
#define SNMP_BUFFER_LEN 128

static char line[MAX_LINE_LEN];

static int SocketFd = -1;
static int route_found = 0;
int ifIndex;

struct rtentry g_rtentry;	/* hold routing table entry info */

static char devName[MAX_DEV_NUM][DEV_NAME_LEN];  /* dev name and index */

extern OID      nullOID;

int open_socket()
{
    FNAME("open_socket");
    /* Open socket for ioctl call */

    if((SocketFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
       DPRINTF((APERROR, "%s: can not create socket\n", Fname));
       return -1;
    }
    return 1;
}

int k_initialize()
{
    int ret_val;
    FNAME("k_initialize");

    ret_val = open_socket();
    if(ret_val!=1){
       goto fail;
    }
    return 1;

 fail:
    /* k_terminate(); */
    return -1;

}

int update_devName()
{
    static char ifbuff[32];
    L7_ulong32 a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p; /* dummy */
    int count, num;
    char *ptr;
    FILE *fp;

    /* Get device information from /proc/net/dev */

    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        DPRINTF((APWARN, "get_ifIndex: Cannot open /proc/net/dev\n"));
        perror("");
        return -1;
    }
    count = 0;

    for(num=0; num<MAX_DEV_NUM; num++){
       memset(&devName[num], 0, sizeof(char)*DEV_NAME_LEN);
    }

    /* update devname table */

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if ((ptr = strrchr(line, ':')) != NULL) {
            *ptr = ' ';
        }
        if (sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu \
				%lu %lu %lu %lu %lu %lu %lu %lu %lu ",
                         ifbuff, &a, &b, &c, &d, &e, &f, &g, &h,
			 &i, &j, &k, &l, &m, &n, &o, &p) == 17) {
            count++;
	    memcpy(&devName[count], ifbuff, strlen(ifbuff));
        }
    }
    fclose(fp);
    strcpy(devName[0],"eth0");
    return 1;
}

int
get_ifIndex(ifname)
    char *ifname;
{
    static char ifbuff[32];
    int ifIndex = -1;
    L7_ulong32 a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p; /* dummy */
    int count;
    char *ptr;
    FILE *fp;

    /* Get device information from /proc/net/dev */

    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        DPRINTF((APWARN, "get_ifIndex: Cannot open /proc/net/dev\n"));
        perror("");
        return -1;
    }
    count = 0;


    while (fgets (line, MAX_LINE_LEN, fp)) {
        if ((ptr = strrchr(line, ':')) != NULL) {
            *ptr = ' ';
        }
        if (sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu \
				%lu %lu %lu %lu %lu %lu %lu %lu %lu ",
                         ifbuff, &a, &b, &c, &d, &e, &f, &g, &h,
			 &i, &j, &k, &l, &m, &n, &o, &p) == 17) {
            count++;
            if (strncmp(ifbuff, ifname, strlen(ifname)) == 0) {
                ifIndex = count;
                break;
            }
        }
    }
    fclose(fp);

    if (ifIndex == -1) {
        DPRINTF((APWARN, "get_ifIndex failed\n"));
    }
    return ifIndex;
}

#ifdef LINUX_OLD_CODE
CmpIpNetToMediaEntry(arp1, arp2)
    ipNetToMediaEntry_t *arp1;
    ipNetToMediaEntry_t *arp2;
{

    if (arp1->ipNetToMediaIfIndex > arp2->ipNetToMediaIfIndex) {
        return 1;
    } else if (arp1->ipNetToMediaIfIndex < arp2->ipNetToMediaIfIndex) {
        return -1;
    }

    if (arp1->ipNetToMediaNetAddress > arp2->ipNetToMediaNetAddress) {
        return 1;
    } else if (arp1->ipNetToMediaNetAddress < arp2->ipNetToMediaNetAddress) {
        return -1;
    }

    return 0;
}
#endif /* LINUX_OLD_CODE */

ipNetToMediaEntry_t *
k_ipNetToMediaEntry_get(serialNum, contextInfo, nominator, searchType, ipNetToMediaIfIndex, ipNetToMediaNetAddress)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_INT32 ipNetToMediaIfIndex;
SR_UINT32 ipNetToMediaNetAddress;
{
    FNAME("k_ipNetToMediaEntry_get")
    static ipNetToMediaEntry_t ipNetToMediaEntryData;
    
#ifdef LINUX_OLD_CODE
    
    static ipNetToMediaEntry_t best;
    ipNetToMediaEntry_t start, cur;
    static OctetString os;
    FILE *fp;
    int found = 0;
    int a, b, c, d;
    int hwa, hwb, hwc, hwd, hwe, hwf;
    int hwtype, flags;
    int curIndex;
    static char physaddr_buff[6];

    /* Get arp cache table from /proc/net/arp */

    fp = fopen ("/proc/net/arp", "r");
    if (fp == NULL) {
        return NULL;
    }

    curIndex = get_ifIndex("eth0");
    if ((ifIndex > curIndex) || (curIndex < 0)) {
        fclose(fp);
        return NULL;
    }
    if (searchType == EXACT && curIndex != ifIndex) {
        fclose(fp);
        return NULL;
    }
    start.ipNetToMediaIfIndex = curIndex;
    start.ipNetToMediaNetAddress = netaddr;

    best.ipNetToMediaIfIndex = (SR_INT32) 0x7fffffff;
    best.ipNetToMediaNetAddress = (SR_UINT32) 0xffffffff;

    while (line == fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "%d.%d.%d.%d %lx %lx %lx:%lx:%lx:%lx:%lx:%lx",
                  &a, &b, &c, &d,
                  &hwtype, &flags,
                  &hwa, &hwb, &hwc, &hwd, &hwe, &hwf) == 12) {

            cur.ipNetToMediaIfIndex = curIndex;

            cur.ipNetToMediaNetAddress = 0;
            cur.ipNetToMediaNetAddress |= (SR_UINT32) a << ((3) * 8);
            cur.ipNetToMediaNetAddress |= (SR_UINT32) b << ((2) * 8);
            cur.ipNetToMediaNetAddress |= (SR_UINT32) c << ((1) * 8);
            cur.ipNetToMediaNetAddress |= (SR_UINT32) d << ((0) * 8);

            if (CmpIpNetToMediaEntry(&cur, &start) >= 0) {
                if (CmpIpNetToMediaEntry(&cur, &best) <= 0) {
                    physaddr_buff[0] = (char) hwa;
                    physaddr_buff[1] = (char) hwb;
                    physaddr_buff[2] = (char) hwc;
                    physaddr_buff[3] = (char) hwd;
                    physaddr_buff[4] = (char) hwe;
                    physaddr_buff[5] = (char) hwf;
                    cur.ipNetToMediaPhysAddress = &os;
                    cur.ipNetToMediaPhysAddress->octet_ptr = physaddr_buff; 
                    cur.ipNetToMediaPhysAddress->length = 6; 

                    if (flags == 0x00 ) { /* incomplete */
			    cur.ipNetToMediaType = D_ipNetToMediaType_invalid;
                    } else if (flags & ATF_PERM) { /* static */
                            cur.ipNetToMediaType = D_ipNetToMediaType_static;
                    } else { /* dynamic */
                            cur.ipNetToMediaType = D_ipNetToMediaType_dynamic;
		    }

                    memcpy(&best, &cur, sizeof(ipNetToMediaEntry_t));
                    found++;
                }
            }
        }
    }
    fclose(fp);

    if (!found) {
        return NULL;
    }

    if (searchType == EXACT) {
        if (best.ipNetToMediaIfIndex != ifIndex) {
            return NULL;
        }
        if (best.ipNetToMediaNetAddress != netaddr) {
            return NULL;
        }
    }

    SET_ALL_VALID(best.valid);
    return &best;

#endif /* LINUX_OLD_CODE */
    
    OctetString *temp_os;
    L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
    L7_uint32 snmp_buffer_len;
    L7_uint32 intIfNum;

    ZERO_VALID(ipNetToMediaEntryData.valid);
    ipNetToMediaEntryData.ipNetToMediaIfIndex = ipNetToMediaIfIndex;
    SET_VALID(I_ipNetToMediaIfIndex, ipNetToMediaEntryData.valid);
    ipNetToMediaEntryData.ipNetToMediaNetAddress = ipNetToMediaNetAddress;
    SET_VALID(I_ipNetToMediaNetAddress, ipNetToMediaEntryData.valid);


    if ((usmDbIntIfNumFromExtIfNum(ipNetToMediaEntryData.ipNetToMediaIfIndex, &intIfNum) != L7_SUCCESS) ||
       (usmDbIpNetToMediaEntryGet(USMDB_UNIT_CURRENT, intIfNum, ipNetToMediaEntryData.ipNetToMediaNetAddress) != L7_SUCCESS))
    {
      if ((searchType == EXACT) ||
          ((usmDbIntIfNumFromExtIfNum(ipNetToMediaEntryData.ipNetToMediaIfIndex, &intIfNum) != L7_SUCCESS) &&
           ((usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT, ipNetToMediaEntryData.ipNetToMediaIfIndex, &ipNetToMediaEntryData.ipNetToMediaIfIndex) != L7_SUCCESS) ||
           (usmDbIntIfNumFromExtIfNum(ipNetToMediaEntryData.ipNetToMediaIfIndex, &intIfNum) != L7_SUCCESS))) ||
          (usmDbIpNetToMediaEntryNextGet(USMDB_UNIT_CURRENT, &intIfNum, &ipNetToMediaEntryData.ipNetToMediaNetAddress) != L7_SUCCESS) ||
          (usmDbExtIfNumFromIntIfNum(intIfNum, &ipNetToMediaEntryData.ipNetToMediaIfIndex) != L7_SUCCESS))
      {
        ZERO_VALID(ipNetToMediaEntryData.valid);
        return NULL;
      }
    }

    switch (nominator)
    {
    case -1:
    case I_ipNetToMediaIfIndex:
    case I_ipNetToMediaNetAddress:
      break;

    case I_ipNetToMediaPhysAddress:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      if (usmDbIpNetToMediaPhysAddressGet(USMDB_UNIT_CURRENT, ipNetToMediaEntryData.ipNetToMediaIfIndex, ipNetToMediaEntryData.ipNetToMediaNetAddress,
                                          snmp_buffer, &snmp_buffer_len) == L7_SUCCESS)
      {
        if ((temp_os = MakeOctetString(snmp_buffer, snmp_buffer_len)) != NULL)
        {
          FreeOctetString(ipNetToMediaEntryData.ipNetToMediaPhysAddress);
          ipNetToMediaEntryData.ipNetToMediaPhysAddress = temp_os;
          SET_VALID(I_ipNetToMediaPhysAddress, ipNetToMediaEntryData.valid);
        }
      }
      break;

    case I_ipNetToMediaType:
      if (usmDbIpNetToMediaTypeGet(USMDB_UNIT_CURRENT, ipNetToMediaEntryData.ipNetToMediaIfIndex, ipNetToMediaEntryData.ipNetToMediaNetAddress,
                                   &ipNetToMediaEntryData.ipNetToMediaType) == L7_SUCCESS)
      {
        SET_VALID(I_ipNetToMediaType, ipNetToMediaEntryData.valid);
        switch (ipNetToMediaEntryData.ipNetToMediaType)
        {
        case L7_IP_AT_TYPE_OTHER:
          ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_other;
          break;
        case L7_IP_AT_TYPE_INVALID:
          ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_invalid;
          break;
        case L7_IP_AT_TYPE_DYNAMIC:
          ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_dynamic;
          break;
        case L7_IP_AT_TYPE_STATIC:
          ipNetToMediaEntryData.ipNetToMediaType = D_ipNetToMediaType_static;
          break;
        default:
          ipNetToMediaEntryData.ipNetToMediaType = 0;
          CLR_VALID(I_ipNetToMediaType, ipNetToMediaEntryData.valid);
          break;
        }
        break;
      }
    default:
      /* unknown nominator */
      return(NULL);
      break;
    }

    if (nominator >= 0 && !VALID(nominator, ipNetToMediaEntryData.valid))
      return(NULL);

    return(&ipNetToMediaEntryData);

}

#ifdef SETS
int
k_ipNetToMediaEntry_set(data, contextInfo, function)
ipNetToMediaEntry_t *data;
ContextInfo *contextInfo;
int function;
{
  FNAME("k_ipNetToMediaEntry_set")
#ifdef OLD_CODE
  static M2_IPATRANSTBL ipAtEntryToSet;

  if (VALID(I_ipNetToMediaType, data->valid))
  {
    switch (data->ipNetToMediaType)
    {
    case D_ipNetToMediaType_other:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_other;
      break;
    case D_ipNetToMediaType_invalid:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_invalid;
      break;
    case D_ipNetToMediaType_dynamic:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_dynamic;
      break;
    case D_ipNetToMediaType_static:
      ipAtEntryToSet.ipNetToMediaType = M2_ipNetToMediaType_static;
      break;
    }
  }
  else
  {
    return(GEN_ERROR);
  }

  if (ipAtEntryToSet.ipNetToMediaType != M2_ipNetToMediaType_invalid)
  {
    if (VALID(I_ipNetToMediaPhysAddress, data->valid))
    {
      ipAtEntryToSet.ipNetToMediaPhysAddress.addrLength = MIN(data->ipNetToMediaPhysAddress->length, MAXIFPHYADDR);
      memcpy(ipAtEntryToSet.ipNetToMediaPhysAddress.phyAddress, data->ipNetToMediaPhysAddress->octet_ptr, ipAtEntryToSet.ipNetToMediaPhysAddress.addrLength);
    }
    else
    {
      return(GEN_ERROR);
    }
  }

  ipAtEntryToSet.ipNetToMediaIfIndex = (long) data->ipNetToMediaIfIndex;
  ipAtEntryToSet.ipNetToMediaNetAddress = (unsigned long) osapiNtohl((unsigned long) data->ipNetToMediaNetAddress);

  DPRINTF((APTRACE, "%s: calling m2IpAtransTblEntrySet()\n", Fname));
  if (m2IpAtransTblEntrySet(&ipAtEntryToSet)==OK)
  {
    return(NO_ERROR);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpAtransTblEntrySet returned ERROR\n", Fname));
    print_errno();
    return(GEN_ERROR);
  }
#else /* OLD_CODE */

  /* TODO: test this code on network port entries */

  if (usmDbIpNetToMediaEntryGet(USMDB_UNIT_CURRENT, data->ipNetToMediaIfIndex, 
                                data->ipNetToMediaNetAddress) != L7_SUCCESS)
  {
    return(L7_NULL);
  }

  if (VALID(I_ipNetToMediaType, data->valid))
  {
    switch (data->ipNetToMediaType)
    {
    case D_ipNetToMediaType_other:
      data->ipNetToMediaType = L7_IP_AT_TYPE_OTHER;
      break;

    case D_ipNetToMediaType_invalid:
      data->ipNetToMediaType = L7_IP_AT_TYPE_INVALID;
      break;

    case D_ipNetToMediaType_dynamic:
      data->ipNetToMediaType = L7_IP_AT_TYPE_DYNAMIC;
      break;

    case D_ipNetToMediaType_static:
      data->ipNetToMediaType = L7_IP_AT_TYPE_STATIC;
      break;

    default:
      return COMMIT_FAILED_ERROR;
      break;
    }

    if (usmDbIpNetToMediaTypeSet(USMDB_UNIT_CURRENT, data->ipNetToMediaIfIndex,
                                 data->ipNetToMediaNetAddress,
                                 data->ipNetToMediaType) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }

  /* check that the type wasn't set to invalid */
  if (VALID(I_ipNetToMediaPhysAddress, data->valid) &&
      (!VALID(I_ipNetToMediaType, data->valid) || data->ipNetToMediaType != L7_IP_AT_TYPE_INVALID) &&
      usmDbIpNetToMediaPhysAddressSet(USMDB_UNIT_CURRENT, data->ipNetToMediaIfIndex,
                                      data->ipNetToMediaNetAddress,
                                      data->ipNetToMediaPhysAddress->octet_ptr,
                                      data->ipNetToMediaPhysAddress->length) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
#endif /* OLD_CODE */
}
#endif /* SETS */

#ifdef LINUX_OLD_CODE
icmp_t         *
k_icmp_get(id, contextInfo, nominator)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
{
    static icmp_t icmpData;
    FILE *fp;
    int found = 0;

    /* Get icmp infomation from /proc/net/snmp */
    /* it provides snmp related information */

    fp = fopen ("/proc/net/snmp", "r");
    if (fp == NULL) {
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "Icmp: %lu %lu %lu %lu %lu %lu %lu %lu %lu \
			%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu \
			%lu %lu %lu %lu %lu %lu\n",
           &icmpData.icmpInMsgs,
           &icmpData.icmpInErrors,
           &icmpData.icmpInDestUnreachs,
           &icmpData.icmpInTimeExcds,
           &icmpData.icmpInParmProbs,
           &icmpData.icmpInSrcQuenchs,
           &icmpData.icmpInRedirects,
           &icmpData.icmpInEchos,
           &icmpData.icmpInEchoReps,
           &icmpData.icmpInTimestamps,
           &icmpData.icmpInTimestampReps,
           &icmpData.icmpInAddrMasks,
           &icmpData.icmpInAddrMaskReps,
           &icmpData.icmpOutMsgs,
           &icmpData.icmpOutErrors,
           &icmpData.icmpOutDestUnreachs,
           &icmpData.icmpOutTimeExcds,
           &icmpData.icmpOutParmProbs,
           &icmpData.icmpOutSrcQuenchs,
           &icmpData.icmpOutRedirects,
           &icmpData.icmpOutEchos,
           &icmpData.icmpOutEchoReps,
           &icmpData.icmpOutTimestamps,
           &icmpData.icmpOutTimestampReps,
           &icmpData.icmpOutAddrMasks,
           &icmpData.icmpOutAddrMaskReps) == 26) {
            
            found++;
            SET_ALL_VALID(icmpData.valid);
            break;
        }
    }
    
    fclose(fp);
    if (!found) {
        return NULL;
    }
    return &icmpData;
}
#else /* LINUX_OLD_CODE */

icmp_t *
k_icmp_get(serialNum, contextInfo, nominator)
int serialNum;
ContextInfo *contextInfo;
int nominator;
{
  FNAME("k_icmp_get")
  static icmp_t icmpData;

  ZERO_VALID(icmpData.valid);

  switch (nominator)
  {
  case I_icmpInMsgs:
    if (usmDbIcmpInMsgsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInMsgs) == L7_SUCCESS)
      SET_VALID(I_icmpInMsgs, icmpData.valid);
    break;

  case I_icmpInErrors:
    if (usmDbIcmpInErrorsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInErrors) == L7_SUCCESS)
      SET_VALID(I_icmpInErrors, icmpData.valid);
    break;

  case I_icmpInDestUnreachs:
    if (usmDbIcmpInDestUnreachsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInDestUnreachs) == L7_SUCCESS)
      SET_VALID(I_icmpInDestUnreachs, icmpData.valid);
    break;

  case I_icmpInTimeExcds:
    if (usmDbIcmpInTimeExcdsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInTimeExcds) == L7_SUCCESS)
      SET_VALID(I_icmpInTimeExcds, icmpData.valid);
    break;

  case I_icmpInParmProbs:
    if (usmDbIcmpInParmProbsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInParmProbs) == L7_SUCCESS)
      SET_VALID(I_icmpInParmProbs, icmpData.valid);
    break;

  case I_icmpInSrcQuenchs:
    if (usmDbIcmpInSrcQuenchsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInSrcQuenchs) == L7_SUCCESS)
      SET_VALID(I_icmpInSrcQuenchs, icmpData.valid);
    break;

  case I_icmpInRedirects:
    if (usmDbIcmpInRedirectsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInRedirects) == L7_SUCCESS)
      SET_VALID(I_icmpInRedirects, icmpData.valid);
    break;

  case I_icmpInEchos:
    if (usmDbIcmpInEchosGet(USMDB_UNIT_CURRENT, &icmpData.icmpInEchos) == L7_SUCCESS)
      SET_VALID(I_icmpInEchos, icmpData.valid);
    break;

  case I_icmpInEchoReps:
    if (usmDbIcmpInEchoRepsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInEchoReps) == L7_SUCCESS)
      SET_VALID(I_icmpInEchoReps, icmpData.valid);
    break;

  case I_icmpInTimestamps:
    if (usmDbIcmpInTimestampsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInTimestamps) == L7_SUCCESS)
      SET_VALID(I_icmpInTimestamps, icmpData.valid);
    break;

  case I_icmpInTimestampReps:
    if (usmDbIcmpInTimestampRepsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInTimestampReps) == L7_SUCCESS)
      SET_VALID(I_icmpInTimestampReps, icmpData.valid);
    break;

  case I_icmpInAddrMasks:
    if (usmDbIcmpInAddrMasksGet(USMDB_UNIT_CURRENT, &icmpData.icmpInAddrMasks) == L7_SUCCESS)
      SET_VALID(I_icmpInAddrMasks, icmpData.valid);
    break;

  case I_icmpInAddrMaskReps:
    if (usmDbIcmpInAddrMaskRepsGet(USMDB_UNIT_CURRENT, &icmpData.icmpInAddrMaskReps) == L7_SUCCESS)
      SET_VALID(I_icmpInAddrMaskReps, icmpData.valid);
    break;

  case I_icmpOutMsgs:
    if (usmDbIcmpOutMsgsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutMsgs) == L7_SUCCESS)
      SET_VALID(I_icmpOutMsgs, icmpData.valid);
    break;

  case I_icmpOutErrors:
    if (usmDbIcmpOutErrorsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutErrors) == L7_SUCCESS)
      SET_VALID(I_icmpOutErrors, icmpData.valid);
    break;

  case I_icmpOutDestUnreachs:
    if (usmDbIcmpOutDestUnreachsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutDestUnreachs) == L7_SUCCESS)
      SET_VALID(I_icmpOutDestUnreachs, icmpData.valid);
    break;

  case I_icmpOutTimeExcds:
    if (usmDbIcmpOutTimeExcdsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutTimeExcds) == L7_SUCCESS)
      SET_VALID(I_icmpOutTimeExcds, icmpData.valid);
    break;

  case I_icmpOutParmProbs:
    if (usmDbIcmpOutParmProbsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutParmProbs) == L7_SUCCESS)
      SET_VALID(I_icmpOutParmProbs, icmpData.valid);
    break;

  case I_icmpOutSrcQuenchs:
    if (usmDbIcmpOutSrcQuenchsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutSrcQuenchs) == L7_SUCCESS)
      SET_VALID(I_icmpOutSrcQuenchs, icmpData.valid);
    break;

  case I_icmpOutRedirects:
    if (usmDbIcmpOutRedirectsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutRedirects) == L7_SUCCESS)
      SET_VALID(I_icmpOutRedirects, icmpData.valid);
    break;

  case I_icmpOutEchos:
    if (usmDbIcmpOutEchosGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutEchos) == L7_SUCCESS)
      SET_VALID(I_icmpOutEchos, icmpData.valid);
    break;

  case I_icmpOutEchoReps:
    if (usmDbIcmpOutEchoRepsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutEchoReps) == L7_SUCCESS)
      SET_VALID(I_icmpOutEchoReps, icmpData.valid);
    break;

  case I_icmpOutTimestamps:
    if (usmDbIcmpOutTimestampsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutTimestamps) == L7_SUCCESS)
      SET_VALID(I_icmpOutTimestamps, icmpData.valid);
    break;

  case I_icmpOutTimestampReps:
    if (usmDbIcmpOutTimestampRepsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutTimestampReps) == L7_SUCCESS)
      SET_VALID(I_icmpOutTimestampReps, icmpData.valid);
    break;

  case I_icmpOutAddrMasks:
    if (usmDbIcmpOutAddrMasksGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutAddrMasks) == L7_SUCCESS)
      SET_VALID(I_icmpOutAddrMasks, icmpData.valid);
    break;

  case I_icmpOutAddrMaskReps:
    if (usmDbIcmpOutAddrMaskRepsGet(USMDB_UNIT_CURRENT, &icmpData.icmpOutAddrMaskReps) == L7_SUCCESS)
      SET_VALID(I_icmpOutAddrMaskReps, icmpData.valid);
    break;

  default:
    return(NULL);
  }

  if (nominator != 0 && !VALID(nominator, icmpData.valid))
    return(NULL);

  return(&icmpData);
}
#endif /* LINUX_OLD_CODE */


#if 0 /* Removed to allow support of RFC 2333 */

interfaces_t   *
k_interfaces_get(id, contextInfo, nominator)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
{
    static interfaces_t ifData;
    unsigned long a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p; /* dummy */
    char buff[32];
    int cc;
    char *ptr;
    FILE *fp;


    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        return NULL;
    }

    ifData.ifNumber = 0;
    while (fgets (line, MAX_LINE_LEN, fp)) {
        if ((ptr = strrchr(line, ':')) != NULL) {
            *ptr = ' ';
        }
        if (cc = sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu \
				%lu %lu %lu %lu %lu %lu %lu %lu %lu ",
                         buff, &a, &b, &c, &d, &e, &f, &g, &h,
			 &i, &j, &k, &l, &m, &n, &o, &p) == 17) {
            ifData.ifNumber++;
        }
    }

    fclose(fp);
    return &ifData;
}


ifEntry_t      *
k_ifEntry_get(id, contextInfo, nominator, searchType, ifIndex)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    SR_INT32        ifIndex;
{
    static ifEntry_t ifEntryData;
    static char ifname[32];
    unsigned long in_packets, out_packets;
    unsigned long in_fifo, in_frame, in_compressed;
    unsigned long out_fifo, out_frame, out_colls, out_carrier, out_compressed;
    FILE *fp;
    char *ptr;
    OctetString os;
    int found = 0;

    struct ifreq ifrq;
    static char hwaddr_buff[6];
    OctetString hwaddr_os;

    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        DPRINTF((APWARN, "k_ifEntry_get: Cannot open /proc/net/dev\n"));
        perror("");
        return NULL;
    }

    ifEntryData.ifIndex = 0;
    while (fgets (line, MAX_LINE_LEN, fp)) {
        if ((ptr = strrchr(line, ':')) != NULL) {
            *ptr = ' ';
        }
        if (sscanf(line,"%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu \
        		%lu %lu %lu %lu",
                   ifname,
                   &ifEntryData.ifInOctets,
                   &in_packets, 
                   &ifEntryData.ifInErrors,
                   &ifEntryData.ifInDiscards,
		   &in_fifo,
		   &in_frame,
		   &in_compressed,
                   &ifEntryData.ifInNUcastPkts,
                   &ifEntryData.ifOutOctets,
                   &out_packets, 
                   &ifEntryData.ifOutErrors,
                   &ifEntryData.ifOutDiscards,
		   &out_fifo,
		   &out_colls,
		   &out_carrier,
		   &out_compressed) == 17) {

            ifEntryData.ifInUcastPkts = in_packets - ifEntryData.ifInNUcastPkts;
            ifEntryData.ifIndex++;
            if (ifEntryData.ifIndex >= ifIndex) {
                if (searchType == EXACT && ifEntryData.ifIndex != ifIndex) {
                    break;
                }
                found++;
                break;
            }
        }
    }
    fclose(fp);
    if (!found) { 
        return NULL;
    }

    ifEntryData.ifDescr = &os;
    ifEntryData.ifDescr->octet_ptr = ifname;
    ifEntryData.ifDescr->length = strlen(ifname);

    ifEntryData.ifSpecific = &nullOID;

    strcpy (ifrq.ifr_name, ifname);
    if (ioctl(SocketFd, SIOCGIFHWADDR, &ifrq) < 0) {
        DPRINTF((APWARN, "k_ifEntry_get: ioctl failed\n"));
        return NULL;
    } else {
        memcpy(hwaddr_buff, ifrq.ifr_hwaddr.sa_data, 6); 
        ifEntryData.ifPhysAddress = &hwaddr_os;
        ifEntryData.ifPhysAddress->length = 6;
        ifEntryData.ifPhysAddress->octet_ptr = hwaddr_buff;
    }

    if (strncmp(ifname, "lo", 2) == 0) {
        ifEntryData.ifType = D_ifType_softwareLoopback;
        ifEntryData.ifSpeed = 0;
    } else if (strncmp(ifname, "eth", 3) == 0) {
        ifEntryData.ifType = D_ifType_ethernet_csmacd;
        ifEntryData.ifSpeed = 10000000;
    } else if (strncmp(ifname, "ppp", 3) == 0) {
        ifEntryData.ifType = D_ifType_ppp;
        ifEntryData.ifSpeed = 0;
    } else if (strncmp(ifname, "sl", 2) == 0) {
        ifEntryData.ifType = D_ifType_slip;
        ifEntryData.ifSpeed = 0;
    } else {
        ifEntryData.ifType = D_ifType_other;
        ifEntryData.ifSpeed = 0;
    }

    strcpy (ifrq.ifr_name, ifname);
    if (ioctl(SocketFd, SIOCGIFMTU, &ifrq) < 0) {
        DPRINTF((APWARN, "k_ifEntry_get: ioctl failed\n"));
        return NULL;
    } else {
            ifEntryData.ifMtu = ifrq.ifr_mtu;
    }

    strcpy (ifrq.ifr_name, ifname);
    if (ioctl(SocketFd, SIOCGIFMTU, &ifrq) < 0) {
        DPRINTF((APWARN, "k_ifEntry_get: ioctl failed\n"));
        return NULL;
    } else {
            ifEntryData.ifMtu = ifrq.ifr_mtu;
    }

    strcpy (ifrq.ifr_name, ifname);
    if (ioctl(SocketFd, SIOCGIFFLAGS, &ifrq) < 0) {
        DPRINTF((APWARN, "k_ifEntry_get: ioctl failed\n"));
        return NULL;
    } else {
        /* ifEntryData.ifAdminStatus = ifrq.ifr_flags & IFF_RUNNING ? 1 : 2; */
        ifEntryData.ifAdminStatus = ifrq.ifr_flags & IFF_UP ? 1 : 2; 
        ifEntryData.ifOperStatus = ifrq.ifr_flags & IFF_UP ? 1 : 2;
    }

    ifEntryData.ifLastChange = 0;

    SET_ALL_VALID(ifEntryData.valid);
    return(&ifEntryData);

#ifdef NOT_YET
    ifEntryData.ifInUnknownProtos
    ifEntryData.ifOutUcastPkts
    ifEntryData.ifOutNUcastPkts
    ifEntryData.ifOutQLen
#endif
}

k_ifEntry_set(data, contextInfo, state)
    ifEntry_t      *data;
    ContextInfo    *contextInfo;
    int             state;
{
    struct ifreq ifrq;
   
    /* ifAdminStatus is only object which is read-write in ifEntry */

    if(!VALID(I_ifAdminStatus, data->valid)) {
       return NO_ERROR;
    }
    memset(&ifrq, 0, sizeof(ifrq));

    strncpy(ifrq.ifr_name, (char *)data->ifDescr->octet_ptr,
			data->ifDescr->length);

    /* Check current ifAdminStatus */
    if(ioctl(SocketFd, SIOCGIFFLAGS, &ifrq) < 0){
       DPRINTF((APWARN, "k_ifEntry_set(SIOCGIFFLAGS): ioctl failed\n"));
       return GEN_ERROR;
    }

    switch (data->ifAdminStatus) {
       case D_ifAdminStatus_up:
          /* If it is down, bring it up */
          /* Which one you want to use: IFF_UP or IFF_RUNNING ? */
          if((ifrq.ifr_flags & IFF_UP) == 0) { 
	     ifrq.ifr_flags |= IFF_UP; 

	     if(ioctl(SocketFd, SIOCSIFFLAGS, &ifrq) < 0){
                DPRINTF((APWARN, "k_ifEntry_set(SIOCSFFLAGS): ioctl failed\n"));
                return GEN_ERROR;
             }
	  }
	  break;
       case D_ifAdminStatus_down:
          /* If it is up, bring it down */
          /* Which one you want to use: IFF_UP or IFF_RUNNING ? */
          if((ifrq.ifr_flags & IFF_UP) > 0) { 
	     ifrq.ifr_flags &= ~IFF_UP; 

	     if(ioctl(SocketFd, SIOCSIFFLAGS, &ifrq) < 0){
                DPRINTF((APWARN, "k_ifEntry_set(SIOCSIFFLAGS): ioctl failed\n"));
                return GEN_ERROR;
             }
	  }
	  break;
       default:
	  return GEN_ERROR;
    }
    return NO_ERROR;
}
#endif /* 0 Removed to allow for support of RFC 2333 */

#include "k_ipforw.c"


ip_t           *
k_ip_get(id, contextInfo, nominator)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
{
    FNAME("k_ip_get")
    static ip_t ipData;

#ifdef LINUX_OLD_CODE
    FILE *fp;
    int found = 0;

    printf("SNMP k_ip_get called\n");     
    
    fp = fopen ("/proc/net/snmp", "r");
    if (fp == NULL) {
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "Ip: %lu %lu %lu %lu %lu %lu %lu %lu %lu \
			     %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n",
            &ipData.ipForwarding,
            &ipData.ipDefaultTTL,
            &ipData.ipInReceives,
            &ipData.ipInHdrErrors,
            &ipData.ipInAddrErrors,
            &ipData.ipForwDatagrams,
            &ipData.ipInUnknownProtos,
            &ipData.ipInDiscards,
            &ipData.ipInDelivers,
            &ipData.ipOutRequests,
            &ipData.ipOutDiscards,
            &ipData.ipOutNoRoutes,
            &ipData.ipReasmTimeout,
            &ipData.ipReasmReqds,
            &ipData.ipReasmOKs,
            &ipData.ipReasmFails,
            &ipData.ipFragOKs,
            &ipData.ipFragFails,
            &ipData.ipFragCreates) == 19) {

            found++;
            SET_ALL_VALID(ipData.valid);
            break;
        }
    }

    fclose(fp);
    if (!found) {
        return NULL;
    }
    return &ipData;

#endif 

  ZERO_VALID(ipData.valid);

  switch (nominator)
  {
  case I_ipForwarding:
    if (usmDbIpRtrAdminModeGet(USMDB_UNIT_CURRENT, &ipData.ipForwarding) == L7_SUCCESS)
    {
/* lvl7_@p1112 start */
      SET_VALID(I_ipForwarding, ipData.valid);
      switch(ipData.ipForwarding)
      {
      case L7_ENABLE:
        ipData.ipForwarding = D_ipForwarding_forwarding;
        break;
      case L7_DISABLE:
        ipData.ipForwarding = D_ipForwarding_not_forwarding;
        break;
      default:
        CLR_VALID(I_ipForwarding, ipData.valid);
        break;
      }
    }
/* lvl7_@p1112 end */
    break;

  case I_ipDefaultTTL:
    if (usmDbIpDefaultTTLGet(USMDB_UNIT_CURRENT, &ipData.ipDefaultTTL) == L7_SUCCESS)
      SET_VALID(I_ipDefaultTTL, ipData.valid);
    break;

  case I_ipInReceives:
    if (usmDbIpInReceivesGet(USMDB_UNIT_CURRENT, &ipData.ipInReceives) == L7_SUCCESS)
      SET_VALID(I_ipInReceives, ipData.valid);
    break;

  case I_ipInHdrErrors:
    if (usmDbIpInHdrErrorsGet(USMDB_UNIT_CURRENT, &ipData.ipInHdrErrors) == L7_SUCCESS)
      SET_VALID(I_ipInHdrErrors, ipData.valid);
    break;

  case I_ipInAddrErrors:
    if (usmDbIpInAddrErrorsGet(USMDB_UNIT_CURRENT, &ipData.ipInAddrErrors) == L7_SUCCESS)
      SET_VALID(I_ipInAddrErrors, ipData.valid);
    break;

  case I_ipForwDatagrams:
    if (usmDbIpForwDatagramsGet(USMDB_UNIT_CURRENT, &ipData.ipForwDatagrams) == L7_SUCCESS)
      SET_VALID(I_ipForwDatagrams, ipData.valid);
    break;

  case I_ipInUnknownProtos:
    if (usmDbIpInUnknownProtosGet(USMDB_UNIT_CURRENT, &ipData.ipInUnknownProtos) == L7_SUCCESS)
      SET_VALID(I_ipInUnknownProtos, ipData.valid);
    break;

  case I_ipInDiscards:
    if (usmDbIpInDiscards(USMDB_UNIT_CURRENT, &ipData.ipInDiscards) == L7_SUCCESS)
      SET_VALID(I_ipInDiscards, ipData.valid);
    break;

  case I_ipInDelivers:
    if (usmDbIpInDelivers(USMDB_UNIT_CURRENT, &ipData.ipInDelivers) == L7_SUCCESS)
      SET_VALID(I_ipInDelivers, ipData.valid);
    break;

  case I_ipOutRequests:
    if (usmDbIpOutRequests(USMDB_UNIT_CURRENT, &ipData.ipOutRequests) == L7_SUCCESS)
      SET_VALID(I_ipOutRequests, ipData.valid);
    break;

  case I_ipOutDiscards:
    if (usmDbIpOutDiscardsGet(USMDB_UNIT_CURRENT, &ipData.ipOutDiscards) == L7_SUCCESS)
      SET_VALID(I_ipOutDiscards, ipData.valid);
    break;

  case I_ipOutNoRoutes:
    if (usmDbIpNoRoutesGet(USMDB_UNIT_CURRENT, &ipData.ipOutNoRoutes) == L7_SUCCESS)
      SET_VALID(I_ipOutNoRoutes, ipData.valid);
    break;

  case I_ipReasmTimeout:
    if (usmDbIpReasmTimeoutGet(USMDB_UNIT_CURRENT, &ipData.ipReasmTimeout) == L7_SUCCESS)
      SET_VALID(I_ipReasmTimeout, ipData.valid);
    break;

  case I_ipReasmReqds:
    if (usmDbIpReasmReqdsGet(USMDB_UNIT_CURRENT, &ipData.ipReasmReqds) == L7_SUCCESS)
      SET_VALID(I_ipReasmReqds, ipData.valid);
    break;

  case I_ipReasmOKs:
    if (usmDbIpReasmOKsGet(USMDB_UNIT_CURRENT, &ipData.ipReasmOKs) == L7_SUCCESS)
      SET_VALID(I_ipReasmOKs, ipData.valid);
    break;

  case I_ipReasmFails:
    if (usmDbIpReasmFailsGet(USMDB_UNIT_CURRENT, &ipData.ipReasmFails) == L7_SUCCESS)
      SET_VALID(I_ipReasmFails, ipData.valid);
    break;

  case I_ipFragOKs:
    if (usmDbIpFragOKsGet(USMDB_UNIT_CURRENT, &ipData.ipFragOKs) == L7_SUCCESS)
      SET_VALID(I_ipFragOKs, ipData.valid);
    break;

  case I_ipFragFails:
    if (usmDbIpFragFailsGet(USMDB_UNIT_CURRENT, &ipData.ipFragFails) == L7_SUCCESS)
      SET_VALID(I_ipFragFails, ipData.valid);
    break;

  case I_ipFragCreates:
    if (usmDbIpFragCreatesGet(USMDB_UNIT_CURRENT, &ipData.ipFragCreates) == L7_SUCCESS)
      SET_VALID(I_ipFragCreates, ipData.valid);
    break;

  case I_ipRoutingDiscards:
    if (usmDbIpRoutingDiscardsGet(USMDB_UNIT_CURRENT, &ipData.ipRoutingDiscards) == L7_SUCCESS)
      SET_VALID(I_ipRoutingDiscards, ipData.valid);
    break;

  default:
    /* Could not recognize the requested counter id */
    return(NULL);
    break;
  }

  if (nominator != 0 && !VALID(nominator, ipData.valid))
    return(NULL);
  
  return(&ipData);
}

#ifdef SETS
/* lvl7_@p1112 start */
#ifdef OLD_CODE
/* lvl7_@p1112 end */
int
k_ip_set(data, contextInfo, function)
ip_t *data;
ContextInfo *contextInfo;
int function;
{
  FNAME("k_ip_set")
  M2_IP ipVars;
  int varToSet = 0;

  if (VALID(I_ipForwarding, data->valid))
  {
    varToSet |= M2_IPFORWARDING;
    switch (data->ipForwarding)
    {
    case D_ipForwarding_forwarding:
      ipVars.ipForwarding = M2_ipForwarding_forwarding;
      break;
    case D_ipForwarding_not_forwarding:
      ipVars.ipForwarding = M2_ipForwarding_not_forwarding;
      break;
    }
  }
  if (VALID(I_ipDefaultTTL, data->valid))
  {
    varToSet |= M2_IPDEFAULTTTL;
    ipVars.ipDefaultTTL = (long) data->ipDefaultTTL;
  }
  if (varToSet==0)
  {
    return(GEN_ERROR);
  }

  DPRINTF((APTRACE, "%s: calling m2IpGroupInfoSet()\n", Fname));
  if (m2IpGroupInfoSet(varToSet, &ipVars)==OK)
  {
    return(NO_ERROR);
  }
  else
  {
    DPRINTF((APTRACE, "%s: m2IpGroupInfoSet returned ERROR\n", Fname));
    print_errno();
    return(GEN_ERROR);
  }
}
/* lvl7_@p1112 start */
#else /* OLD_CODE */
int
k_ip_set(ip_t *data, ContextInfo *contextInfo, int function)
{
  FNAME("k_ip_set")

  if (VALID(I_ipForwarding, data->valid))
  {
    switch (data->ipForwarding)
    {
    case D_ipForwarding_forwarding:
      if (usmDbIpRtrAdminModeSet(USMDB_UNIT_CURRENT, L7_ENABLE) != L7_SUCCESS)
        return COMMIT_FAILED_ERROR;
      break;
    case D_ipForwarding_not_forwarding:
      if (usmDbIpRtrAdminModeSet(USMDB_UNIT_CURRENT, L7_DISABLE) != L7_SUCCESS)
        return COMMIT_FAILED_ERROR;
      break;
    default:
      return COMMIT_FAILED_ERROR;
      break;
    }
  }

  return NO_ERROR;
}
#endif /* OLD_CODE */
/* lvl7_@p1112 end */
#endif /* SETS */


ipAddrEntry_t *
k_ipAddrEntry_get(serialNum, contextInfo, nominator, searchType, ipAdEntAddr)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_UINT32 ipAdEntAddr;
{
  FNAME("k_ipAddrEntry_get")
  static ipAddrEntry_t ipAddrEntryData;
  L7_uint32 intIfNum = L7_INVALID_INTF;

  ZERO_VALID(ipAddrEntryData.valid);
  ipAddrEntryData.ipAdEntAddr = ipAdEntAddr;
  SET_VALID(I_ipAdEntAddr, ipAddrEntryData.valid);

  if ((searchType == EXACT) ? 
      usmDbIpAdEntAddrGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr,
        intIfNum) != L7_SUCCESS :
    (usmDbIpAdEntAddrGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr,
                         intIfNum) != L7_SUCCESS &&
     usmDbIpAdEntAddrGetNext(USMDB_UNIT_CURRENT, &ipAddrEntryData.ipAdEntAddr,
       &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(ipAddrEntryData.valid);
    return NULL;
  }

  switch (nominator)
  {
  case I_ipAdEntAddr:
    break;

  case I_ipAdEntIfIndex:
    if (usmDbIpEntIfIndexGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntIfIndex) == L7_SUCCESS)
      SET_VALID(I_ipAdEntIfIndex, ipAddrEntryData.valid);
    break;

  case I_ipAdEntNetMask:
    if (usmDbIpAdEntNetMaskGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntNetMask) == L7_SUCCESS)
      SET_VALID(I_ipAdEntNetMask, ipAddrEntryData.valid);
    break;

  case I_ipAdEntBcastAddr:
    if (usmDbIpAdEntBCastGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntBcastAddr) == L7_SUCCESS)
      SET_VALID(I_ipAdEntBcastAddr, ipAddrEntryData.valid);
    break;

  case I_ipAdEntReasmMaxSize:
    if (usmDbIpAdEntReasmMaxSizeGet(USMDB_UNIT_CURRENT, ipAddrEntryData.ipAdEntAddr, &ipAddrEntryData.ipAdEntReasmMaxSize) == L7_SUCCESS)
      SET_VALID(I_ipAdEntReasmMaxSize, ipAddrEntryData.valid);
    break;

  default:
    /* unknown index */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, ipAddrEntryData.valid))
    return(NULL);

  return(&ipAddrEntryData);
}

int CmpIpRouteEntry(ip1, ip2)
    ipRouteEntry_t *ip1;
    ipRouteEntry_t *ip2;
{
    if (ip1->ipRouteDest > ip2->ipRouteDest) {
        return 1;
    } else if (ip1->ipRouteDest < ip2->ipRouteDest) {
        return -1;
    }

    return 0;
}

#ifdef OLD_CODE

ipRouteEntry_t *
k_ipRouteEntry_get(id, contextInfo, nominator, searchType, ipRouteDest)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    SR_UINT32       ipRouteDest;
{
    static ipRouteEntry_t best;
    ipRouteEntry_t start, cur;
    struct sockaddr_in *sin;
    char ifname[32];
    FILE *fp;
    int found = 0;

    int flags;
    int refcount, use, mtu;

    fp = fopen ("/proc/net/route", "r");
    if (fp == NULL) {
	route_found =0;
        return NULL;
    }

    start.ipRouteDest = ipRouteDest;

    best.ipRouteDest = (SR_UINT32) 0xffffffff;

    while (line == fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "%s" INT32_HEX_FORMAT INT32_HEX_FORMAT "%d %d %d %d" INT32_HEX_FORMAT "%d",
                  ifname, 
                  &cur.ipRouteDest,
                  &cur.ipRouteNextHop,
                  &flags,
                  &refcount,
                  &use,
                  &cur.ipRouteMetric1,
                  &cur.ipRouteMask,
                  &mtu) == 9) {

	    if(flags & 0x01){ 
               cur.ipRouteDest = osapiHtonl(cur.ipRouteDest);
               cur.ipRouteNextHop = osapiHtonl(cur.ipRouteNextHop);
               cur.ipRouteMask = osapiHtonl(cur.ipRouteMask);


	       cur.ipRouteType = flags & RTF_GATEWAY ?
			D_ipRouteType_indirect : D_ipRouteType_direct;

#ifdef RTF_MODIFIED
	       cur.ipRouteProto = flags & (RTF_DYNAMIC | RTF_MODIFIED) ?
#else
	       cur.ipRouteProto = flags & RTF_DYNAMIC  ?
#endif
	       D_ipRouteProto_icmp : D_ipRouteProto_other;

               cur.ipRouteMetric2 = -1;
               cur.ipRouteMetric3 = -1;
               cur.ipRouteMetric4 = -1;
               cur.ipRouteMetric5 = -1;
               cur.ipRouteInfo = &nullOID;

               if (CmpIpRouteEntry(&cur, &start) >= 0) {
                  if (CmpIpRouteEntry(&cur, &best) <= 0) {
                      cur.ipRouteIfIndex = get_ifIndex(ifname); 
                      if (cur.ipRouteIfIndex < 0) {
			 route_found = 0;
			 ifIndex =0;
                         fclose(fp);
                         return NULL;
                      }
		      update_devName();
                      memcpy(&best, &cur, sizeof(ipRouteEntry_t));
 	              memset(&g_rtentry, 0, sizeof(struct rtentry));
	              sin = (struct sockaddr_in *)&g_rtentry.rt_dst;
	              sin->sin_family = AF_INET;
	              sin->sin_addr.s_addr = osapiHtonl(cur.ipRouteDest);

	              sin = (struct sockaddr_in *)&g_rtentry.rt_gateway;
	              sin->sin_family = AF_INET;
	              sin->sin_addr.s_addr = osapiHtonl(cur.ipRouteNextHop);

	              sin = (struct sockaddr_in *)&g_rtentry.rt_genmask;
	              sin->sin_family = AF_INET;
	              sin->sin_addr.s_addr = osapiHtonl(cur.ipRouteMask);

	              g_rtentry.rt_flags = flags;
	              g_rtentry.rt_dev = devName[cur.ipRouteIfIndex];
	              g_rtentry.rt_metric = cur.ipRouteMetric1;
                      found++;
		   }
                }
            }
        }
    }

    fclose(fp);
    if (!found) {
	route_found = 0;
	ifIndex = 0;
        return NULL;
    }

    if (searchType == EXACT) {
        if (best.ipRouteDest != ipRouteDest) {
	    route_found = 0;
	    ifIndex = 0;
            return NULL;
        } else {
	    route_found = 1;
	    ifIndex = best.ipRouteIfIndex;
	}
    }

    SET_ALL_VALID(best.valid);
    return &best;
#ifdef NOT_YET
   ipRouteEntryData.ipRouteAge = ;
#endif /* NOT_YET */

}

int k_ipRouteEntry_set(data, contextInfo, state)
    ipRouteEntry_t *data;
    ContextInfo    *contextInfo;
    int             state;
{
    int func = SIOCADDRT;
    int resume;
    struct rtentry rt_entry;
    struct sockaddr_in *sin;

    /* update device name array */

    update_devName();

    memset(&rt_entry, 0, sizeof(struct rtentry));

    /* DEFAULT						*/
    /* we can not add direct destination entry.		*/
    /* so, you have to always provide valid destination	*/
    /* and gateway. Gateway should not be a 0.0.0.0	*/
    /* it will cause state=SR_DELETE			*/

    if(!VALID(I_ipRouteDest, data->valid) ||
       !VALID(I_ipRouteNextHop, data->valid)) {

       return GEN_ERROR;
    }

    /* If Destination address already exist, then remove it from */
    /* routing table first				        */

    resume = 0;
    if(route_found && data->ipRouteType !=D_ipRouteType_invalid &&
			 			state != SR_DELETE){

          if(ioctl(SocketFd, SIOCDELRT, &g_rtentry) < 0){
	     DPRINTF((APWARN, "(SIOCDELRT) ioctl failed. can not remove entry\n"));
	     perror("SIOCDERT");
          }
	  resume = 1;
    } else if(route_found && data->ipRouteType == D_ipRouteType_invalid) {
          if(ioctl(SocketFd, SIOCDELRT, &g_rtentry) < 0){
	     DPRINTF((APWARN, "(SIOCDELRT) ioctl failed. can not remove entry\n"));
	     perror("SIOCDERT");
          }
	  return NO_ERROR;
    }

    /* all family should be AF_INET */
    rt_entry.rt_dst.sa_family = AF_INET;
    rt_entry.rt_gateway.sa_family = AF_INET;
    rt_entry.rt_genmask.sa_family = AF_INET;
 
    rt_entry.rt_flags = (RTF_UP | RTF_HOST);

    /* host or network routing?	*/

    if(!(data->ipRouteMask & 0xff)){
       rt_entry.rt_flags &= ~RTF_HOST;
    }

    /* Should we change device name */
    if(VALID(I_ipRouteIfIndex, data->valid)){
       if(devName[data->ipRouteIfIndex] != NULL){
           rt_entry.rt_dev = devName[data->ipRouteIfIndex];
       } else {
	   DPRINTF((APWARN, "No such device\n"));
	   return GEN_ERROR;
       }
    } else {
       if(route_found){
	  rt_entry.rt_dev = g_rtentry.rt_dev;
       } else {
          /* default setting */
          rt_entry.rt_dev = devName[0];
       }
    }

    sin = (struct sockaddr_in *)&rt_entry.rt_dst;
    sin->sin_addr.s_addr = osapiHtonl(data->ipRouteDest);

    sin = (struct sockaddr_in *)&rt_entry.rt_gateway;
    sin->sin_addr.s_addr = osapiHtonl(data->ipRouteNextHop);

    if(VALID(I_ipRouteMask, data->valid)){
       if(route_found){
          sin = (struct sockaddr_in *)&rt_entry.rt_genmask;
          sin->sin_addr.s_addr = osapiHtonl(data->ipRouteMask);
       } else {
	  /* If it is a first time make mask to 0xffffffff */
	  /* so that you can come back later to change	   */

          sin = (struct sockaddr_in *)&rt_entry.rt_genmask;
          sin->sin_addr.s_addr = osapiHtonl(0xffffffff);
       }
    } else {
       /* make mask = 0xffffffff as default */

       sin = (struct sockaddr_in *)&rt_entry.rt_genmask;
       sin->sin_addr.s_addr = osapiHtonl(0xffffffff);
    }

    /* route add : SIOCADDRT,   route delete: SIOCDELRT */

    func = (state == SR_DELETE) ? SIOCDELRT : SIOCADDRT;

    /* route type: invalid, indirect, direct, other	*/

    if(VALID(I_ipRouteType, data->valid)){
       if(data->ipRouteType == D_ipRouteType_indirect){
          rt_entry.rt_flags |= RTF_GATEWAY;
       }
       if(data->ipRouteType == D_ipRouteType_invalid){
	  func = SIOCDELRT;
       }
    }

    /* Metric support. metric2, 3, 4, 5 are not supported */

    if(VALID(I_ipRouteMetric1, data->valid)){
       if(data->ipRouteMetric1 < 0){
	  rt_entry.rt_metric = 0;
       } else {
	  rt_entry.rt_metric = data->ipRouteMetric1;
       }
    }

    /* If gateway address is 0.0.0.0, don't set RTF_GATEWAY since 0.0.0.0 */
    /* is default gateway setting for host(direct)			  */

    if((rt_entry.rt_flags & RTF_GATEWAY) && 
       ((struct sockaddr_in *)&rt_entry.rt_gateway)->sin_addr.s_addr == 0) {

       rt_entry.rt_flags &= ~RTF_GATEWAY;
    }

    if(ioctl(SocketFd, func, &rt_entry) < 0){

       /* try to resume previous entry. we don't care the result */
       if(resume){
          ioctl(SocketFd, SIOCADDRT, &g_rtentry);
       }

       if(errno != EEXIST && func == SIOCADDRT){
	  DPRINTF((APWARN, "ioctl failed: Unable to add route\n"));
	  perror("SIOCADDRT");
	  return GEN_ERROR;
       }
       if(errno != ESRCH && func == SIOCDELRT){
	  DPRINTF((APWARN, "ioctl failed: Unable to delete route\n"));
	  perror("SIOCDELRT");
	  return GEN_ERROR;
       }
    }

    return NO_ERROR;
}

#else /* OLD_CODE */

ipRouteEntry_t *
k_ipRouteEntry_get(serialNum, contextInfo, nominator, searchType, ipRouteDest)
int serialNum;
ContextInfo *contextInfo;
int nominator;
int searchType;
SR_UINT32 ipRouteDest;
{
  FNAME("k_ipRouteEntry_get")
  static ipRouteEntry_t ipRouteEntryData;
  OID *ipRouteInfoOID;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    ipRouteEntryData.ipRouteInfo = MakeOID(NULL, 0);

    firstTime = L7_FALSE;
  }

  ZERO_VALID(ipRouteEntryData.valid);
  ipRouteEntryData.ipRouteDest = ipRouteDest;
  SET_VALID(I_ipRouteDest, ipRouteEntryData.valid);

  if ((searchType == EXACT) ?
      (usmDbIpRouteEntryGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest) != L7_SUCCESS) :
	  (usmDbIpRouteEntryGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest) != L7_SUCCESS) &&
       (usmDbIpRouteEntryGetNext(USMDB_UNIT_CURRENT, &ipRouteEntryData.ipRouteDest) != L7_SUCCESS))
  {
    ZERO_VALID(ipRouteEntryData.valid);
    return(NULL);
  }

  if (nominator == -1)
    return(&ipRouteEntryData);

  switch (nominator)
  {
  case I_ipRouteDest:
    break;

  case I_ipRouteIfIndex:
    if (usmDbIpRouteIfIndexGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest, 
                               &ipRouteEntryData.ipRouteIfIndex) == L7_SUCCESS &&
        usmDbExtIfNumFromIntIfNum(ipRouteEntryData.ipRouteIfIndex,
                                  &ipRouteEntryData.ipRouteIfIndex) == L7_SUCCESS)
      SET_VALID(I_ipRouteIfIndex, ipRouteEntryData.valid);
    break;

  case I_ipRouteMetric1:
    if (usmDbIpRouteMetric1Get(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                               &ipRouteEntryData.ipRouteMetric1) == L7_SUCCESS)
      SET_VALID(I_ipRouteMetric1, ipRouteEntryData.valid);
    break;

  case I_ipRouteMetric2:
    if (usmDbIpRouteMetric2Get(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                               &ipRouteEntryData.ipRouteMetric2) == L7_SUCCESS)
      SET_VALID(I_ipRouteMetric2, ipRouteEntryData.valid);
    break;

  case I_ipRouteMetric3:
    if (usmDbIpRouteMetric3Get(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                               &ipRouteEntryData.ipRouteMetric3) == L7_SUCCESS)
      SET_VALID(I_ipRouteMetric3, ipRouteEntryData.valid);
    break;

  case I_ipRouteMetric4:
    if (usmDbIpRouteMetric4Get(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                               &ipRouteEntryData.ipRouteMetric4) == L7_SUCCESS)
      SET_VALID(I_ipRouteMetric4, ipRouteEntryData.valid);
    break;

  case I_ipRouteNextHop:
    if (usmDbIpRouteNextHopGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                               &ipRouteEntryData.ipRouteNextHop) == L7_SUCCESS)
      SET_VALID(I_ipRouteNextHop, ipRouteEntryData.valid);
    break;

  case I_ipRouteType:
    if (usmDbIpRouteTypeGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                            &ipRouteEntryData.ipRouteType) == L7_SUCCESS)
      SET_VALID(I_ipRouteType, ipRouteEntryData.valid);
    break;

  case I_ipRouteProto:
    if (usmDbIpRouteProtoGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                             &ipRouteEntryData.ipRouteProto) == L7_SUCCESS)
      SET_VALID(I_ipRouteProto, ipRouteEntryData.valid);
    break;

  case I_ipRouteAge:
    if (usmDbIpRouteAgeGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                           &ipRouteEntryData.ipRouteAge) == L7_SUCCESS)
      SET_VALID(I_ipRouteAge, ipRouteEntryData.valid);
    break;

  case I_ipRouteMask:
    if (usmDbIpRouteMaskGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                            &ipRouteEntryData.ipRouteMask) == L7_SUCCESS)
      SET_VALID(I_ipRouteMask, ipRouteEntryData.valid);
    break;

  case I_ipRouteMetric5:
    if (usmDbIpRouteMetric5Get(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest,
                               &ipRouteEntryData.ipRouteMetric5) == L7_SUCCESS)
      SET_VALID(I_ipRouteMetric5, ipRouteEntryData.valid);
    break;

  case I_ipRouteInfo:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbIpRouteInfoGet(USMDB_UNIT_CURRENT, ipRouteEntryData.ipRouteDest, 
                            snmp_buffer) == L7_SUCCESS)
    {
      if ((ipRouteInfoOID = MakeOIDFromDot(snmp_buffer)) != NULL)
      {
        FreeOID(ipRouteEntryData.ipRouteInfo);
        ipRouteEntryData.ipRouteInfo = ipRouteInfoOID;
        SET_VALID(I_ipRouteInfo, ipRouteEntryData.valid);
      }
    }
    break;

  default:
    /* requested member not found */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, ipRouteEntryData.valid))
    return(NULL);

  return(&ipRouteEntryData);
}

int
k_ipRouteEntry_set(ipRouteEntry_t *data, ContextInfo *contextInfo, int function)
{
  FNAME("k_ipRouteEntry_set")
  L7_uint32 subnetMask = 0, nextHopRtr = 0,  cost = L7_IP_RTR_ROUTE_DEFAULT_COST;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 pref = FD_RTR_RTO_PREF_STATIC;
  L7_uint32 intIfNum = L7_INVALID_INTF, routeFlags = 0;

  /* If route doesn't exist, try to create it */
  if (usmDbIpRouteEntryGet(USMDB_UNIT_CURRENT, data->ipRouteDest) != L7_SUCCESS)
  {
    /* Don't allow deletion of a route that doesn't exist */
    if ( (VALID(I_ipRouteType, data->valid)) && (data->ipRouteType == D_ipRouteType_invalid) )
      return COMMIT_FAILED_ERROR;

    /* If dest network is zero, try to create a default route... next hop is required */
    if (data->ipRouteDest == 0)
    {
      if ( !(VALID(I_ipRouteNextHop, data->valid)) )
        return COMMIT_FAILED_ERROR;
      rc = usmDbIpStaticRouteAdd(USMDB_UNIT_CURRENT, 0, 0,
          data->ipRouteNextHop, pref, intIfNum, L7_RTF_DEFAULT_ROUTE);
    }
    else
    {
      /* Try to create a static route... next hop is required */
      if (VALID(I_ipRouteNextHop, data->valid))
        nextHopRtr = data->ipRouteNextHop;
      else
        return COMMIT_FAILED_ERROR;

      /* If subnet mask or cost (metric1) are valid, use them for route creation */
      if (VALID(I_ipRouteMask, data->valid))
        subnetMask = data->ipRouteMask;
      if ( (VALID(I_ipRouteMetric1, data->valid)) && (data->ipRouteMetric1 != -1) )
        cost = data->ipRouteMetric1;
      rc = usmDbIpStaticRouteAdd(USMDB_UNIT_CURRENT, data->ipRouteDest, 
                                 subnetMask, nextHopRtr, cost, intIfNum, 0);
    }

    /* Return from here, no matter what the result of the create was */
    /* L7_NOT_EXIST means the static route was added to the configuration, but 
     * it cannot be activated until a interface in the same subnet as the next
     * hop comes up.
     */
    if ((rc == L7_SUCCESS) || (rc == L7_NOT_EXIST))
      return NO_ERROR;
    else
      return COMMIT_FAILED_ERROR;
  }

  /* If we got here, the route exists.  The only action we currently support from here is to set
  ** the ipRouteType to invalid, which deletes the route.
  */
  if ( (VALID(I_ipRouteType, data->valid)) && (data->ipRouteType == D_ipRouteType_invalid) )
  {
    /* If dest network is zero, try to delete the default route... otherwise it's a static route */
    if (data->ipRouteDest == 0)
      rc = usmDbIpStaticRouteDelete(USMDB_UNIT_CURRENT, 0, 0, 0, intIfNum, 0);
    else
    {
      /* Get the mask and the next hop for this static route */
      if ( (usmDbIpRouteMaskGet(USMDB_UNIT_CURRENT, data->ipRouteDest,
                                &data->ipRouteMask) != L7_SUCCESS) ||
           (usmDbIpRouteNextHopGet(USMDB_UNIT_CURRENT, data->ipRouteDest,
                                   &data->ipRouteNextHop) != L7_SUCCESS) )
        return COMMIT_FAILED_ERROR;

      if (data->ipRouteNextHop == 0)
      {
        routeFlags = L7_RTF_REJECT;
      }
      rc = usmDbIpStaticRouteDelete(USMDB_UNIT_CURRENT, data->ipRouteDest,
                                    data->ipRouteMask, data->ipRouteNextHop,
                                    intIfNum, routeFlags);
    }

    if (rc == L7_SUCCESS)
      return NO_ERROR;
    else
      return COMMIT_FAILED_ERROR;
  }

  /* We currently only support creating and deleting static and default routes.  If we got here,
  ** it means the user is trying to modify an existing route, which is currently not supported.
  */
  return COMMIT_FAILED_ERROR;
}

#endif /* OLD_CODE */

int CmpTcpConnEntry(tcp1, tcp2)
    tcpConnEntry_t *tcp1;
    tcpConnEntry_t *tcp2;
{
    if (tcp1->tcpConnLocalAddress > tcp2->tcpConnLocalAddress) {
        return 1;
    } else if (tcp1->tcpConnLocalAddress < tcp2->tcpConnLocalAddress) {
        return -1;
    }

    if (tcp1->tcpConnLocalPort > tcp2->tcpConnLocalPort) {
        return 1;
    } else if (tcp1->tcpConnLocalPort < tcp2->tcpConnLocalPort) {
        return -1;
    }

    if (tcp1->tcpConnRemAddress > tcp2->tcpConnRemAddress) {
        return 1;
    } else if (tcp1->tcpConnRemAddress < tcp2->tcpConnRemAddress) {
        return -1;
    }

    if (tcp1->tcpConnRemPort > tcp2->tcpConnRemPort) {
        return 1;
    } else if (tcp1->tcpConnRemPort < tcp2->tcpConnRemPort) {
        return -1;
    }
    
    return 0;
}

static int map_states[] = { 1, 5, 3, 4, 7, 10, 11, 1, 6, 9, 2, 8 };

tcpConnEntry_t *
k_tcpConnEntry_get(id, contextInfo, nominator, searchType,
		   tcpConnLocalAddress, tcpConnLocalPort, 
                   tcpConnRemAddress, tcpConnRemPort)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    SR_UINT32 tcpConnLocalAddress;
    SR_INT32 tcpConnLocalPort;
    SR_UINT32 tcpConnRemAddress;
    SR_INT32 tcpConnRemPort;
{
    static tcpConnEntry_t  best;
    tcpConnEntry_t start, cur;
    int num;
    FILE *fp;
    int found = 0;

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    L7_uint32 source_addr[3];
    L7_uint32 dest_addr[3];

    fp = fopen ("/proc/net/tcp6", "r");
#else
    fp = fopen ("/proc/net/tcp", "r");
#endif

    if (fp == NULL) {
        return NULL;
    }

    start.tcpConnLocalAddress = tcpConnLocalAddress;
    start.tcpConnLocalPort = tcpConnLocalPort;
    start.tcpConnRemAddress = tcpConnRemAddress;
    start.tcpConnRemPort = tcpConnRemPort;

    best.tcpConnLocalAddress = (SR_UINT32) 0xffffffff;
    best.tcpConnLocalPort = (SR_INT32) 0x7fffffff;
    best.tcpConnRemAddress = (SR_UINT32) 0xffffffff;
    best.tcpConnRemPort = (SR_INT32) 0x7fffffff;
    
    while (line == fgets (line, MAX_LINE_LEN, fp)) {
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
        if (sscanf(line, "%d: %8lx%8lx%8lx%8lx:%lx %8lx%8lx%8lx%8lx:%lx %lx", &num,
                   &source_addr[0], &source_addr[1], &source_addr[2], 
                   &cur.tcpConnLocalAddress, &cur.tcpConnLocalPort, 
                   &dest_addr[0], &dest_addr[1], &dest_addr[2], 
                   &cur.tcpConnRemAddress, &cur.tcpConnRemPort, 
                   &cur.tcpConnState) == 12 &&
            source_addr[0] == 0 && source_addr[1] == 0 && (source_addr[2] == 0xFFFF || source_addr[2] == 0) &&
            dest_addr[0] == 0 && dest_addr[1] == 0 && (dest_addr[2] == 0xFFFF || dest_addr[2] == 0)) {

#else
        if (sscanf(line, "%d: %lx:%lx %lx:%lx %lx", &num,
                   &cur.tcpConnLocalAddress, &cur.tcpConnLocalPort, 
                   &cur.tcpConnRemAddress, &cur.tcpConnRemPort, 
                   &cur.tcpConnState) == 6) {
#endif
 
            cur.tcpConnLocalAddress = osapiHtonl(cur.tcpConnLocalAddress);
            cur.tcpConnRemAddress = osapiHtonl(cur.tcpConnRemAddress);

            if ((cur.tcpConnState >= 0) && (cur.tcpConnState < 12)) {
                cur.tcpConnState = map_states[cur.tcpConnState];
            } else {
                cur.tcpConnState = 1;
            }
            if (CmpTcpConnEntry(&cur, &start) >= 0) {
                if (CmpTcpConnEntry(&cur, &best) <= 0) {
                    memcpy(&best, &cur, sizeof(tcpConnEntry_t));
                    found++;
                }
            }
        }
    }

    fclose(fp);
    if (!found) {
        return NULL;
    }
    
    SET_ALL_VALID(best.valid);

    if (searchType == EXACT) {
        if (best.tcpConnLocalAddress != tcpConnLocalAddress) {
            return NULL;
        }
        if (best.tcpConnRemAddress != tcpConnRemAddress) {
            return NULL;
        }
        if (best.tcpConnLocalPort != tcpConnLocalPort) {
            return NULL;
        }
        if (best.tcpConnRemPort != tcpConnRemPort) {
            return NULL;
        }
    }
    return &best;
}

int k_tcpConnEntry_set(data, contextInfo, state)
    tcpConnEntry_t *data;
    ContextInfo    *contextInfo;
    int             state;
{
    /* does not handle tcpConnState here */ 
    return GEN_ERROR;
}



tcp_t          *
k_tcp_get(id, contextInfo, nominator)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
{
    static tcp_t tcpData;
    FILE *fp;
    int found = 0;

    fp = fopen ("/proc/net/snmp", "r");
    if (fp == NULL) {
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "Tcp:" INT32_FORMAT INT32_FORMAT INT32_FORMAT INT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT"\n",
            &tcpData.tcpRtoAlgorithm,
            &tcpData.tcpRtoMin,
            &tcpData.tcpRtoMax,
            &tcpData.tcpMaxConn,
            &tcpData.tcpActiveOpens,
            &tcpData.tcpPassiveOpens,
            &tcpData.tcpAttemptFails,
            &tcpData.tcpEstabResets,
            &tcpData.tcpCurrEstab,
            &tcpData.tcpInSegs,
            &tcpData.tcpOutSegs,
            &tcpData.tcpRetransSegs,
	    &tcpData.tcpInErrs,
	    &tcpData.tcpOutRsts) == 14) {

            found++;
            SET_ALL_VALID(tcpData.valid);
            break;
        }
    }
    

#if 0
    if (tcpData.tcpRtoAlgorithm < 1 || tcpData.tcpRtoAlgorithm > 4) {
        tcpData.tcpRtoAlgorithm = D_tcpRtoAlgorithm_other;
    }
#endif

    tcpData.tcpRtoAlgorithm = (SR_INT32) D_tcpRtoAlgorithm_vanj;
    tcpData.tcpRtoMin = (SR_INT32) 200;
    tcpData.tcpRtoMax = (SR_INT32) 120000;
    tcpData.tcpMaxConn = (SR_INT32) FD_SETSIZE;

    fclose(fp);
    if (!found) {
        return NULL;
    }
    return &tcpData;
}

int CmpUdpEntry(udp1, udp2)
    udpEntry_t *udp1;
    udpEntry_t *udp2;
{
    if (udp1->udpLocalAddress > udp2->udpLocalAddress) {
        return 1;
    } else if (udp1->udpLocalAddress < udp2->udpLocalAddress) {
        return -1;
    }

    if (udp1->udpLocalPort > udp2->udpLocalPort) {
        return 1;
    } else if (udp1->udpLocalPort < udp2->udpLocalPort) {
        return -1;
    }

    return 0;
}

udpEntry_t     *
k_udpEntry_get(id, contextInfo, nominator, searchType, laddr, lport)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    SR_UINT32       laddr;
    SR_INT32        lport;
{
    static udpEntry_t best;
    udpEntry_t start, cur;
    int num; 
    FILE *fp;
    int found = 0;

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    L7_uint32 source_addr[3];

    fp = fopen ("/proc/net/udp6", "r");
#else
    fp = fopen ("/proc/net/udp", "r");
#endif

    if (fp == NULL) {
        return NULL;
    }

    start.udpLocalAddress = laddr;
    start.udpLocalPort = lport;

    best.udpLocalAddress = (SR_UINT32) 0xffffffff;
    best.udpLocalPort = (SR_INT32) 0x7fffffff;

    while (line == fgets (line, MAX_LINE_LEN, fp)) {
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
        if (sscanf(line, "%d: %8lx%8lx%8lx%8lx:%lx", &num,
                   &source_addr[0], &source_addr[1], &source_addr[2], 
                   &cur.udpLocalAddress, &cur.udpLocalPort) == 6 &&
            source_addr[0] == 0 && source_addr[1] == 0 && (source_addr[2] == 0xFFFF || source_addr[2] == 0)) {
#else
        if (sscanf(line, "%d: %lx:%lx", &num,
                  &cur.udpLocalAddress, &cur.udpLocalPort) == 3) {
#endif
            cur.udpLocalAddress = osapiHtonl(cur.udpLocalAddress);

            if (CmpUdpEntry(&cur, &start) >= 0) {
                if (CmpUdpEntry(&cur, &best) <= 0) {
                    memcpy(&best, &cur, sizeof(udpEntry_t));
                    found++;
                }
            }
        }
    }

    fclose(fp);
    if (!found) {
        return NULL;
    }

    SET_ALL_VALID(best.valid);
    if (searchType == EXACT) {
        if (best.udpLocalAddress != laddr) {
            return NULL;
        }
        if (best.udpLocalPort != lport) {
            return NULL;
        }
    }

    return &best;
}



udp_t          *
k_udp_get(id, contextInfo, nominator)
    int             id;
    ContextInfo    *contextInfo;
    int             nominator;
{
    static udp_t udpData;
    FILE *fp;
    int found = 0;

    fp = fopen ("/proc/net/snmp", "r");
    if (fp == NULL) {
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "Udp:" UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT UINT32_FORMAT"\n",
            &udpData.udpInDatagrams,
            &udpData.udpNoPorts,
            &udpData.udpInErrors,
            &udpData.udpOutDatagrams) == 4) {

            found++;
            SET_ALL_VALID(udpData.valid);
            break;
        }
    }

    fclose(fp);
    if (!found) {
        return NULL;
    }
    return &udpData;
}


