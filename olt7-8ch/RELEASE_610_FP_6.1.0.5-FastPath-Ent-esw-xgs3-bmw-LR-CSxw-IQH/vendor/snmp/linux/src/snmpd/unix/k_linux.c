/*
 *
 * Copyright (C) 1996-2006 by SNMP Research, Incorporated.
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

/*
 * Added cache for TCP Conn Table contents
 * Started by FAM, finished by JDC
 *
 */

/*
 * Arguments used to create the cache fragment of this file:
 * -agent -c mib2 +tcpConnTable -search_table (implies -row_status) 
 */

/* LVL7 */
#ifdef SNMP_SWITCHING
  #ifndef SNMP_ROUTING
    #include <k_linux_switching.c>
  #endif
#endif

#ifdef SNMP_ROUTING
  #include <k_linux_routing.c>
#endif

#ifdef OLD_CODE
/* LVL7 */

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

#include "tmq.h"

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
#include <sys/utsname.h>


#include <unistd.h> 
#include <linux/sockios.h> 
#include <arpa/inet.h> 
#include <net/route.h>
#include <errno.h> 
#include <linux/version.h>


#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "min_v.h"


#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif /* KERNEL_VERSION */

#ifndef ATF_PERM
#define ATF_PERM 0x04
#endif /* ATF_PERM */

#ifndef ATF_COM
#define ATF_COM 0x02
#endif /* ATF_COM */


#define MAX_LINE_LEN 512
#define MAX_DEV_NUM 20
#define DEV_NAME_LEN 32
#define KERNEL20 20  /* kernel 2.0.x */
#define KERNEL22 22  /* kernel 2.2.x */
#define KERNEL24 24  /* kernel 2.4.x */
#define KERNEL26 26  /* kernel 2.6.x */

extern SR_INT32 tcp_cache_interval;
int updateTcpConnCache(SnmpV2Table *tcpt, int id);

static char line[MAX_LINE_LEN];

static int SocketFd = -1;
static int route_found = 0;
int ifIndex;

#ifdef POLL_INTERFACES
extern int connected;
#endif /* POLL_INTERFACES */

static const SR_UINT32 ifIndex_sids[] =
    { 1, 3, 6, 1, 2, 1, 2, 2, 1, 1 };
static const OID ifIndexOID = {
    OIDSIZE(ifIndex_sids),
    (SR_UINT32 *) ifIndex_sids
};

static const SR_UINT32 ifAdminStatus_sids[] =
    { 1, 3, 6, 1, 2, 1, 2, 2, 1, 7 };
static const OID ifAdminStatusOID = {
    OIDSIZE(ifAdminStatus_sids),
    (SR_UINT32 *) ifAdminStatus_sids
};

static const SR_UINT32 ifOperStatus_sids[] =
    { 1, 3, 6, 1, 2, 1, 2, 2, 1, 8 };
static const OID ifOperStatusOID = {
    OIDSIZE(ifOperStatus_sids),
    (SR_UINT32 *) ifOperStatus_sids
};

VarBind *createLinkVarBindList(SR_INT32 index);

struct rtentry g_rtentry;	/* hold routing table entry info */

static char devName[MAX_DEV_NUM][DEV_NAME_LEN];  /* dev name and index */

extern OID      nullOID;

static int k_version = KERNEL24;  /* default kernel version */
#define D_ifType_tunnel 131

/* global data describing the tcpConnTable entries */
Index_t tcpConnEntryIndex[] = {
    { offsetof(tcpConnEntry_t, tcpConnLocalAddress),  T_uint},
    { offsetof(tcpConnEntry_t, tcpConnLocalPort),  T_uint},
    { offsetof(tcpConnEntry_t, tcpConnRemAddress),  T_uint},
    { offsetof(tcpConnEntry_t, tcpConnRemPort),  T_uint}
};

SnmpV2Table tcpConnTable = {
    NULL,
    tcpConnEntryIndex,
    0,
    4,
    sizeof(tcpConnEntry_t)
};

typedef struct _tcpConnTable_Cache_Info_t {
    SR_INT32 serial_number;
    SR_INT32 cache_time;
    SR_INT32 dirty_flag;
} tcpConnTable_Cache_Info_t;

static tcpConnTable_Cache_Info_t tcpConnTable_Cache_Info;

tcpConnEntry_t *
getTcpEntryFromCache(
      int             id,
      ContextInfo     *contextInfo,
      int             nominator,
      int             searchType,
      SR_UINT32       tcpConnLocalAddress,
      SR_INT32        tcpConnLocalPort,
      SR_UINT32       tcpConnRemAddress,
      SR_INT32        tcpConnRemPort);

extern system_t systemData;

void poll_interfaces(void);
static int do_ifinfo(void);

static int      ifinfo_valid    = 0;    /* Initially not valid.         */
static int      interface_count  = 0;    /* Initially no interfaces.     */
static int      interface_array_size  = 10;  

#define IfInfoValid             (ifinfo_valid ? 1 : do_ifinfo())

typedef struct _ifinfoType {
    struct ifreq    ifreq;
    struct ifnet   *ifstats;    /* in BSD, kernel address of struct ifnet */

    int             ifIndex;
    int             ifType;
    int             ifSpeed;
    SR_UINT32   ifLastChange;   /* modify on timed poll */
    short           ifFlags;
    int             hasPhysAddr;
    unsigned char   ifPhysAddr[PHYS_ADDR_SIZE];
} ifinfoType;

static ifEntry_t       *ifinfo = NULL;


