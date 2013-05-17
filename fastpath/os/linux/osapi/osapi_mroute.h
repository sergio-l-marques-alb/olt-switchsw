#ifdef _L7_OS_LINUX_
#ifndef OSAPI_MROUTE_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_mroute.h
*
* @purpose   IPv4 Multicast specific defines
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
* we are mentioning these defines here.
* They are copied from linux2.6.14 source.
*********************************************************************/

#define MRT_BASE        200
#define MRT_INIT        (MRT_BASE)      /* Activate the kernel mroute code      */
#define MRT_DONE        (MRT_BASE+1)    /* Shutdown the kernel mroute           */
#define MRT_ADD_VIF     (MRT_BASE+2)    /* Add a virtual interface              */
#define MRT_DEL_VIF     (MRT_BASE+3)    /* Delete a virtual interface           */
#define MRT_ADD_MFC     (MRT_BASE+4)    /* Add a multicast forwarding entry     */
#define MRT_DEL_MFC     (MRT_BASE+5)    /* Delete a multicast forwarding entry  */
#define MRT_VERSION     (MRT_BASE+6)    /* Get the kernel multicast version     */
#define MRT_ASSERT      (MRT_BASE+7)    /* Activate PIM assert mode             */
#define MRT_PIM         (MRT_BASE+8)    /* enable PIM code      */

#endif /* OSAPI_MROUTE_H */
#endif /* _L7_OS_LINUX_ */

