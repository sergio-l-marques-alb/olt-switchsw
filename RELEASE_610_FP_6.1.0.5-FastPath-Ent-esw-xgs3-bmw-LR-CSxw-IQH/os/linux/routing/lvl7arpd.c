/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  lvl7arpd.c
*
* @purpose   development implentation of netlink shim for fastpath arpd
*
* @component ???
*
* @comments
*
* @create    2/20/02
*
* @author    Neil Horman
*
*
* @end
*
*********************************************************************/
/*********************************************************************
 *
 *********************************************************************/

/*
 *USEFULL DEFINES:
 */
#define DYNAMIC_ARP_ENTRY 0
#define STATIC_ARP_ENTRY 1

/*
 *SYSTEM INCLUDES
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stropts.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/sockios.h>
#include <linux/if_arp.h>
#include <linux/in.h>

/* PTin added */
#include <net/if.h>

/*
 *LOCAL INCLUDES
 */
#include "commdefs.h"
#include "datatypes.h"
#include "l7_ipmap_arp_api.h"
#include "l3end_api.h"
#include "osapi_priv.h"
#include "osapi_support.h"
#include "platform_config.h"
#include "l7_ip_api.h"

/*
 *DATA STRUCTURES
 */
struct arpd_sock_s
{
  int fd;
  int ioctl_fd;
  unsigned int sequence;
  unsigned int pid;
  struct sockaddr_nl local;
};


/*
 *FILE SCOPE VARIABLES
 */
static struct arpd_sock_s arpd_sock;



/*
 *PROTOTYPES
 */

extern L7_RC_t ipMapArpAddrResolve(L7_uint32 intIfNum, L7_uint32 ipAddr,
                   L7_uchar8 *pMacAddr,
                   ipMapArpResCallback_ft pCallbackFn,
                   L7_uint32 cbParm1, L7_uint32 cbParm2);

L7_RC_t process_getneigh_request(struct nlmsghdr *gnr);

void arp_resolver_callback(L7_uint32 ipaddr,
               L7_uint32 ifindex,
               L7_uchar8 *mac_addr,
               L7_RC_t  rc);

L7_RC_t update_kernel_arp_table(L7_uint32 ipaddr,
                L7_uchar8 *mac_addr,
                L7_uint32 ifindex,
                L7_uchar8 entry_type);
/*
 *FUNCTIONS
 */

int do_ioctl_setup()
{
   int rc;



   rc = arpd_sock.ioctl_fd = socket(AF_INET,SOCK_DGRAM,0);

   if(rc < 0)
   {
    perror("socket open on ioctl socket failed");
        return 1;
   }
   return 0;
}


/*
 *this function is responsible for
 *the creation and setup of the
 *netlink socket on which we will listen
 */
int do_netlink_setup()
{
  int rc;
  int addr_length;

  /*
   *first lets zero out our data, to be safe
   */
  memset(&arpd_sock,0,sizeof(arpd_sock));



  /*
   *first create the socket
   */
  arpd_sock.fd = socket(AF_NETLINK,SOCK_RAW,NETLINK_ROUTE);
  if(arpd_sock.fd < 0)
    {
      perror("cannot open netlink socket");
      return -1;
    }

  /*
   *now bind the socket such that we listen to no multicast groups
   *I think :)
   */
  memset(&arpd_sock.local,0,sizeof(arpd_sock.local));
  arpd_sock.local.nl_family = AF_NETLINK;
  arpd_sock.local.nl_groups = 0x4;

  rc = bind(arpd_sock.fd,(struct sockaddr *)&arpd_sock.local,sizeof(arpd_sock.local));
  if(rc < 0)
    {
      perror("cannot bind netlink socket!");
      close(arpd_sock.fd);
      return -2;
    }


  /*
   *get the socket name. Not really needed, but nice extra info to have
   *I think
   */
  addr_length = sizeof(arpd_sock.local);
  rc = getsockname(arpd_sock.fd,(struct sockaddr *)&arpd_sock.local,&addr_length);
  if(rc < 0)
    {
      perror("unable to retrieve socket name");
      close(arpd_sock.fd);
      return -3;
    }

  /*
   *verify that our size and family are still correct
   */
  if(addr_length != sizeof(arpd_sock.local))
    {
#ifdef L7_ARPD_DEBUG
      printf("error getting socket name! length mismatch!\n");
#endif
      close(arpd_sock.fd);
      return -4;
    }
  if(arpd_sock.local.nl_family != AF_NETLINK)
    {
#ifdef L7_ARPD_DEBUG
      printf("error getting socket name! family mismatch\n");
#endif
      close(arpd_sock.fd);
      return -5;
    }

  /*
   *if we make it to here then everything is ok.
   */
  arpd_sock.sequence = 1;
  arpd_sock.pid = osapiGetpid();
  /*
   *we can return
   */
  return 0;
}