int 
do_ifinfo(void)
{
    unsigned long in_packets, out_packets; 
    unsigned long in_fifo, in_frame, in_compressed;
    unsigned long out_fifo, out_colls;
    unsigned long out_carrier, out_compressed;
    unsigned long long tmp_ifInOctets=0, tmp_ifOutOctets=0; 
    int prev_interface_count, prevOperStatus;
    static char ifname[32];
    FILE *fp;              
    char *ptr;             
    struct ifreq ifrq;
    VarBind *linkvb = NULL;

    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        DPRINTF((APWARN, "do_ifinfo: Cannot open /proc/net/dev. %s\n",
                                                        sys_errname(errno)));
        return 0;
    }

    prev_interface_count = interface_count;
    interface_count = 0;

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if ((ptr = strrchr(line, ':')) != NULL) {
            *ptr = ' ';
        }
        switch(k_version) {
            case KERNEL20:
                if (sscanf(line,"%s %lu %u %u %lu %lu %lu \
                                 %u %u %lu %lu %lu ",
                           ifname,
                           &in_packets,
                           &ifinfo[interface_count].ifInErrors,
                           &ifinfo[interface_count].ifInDiscards,
                           &in_fifo,
                           &in_frame,
                           &out_packets,
                           &ifinfo[interface_count].ifOutErrors,
                           &ifinfo[interface_count].ifOutDiscards,
                           &out_fifo,
                           &out_colls,
                           &out_carrier) == 12) {
                    ifinfo[interface_count].ifIndex = interface_count+1;
                    interface_count++;
                } else {
                    continue;
                } 
                break;
            case KERNEL22:
            case KERNEL24:
            case KERNEL26:

                if (sscanf(line,"%s %Lu %lu %u %u %lu %lu %lu \
                                 %lu %Lu %lu %u %u %lu %lu %lu %lu",
                           ifname,
                           &tmp_ifInOctets,
                           // &ifinfo[interface_count].ifInOctets,
                           &in_packets,
                           &ifinfo[interface_count].ifInErrors,
                           &ifinfo[interface_count].ifInDiscards,
                           &in_fifo,
                           &in_frame,
                           &in_compressed,
                           (long unsigned int *) &ifinfo[interface_count].ifInNUcastPkts,
                           //&ifinfo[interface_count].ifOutOctets,
                           &tmp_ifOutOctets,
                           &out_packets,
                           &ifinfo[interface_count].ifOutErrors,
                           &ifinfo[interface_count].ifOutDiscards,
                           &out_fifo,
                           &out_colls,
                           &out_carrier,
                           &out_compressed) == 17) {
                /* If the value is great than 32-bit, can be on 64-bit systems *
                 * then wrap value around */
                     ifinfo[interface_count].ifInOctets = \
                                      tmp_ifInOctets % 4294967295U;
                     ifinfo[interface_count].ifInNUcastPkts = \
                                      tmp_ifOutOctets % 4294967295U;
                    ifinfo[interface_count].ifInUcastPkts =
                            in_packets - ifinfo[interface_count].ifInNUcastPkts;
                    ifinfo[interface_count].ifIndex = interface_count+1;
                    interface_count++;
                } else {
                    continue;
                } 
                break;
            default:
                DPRINTF((APWARN,"do_ifinfo: unsupported kernel version %d\n", k_version));
                return 0;
        }

        if (interface_count >= interface_array_size) {
            ifEntry_t *new_ifinfo;
            int i;
            int offset = interface_array_size * sizeof(ifEntry_t); 
            interface_array_size += 10;
            new_ifinfo = (ifEntry_t *)realloc((void *)ifinfo, 
                                (sizeof(ifEntry_t) * interface_array_size));
            if (new_ifinfo == NULL) {
                DPRINTF((APWARN,"do_ifinfo: malloc failed\n"));
                return 0;
            }
            ifinfo = new_ifinfo;
            memset((char *)(ifinfo + offset), 0,
		   (interface_array_size * sizeof(ifEntry_t)) - offset);
            for( i = interface_count; i < interface_array_size; i++ ) {
              /* Set pointers to NULL in newly added memory */
              ifinfo[i].ifDescr = NULL;
              ifinfo[i].ifPhysAddress = NULL;
              ifinfo[i].ifSpecific = NULL;
            }
        }
        if (ifinfo[interface_count-1].ifDescr != NULL) {
            FreeOctetString(ifinfo[interface_count-1].ifDescr);
        } 
        ifinfo[interface_count-1].ifDescr = MakeOctetString(ifname, 
                                                            strlen(ifname));
        /* 
         * verify memory frees later?
         * ifEntryData.ifDescr->octet_ptr = ifname;
         * ifEntryData.ifDescr->length = strlen(ifname);
         */

        ifinfo[interface_count-1].ifSpecific = &nullOID;

        strcpy (ifrq.ifr_name, ifname);
        if (ioctl(SocketFd, SIOCGIFHWADDR, &ifrq) < 0) {
            DPRINTF((APWARN, "do_ifinfo: ioctl failed\n"));
            return 0;
        } else {
            if (ifinfo[interface_count-1].ifPhysAddress != NULL) {
                FreeOctetString(ifinfo[interface_count-1].ifPhysAddress);
            }
            ifinfo[interface_count-1].ifPhysAddress = MakeOctetString(NULL, 
                                                               PHYS_ADDR_SIZE);
            memcpy(ifinfo[interface_count-1].ifPhysAddress->octet_ptr, 
                   ifrq.ifr_hwaddr.sa_data, 6); 
        }

        if (strncmp(ifname, "lo", 2) == 0) {
            ifinfo[interface_count-1].ifType = D_ifType_softwareLoopback;
            ifinfo[interface_count-1].ifSpeed = 0;
        } else if (strncmp(ifname, "eth", 3) == 0) {
            ifinfo[interface_count-1].ifType = D_ifType_ethernet_csmacd;
            ifinfo[interface_count-1].ifSpeed = 10000000;
        } else if (strncmp(ifname, "ppp", 3) == 0) {
            ifinfo[interface_count-1].ifType = D_ifType_ppp;
            ifinfo[interface_count-1].ifSpeed = 0;
        } else if (strncmp(ifname, "sl", 2) == 0) {
            ifinfo[interface_count-1].ifType = D_ifType_slip;
            ifinfo[interface_count-1].ifSpeed = 0;
        }else if (strncmp(ifname, "sit", 3) == 0) {
            ifinfo[interface_count-1].ifType = D_ifType_tunnel;
            ifinfo[interface_count-1].ifSpeed = 0;
        } else {
            ifinfo[interface_count-1].ifType = D_ifType_other;
            ifinfo[interface_count-1].ifSpeed = 0;
        }
    
        strcpy (ifrq.ifr_name, ifname);
        if (ioctl(SocketFd, SIOCGIFMTU, &ifrq) < 0) {
            DPRINTF((APWARN, "do_ifinfo: ioctl failed\n"));
            return 0;
        } else {
                ifinfo[interface_count-1].ifMtu = ifrq.ifr_mtu;
        }
    
        strcpy (ifrq.ifr_name, ifname);
        if (ioctl(SocketFd, SIOCGIFMTU, &ifrq) < 0) {
            DPRINTF((APWARN, "do_ifinfo: ioctl failed\n"));
            return 0;
        } else {
                ifinfo[interface_count-1].ifMtu = ifrq.ifr_mtu;
        }
    
        strcpy (ifrq.ifr_name, ifname);
        if (ioctl(SocketFd, SIOCGIFFLAGS, &ifrq) < 0) {
            DPRINTF((APWARN, "do_ifinfo: ioctl failed\n"));
            return 0;
        } else {
            prevOperStatus = -1;
            if (interface_count <= prev_interface_count) {
                /* interface previously polled, check for status change */
                prevOperStatus = ifinfo[interface_count-1].ifOperStatus;
            }

            /* ifinfo[interface_count-1].ifAdminStatus = ifrq.ifr_flags & IFF_RUNNING ? 1 : 2; */
            ifinfo[interface_count-1].ifAdminStatus = ifrq.ifr_flags & IFF_UP ? 1 : 2; 
            ifinfo[interface_count-1].ifOperStatus = (ifrq.ifr_flags & IFF_UP) && (ifrq.ifr_flags & IFF_RUNNING) ? 1 : 2;
#ifdef POLL_INTERFACES
             if ( connected ) {
#endif /* POLL_INTERFACES */

            if ( prevOperStatus != -1  && 
                 (prevOperStatus != ifinfo[interface_count-1].ifOperStatus)) {

                linkvb = createLinkVarBindList(interface_count-1);

                switch (ifinfo[interface_count-1].ifOperStatus) {
                    case D_ifOperStatus_up:
                        DPRINTF((APTRACE, "Sending linkUpTrap\n"));
                        do_trap(LINK_UP_TRAP, (SR_INT32) 0, (VarBind *) linkvb,
                                systemData.sysObjectID,
                                (char *) NULL);
                        break;
                    case D_ifOperStatus_down:
                        DPRINTF((APTRACE, "Sending linkDownTrap\n"));
                        do_trap(LINK_DOWN_TRAP, (SR_INT32) 0, (VarBind *) linkvb,
                                systemData.sysObjectID,
                                (char *) NULL);
                        break;
                }
            } 
#ifdef POLL_INTERFACES
          }  /* connected */
#endif /* POLL_INTERFACES */
        }
    
        ifinfo[interface_count-1].ifLastChange = 0;
    
    }
    fclose(fp);
   
    ifinfo_valid = 1;
    return 1;
}    


