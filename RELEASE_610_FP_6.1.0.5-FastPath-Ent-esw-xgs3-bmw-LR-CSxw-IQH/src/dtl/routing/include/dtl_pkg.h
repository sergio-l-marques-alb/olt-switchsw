/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dtl_pkg.h
*
* @purpose The purpose of this file is to have the prototypes for 
*          all dtl pakaging functions
*
* @component Device Transformation Layer
*
* @comments none
*
* @create 8/20/2001
*
* @author Sumanth Kanchireddy
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



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
L7_RC_t dtlIpRegPkg(); 

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
L7_RC_t dtlEndObjIpSend(L7_uint32 intIfNum, void *pEndObj);