/*
 *This function loops for ever
 *listening for RTM_GETNEIGH requests
 *to which it will eventually respond
 *using the lvl7 arp daemon
 */
void service_arp_requests()
{
  /*
   *general return code
   */
  int rc;

  /*
   *this is our buffer for listening for
   *RTM_GETNEIGH requests
   */
  char msg_buf[512];
  /*
   *a pointer to the structure returned
   *in the head of the receive buffer
   */
  struct nlmsghdr *rmsg_header;
  /*
   *this structure will contain information
   *about the source of the message after a
   *return from recvmsg
   */
  struct sockaddr_nl nlmsg_info;
  /*
   *and these are supporting structures
   *for use with the recvmsg call
   */
  struct iovec msg_iov = {(void *)&msg_buf,sizeof(msg_buf)};
  struct msghdr rmsg = {
    (void *)&nlmsg_info,sizeof(nlmsg_info),
    &msg_iov, 1,
    NULL,0,
    0};

  /*
   *do the following for ever
   */
  for(;;)
    {
      /*
       *zero the contents of the nlm message,
       *so we know that we have new data
       *on the return from recvmsg
       */
      memset(&nlmsg_info,0,sizeof(nlmsg_info));

      /*
       *now wait for a message
       */
#ifdef L7_ARPD_DEBUG
      printf("waiting on a message\n");
#endif
      rc = recvmsg(arpd_sock.fd,&rmsg,0);
      if(rc < 0)
    {
      /*
       *don't bother to return from here, just continue, and
       *try to read again.
       * NOTE: probably rcvbuf overrun caused by ipv6 ndp deletes.
       */
      continue;
    }

      /*
       *we have successfully received a message,
       *lets take a look at
       *the contents
       */
#ifdef L7_ARPD_DEBUG
      printf("received a message!\n");
#endif
      rmsg_header = (struct nlmsghdr *)msg_buf;
#ifdef L7_ARPD_DEBUG
      printf("message type = 0x%x!\n",rmsg_header->nlmsg_type);
      printf("flags = 0x%x\n",rmsg_header->nlmsg_flags);
#endif
      /*
       *check if its a request
       */
      if(rmsg_header->nlmsg_flags & NLM_F_REQUEST)
    {
#ifdef L7_ARPD_DEBUG
      printf("its a request!\n");
#endif
    }
      else
    {
      /*
       *if its not a request then move on
       */
      continue;
    }
      switch(rmsg_header->nlmsg_type)
    {

    case RTM_GETNEIGH:
      {
#ifdef L7_ARPD_DEBUG
        printf("RTM_GETNEIGH\n");
#endif
        /*
         *get neighbor requests oblige us to
         *preform a lookup in the fastpath
         *arp database
         */
        rc = process_getneigh_request(rmsg_header);
        break;
      }
    default:
      {
#ifdef L7_ARPD_DEBUG
        printf("MSG_TYPE_OTHER\n");
#endif
        break;
      }
    }

    }/*end for(;;)*/

}





/*======================================================================*/
/*
 *the main routine really just launches
 *our socket and goes to our wait forever loop
 */