void
poll_interfaces(void)
{
   (void) do_ifinfo();
}



void check_version(void)
{
    unsigned int version;
    int a, b, c, ret;
    struct utsname buf;

    ret = uname(&buf);
    if (ret == -1) {
        DPRINTF((APERROR, "Can not get OS version\n"));
        return;
    }

    sscanf(buf.release, "%d.%d.%d", &a, &b, &c);
    version = KERNEL_VERSION(a, b, c);
    if (version < KERNEL_VERSION(2, 2, 0)) {
        k_version = KERNEL20;
    }
    else if (version < KERNEL_VERSION(2, 4, 0)) {
        k_version = KERNEL22;
    }
    else if (version < KERNEL_VERSION(2, 6, 0)) {
        k_version = KERNEL24;
    }
    else {
        k_version = KERNEL26;
    }
}

    
int open_socket(void)
{
    FNAME("open_socket");
    /* Open socket for ioctl call */

    if((SocketFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
       DPRINTF((APERROR, "%s: can not create socket\n", Fname));
       return -1;
    }
    return 1;
}

int
k_initialize(void)
{
    int ret_val;

    check_version();
    ret_val = open_socket();
    if(ret_val!=1){
       goto fail;
    }


        
    ifinfo = (ifEntry_t *)malloc(interface_array_size*sizeof(ifEntry_t));
    memset((char *)ifinfo, 0, interface_array_size*sizeof(ifEntry_t));
    if (ifinfo == NULL) {
        goto fail;
    }

#ifdef POLL_INTERFACES
    /* set timer for polling interval in centiseconds */
    SetPeriodicTimeout(TimerCurTime()+(POLL_INTERVAL*100), 
                       POLL_INTERVAL*100, NULL);
#endif /* POLL_INTERFACES */

    return 1;

 fail:
    (void) k_terminate();
    return -1;

}

int k_terminate(void)
{
    if (SocketFd != -1) {
        close(SocketFd);
        SocketFd = -1;
    }
    return 0;
}


int update_devName(void)
{
    static char ifbuff[32];
    int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p; /* dummy */
    int count, num;
    char *ptr;
    FILE *fp;

    /* Get device information from /proc/net/dev */

    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        DPRINTF((APWARN, "update_devName: Cannot open /proc/net/dev. %s\n",
                                                       sys_errname(errno)));
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
        switch(k_version) {
            case KERNEL20:
                if (sscanf(line, "%s %d %d %d %d %d %d %d %d %d %d %d ",
                                  ifbuff, &a, &b, &c, &d, &e, &f, &g, &h, &i, 
                                  &j, &k) == 12) {
                    count++;
                    memcpy(&devName[count], ifbuff, strlen(ifbuff));
                }
                break;
            case KERNEL22:
            case KERNEL24:
            case KERNEL26:
                if (sscanf(line, "%s %d %d %d %d %d %d %d \
                                  %d %d %d %d %d %d %d %d %d ",
                                  ifbuff, &a, &b, &c, &d, &e, &f, &g, &h,
                                  &i, &j, &k, &l, &m, &n, &o, &p) == 17) {
                    count++;
                    memcpy(&devName[count], ifbuff, strlen(ifbuff));
                }
                break;
            default:
                break;
        }
    }
    fclose(fp);
    strcpy(devName[0],"eth0");
    return 1;
}

