/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_pkg.c
*
* Purpose: This file contains the pakaging functions between 201 and 300.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Sumanth Kanchireddy 8/20/2001 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/
#include "dtlinclude.h"
#include "dtl_pkg.h"


/*********************************************************************
* @purpose  Package function for registering for IP and ARP packets
* @purpose  with sysnet 
*          
* @param 	  none
*
* @returns  L7_SUCCESS    If registration is successful 
* @returns  L7_FAILURE    If registration is not successful
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlIpRegPkg()
{
  L7_RC_t rc = L7_SUCCESS;
#ifndef L7_IPV6_PACKAGE
  sysnetNotifyEntry_t snEntry;

  LOG_INFO(LOG_CTX_STARTUP,"Going to register dtlRecvIP6 related to type=%u, protocol_type=%u: 0x%08x",
           SYSNET_ETHERTYPE_ENTRY, L7_ETYPE_IPV6, (L7_uint32) dtlRecvIP6);

  bzero((char *)&snEntry, sizeof(sysnetNotifyEntry_t));
  strcpy(snEntry.funcName, "dtlRecvIP6");
  snEntry.notify_pdu_receive = dtlRecvIP6;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_IPV6;
  rc = sysNetRegisterPduReceive(&snEntry);
  if (rc == L7_FAILURE)
  {
    return rc;
  }
  
#endif

  return rc;
}

/*********************************************************************
* @purpose  Finds END_OBJ corresponding to an interface
*
* @param          none
*
* @returns  L7_SUCCESS    If able to obtain END_OBJ
* @returns  L7_FAILURE    If unable to obtain END_OBJ
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlEndObjIpSend(L7_uint32 intIfNum, void *pEndObj)
{
#ifdef L7_OS_VXWORKS
  if(l3EndObjIpSend(intIfNum, pEndObj) != L7_SUCCESS)
    return L7_FAILURE;
  else
#endif
    return L7_SUCCESS;
}

