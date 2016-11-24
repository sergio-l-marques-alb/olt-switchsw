#ifdef _L7_OS_LINUX_
#ifndef OSAPI_MROUTE6_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_mroute6.h
*
* @purpose   IPv6 Multicast specific defines
*
* @component osapi
*
* @comments 
*
* @create    12/12/2006
*
* @author    Kiran Kumar Kella
*
* @end
*
*********************************************************************/

/*********************************************************************
* The standard linux header files don't include these defines.
* For the compilation of the applications using these defines to go ahead
* we are mentioning these defines and structures here.
* They are copied from linux2.6.14 source.
*********************************************************************/

#define MRT6_BASE       200
#define MRT6_INIT       (MRT6_BASE)     /* Activate the kernel mroute code      */
#define MRT6_DONE       (MRT6_BASE+1)   /* Shutdown the kernel mroute           */
#define MRT6_ADD_MIF    (MRT6_BASE+2)   /* Add a virtual interface              */
#define MRT6_DEL_MIF    (MRT6_BASE+3)   /* Delete a virtual interface           */
#define MRT6_ADD_MFC    (MRT6_BASE+4)   /* Add a multicast forwarding entry     */
#define MRT6_DEL_MFC    (MRT6_BASE+5)   /* Delete a multicast forwarding entry  */
#define MRT6_VERSION    (MRT6_BASE+6)   /* Get the kernel multicast version     */
#define MRT6_ASSERT     (MRT6_BASE+7)   /* Activate PIM assert mode             */
#define MRT6_PIM        (MRT6_BASE+8)   /* enable PIM code      */


/* Structures Passed to the stack during MRT6_ADD_MIF / MRT6_DEL_MIF */
 
typedef unsigned short mifi_t;

struct mif6ctl {
	mifi_t	mif6c_mifi;		/* Index of MIF */
	unsigned char mif6c_flags;	/* MIFF_ flags */
	unsigned char vifc_threshold;	/* ttl limit */
	unsigned int vifc_rate_limit;	/* Rate limiter values (NI) */
	u_short	 mif6c_pifi;		/* the index of the physical IF */
};

#endif /* OSAPI_MROUTE6_H */
#endif /* _L7_OS_LINUX_ */