int
get_ifIndex(char *ifname)
{
    static char ifbuff[32];
    int ifIndex = -1;
    int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p; /* dummy */
    int count, found;
    char *ptr;
    FILE *fp;

    /* Get device information from /proc/net/dev */

    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        DPRINTF((APWARN, "get_ifIndex: Cannot open /proc/net/dev. %s\n",
                                                        sys_errname(errno)));
        return -1;
    }
    count = 0;
    found = 0;

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if ((ptr = strrchr(line, ':')) != NULL) {
            *ptr = ' ';
        }
        switch(k_version) {
            case KERNEL20:
                if (sscanf(line, "%s %d %d %d %d %d %d %d \
                                  %d %d %d %d ",
                                  ifbuff, &a, &b, &c, &d, &e, &f, &g, &h,
                                  &i, &j, &k) == 12) {
                    count++;
                    if (strncmp(ifbuff, ifname, strlen(ifname)) == 0) {
                        ifIndex = count;
                        found = 1;
                    }
                }
                break;
            case KERNEL22:
            case KERNEL24:
            case KERNEL26:
                if (sscanf(line, "%s %d %d %d %d %d %d %d \
                                  %d %d %d %d %d %d %d %d %d ",
                                  ifbuff, &a, &b, &c, &d, &e, &f, &g, &h,
                                  &i, &j, &k, &l, &m, &n, &o, &p) == 17) {
                    count++;
                    if (strncmp(ifbuff, ifname, strlen(ifname)) == 0) {
                        ifIndex = count;
                        found = 1;
                    }
                }
                break;
            default:
                break;
        }
        if (found) {
            break;
        }
    }
    fclose(fp);

    if (ifIndex == -1) {
        DPRINTF((APWARN, "get_ifIndex failed\n"));
    }
    return ifIndex;
}

int
CmpIpNetToMediaEntry(ipNetToMediaEntry_t *arp1, ipNetToMediaEntry_t *arp2)
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

ipNetToMediaEntry_t *
k_ipNetToMediaEntry_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    SR_INT32        ifIndex,
    SR_UINT32       netaddr) 
{
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
        if (sscanf(line, "%d.%d.%d.%d %x %x %x:%x:%x:%x:%x:%x",
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
}


int
k_ipNetToMediaEntry_set(
    ipNetToMediaEntry_t *data,
    ContextInfo    *contextInfo,
    int             state)
{
    struct arpreq arp;			/* new arprep  structure */
    struct sockaddr_in *netaddr;	/* network address pointer */
    struct sockaddr *physaddr;		/* hardware address pointer */
    int func;
    int exist;

    memset(&arp, 0, sizeof(arp));

    netaddr = (struct sockaddr_in *) &arp.arp_pa;
    physaddr = &arp.arp_ha;

    netaddr->sin_family = AF_INET;
    physaddr->sa_family = AF_UNSPEC;

    /* The deveice for linux is always "eth0" 		*/
    /* If additional device setting will be added later	*/
    memcpy(&arp.arp_dev, "eth0", 4);

    arp.arp_flags = ATF_COM;    /* It is OK for retrieve arp cache */
    
    /* Check existing arp entry using ioctl */

    netaddr->sin_addr.s_addr = htonl(data->ipNetToMediaNetAddress);

    /* ioctl won't return an entry if the flag is set to ATF_PUBL */

    if((exist = ioctl(SocketFd, SIOCGARP, (char *)&arp)) < 0){
       DPRINTF((APWARN, "SIOCGRAP: ioctl failed. No entry in arp cache\n"));
    } 

    /* Prepare for setiing arp cache. the flag should not be 0x00, */
    /* so change the flag to ATF_COM(0x02) for normal request	   */

    if(arp.arp_flags == 0x00){
       arp.arp_flags |= ATF_COM;
    }

    if(VALID(I_ipNetToMediaNetAddress, data->valid)) {
       netaddr->sin_addr.s_addr = htonl(data->ipNetToMediaNetAddress);
    } else if (state != SR_DELETE) {
       return GEN_ERROR;
    } else {
       netaddr->sin_addr.s_addr = htonl(data->ipNetToMediaNetAddress);
       func = SIOCDARP;
    }

    if(VALID(I_ipNetToMediaPhysAddress, data->valid)) {
       memcpy((char *)physaddr->sa_data,
	      (char *)data->ipNetToMediaPhysAddress->octet_ptr, 6);
    } else if (state != SR_DELETE) {
       return GEN_ERROR;
    } else {
       func = SIOCDARP;
    }

    /* test again if it is a undo routine */

    func = (state == SR_DELETE) ? SIOCDARP : SIOCSARP;

    /* You only can specify ATF_PERM for your request    		*/
    /* adding/deleting  ATF_PUBL type is not supported by mib2agt	*/    

    if(VALID(I_ipNetToMediaType, data->valid)) {
       switch (data->ipNetToMediaType) {
          case D_ipNetToMediaType_invalid:
	       func = SIOCDARP;
	       break;
          case D_ipNetToMediaType_dynamic:
	       arp.arp_flags &= ~ATF_PERM;
	       break;
          case D_ipNetToMediaType_static:
	       arp.arp_flags |=ATF_PERM;
	       break;
	  default:
	       break;
       }
    }
    
    /* Type change permitted		*/
    /* invalid -> dynamic : OK		*/
    /* invalid -> static  : OK		*/
    /* dynamic -> static  : OK		*/
    /* dynamic -> invalid : OK		*/
    /* static  -> invalid : OK		*/
    /* static  -> dynamic : NO		*/
    /* dynamic -> other   : NO		*/
    /* static  -> other   : NO		*/

    if(!(exist && func == SIOCDARP)){ /* It does nothing now. future purpose */
       if(ioctl(SocketFd, func, &arp) < 0) {
          if(func == SIOCDARP){
             DPRINTF((APWARN, " SIOCDARP: ioctl failed.\n"));
          }else{
             DPRINTF((APWARN, " SIOCSARP: ioctl failed.\n"));
          }
          return GEN_ERROR;
       }
    }else{
       DPRINTF((APWARN, "SIOCDRAP: No such entry n arp cache to delete\n"));
    }
    return NO_ERROR;
}


icmp_t         *
k_icmp_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator)
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
        if (sscanf(line, "Icmp: %u %u %u %u %u %u %u %u %u %u %u %u \
                          %u %u %u %u %u %u %u %u %u %u %u %u %u %u \n",
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


interfaces_t   *
k_interfaces_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator)
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
        switch(k_version) {
            case KERNEL20:
                if ((cc = sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu \
                               %lu %lu %lu %lu ",
                               buff, &a, &b, &c, &d, &e, &f, &g, &h,
                               &i, &j, &k)) == 12) {
                    ifData.ifNumber++;
                }
                break;
            case KERNEL22:
            case KERNEL24:
            case KERNEL26:
                if ((cc = sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu \
                               %lu %lu %lu %lu %lu %lu %lu %lu %lu ",
                               buff, &a, &b, &c, &d, &e, &f, &g, &h,
                               &i, &j, &k, &l, &m, &n, &o, &p)) == 17) {
                    ifData.ifNumber++;
                }
                break;
            default:
                break;
        }
    }

    fclose(fp);
    return &ifData;
}


