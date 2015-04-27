/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_init.h
*
* @purpose
*
* @component  PIM-DM
*
* @comments   none
*
* @create
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_INIT_H
#define _PIMDM_INIT_H

#include "l3_mcast_commdefs.h"
#include "pimdm_main.h"

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/
extern L7_RC_t
pimdmMemoryInit (L7_uchar8 addrFamily);

extern MCAST_CB_HNDL_t
pimdmCtrlBlockInit (L7_uchar8 addrFamily);

extern L7_RC_t
pimdmMemoryDeInit (L7_uchar8 addrFamily);

extern L7_RC_t
pimdmCtrlBlockDeInit (MCAST_CB_HNDL_t cbHandle);

extern void
pimdmCtrlBlockMembersShow (pimdmCB_t* pimdmCB);

#endif /* _PIMDM_INIT_H */
