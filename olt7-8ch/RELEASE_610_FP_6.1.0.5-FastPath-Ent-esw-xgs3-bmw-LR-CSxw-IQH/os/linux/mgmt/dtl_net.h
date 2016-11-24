#ifndef _DTL_NET_H_
#define _DTL_NET_H_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_net.h
*
* @purpose Linux IP Stack Interface
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   John Linville   11/21/2001
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "l7_common.h"

#define	AF_DTL	(AF_MAX-1)

#define	PF_DTL	AF_DTL

struct sockaddr_dtl {

   L7_uchar8 ifname[IFNAMSIZ];
   L7_uchar8 mac[6];

};

#endif /* _DTL_NET_H_ */
