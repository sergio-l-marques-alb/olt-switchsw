/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_l2_fdb.h
*
* @purpose This header file contains definitions to support the       
*          L2 MAC component.      
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   Shekhar Kalyanam 3/14/2001
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTLCTRL_L2_MAC_H
#define INCLUDE_DTLCTRL_L2_MAC_H
/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/



/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/


/*
**********************************************************************
*                           GLOBAL DATA
**********************************************************************
*/
#ifdef  DTLCTRL_L2_MAC_GLOBALS
  #define DTLCTRL_L2_MAC_EXT
#else 
  #define DTLCTRL_L2_MAC_EXT extern
#endif  /*DTLCTRL_L2_MAC_GLOBALS*/




/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DTL INTERNAL USE ONLY
***********************************************************************
*/
/*********************************************************************
* @purpose  Learned entry callback from received frames
*
* @param    ddusp          @b{(input)}device driver reference to unit slot and port
* @param	  family         @b{(input)}Device Driver family type 
* @param    cmd            @b{(input)}command of type DAPI_CMD_t
* @param    event          @b{(input)}event of type DAPI_EVENT_t
* @param    dapiEventInfo  @b{(input)}pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlFdbReceive(DAPI_USP_t *ddusp, 
                      DAPI_FAMILY_t family, 
                      DAPI_CMD_t cmd, 
                      DAPI_EVENT_t event,
                      void *dapiEventInfo);

#endif     /*INCLUDE_DTLCTRL_L2_MAC_H*/
