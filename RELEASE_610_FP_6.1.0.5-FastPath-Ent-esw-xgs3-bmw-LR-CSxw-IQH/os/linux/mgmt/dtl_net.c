#ifdef _L7_OS_LINUX_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_net.c
*
* @purpose DTL interface driver for the Linux stack.
*
* @component Device Transformation Layer
*
* @comments none
*
* @create 11/21/2001
*
* @author John Linville
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#define DTL_USE_TAP

  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <sys/socket.h>
  #include <net/if.h>
  #include <net/if_arp.h>
  #include <netinet/if_ether.h>
  #include <netinet/ip.h>
  #include <sys/ioctl.h>
  #include <string.h>
  #include <linux/if_tun.h>
  #include <dtl_net.h>
  #include <ptin_translate_api.h>
#ifdef DTL_USE_TAP
  #include <dtl_tap_monitor.h>
#endif
  #include "errno.h"

  #include "avl_api.h"
  #include "log.h"
  #include "osapi_support.h"
  #include "sysapi.h"
  #include "dtlapi.h"
  #include "fdb_api.h"
  #include "simapi.h"
  #include "sysnet_api_ipv4.h"
 #include "osapi_priv.h"
  #include "sim_debug_api.h"
  #include "ipv6_commdefs.h"


#ifdef DTL_USE_TAP
void dtlSendCmd(int fd, L7_uint32 intIfNum, L7_netBufHandle handle, tapDtlInfo *info);
#endif

/* need init function to open sockets for NET driver */
/* need to start a task to read from socket and write to DTL */

  #define L7_MAX_DTL_SEND_ENTRIES 512
  #define DTL_MIN_PDU_SIZE        0x3C


typedef struct dtlSendData_s
{

  L7_uchar8               dtlSend_macAddr[L7_MAC_ADDR_LEN];
  L7_uint32               dtlSend_intIfNum;
  struct dtlSendData_s *next;

} dtlSendData_t;

typedef struct dtl_stats_s
{

  L7_uint32 dup_adds;            /* additions of existing entries */
  L7_uint32 bad_adds;            /* failure to allocate space for add */
  L7_uint32 chg_adds;            /* update intIfNum for existing MAC addr */
  L7_uint32 bad_dels;            /* attempts to delete non-existant entries */
  L7_uint32 cur_entries;         /* current entries in table */

} dtl_stats_t;

static avlTreeTables_t   dtlSendTreeHeap[L7_MAX_DTL_SEND_ENTRIES];
static dtlSendData_t     dtlSendDataHeap[L7_MAX_DTL_SEND_ENTRIES];
static avlTree_t         dtlSendTreeData;
static dtl_stats_t       dtlStats;

extern L7_BOOL dtlNetInitDone;

#ifdef DTL_USE_TAP
#define TAP_DRV_NAME "/dev/tap"
#define TUN_DRV_NAME "/dev/net/tun"
int dtl_net_fd;
#endif

void dtlDeleteAll()
{
  L7_uint32 i;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  dtlSendData_t *pData;

  memset(mac, 0x00, L7_MAC_ADDR_LEN);

  osapiSemaTake(dtlSendTreeData.semId, L7_WAIT_FOREVER);

  for ( i = 0 ; i < L7_MAX_DTL_SEND_ENTRIES; i++ )
  {

    pData = avlSearchLVL7 (&dtlSendTreeData, mac, L7_MATCH_GETNEXT);

    if ( pData == L7_NULL )
    {
      LOG_MSG ("DTL avl mac delete ALL error!\n");
      memset(&dtlStats, 0x00, sizeof(dtl_stats_t));
      break;
    }

    memcpy(mac, pData->dtlSend_macAddr, L7_MAC_ADDR_LEN);

    pData = avlDeleteEntry(&dtlSendTreeData, pData);

    if ( !pData )
      dtlStats.bad_dels++;
    else
      dtlStats.cur_entries--;
  }

  if ( dtlStats.cur_entries )
  {
    LOG_MSG ("DTL avl mac delete error!\n");
  }

  osapiSemaGive(dtlSendTreeData.semId);
}

