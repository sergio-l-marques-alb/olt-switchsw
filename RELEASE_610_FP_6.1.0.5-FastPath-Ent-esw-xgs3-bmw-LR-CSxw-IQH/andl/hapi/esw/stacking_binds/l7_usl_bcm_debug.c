/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_debug.c
*
* @purpose    Handle debug facilities for usl
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#include "l7_common.h"
#include <string.h>
#include "sysapi.h"
#include "log.h"
#include "l7utils_api.h"
#include "l7_usl_bcm_l2.h"
#include "l7_usl_bcm_l3.h"
#include "l7_usl_bcm_ipmcast.h"
#include "platform_config.h"

static L7_BOOL uslPrintLog = L7_FALSE;

void usl_log_msg_fmt(L7_char8 * file, L7_ulong32 line, L7_char8 * msg,L7_BOOL overRide)
{
  L7_int32 len;
  L7_uchar8 log_buf[LOG_MSG_MAX_MSG_SIZE];

  if ((uslPrintLog == L7_TRUE) || (overRide == L7_TRUE))
  {
    bzero(log_buf,sizeof(log_buf));

    l7utilsFilenameStrip(&file);

    osapiSnprintf (log_buf, LOG_MSG_MAX_MSG_SIZE, "%s:%d : ", file, (L7_int32)line);

    len = (L7_int32)strlen(log_buf);

    if (len < LOG_MSG_MAX_MSG_SIZE)
    {
      strncat(log_buf,msg,(size_t)(LOG_MSG_MAX_MSG_SIZE - len));
    }

    sysapiPrintf(log_buf);
  }
} 

void usl_print_log(L7_BOOL set)
{
  uslPrintLog = (set == L7_TRUE)?L7_TRUE:L7_FALSE;
}


void usl_lplist_print(bcmx_lplist_t *list)
{
  int i;
  bcmx_lport_t port;

  if (list == L7_NULLPTR)
    return;

  /* get the first port in the list */

  for (i=0; i < list->lp_last; i++)
  {
    port = bcmx_lplist_index(list, i);
    if (port == BCMX_NO_SUCH_LPORT)
       break;

    sysapiPrintf("%d , ",port);
  }
}

/*
 * Debug routines to test the database access without hardware  
 */
#ifdef USL_NO_HW
void usl_test_fill_db()
{
  L7_uint32 max,idx;
  bcmx_l2_addr_t l2addr;  
  bcmx_mcast_addr_t mcaddr;  
  bcm_stg_t stgId;
  bcm_trunk_t tid;

  memset((void *)&l2addr,0,sizeof(bcmx_l2_addr_t)); 
  memset((void *)&mcaddr,0,sizeof(bcmx_mcast_addr_t)); 

  /* test filling the l2 address table */
  max = platFdbTotalMacEntriesGet();

  for (idx = 1; idx <= max ; idx++)
  {
    l2addr.mac[1] = idx/256;
    l2addr.mac[0] = idx%256;
    l2addr.vid = idx % 4000;

    if (usl_bcmx_l2_addr_add(&l2addr) != BCM_E_NONE)
    {
      sysapiPrintf("Error adding l2 address\n");
      sysapiPrintf("Problem inserting mac 0:0:0:0:%0.2x:%0x2x vid=%d\n",
                   idx/256,idx%256,idx %4000);
      break;
    }
  }

  /* test filling the l2 mcast table */
  max = platMfdbTotalMaxEntriesGet();

  /* create our own list of ports */
  bcmx_lplist_init(&mcaddr.ports,0,0);
  bcmx_lplist_init(&mcaddr.untag_ports,0,0);

  for (idx = 1; idx <= max ; idx++)
  {
    mcaddr.mac[5] = idx/256;
    mcaddr.mac[4] = idx%256;
    mcaddr.vid = idx % 4000;

    if (usl_bcmx_mcast_addr_add(&mcaddr) != BCM_E_NONE)
    {
      sysapiPrintf("Error in L2 Mcast address add\n");
      sysapiPrintf("Problem inserting mac %0.2x:%0x2x:00:00:00:00 vid=%d\n",
                   idx/256,idx%256,idx %4000);
      break;
    }
  }

  /* test filling the vlan table */
  max = 4096; 

  for (idx = 1; idx < 4096 ; idx++)
  {
    if (usl_bcmx_vlan_create((bcm_vlan_t) idx) != BCM_E_NONE)
    {
      sysapiPrintf("Error adding vlan %d to table\n",idx);
      break;
    }
  }

  /* test filling the stg table */
  max =  platDot1sInstancesMaxEntriesGet() + 1; /* Add one for CST */

  for (idx = 0; idx < max; idx++)
  {
    if (usl_bcmx_stg_create(&stgId) == BCM_E_NONE)
    {
      if (usl_bcmx_stg_vlan_add(stgId,(bcm_vlan_t) stgId) != BCM_E_NONE)
      {
        sysapiPrintf("Error adding vlan idx to stg %d\n",idx,stgId); 
        break;
      }
    }
    else
    {
      sysapiPrintf("Error creating STG %d \n", idx);
      break;
    }
  }

  /* test filling the trunk table */
  max = platIntfLagIntfMaxCountGet();

  for (idx = 0; idx < max; idx++)
  {
    if (usl_bcmx_trunk_create(&tid) != BCM_E_NONE)
    {
      sysapiPrintf("Error creating Trunk %d\n", idx);
      break;
    }
  }
}