ifEntry_t      *
k_ifEntry_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    SR_INT32        ifIndex)
{
    static ifEntry_t ifEntryData;
    int ifnum;
  
    if (!IfInfoValid) {
        DPRINTF((APWARN, "Cannot get interface count.\n"));
        return NULL;
    }

    ifnum = ifIndex - 1;   
    if (searchType == NEXT) {
        if(ifnum < 0) ifnum = 0;
    }

    if ((ifnum < 0) || (ifnum >= interface_count)) {
        return NULL;
    }          

    ifEntryData = ifinfo[ifnum];

    SET_ALL_VALID(ifEntryData.valid);
    return(&ifEntryData);

#ifdef NOT_YET
    ifEntryData.ifInUnknownProtos
    ifEntryData.ifOutUcastPkts
    ifEntryData.ifOutNUcastPkts
    ifEntryData.ifOutQLen
#endif
}


int
k_ifEntry_set(
    ifEntry_t      *data,
    ContextInfo    *contextInfo,
    int             state)
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
 
    /* invalidate ifinfo cache */   
    ifinfo_valid = 0;

    return NO_ERROR;
}


#include "k_ipforw.c"


ip_t           *
k_ip_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static ip_t ipData;
    FILE *fp;
    int found = 0;

    fp = fopen ("/proc/net/snmp", "r");
    if (fp == NULL) {
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "Ip: %u %u %u %u %u %u %u %u %u %u \
                              %u %u %u %u %u %u %u %u %u\n",
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
}


int
k_ip_set(
    ip_t           *data,
    ContextInfo    *contextInfo,
    int             state)
{
   unsigned char ipforw;
   unsigned char ipTTL[20];
   int length;
   FILE *fp;

    /* you can update ip_forwarding from /proc/sys/net/ipv4/ip_forward */
    /* SUPPORT_IP_FORWARDING should be defined to enable ip_forwarding */
    
    if(VALID(I_ipForwarding, data->valid)){
       switch (data->ipForwarding) {
	  case D_ipForwarding_forwarding:
	       ipforw = (unsigned char) '1';
	       break;
	  case D_ipForwarding_not_forwarding:
	       ipforw = (unsigned char) '0';
	       break;
	  default:
	       return GEN_ERROR; /* should not happen */
       }
       fp = fopen ("/proc/sys/net/ipv4/ip_forward", "r+");
       if(fp == NULL){
	  DPRINTF((APWARN, "k_ip_set: Can not open /proc/sys/net/ipv4/ip_forward\n"));
	  return GEN_ERROR;
       }
       /* update ip_forwarding parameter */
       if(fwrite(&ipforw, sizeof(unsigned char), 1, fp) < 0){
	  DPRINTF((APWARN, "k_ip_set(ip_forward) fwrite failed\n"));
          fclose(fp);
	  return GEN_ERROR;
       }
       fclose(fp);
    }

    /* you can update ip_default_ttl from /proc/sys/net/ipv4/ip_default_ttl */

    if(VALID(I_ipDefaultTTL, data->valid)){
       if(data->ipDefaultTTL < 1){
	  return GEN_ERROR;
       }
       memset(&ipTTL, 0, sizeof(ipTTL));
       sprintf(ipTTL, "%d", data->ipDefaultTTL);

       length = strlen(ipTTL);
       if(length < 1){
	  return GEN_ERROR;
       }

       fp = fopen ("/proc/sys/net/ipv4/ip_default_ttl", "r+");
       if(fp == NULL){
	  DPRINTF((APWARN, "k_ip_set: Can not open /proc/sys/net/ipv4/ip_default_ttl\n"));
	  return GEN_ERROR;
       }
       /* update ip_default_ttl parameter */
       if(fwrite(&ipTTL, sizeof(unsigned char), length, fp) < 0){
	  DPRINTF((APWARN, "k_ip_set(ip_default_ttl) fwrite failed\n"));
          fclose(fp);
	  return GEN_ERROR;
       }
       fclose(fp);
    }
    return NO_ERROR;
}