void dtlMacToPortShow()
{
  L7_uint32 i;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  dtlSendData_t *pData;

  memset(mac, 0x00, L7_MAC_ADDR_LEN);

  printf("\nMAC Address           Port");

  osapiSemaTake(dtlSendTreeData.semId, L7_WAIT_FOREVER);

  for ( i = 0 ; i < L7_MAX_DTL_SEND_ENTRIES; i++ )
  {
    pData = avlSearchLVL7 (&dtlSendTreeData, mac, L7_MATCH_GETNEXT);

    if ( pData == L7_NULL )
    {
      break;
    }
    if (pData->dtlSend_intIfNum)
    {
      memcpy(mac, pData->dtlSend_macAddr, L7_MAC_ADDR_LEN);
      printf("\n%02X:%02X:%02X:%02X:%02X:%02X     ",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      nimGetIntfName(pData->dtlSend_intIfNum, L7_SYSNAME, ifName);
      printf(ifName);
    }
  }

  osapiSemaGive(dtlSendTreeData.semId);
}

void dtlInsert ( char *mac, L7_uint32 intIfNum )
{
  dtlSendData_t data;
  dtlSendData_t *pData;

  if ( dtlStats.cur_entries == L7_MAX_DTL_SEND_ENTRIES )
  {
    dtlDeleteAll();
    osapiArpFlush(L7_NULLPTR);
  }

  memcpy(data.dtlSend_macAddr, mac, L7_MAC_ADDR_LEN);
  data.dtlSend_intIfNum = intIfNum;

  osapiSemaTake(dtlSendTreeData.semId, L7_WAIT_FOREVER);

  pData = avlInsertEntry(&dtlSendTreeData, &data);

  if ( pData == L7_NULL )
  {
    dtlStats.cur_entries++;
  }
  else if (!memcmp(pData->dtlSend_macAddr, data.dtlSend_macAddr, L7_MAC_ADDR_LEN) &&
           (pData->dtlSend_intIfNum == data.dtlSend_intIfNum))
    /* existing entry is exact match */
    dtlStats.dup_adds++;

  else if ( pData )
  {
    /* update intIfNum for existing MAC addr */
    pData->dtlSend_intIfNum = intIfNum;
    dtlStats.chg_adds++;
  }

  osapiSemaGive(dtlSendTreeData.semId);
}

L7_RC_t dtlFind(char *mac, L7_uint32 *intIfNum)
{
  dtlSendData_t *pData;

  osapiSemaTake(dtlSendTreeData.semId, L7_WAIT_FOREVER);

  pData = avlSearchLVL7 (&dtlSendTreeData, mac, L7_MATCH_EXACT);

  if ( pData != L7_NULL )
    *intIfNum = pData->dtlSend_intIfNum;

  osapiSemaGive(dtlSendTreeData.semId);

  if ( pData == L7_NULL )
  {
    return(L7_FAILURE);
  }
  return(L7_SUCCESS);
}

/* test code */
void dumpdtlStats()
{
  printf("cur entries  - %10d\n",dtlStats.cur_entries);
  printf("dup adds     - %10d\n",dtlStats.dup_adds);
  printf("chg adds     - %10d\n",dtlStats.chg_adds);
  printf("bad adds     - %10d\n",dtlStats.bad_adds);
  printf("bad dels     - %10d\n",dtlStats.bad_dels);
}

void dtlGlobalInit (void)
{

  avlCreateAvlTree(&dtlSendTreeData, dtlSendTreeHeap, dtlSendDataHeap,
                   L7_MAX_DTL_SEND_ENTRIES, sizeof(dtlSendData_t), 0x10,
                   sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);

  memset(&dtlStats, 0x00, sizeof(dtl_stats_t));

#ifdef DTL_USE_TAP
   tap_monitor_init(1);
#endif
} /* dtlGlobalInit */

/********************************************************************************
* dtlIPProtoRecv - function to process IP packets from the MUX
* For switching with ipv6 mgmt tagged vlan packets have to be parsed by this
* function. For routing v6 the vlan tag is removed  before giving
* to this function
*
*
* RETURNS: L7_SUCCESS  - Frame is consumed by the END driver.
* RETURNS: L7_FAILURE  - Frame has not been consumed by the end driver.
*/

L7_RC_t dtlIPProtoRecv (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{

  L7_uint32 nbytes;
  register L7_ipHeader_t *ip_header;
  register L7_ip6Header_t *ip6_header;
  char *data;
  L7_uint32 headerOffset;
  L7_uint32 intIfNum;
#ifdef DTL_USE_TAP
  L7_ushort16 vlan_id;
  L7_ushort16 pktEtherType;
  L7_ushort16 ether_type;
  unsigned char tmp_buf[12];
#endif
  L7_BOOL is_local = L7_FALSE;
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS*2];
  L7_uint32 i = 0, acount = 0;

  intIfNum = pduInfo->intIfNum;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, nbytes);

  headerOffset = sysNetDataOffsetGet(data);
  memcpy((L7_char8 *)&pktEtherType, (data + headerOffset - 2), sizeof(L7_ushort16));
  ether_type = osapiNtohs(pktEtherType);

  if (ether_type == L7_ETYPE_IPV6)
  {
    ip6_header = (L7_ip6Header_t *)(data + headerOffset);

    /* Check to see if it's for the system mgmt interface */
    if (L7_IP6_IS_ADDR_LINK_LOCAL(ip6_header->dst))
      is_local = L7_TRUE;
    else if (L7_IP6_IS_ADDR_MULTICAST_LOC_SCOPE(ip6_header->dst))
      is_local = L7_TRUE;
    else
    {
      /* Need to check for Network Port IPv6 global addresses here too. */
      acount = L7_RTR6_MAX_INTF_ADDRS*2;   /* allow for both static and dynamic addresses */
      if (simGetSystemIPV6Addrs( ipv6Prefixes, &acount) == L7_SUCCESS)
      {
        for (i = 0; i < acount; i++)
        {
          L7_in6_addr_t in6Addr;
          memset (&in6Addr, 0, sizeof (L7_in6_addr_t));
          memcpy (&in6Addr, ip6_header->dst, sizeof (L7_in6_addr_t));
          if (L7_IP6_IS_ADDR_EQUAL(&ipv6Prefixes[i].in6Addr, &in6Addr))
          {
            is_local = L7_TRUE;
            break;
          }
        }
      }
    }

    if (is_local)
    {
      /* Always update the physical interface for a MAC addr in the network port fdb. */
      dtlInsert ( data + 6, intIfNum );

      /* Call interceptors who are interested in IPv6 frames to be
      ** delivered to the local IP stack.  If L7_TRUE is returned,
      ** the frame was either discarded or consumed, which means that
      ** the network buffer has been freed by the intercept call, or
      ** will be freed by the consumer.
      */
      if (SYSNET_PDU_INTERCEPT(L7_AF_INET6, SYSNET_INET_LOCAL_IN, bufHandle,
                               pduInfo, L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
      {
        return L7_SUCCESS;
      }
    }
  }
  else
  {
    ip_header = (L7_ipHeader_t *)(data + headerOffset);

    if(ip_header->iph_prot == IP_PROT_ICMP)
            pingDebugPacketRxTrace(intIfNum,(char *)ip_header);

    /* Check to see if it's for me */
    if ((ip_header->iph_dst == osapiHtonl(simGetSystemIPAddr())) /* unicast */
      || (ip_header->iph_dst == osapiHtonl(0xffffffff))        /* broadcast */
      || (ip_header->iph_dst == (osapiHtonl(simGetSystemIPAddr()) | ~(osapiHtonl(simGetSystemIPNetMask()))))) /* directed broadcast */
    {
      is_local = L7_TRUE;
      dtlInsert ( data + 6, intIfNum );

      /* Call interceptors who are interested in IP frames to be delivered to the local IP stack.
      ** If L7_TRUE is returned, the frame was either discarded or consumed, which means that the
      ** network buffer has been freed by the intercept call, or will be freed by the consumer.
      */
      if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_LOCAL_IN, bufHandle, pduInfo,
                             L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
        return L7_SUCCESS;
    }
  }


#ifdef DTL_USE_TAP
  if(is_local)
  {
    /* added here to check for tagged VLan packet */
    memcpy(&pktEtherType, data + L7_ENET_HDR_SIZE, sizeof(L7_ushort16));
    ether_type = osapiNtohs(pktEtherType);

    if (ether_type == L7_ETYPE_8021Q || ether_type == FD_DVLANTAG_802_1Q_ETHERTYPE )
    {
       /*make sure its the default vlan*/
       memcpy(&vlan_id, &data[14], sizeof(vlan_id));
       vlan_id = osapiNtohs(vlan_id);
       vlan_id &= L7_VLAN_TAG_VLAN_ID_MASK;
       if(vlan_id != simMgmtVlanIdGet())
       {
          return L7_FAILURE;
       }
       memcpy(tmp_buf,data,12);
       memcpy(data+4,tmp_buf,12);
       data += 4;
       nbytes -= 4;
    }
    write(dtl_net_fd,data,nbytes);
  }
#endif

  /* Update MIB2 statistics. */ /*???*/

  SYSAPI_NET_MBUF_FREE (bufHandle);

  return(L7_SUCCESS);

}

/********************************************************************************
* dtlARPProtoRecv - function to process ARP packets from the MUX
*
*
* RETURNS: L7_SUCCESS
*/

L7_RC_t dtlARPProtoRecv(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{

  L7_uint32 nbytes;
  register L7_ether_arp_t *arp_header;
  L7_uint32 ip_address;
  int op;
  char *data;
  L7_uint32 headerOffset;
  L7_uint32 intIfNum;
#ifdef DTL_USE_TAP
  L7_ushort16 ether_type, vlan_id;
  unsigned char tmp_buf[12];
#endif

  intIfNum = pduInfo->intIfNum;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, nbytes);

  headerOffset = sysNetDataOffsetGet(data);
  arp_header = (L7_ether_arp_t *)(data + headerOffset);
  op = osapiNtohs (arp_header->ea_hdr.ar_op);

  /* Only create a new entry if one doesn't exist */
  /* AND it's destined for me. Don't worry about  */
  /* freeing the mBlk, the ARP code will do it.   */
  /* ALWAYS set the port number!.                 */

  bcopy (arp_header->arp_tpa, (char *) &ip_address, 4);

  ip_address = osapiNtohl(ip_address);
  if (ip_address == simGetSystemIPAddr())
  {
    dtlInsert ( arp_header->arp_sha, intIfNum );
  }
  else
  {
    /* But it's a grat ARP on the mgmt subnet, update the mgmt fdb for the source
       * and pass to IP stack to update its ARP cache. */
      L7_uint32 spa, tpa;
      memcpy(&spa, arp_header->arp_spa, sizeof(L7_uint32));
      memcpy(&tpa, arp_header->arp_tpa, sizeof(L7_uint32));

      if ( (spa == tpa) &&
           ((osapiNtohl(tpa) & simGetSystemIPNetMask()) ==
            (simGetSystemIPAddr() & simGetSystemIPNetMask())) )
      {
        dtlInsert ( arp_header->arp_sha, intIfNum );
      }
      else
      {
        /* drop packet */
        return L7_FAILURE;
      }
  }

#ifdef DTL_USE_TAP
    memcpy(&ether_type, &data[12], sizeof(ether_type));
    ether_type = osapiNtohs(ether_type);

    if (ether_type == L7_ETYPE_8021Q || ether_type == FD_DVLANTAG_802_1Q_ETHERTYPE  )
    {
       /*make sure its the default vlan*/
       memcpy(&vlan_id, &data[14], sizeof(vlan_id));
       vlan_id = osapiNtohs(vlan_id);
       vlan_id &= L7_VLAN_TAG_VLAN_ID_MASK;
       if(vlan_id != simMgmtVlanIdGet())
       {
          return L7_FAILURE;
       }
       memcpy(tmp_buf,data,12);
       memcpy(data+4,tmp_buf,12);
       data += 4;
       nbytes -= 4;
    }
   write(dtl_net_fd,data,nbytes);
#endif

  /* Update MIB2 statistics. */ /*???*/


  SYSAPI_NET_MBUF_FREE (bufHandle);

  return L7_SUCCESS;

} /* dtlARPProtoRecv */

/**************************************************************************
* @purpose  Changes the system network mac address between Burned-In or
*           Locally Administered Mac Address
*
* @param    newMac  L7_uchar8 pointer to a mac address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
L7_int32 dtlFdbMacAddrChange( L7_uchar8 *newMac )
{
  int    sock;
  struct ifreq ifr;
  L7_short16 flags;
  L7_uint32 vlanId;

  if((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    return L7_FAILURE;
  }

  memset(&ifr, 0x00, sizeof (ifr));
  sprintf (ifr.ifr_name, "%s%d", L7_DTL_PORT_IF, 0);

  /* Get the current flags */
  if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)
  {
    LOG_ERROR(errno);
  }

  flags = ifr.ifr_flags;

  /* If the interface is not already down, then bring it down */
  if ((ifr.ifr_flags & IFF_UP) != 0)
  {
    ifr.ifr_flags &= ~IFF_UP;

    if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1)
    {
      LOG_ERROR(errno);
    }
  }

  /* Set the system network MAC address */
  ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
  memcpy(&ifr.ifr_hwaddr.sa_data, newMac, L7_MAC_ADDR_LEN);

  if (ioctl(sock, SIOCSIFHWADDR, &ifr) == -1)
  {
    LOG_ERROR(errno);
  }

  /* Set the old flags back */
  ifr.ifr_flags = flags;

  if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1)
  {
      LOG_ERROR(errno);
  }

  /*
  Add the address to the search machines
  */

  vlanId = simMgmtVlanIdGet();
  fdbSysMacAddEntry(newMac, vlanId, 1, L7_FDB_ADDR_FLAG_MANAGEMENT);

  close(sock);
  return(L7_SUCCESS);
}


