/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008-2009
*
**********************************************************************
*
* @filename dtl_l2_dot3ah.h
*
* @purpose This header file contains definitions to support the       
*          dot1ad component.      
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   Sengottuvelan Srirangan 04/14/2008
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTLCTRL_L2_DOT3AH_H
#define INCLUDE_DTLCTRL_L2_DOT3AH_H
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

L7_RC_t dtlDot3ahEnable(L7_uint32 intIfNum);
L7_RC_t dtlDot3ahDisable(L7_uint32 intIfNum);
L7_RC_t dtlDot3ahRemLbEnable(L7_uint32 intIfNum);
L7_RC_t dtlDot3ahRemLbDisable(L7_uint32 intIfNum);
L7_RC_t dtlDot3ahRemLbInit(L7_uint32 intIfNum);
L7_RC_t dtlDot3ahRemLbInitUni(void);
L7_RC_t dtlDot3ahRemLbExitUni(L7_uint32 intIfNum);
L7_RC_t dtlDot3ahRemLbExit(L7_uint32 intIfNum);
/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DTL INTERNAL USE ONLY
***********************************************************************
*/


#endif   /*INCLUDE_DTLCTRL_DOT3AH_H*/