ipAddrEntry_t  *
k_ipAddrEntry_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    SR_UINT32       netaddr)
{
    static ipAddrEntry_t ipAddrEntryData;
    SR_UINT32 cur_addr = 0, best_addr = 0xffffffff;
    FILE *fp;
    int found = 0;
    unsigned long a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p; /* dummy */
    struct ifreq ifrq;
    char ifname[32];
    char *ptr;

    fp = fopen ("/proc/net/dev", "r");
    if (fp == NULL) {
        DPRINTF((APWARN, "k_ifEntry_get: Cannot open /proc/net/dev\n"));
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if ((ptr = strrchr(line, ':')) != NULL) {
            *ptr = ' ';
        }
        if (sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu \
				%lu %lu %lu %lu %lu %lu %lu %lu %lu ",
                         ifname, &a, &b, &c, &d, &e, &f, &g, &h,
			 &i, &j, &k, &l, &m, &n, &o, &p) == 17) {

            strcpy (ifrq.ifr_name, ifname);
            if (ioctl(SocketFd, SIOCGIFFLAGS, &ifrq) < 0) {
                DPRINTF((APTRACE, "k_ipAddrEntry_get: ioctl failed\n"));
                continue;
            } else {
                if (!(ifrq.ifr_flags & IFF_UP)) {
                    continue;
                }
            }

            strcpy (ifrq.ifr_name, ifname);
            if (ioctl (SocketFd, SIOCGIFADDR, &ifrq) < 0) {
                DPRINTF((APTRACE, "k_ipAddrEntry_get: ioctl failed\n"));
                continue;
            } else {
               cur_addr = ((struct sockaddr_in *) &(ifrq.ifr_addr))->sin_addr.s_addr;
               cur_addr = htonl(cur_addr);
            }

            if ((cur_addr >= netaddr) && (cur_addr <= best_addr)) {
                best_addr = cur_addr;
                ipAddrEntryData.ipAdEntIfIndex = get_ifIndex(ifname);
                found++;
            } else {
                continue;
            }

            strcpy (ifrq.ifr_name, ifname);
            if (ioctl (SocketFd, SIOCGIFNETMASK, &ifrq) < 0) {
                DPRINTF((APWARN, "k_ipAddrEntry_get: ioctl failed\n"));
                fclose(fp);
                return NULL;
            } else {
               ipAddrEntryData.ipAdEntNetMask = htonl(((struct sockaddr_in *) &(ifrq.ifr_netmask))->sin_addr.s_addr);
            }

            strcpy (ifrq.ifr_name, ifname);
            if (ioctl (SocketFd, SIOCGIFBRDADDR, &ifrq) < 0) {
                DPRINTF((APWARN, "k_ipAddrEntry_get: ioctl failed\n"));
                fclose(fp);
                return NULL;
            } else {
               ipAddrEntryData.ipAdEntBcastAddr = htonl(((struct sockaddr_in *) &(ifrq.ifr_broadaddr))->sin_addr.s_addr) & 1;
            }
        }
    }
    fclose(fp);
    if (!found) { 
        return NULL;
    }
    
    if ((searchType == EXACT) && (best_addr != netaddr)) {
        return NULL;
    }
    ipAddrEntryData.ipAdEntAddr = best_addr;
    if (ipAddrEntryData.ipAdEntIfIndex < 0) {
        return NULL;
    }
    return &ipAddrEntryData;

#ifdef NOT_YET
   ipAddrEntryData.ipAdEntReasmMaxSize = ;
#endif /* NOT_YET */
}


int
CmpIpRouteEntry(ipRouteEntry_t *ip1, ipRouteEntry_t *ip2)
{
    if (ip1->ipRouteDest > ip2->ipRouteDest) {
        return 1;
    } else if (ip1->ipRouteDest < ip2->ipRouteDest) {
        return -1;
    }

    return 0;
}


ipRouteEntry_t *
k_ipRouteEntry_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    SR_UINT32       ipRouteDest)
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
        if (sscanf(line, "%s %x %x %d %d %d %d %x %d",
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
               cur.ipRouteDest = htonl(cur.ipRouteDest);
               cur.ipRouteNextHop = htonl(cur.ipRouteNextHop);
               cur.ipRouteMask = htonl(cur.ipRouteMask);


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
	              sin->sin_addr.s_addr = htonl(cur.ipRouteDest);

	              sin = (struct sockaddr_in *)&g_rtentry.rt_gateway;
	              sin->sin_family = AF_INET;
	              sin->sin_addr.s_addr = htonl(cur.ipRouteNextHop);

	              sin = (struct sockaddr_in *)&g_rtentry.rt_genmask;
	              sin->sin_family = AF_INET;
	              sin->sin_addr.s_addr = htonl(cur.ipRouteMask);

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


int
k_ipRouteEntry_set(
    ipRouteEntry_t *data,
    ContextInfo    *contextInfo,
    int             state)
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
	     DPRINTF((APWARN, 
                     "(SIOCDELRT) ioctl failed. can not remove entry. %s\n",
                                                        sys_errname(errno)));
          }
	  resume = 1;
    } else if(route_found && data->ipRouteType == D_ipRouteType_invalid) {
          if(ioctl(SocketFd, SIOCDELRT, &g_rtentry) < 0){
	     DPRINTF((APWARN, 
                     "(SIOCDELRT) ioctl failed. can not remove entry. %s\n",
                                                        sys_errname(errno)));
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
    sin->sin_addr.s_addr = htonl(data->ipRouteDest);

    sin = (struct sockaddr_in *)&rt_entry.rt_gateway;
    sin->sin_addr.s_addr = htonl(data->ipRouteNextHop);

    if(VALID(I_ipRouteMask, data->valid)){
       if(route_found){
          sin = (struct sockaddr_in *)&rt_entry.rt_genmask;
          sin->sin_addr.s_addr = htonl(data->ipRouteMask);
       } else {
	  /* If it is a first time make mask to 0xffffffff */
	  /* so that you can come back later to change	   */

          sin = (struct sockaddr_in *)&rt_entry.rt_genmask;
          sin->sin_addr.s_addr = 0xffffffff;
       }
    } else {
       /* make mask = 0xffffffff as default */

       sin = (struct sockaddr_in *)&rt_entry.rt_genmask;
       sin->sin_addr.s_addr = 0xffffffff;
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
	  DPRINTF((APWARN, "ioctl failed: Unable to add route. %s\n",
                                                         sys_errname(errno)));
	  return GEN_ERROR;
       }
       if(errno != ESRCH && func == SIOCDELRT){
	  DPRINTF((APWARN, "ioctl failed: Unable to delete route. %s\n",
                                                         sys_errname(errno)));
	  return GEN_ERROR;
       }
    }

    return NO_ERROR;
}


int
CmpTcpConnEntry(tcpConnEntry_t *tcp1, tcpConnEntry_t *tcp2)
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