/**************************************************************************
* @purpose  Initialize DTL interface.
*
* @param    none
*
* @returns   none
*
* @comments    none.
*
* @end
*************************************************************************/
void dtlNetInit(void)
{

  L7_uchar8 empty[6], mac[6];
  L7_uint32 ipaddr, netmask, gateway, vlanId;
#ifdef DTL_USE_TAP
  struct ifreq ifr;
#endif
  unsigned long queue_len;
  char queue_len_buf[11];

  dtlGlobalInit();

  memset(empty, 0x00, L7_MAC_ADDR_LEN);

#ifdef DTL_USE_TAP
  /*
   *if we are using the tap driver here
   *for the dtl interface then we need to
   *a file descriptor to it and set up
   *the corresponding network interface
   */
  dtl_net_fd = open(TAP_DRV_NAME,O_RDWR);
  if(dtl_net_fd < 0)
  {
     /*SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Unable to open %s\n",TAP_DRV_NAME);*/
     dtl_net_fd = open(TUN_DRV_NAME,O_RDWR);
     if(dtl_net_fd < 0)
     {
       SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Unable to open %s\n",TUN_DRV_NAME);
       return;
     }
  }

  /*
   *register this fd with the tap monitor.
   *make sure to add a dtlCmd function
   */
  if(tap_monitor_register(dtl_net_fd,0,dtlSendCmd) == L7_FAILURE)
  {
     SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Failed TAP REGISTRATION\n");
     close(dtl_net_fd);
     return;
  }

 /*
  *once its open we need to create the corresponding
  *network interface.  We do this with an ioctl
  */
 memset(&ifr, 0, sizeof(ifr));
 strcpy(ifr.ifr_name,"dtl0");

 /*
  *Inidicate that this is a tap interface
  *and that we provide no additional packet information
  */
 ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
 if(ioctl(dtl_net_fd,TUNSETIFF,&ifr) < 0)
 {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Unable to create corresponding dtl net ifc\n");
    close(dtl_net_fd);
    LOG_ERROR(dtl_net_fd);
    return;
 }
#endif

  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {

    simGetSystemIPBurnedInMac(mac);

  }
  else
  {

    simGetSystemIPLocalAdminMac(mac);

  }

#ifdef DTL_USE_TAP
  /*
   *we need to override the random
   *mac given to the tap interface
   *with our own defined MAC
   */

 /*
  *  Do not set the interface mac here.
  *  SIM config data has not yet been populated
  *  at this point in the bootup, so the mac in there is all
  *  zeros.  Newer kernel versions will error and not allow setting
  *  the mac to all zeros.  The mac gets set properly later when
  *  dtlFdbMacAddrChange() is called.  
  */

#endif

  queue_len = L7_MAX_NETWORK_BUFF_PER_BOX;
  osapiSnprintf(queue_len_buf, sizeof(queue_len_buf), "%lu", queue_len);
  osapi_proc_set("/proc/sys/net/ipv4/neigh/default/unres_qlen", queue_len_buf);

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  /* need to check if this is needed as service port is enabled from sim*/
  osapi_proc_set("/proc/sys/net/ipv6/conf/dtl0/ipv6_enable", "1");
  osapi_proc_set("/proc/sys/net/ipv6/neigh/default/unres_qlen", queue_len_buf);
  osapi_proc_set("/proc/sys/net/ipv6/conf/dtl0/autoconf", "0");
#endif

  /* Add our MAC Address to the Search Machines. */
  if (bcmp (empty, mac, L7_MAC_ADDR_LEN))
  {
    vlanId = simMgmtVlanIdGet();
    fdbSysMacAddEntry(mac, vlanId, 1, L7_FDB_ADDR_FLAG_MANAGEMENT);

  }

  ipaddr = simGetSystemIPAddr();
  netmask = simGetSystemIPNetMask();
  gateway = simGetSystemIPGateway();

  if (ipaddr)
  {

    sysapiConfigSwitchIp (ipaddr, netmask);

    dtlIPAddrSystemSet(ipaddr);
  }

  if (gateway)
  {
    /* set gateway */
    simSetSystemIPGateway (gateway, L7_TRUE);
  }

  dtlNetInitDone = L7_TRUE;


}