void arpd_entry_fn()
{
  int rc;

#ifdef L7_ARPD_DEBUG
  printf("Starting Fastpath ARP monitor for Linux\n");
#endif

  /* Set the kernel's neighbour tables sizes to our application arp and 
     ipv6 ND table sizes. */
  {
    unsigned long nt_size;
    char nt_size_buf[11];
    
    nt_size = platRtrArpMaxEntriesGet();
    osapiSnprintf(nt_size_buf, 11, "%lu", nt_size);
    osapi_proc_set("/proc/sys/net/ipv4/neigh/default/gc_thresh3", nt_size_buf);
    nt_size /= 2;
    osapiSnprintf(nt_size_buf, 11, "%lu", nt_size);
    osapi_proc_set("/proc/sys/net/ipv4/neigh/default/gc_thresh2", nt_size_buf);
    nt_size /= 4;
    osapiSnprintf(nt_size_buf, 11, "%lu", nt_size);
    osapi_proc_set("/proc/sys/net/ipv4/neigh/default/gc_thresh1", nt_size_buf);
    
#ifdef L7_IPV6_PACKAGE
    nt_size = L7_IPV6_NDP_CACHE_SIZE;
    osapiSnprintf(nt_size_buf, 11, "%lu", nt_size);
    osapi_proc_set("/proc/sys/net/ipv6/neigh/default/gc_thresh3", nt_size_buf);
    nt_size /= 2;
    osapiSnprintf(nt_size_buf, 11, "%lu", nt_size);
    osapi_proc_set("/proc/sys/net/ipv6/neigh/default/gc_thresh2", nt_size_buf);
    nt_size /= 4;
    osapiSnprintf(nt_size_buf, 11, "%lu", nt_size);
    osapi_proc_set("/proc/sys/net/ipv6/neigh/default/gc_thresh1", nt_size_buf);
#endif
  }
  /*
   *all we really need to do here is open and
   *setup a netlink socket, and then wait for messages
   */
  rc = do_netlink_setup();
  if(rc < 0)
    {
      /*
       *do_netlink_setup will display an informative error
       */
      exit(0);
    }
  rc = do_ioctl_setup();
  if(rc != 0)
  {
     exit(0);
  }

  /*
   *after we do the netlink startup
   *we need to wait for a message to arrive
   */
#ifdef L7_ARPD_DEBUG
  printf("starting service...\n");
#endif
  service_arp_requests();

  /*
   *we should never return from the service_arp_requests
   *function
   */
#ifdef L7_ARPD_DEBUG
  printf("abnormal return from service_arp_requests()\n");
#endif
  /*
   *and we're done
   */
  exit(0);

}






/******************************************************************
 *Function: process_getneigh_request
 *
 *Parameters: nlmsghdr pointer to get neighbor netlink request msg
 *
 *Returns: L7_RC_t
 *
 *Description; Parses getneighbor request message and passes data to
 *fastpath arpd interface
 *
 *NOTES: Not sure, but we may need to handle multipart nlmsgheaders
 *       but I don't  think that we will ever get one
 ******************************************************************/

L7_RC_t process_getneigh_request(struct nlmsghdr *gnr)
{
  L7_RC_t rc;
  int buf_len;
  struct ndmsg *neigh_hdr; /*the neighbor message header*/
  struct rtattr *rtattrib; /*the attributes structure*/
  L7_uchar8 mac_addr[6];
  L7_uint32 ipaddr = 0;
  L7_uint32 *ipaddrptr;
  L7_uint32 IntIfNum;

  /*
   *first extract the neigh_hdr
   */
  neigh_hdr = (struct ndmsg *)NLMSG_DATA(gnr);
  buf_len = NLMSG_PAYLOAD(gnr,sizeof(*gnr));

  /* ignore ipv6 */
  if(neigh_hdr->ndm_family != AF_INET){
       return L7_SUCCESS;
  }

#ifdef L7_ARPD_DEBUG
   printf("ndmsg attributes:\n");
   printf("ndm_family = %d\n",neigh_hdr->ndm_family);
   printf("ndm_ifindex = %d\n",neigh_hdr->ndm_ifindex);
   printf("state = 0x%x\n",neigh_hdr->ndm_state);
   printf("flags = 0x%x\n",neigh_hdr->ndm_flags);
#endif
   /*clear the result table*/
   memset(&mac_addr[0],0,sizeof(mac_addr));

   /*
    *now loop through the attributes looking for
    *a destination IP address
    */
   rtattrib = ((struct rtattr*)(((char*)(neigh_hdr)) + NLMSG_ALIGN(sizeof(struct ndmsg))));

   /*
    *now that we have used the the undocumented
    *macros from rtnetlink.h to get to the rtattr
    *structures, we can look for our destination
    *IP address
    */
   ipaddrptr = NULL;
   while(RTA_OK(rtattrib,buf_len) && (ipaddrptr == NULL))
     {
#ifdef L7_ARPD_DEBUG
       printf("searching attribues with buffer length at %d bytes\n",buf_len);
#endif
       /*reduce the buffer running length*/
       switch(rtattrib->rta_type)
     {
     case NDA_DST:
       {
         /*this is the destination IP address*/
         ipaddrptr = (unsigned int*)RTA_DATA(rtattrib);
         ipaddr = osapiNtohl(*ipaddrptr);
#ifdef L7_ARPD_DEBUG
         printf("found ip address 0x%x\n", ipaddr);
#endif
         break;
       }
     default:
       {
         /*just keep looking....*/
         continue;
       }
     }/*end switch*/
     rtattrib = (struct rtattr *)RTA_NEXT(rtattrib,buf_len);
     }/*end while loop*/

   /*
    *check to make sure we found a destination
    */
   if(ipaddrptr == NULL)
     {
#ifdef L7_ARPD_DEBUG
       printf("found no destination ip address attribute\n");
#endif
       return L7_ERROR;
     }

   /*
    *now that we have the ip address
    *we can grab our interface number
    */
   rc = ipMapRouterIfResolve(ipaddr, &IntIfNum);
   if(rc != L7_SUCCESS)
   {
#ifdef L7_ARPD_DEBUG
     printf("failed to convert ip addr 0x%x to internal interface! rc = %d\n",ipaddr, rc);
#endif
     return rc;
   }

   /*
    *now call the arp database
    */
   rc = ipMapArpAddrResolve(IntIfNum,
                ipaddr,
                &mac_addr[0],
                arp_resolver_callback,
                ipaddr,
                neigh_hdr->ndm_ifindex);
   if(rc == L7_FAILURE)
     {
       /*
    *the address will not be at all resolved
    *This is a failing condition
    */
       return rc;
     }
   if(rc == L7_SUCCESS)
     {
       /*
    *we now have the mac address in hand
    *and should send a response
    */
       update_kernel_arp_table(ipaddr, &mac_addr[0], neigh_hdr->ndm_ifindex,DYNAMIC_ARP_ENTRY);
     }
   /*
    *if the error code is not success or failure, then
    *the arp processing is delayed until an arp request returns
    *In this case, just return succes, and wait for the callback to be
    *invoked
    */
   return L7_SUCCESS;
}





