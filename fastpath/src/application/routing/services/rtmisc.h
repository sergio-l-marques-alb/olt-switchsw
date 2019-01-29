/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rtmisc.h
*
* @purpose Definition of the storage structure passed to an arp callback
*
* @component 
*
* @create 03/22/01
*
* @author alt
* @end
*
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/




/* Storage structure for an arp callback.
*/
typedef struct cbparam_s {
    L7_uint32 intIfNum;      /* internal interface number of the gateway */   
    L7_uint32 routerIP;      /* gateway IP Address */
    L7_uint32 srcrt;         /* indicates whether the packet is being forwarded */
}cbparam_t;