#ifdef DTL_USE_TAP

void dtlSendCmd(int fd, L7_uint32 dummy_intIfNum, L7_netBufHandle handle, tapDtlInfo *info)
{
   L7_uchar8 *data;
   L7_uint32 data_length;
   L7_ushort16 etype = 0;
   L7_uint32 intIfNum;
   L7_uint32 activeState;
   char dtlIfName[32];
   register L7_ipHeader_t *ip_header;
   sysnet_pdu_info_t pduInfo;
   SYSNET_PDU_RC_t hookAction;

   /*
    *zero out the dtlCmd structure
    */
   memset(&info->dtlCmdInfo,0,sizeof(DTL_CMD_TX_INFO_t));
   info->discard = L7_TRUE;

   /*
    *get the data start and length of this header
    */
   SYSAPI_NET_MBUF_GET_DATASTART(handle,data);
   SYSAPI_NET_MBUF_GET_DATALENGTH(handle,data_length);


   L7_uint16 vlanId;

   if (ptin_intif_vlan_translate_egress_get(9, simMgmtVlanIdGet(), &vlanId)!=L7_SUCCESS)
   {
     vlanId = simMgmtVlanIdGet();
   }

   /*
    *check to see if this is a multicast frame
    */
   if(data[0] & 0x1)
   {
      /*
       *this is a multicast address
       */
      // PTin added
      // Insert tag
      L7_int i;
      for (i=data_length-1; i>=12; i--)  {
        data[i+4] = data[i];
      }
      data[12]= (L7_uint8) ((L7_ETYPE_8021Q >> 8) & 0xFF);
      data[13]= (L7_uint8) (L7_ETYPE_8021Q & 0xFF);
      data[14]=(L7_uint8) ((vlanId>>8) & 0xFF);
      data[15]=(L7_uint8) (vlanId & 0xFF);
      data_length += 4;
      SYSAPI_NET_MBUF_SET_DATALENGTH(handle,data_length);
      // PTin end

      info->dtlCmdInfo.intfNum = 0;
      info->dtlCmdInfo.priority = 0;
      info->dtlCmdInfo.typeToSend = DTL_VLAN_MULTICAST;
      info->dtlCmdInfo.cmdType.L2.domainId = simMgmtVlanIdGet();
      info->dtlCmd = DTL_CMD_TX_L2;
      /*
       *we are done
       */
      info->discard = L7_FALSE;
      goto dtlSendCmdExit;
   }/*end if multicast*/
   else
   {
      /*
       *this is not a multicast frame
       *It is either ARP or IP
       */
      memcpy(&etype, &data[12], sizeof(etype));
      etype = osapiNtohs(etype);
      if(etype == L7_ETYPE_ARP)
      {
          /*
           *its an arp frame
           *Find the appropriate interface number
           */
          if(dtlFind(data,&intIfNum) != L7_SUCCESS)
          {
             /*
              *failed search, tell tap monitor to
              *discard
              */
             goto dtlSendCmdExit;
          }
          else
          {
             /*
              *found our interface, fill in dtlCmdInfo
              */
             info->dtlCmdInfo.intfNum = intIfNum;
             info->dtlCmdInfo.priority = 0;
             info->dtlCmdInfo.typeToSend = DTL_NORMAL_UNICAST;
             info->dtlCmdInfo.cmdType.L2.domainId = vlanId; //simMgmtVlanIdGet();   // PTin modified
             info->dtlCmd = DTL_CMD_TX_L2;
             info->discard = L7_FALSE;
             goto dtlSendCmdExit;
          }
      }/*end if(ether_type...*/
      else if(etype == L7_ETYPE_IP)
      {
         /* Find the physical port where the destination MAC address is. If
         * that port is not active, clear the ARP cache in the IP stack. It may
         * be that the port mapping is stale and we need to ARP again to update
         * the mapping. */
        if ((dtlFind(data,&intIfNum) != L7_SUCCESS) ||
            (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) ||
            (activeState != L7_ACTIVE))
        {
          dtlDeleteAll();
          sprintf( dtlIfName, "%s%d", L7_DTL_PORT_IF, 0 );
          osapiArpFlush(dtlIfName);
          goto dtlSendCmdExit;
         }
         else
         {
            ip_header = (L7_ipHeader_t *)&data[14];
            if(ip_header->iph_prot == IP_PROT_ICMP)
                pingDebugPacketTxTrace(intIfNum,data);

            info->dtlCmdInfo.intfNum = intIfNum;
            info->dtlCmdInfo.priority = 0;
            info->dtlCmdInfo.typeToSend = DTL_NORMAL_UNICAST;
            info->dtlCmdInfo.cmdType.L2.domainId = vlanId; //simMgmtVlanIdGet();  // PTin modified
            info->dtlCmd = DTL_CMD_TX_L2;
            info->discard = L7_FALSE;

            /* Call interceptors who are interested in outgoing IP frames post L2 encapsualtion.  If
             ** L7_TRUE is returned, the frame was either discarded or consumed, which means that the
             ** network buffer has been freed by the intercept call, or will be freed by the consumer.
              */
            bzero((char *)&pduInfo, sizeof(sysnet_pdu_info_t));
            pduInfo.destIntIfNum = intIfNum;
            pduInfo.destVlanId = vlanId; //simMgmtVlanIdGet();
            if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_POSTCAP_OUT, handle, &pduInfo,
                              L7_NULLPTR, &hookAction) == L7_TRUE)
            {
              return;
            }
            goto dtlSendCmdExit;
         }
      }
      else if(etype == L7_ETYPE_IPV6)
      {
         if(dtlFind(data,&intIfNum) != L7_SUCCESS)
         {
            goto dtlSendCmdExit;
         }
         else
         {
            info->dtlCmdInfo.intfNum = intIfNum;
            info->dtlCmdInfo.priority = 0;
            info->dtlCmdInfo.typeToSend = DTL_NORMAL_UNICAST;
            info->dtlCmdInfo.cmdType.L2.domainId = simMgmtVlanIdGet();
            info->dtlCmd = DTL_CMD_TX_L2;
            info->discard = L7_FALSE;
            goto dtlSendCmdExit;
         }
      }
   }/*end else...multicast*/