/******************************************************************
 *Function: arp_resolver_callback
 *
 *Parameters: ip address, mac address and router interface number
 *
 *Returns:
 *
 *Description: receives callback from arp message handler and sends
 *             update notice to kernel
 *
 *
 *NOTES:
 *
 ******************************************************************/
void arp_resolver_callback(L7_uint32 ipaddr, L7_uint32 ifindex, L7_uchar8 *mac_addr, L7_RC_t rc)
{
  if (rc == L7_SUCCESS)
    update_kernel_arp_table(ipaddr,mac_addr,ifindex,DYNAMIC_ARP_ENTRY);
}



/******************************************************************
 *Function: update_kernel_arp_table
 *
 *Parameters: ip address, mac address and router interface number
 *
 *Returns:
 *
 *Description: This function will add a new arp entry to the
 *             kernel arp table
 *
 *
 *
 *NOTES: To avoid being overly complex, this function implements
 *       only the new neighbor request with the NLM_F_REPLACE
 *       flag set.  It also sends these requests on the same
 *       socket which was set up in do_netlink_setup
 *
 *       Note this function should also be reentrant, as it may be
 *       called from several different contexts.  I'll try
 *       To remove this requirement in the future when time allows
 *
 ******************************************************************/
L7_RC_t update_kernel_arp_table(L7_uint32 ipaddr, L7_uchar8 *mac_addr, L7_uint32 ifindex, L7_uchar8 entry_type)
{
   struct arpreq arp;                  /* new arprep  structure */
   struct sockaddr_in *netaddr;        /* network address pointer */
   struct sockaddr *physaddr;          /* hardware address pointer */
   struct ifreq namerq;                /*to get interface name*/
   int rc;

   netaddr = (struct sockaddr_in *)&arp.arp_pa;
   physaddr = (struct sockaddr *)&arp.arp_ha;

   netaddr->sin_family = AF_INET;
   physaddr->sa_family = AF_UNSPEC;

   /*ioctl for the if name from if index*/
   namerq.ifr_ifindex = ifindex;
   rc = ioctl(arpd_sock.ioctl_fd,SIOCGIFNAME,&namerq);
   if(rc < 0)
   {
      return L7_FAILURE;
   }
   memcpy(&arp.arp_dev,namerq.ifr_name,IFNAMSIZ);

   arp.arp_flags = ATF_COM; /*completed entry*/

   netaddr->sin_addr.s_addr = osapiHtonl(ipaddr);

   memcpy(physaddr->sa_data,mac_addr,6);

   rc = ioctl(arpd_sock.ioctl_fd,SIOCSARP,&arp);
   if(rc < 0)
   {
      return L7_FAILURE;
   }

   return L7_SUCCESS;
}