tcpConnEntry_t *
k_tcpConnEntry_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    SR_UINT32 tcpConnLocalAddress,
    SR_INT32 tcpConnLocalPort,
    SR_UINT32 tcpConnRemAddress,
    SR_INT32 tcpConnRemPort)
{

    tcpConnEntry_t *cache_entry;

/* fprintf(stderr, "in k_tcpConnEntry_get(), serial_num = %d\n", id); */

/* fprintf(stderr, "calling getTcpEntryFromCache in k_tcpConnEntry_get(), serial_num = %d\n", id); */

    cache_entry = getTcpEntryFromCache(id, contextInfo, nominator, searchType,
                   tcpConnLocalAddress, tcpConnLocalPort,
                   tcpConnRemAddress, tcpConnRemPort);

/* fprintf(stderr, "back from calling getTcpEntryFromCache in k_tcpConnEntry_get(), serial_num = %d\n", id); */

    return cache_entry;

}


tcpConnEntry_t *
getTcpEntryFromCache(id, contextInfo, nominator, searchType,
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

    int index;

    SR_UINT32 now;

    FNAME("getTcpEntryFromCache");

    /* determine if we need to update the cache */

    /* cache must be updated if dirty */
    if ( tcpConnTable_Cache_Info.dirty_flag == TRUE) {
        DPRINTF((APTRACE, "%s: Cache is dirty and being updated\n", Fname, id));
        updateTcpConnCache(&tcpConnTable, id);
    }
    else { /* cache is not dirty and might be ok */
      /* if cache has already been checked or updated on this packet or
       * cache is recent enough, do not refresh
       * i.e., update if cache is not from this packet and stale */

      /* cache from earlier in this same packet? */
      if ( tcpConnTable_Cache_Info.serial_number == id ) {
        DPRINTF((APTRACE, "%s: Same packet (id=%d) -- Cache does not need to be updated\n", Fname, id));
      }
      else { /* not from this packet -- still need to update if it is stale */
      now = GetTimeNow();
      DPRINTF((APTRACE, "\nId Now: %d, Id Cache: %d, Time Now: %d, Time Cache: %d, Time Diff: %d, Time Threshold: %d, Conclusion: %d\n",
          id,
          tcpConnTable_Cache_Info.serial_number,
          now,
          tcpConnTable_Cache_Info.cache_time,
          now - tcpConnTable_Cache_Info.cache_time,
          tcp_cache_interval,
          (now - tcpConnTable_Cache_Info.cache_time) > tcp_cache_interval));

      if ( (now - tcpConnTable_Cache_Info.cache_time) > tcp_cache_interval) {
              DPRINTF((APTRACE, "%s: Not same packet (id=%d) and cache is stale so cache needs to be updated\n", Fname, id));
              updateTcpConnCache(&tcpConnTable, id);
      }
      else { /* not too old */
              DPRINTF((APTRACE, "%s: Cache does not need to be updated (id=%d)\n", Fname, id));
              tcpConnTable_Cache_Info.serial_number = id;
        } /* not too old */
      } /* not from this packet */
    } /* not dirty */


    /* DPRINTF((APTRACE, "%s: Getting TcpConnTable informationfrom cache\n", Fname)); */

    tcpConnTable.tip[0].value.uint_val = tcpConnLocalAddress;
    tcpConnTable.tip[1].value.uint_val = tcpConnLocalPort;
    tcpConnTable.tip[2].value.uint_val = tcpConnRemAddress;
    tcpConnTable.tip[3].value.uint_val = tcpConnRemPort;

    if ((index = SearchTable(&tcpConnTable, searchType)) == -1) {
        return NULL;
    }

    return (tcpConnEntry_t *) tcpConnTable.tp[index];

}

int
updateTcpConnCache(SnmpV2Table *tcpt, int id)
{
    tcpConnEntry_t cur;
    tcpConnEntry_t *new;
    char line[MAX_LINE_LEN];
    int num;
    FILE *fp;

    int index;

    FNAME("updateTcpConnCache");

    /* DPRINTF((APTRACE, "%s: Update tcpConnTable\n", Fname)); */


    /* DPRINTF((APTRACE, "%s: Purging old entries = %d\n", Fname, tcpt->nitems)); */
    /* purge the old entries */
    while (tcpt->nitems > 0) {
        free(tcpt->tp[0]);
        RemoveTableEntry(tcpt, 0);
    }


    /* DPRINTF((APTRACE, "%s: Opening /proc/net/tcp\n", Fname)); */
    fp = fopen ("/proc/net/tcp", "r");
    if (fp == NULL) {
        return -1;
    }

    while (line == fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "%d: %lx:%lx %lx:%lx %lx", &num,
                   (long unsigned int *) &cur.tcpConnLocalAddress, 
                   (long unsigned int *) &cur.tcpConnLocalPort,
                   (long unsigned int *) &cur.tcpConnRemAddress, 
                   (long unsigned int *) &cur.tcpConnRemPort,
                   (long unsigned int *) &cur.tcpConnState) == 6) {

            cur.tcpConnLocalAddress = htonl(cur.tcpConnLocalAddress);
            cur.tcpConnRemAddress = htonl(cur.tcpConnRemAddress);

            tcpConnTable.tip[0].value.uint_val = cur.tcpConnLocalAddress;
            tcpConnTable.tip[1].value.uint_val = cur.tcpConnLocalPort;
            tcpConnTable.tip[2].value.uint_val = cur.tcpConnRemAddress;
            tcpConnTable.tip[3].value.uint_val = cur.tcpConnRemPort;

            index = NewTableEntry(&tcpConnTable);

             if (index == -1) {
               DPRINTF((APERROR,
                   "%s: Cannot create new row in tcpConnTable cache\n",Fname));
               goto done;
             }
    

             new = (tcpConnEntry_t *) tcpConnTable.tp[index];
     
             switch(cur.tcpConnState) { 
                case TCP_ESTABLISHED:
                   new->tcpConnState = 5;   
                   break;
                case TCP_SYN_SENT:
                   new->tcpConnState = 3;   
                   break;
                case TCP_SYN_RECV:
                   new->tcpConnState = 4;   
                   break;
                case TCP_FIN_WAIT1:
                   new->tcpConnState = 6;   
                   break;
                case TCP_FIN_WAIT2:
                   new->tcpConnState = 7;   
                   break;
                case TCP_TIME_WAIT:
                   new->tcpConnState = 11;   
                   break;
                case TCP_CLOSE:
                   new->tcpConnState = 1;   
                   break;
                case TCP_CLOSE_WAIT:
                   new->tcpConnState = 8;   
                   break;
                case TCP_LAST_ACK:
                   new->tcpConnState = 9;   
                   break;
                case TCP_LISTEN:
                   new->tcpConnState = 2;   
                   break;
                case TCP_CLOSING:
                   new->tcpConnState = 10;   
                       /* now a valid state */
                default:
                   break;
             }
        }
    }