dtlSendCmdExit:
   if(info->discard == L7_TRUE){
      /*
       *if we fall through something unexpected
       *has happened, so drop the frame
       */
      SYSAPI_NET_MBUF_FREE(handle);
   }
   else
   {
      dtlPduTransmit(handle, info->dtlCmd, &info->dtlCmdInfo);
   }
   return;
}
#endif

/*********************************************************************
* @purpose  Transmit OSAPI network buffer on the specified IP interface.
*
* @param    intIfNum      internal interface number
* @param    bufHandle     mbuf containing the frame.
*
* @notes    This function is used by DHCP Server code to
*           send frames on the network.
*
* @end
*********************************************************************/

L7_RC_t dtlIpBufSend(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 innerVlanId, L7_netBufHandle  bufHandle) {

   DTL_CMD_TX_INFO_t dtlCmd;
   L7_uint32 len;
   L7_netBufHandle  newBufHandle;
   L7_uchar8 destMac[6];
   L7_ushort16 flag = 0;
   L7_ushort16 protocol_type;
   L7_uint32 datalen;
   L7_uchar8 *pdataStart;
   L7_uint32 offset = 0;
   L7_uint32 commonHeaderLen = L7_ENET_HDR_SIZE + 2;
   L7_uint32 encapType;
   struct ip *ip_header;
   L7_ushort16 packet_type;
   sysnet_pdu_info_t pduInfo;

   if (nimCheckIfNumber(intIfNum) != L7_SUCCESS) {

      SYSAPI_NET_MBUF_FREE(bufHandle);
      return L7_FAILURE;

   }

   dtlCmd.intfNum = intIfNum;
   dtlCmd.priority = 0;
   dtlCmd.typeToSend = DTL_NORMAL_UNICAST;

   dtlCmd.cmdType.L2.domainId = vlanId;


   SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, datalen);

   if (!datalen) {

      return L7_FAILURE;
     }

   SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pdataStart);

   /* The VLAN tag information from application is kept as it is to have more control.
      The lower layer (HAPI) takes cares of tagging if needed for untagged packets. The DTL 
      needs to hand over the packet to lower layers for transmissioni unless no interested
      sysnet interceptors consumes it.*/  

   offset = sysNetDataOffsetGet(pdataStart);

   ip_header = (struct ip *)(pdataStart + offset);
   bcopy ((pdataStart + offset - 2), (L7_char8 *) &packet_type, 2);


   /* Call interceptors who are interested in outgoing IP frames prior to L2 encapsualtion.
   ** If L7_TRUE is returned, the frame was either discarded or consumed, which means that
   ** the network buffer has been freed by the intercept call, or will be freed by the consumer.
   */
   memset(&pduInfo, 0x00, sizeof(sysnet_pdu_info_t));
   pduInfo.destIntIfNum = intIfNum;

   /*TBD : is it ok to set default VLAN here */
   pduInfo.destVlanId =  0x0001;
   if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_PRECAP_OUT, bufHandle, &pduInfo,
                            L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
    return L7_SUCCESS;

   if (nimEncapsulationTypeGet(intIfNum, &encapType) == L7_SUCCESS) {

      if (((encapType == L7_ENCAP_802) && (offset == commonHeaderLen))
       || ((encapType == L7_ENCAP_ETHERNET)
        && (offset == commonHeaderLen + 8))) {

         memcpy(destMac, pdataStart, 6);
         bcopy (pdataStart + offset - sizeof(L7_ushort16),
                (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16));

         if (encapType == L7_ENCAP_802) {
            flag |= L7_LL_ENCAP_802;

         } else {
            flag |= L7_LL_ENCAP_ENET;

         }

      if (sysNetPduHdrEncapsulate(destMac, intIfNum, &flag, datalen - offset,
                                     0, protocol_type, &newBufHandle,
                                     pdataStart + offset) == L7_SUCCESS) {

        SYSAPI_NET_MBUF_GET_DATALENGTH (newBufHandle, len);
        SYSAPI_NET_MBUF_GET_DATASTART(newBufHandle, pdataStart);

            SYSAPI_NET_MBUF_FREE(bufHandle);

            /* Call interceptors who are interested in outgoing IP frames post L2 encapsualtion.  If
            ** L7_TRUE is returned, the frame was either discarded or consumed, which means that the
            ** network buffer has been freed by the intercept call, or will be freed by the consumer.
            */
            if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_POSTCAP_OUT, newBufHandle, &pduInfo,
                                     L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
              return L7_SUCCESS;

            dtlPduTransmit(newBufHandle, DTL_CMD_TX_L2, &dtlCmd);
            return(L7_SUCCESS);
     }

      }

   }

   // PTin added
   // Integrate inner vlan
   if (innerVlanId!=0)
   {
     L7_uint16 i;
     for (i=datalen-1; i>=16; i--)
     {
       pdataStart[i+4] = pdataStart[i];
     }
     pdataStart[16] = 0x81;
     pdataStart[17] = 0x00;
     pdataStart[18] = innerVlanId>>8 & 0x0F;
     pdataStart[19] = innerVlanId & 0xFF;
     datalen += 4;
     SYSAPI_NET_MBUF_SET_DATALENGTH (bufHandle, datalen);
     //dsUdpCheckSumCalculate(frame, &frameLen, L7_TRUE, 4);
   }

   /* Call interceptors who are interested in outgoing IP frames post L2 encapsualtion.  If
   ** L7_TRUE is returned, the frame was either discarded or consumed, which means that the
   ** network buffer has been freed by the intercept call, or will be freed by the consumer.
   */
   if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_POSTCAP_OUT, bufHandle, &pduInfo,
                            L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
     return L7_SUCCESS;

   SYSAPI_NET_MBUF_GET_DATALENGTH (bufHandle, len);

   dtlPduTransmit(bufHandle, DTL_CMD_TX_L2, &dtlCmd);

   return(L7_SUCCESS);
}

void dltNetDebugSet(int val)
{
}

#endif /* _L7_OS_LINUX_ */