int usl_debug_l2ucast_add(L7_uchar8 m5,L7_uchar8 m4,L7_uchar8 m3,L7_uchar8 m2,L7_uchar8 m1,L7_uchar8 m0,L7_uint32 vid)
{
  bcmx_l2_addr_t l2addr;  

  memset((void *)&l2addr,0,sizeof(bcmx_l2_addr_t));

  l2addr.mac[5] = m5;
  l2addr.mac[4] = m4;
  l2addr.mac[3] = m3;
  l2addr.mac[2] = m2;
  l2addr.mac[1] = m1;
  l2addr.mac[0] = m0;
  l2addr.vid = vid;

  return usl_bcmx_l2_addr_add(&l2addr);
}


int usl_debug_l2ucast_remove(L7_uchar8 m5,L7_uchar8 m4,L7_uchar8 m3,L7_uchar8 m2,L7_uchar8 m1,L7_uchar8 m0,L7_uint32 vid)
{
  bcm_mac_t mac;
  bcm_vlan_t lvid;

  mac[5] = m5;
  mac[4] = m4;
  mac[3] = m3;
  mac[2] = m2;
  mac[1] = m1;
  mac[0] = m0;
  lvid = vid;

  return usl_bcmx_l2_addr_remove(mac,vid);
}

int usl_debug_l2mcast_add(L7_uchar8 m5,L7_uchar8 m4,L7_uchar8 m3,L7_uchar8 m2,L7_uchar8 m1,L7_uchar8 m0,L7_uint32 vid)
{
  bcmx_mcast_addr_t mcaddr;  

  memset((void *)&mcaddr,0,sizeof(bcmx_mcast_addr_t));

  /* create our own list of ports */
  bcmx_lplist_init(&mcaddr.ports,0,0);
  bcmx_lplist_init(&mcaddr.untag_ports,0,0);

  mcaddr.mac[5] = m5;
  mcaddr.mac[4] = m4;
  mcaddr.mac[3] = m3;
  mcaddr.mac[2] = m2;
  mcaddr.mac[1] = m1;
  mcaddr.mac[0] = m0;
  mcaddr.vid = vid;

  return usl_bcmx_mcast_addr_add(&mcaddr);
}

int usl_debug_l2mcast_remove(L7_uchar8 m5,L7_uchar8 m4,L7_uchar8 m3,L7_uchar8 m2,L7_uchar8 m1,L7_uchar8 m0,L7_uint32 vid)
{
  bcm_mac_t mac;
  bcm_vlan_t lvid;

  mac[5] = m5;
  mac[4] = m4;
  mac[3] = m3;
  mac[2] = m2;
  mac[1] = m1;
  mac[0] = m0;
  lvid = vid;

  return usl_bcmx_mcast_addr_remove(mac,vid);
}

int usl_debug_l3_lpm(L7_uint32 ip1,L7_uint32 ip2,L7_uint32 ip3,L7_uint32 ip4,
                         L7_uint32 sn1,L7_uint32 sn2,L7_uint32 sn3,L7_uint32 sn4,
                         L7_BOOL add)
{
  L7_int32 rv;
  bcmx_l3_route_t entry;

  memset((void *)&entry,0,sizeof(bcmx_l3_route_t));

  entry.l3a_ip_mask = (sn1 << 24) | (sn2 <<16) | (sn3 << 8) | sn4;
  entry.l3a_subnet = (ip1 << 24) | (ip2 <<16) | (ip3 << 8) | ip4;

  if (add)
    usl_bcmx_l3_route_add(&entry, 1, &rv);
  else
     usl_bcmx_l3_route_delete(&entry, 1, &rv);

  return rv;
}

int usl_debug_ipmc(L7_uint32 mcip1,L7_uint32 mcip2,L7_uint32 mcip3,L7_uint32 mcip4,
                   L7_uint32 sip1,L7_uint32 sip2,L7_uint32 sip3,L7_uint32 sip4,
                   L7_uint32 vid,L7_BOOL add)
{
  bcmx_ipmc_addr_t ldata;

  memset((void *)&ldata,0,sizeof(bcmx_ipmc_addr_t));

  /* create our own list of ports */
  bcmx_lplist_init(&ldata.l2_ports ,0,0);
  bcmx_lplist_init(&ldata.l2_untag_ports,0,0);
  bcmx_lplist_init(&ldata.l3_ports,0,0);

  ldata.mc_ip_addr = (mcip1 << 24) | (mcip2 <<16) | (mcip3 << 8) | mcip4;
  ldata.s_ip_addr = (sip1 << 24) | (sip2 <<16) | (sip3 << 8) | sip4;
  ldata.vid = vid;

  if (add)
    return usl_bcmx_ipmc_add(&ldata);
  else 
    return usl_bcmx_ipmc_delete(ldata.s_ip_addr,ldata.mc_ip_addr,vid,0);
}

#endif