done:
    /* label the cache */
    tcpConnTable_Cache_Info.serial_number = id;
    tcpConnTable_Cache_Info.cache_time = GetTimeNow();
    tcpConnTable_Cache_Info.dirty_flag = FALSE;
    /* DPRINTF((APTRACE, "%s: Created %d entries in cache\n", Fname, tcpt->nitems)); */

    fclose(fp);

    return 0;
}

int
markTcpConnCacheAsDirty(void)
{
    DPRINTF((APTRACE, "in markTcpConnCacheAsDirty()\n"));
    tcpConnTable_Cache_Info.serial_number = 0;
    tcpConnTable_Cache_Info.cache_time = 0;
    tcpConnTable_Cache_Info.dirty_flag = TRUE;
    return 0;
}


/* initialize support for tcpConnEntry objects */
int
tcpConnEntry_initialize(void)
{
    tcpConnTable.tp = (void **) malloc (sizeof(tcpConnEntry_t *));
    if (tcpConnTable.tp == NULL) {
        return 0;
    }
    tcpConnTable.tip = tcpConnEntryIndex;
    tcpConnTable.nitems = 0;

    markTcpConnCacheAsDirty();


    return 1;
}

int
k_tcpConnEntry_set(
    tcpConnEntry_t *data,
    ContextInfo    *contextInfo,
    int             state)
{
    /* does not handle tcpConnState here */ 
    return GEN_ERROR;
}



tcp_t          *
k_tcp_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static tcp_t tcpData;
    FILE *fp;
    int found = 0;

    fp = fopen ("/proc/net/snmp", "r");
    if (fp == NULL) {
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        switch(k_version) {
            case KERNEL20:
                if (sscanf(line, 
                    "Tcp: %d %d %d %d %u %u %u %u %u %u %u %u\n",
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
                    &tcpData.tcpRetransSegs) == 12) {

                    found++;
                    SET_ALL_VALID(tcpData.valid);
                }
                break;
            case KERNEL22:
            case KERNEL24:
            case KERNEL26:
                if (sscanf(line, "Tcp: %d %d %d %d %u %u %u %u %u %u %u %u %u %u\n",
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
                }
                break;
            default:
                break;
        }
        if (found) {
            break;
        }
    }
    if (tcpData.tcpRtoAlgorithm < 1 || tcpData.tcpRtoAlgorithm > 4) {
        tcpData.tcpRtoAlgorithm = D_tcpRtoAlgorithm_other;
    }

    fclose(fp);
    if (!found) {
        return NULL;
    }
    return &tcpData;
}


int
CmpUdpEntry(udpEntry_t *udp1, udpEntry_t *udp2)
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
k_udpEntry_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    SR_UINT32       laddr,
    SR_INT32        lport)
{
    static udpEntry_t best;
    udpEntry_t start, cur;
    int num; 
    FILE *fp;
    int found = 0;

    fp = fopen ("/proc/net/udp", "r");
    if (fp == NULL) {
        return NULL;
    }

    start.udpLocalAddress = laddr;
    start.udpLocalPort = lport;

    best.udpLocalAddress = (SR_UINT32) 0xffffffff;
    best.udpLocalPort = (SR_INT32) 0x7fffffff;

    while (line == fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "%d: %x:%x", &num,
                  &cur.udpLocalAddress, &cur.udpLocalPort) == 3) {

            cur.udpLocalAddress = htonl(cur.udpLocalAddress);

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
k_udp_get(
    int             id,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static udp_t udpData;
    FILE *fp;
    int found = 0;

    fp = fopen ("/proc/net/snmp", "r");
    if (fp == NULL) {
        return NULL;
    }

    while (fgets (line, MAX_LINE_LEN, fp)) {
        if (sscanf(line, "Udp: %u %u %u %u\n",
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


VarBind *
createLinkVarBindList(SR_INT32 index)
{

    VarBind *vb1 = NULL;
    VarBind *vb2 = NULL;
    VarBind *vb3 = NULL;
    OID *inst = NULL;

    inst = MakeOIDFragFromDot("0");
    inst->oid_ptr[0] = ifinfo[index].ifIndex;
    vb1 = MakeVarBindWithValue(&ifIndexOID,
                               (OID *)inst,
                               INTEGER_TYPE,
                               &ifinfo[index].ifIndex);

    if (vb1 == NULL) { goto fail; }

    vb2 = MakeVarBindWithValue(&ifAdminStatusOID,
                               (OID *)inst,
                               INTEGER_TYPE,
                               &ifinfo[index].ifAdminStatus);

    if (vb2 == NULL) { goto fail; }

    vb3 = MakeVarBindWithValue(&ifOperStatusOID,
                               (OID *)inst,
                               INTEGER_TYPE,
                               &ifinfo[index].ifOperStatus);

    if (vb2 == NULL) { goto fail; }

    vb1->next_var = vb2;
    vb2->next_var = vb3;
    vb3->next_var = NULL;

    if(inst){
      FreeOID(inst);
    }

    return vb1;

fail:
   if (vb1) { FreeVarBind(vb1); }
   if (vb2) { FreeVarBind(vb1); }
   if (vb3) { FreeVarBind(vb1); }

   return NULL;
}

#endif /* OLD_CODE LVL7 */
