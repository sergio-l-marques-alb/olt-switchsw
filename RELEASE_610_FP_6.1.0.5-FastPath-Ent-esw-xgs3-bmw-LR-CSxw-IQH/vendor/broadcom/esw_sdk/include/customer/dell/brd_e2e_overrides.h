/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
* @filename   brd_e2e_overrides.h
*
* @purpose    
*
* @component  andl component
*
* @comments   none
*
* @create     6/25/08
*
* @author     colinw
*
* @end
*             
**********************************************************************/

#ifndef __BRD_E2E_OVERRIDES_H__
#define __BRD_E2E_OVERRIDES_H__

/* Improve responsiveness of E2E Flow Control to the XON condition. */
#undef  E2E_FB_PKT_LIMIT
#define E2E_FB_PKT_LIMIT          2400
#undef  E2E_FB_CELL_LIMIT
#define E2E_FB_CELL_LIMIT         630
#undef  E2E_CELL_DISCARD_LIMIT
#define E2E_CELL_DISCARD_LIMIT    0x3FFF
#undef  E2E_PKTCELL_RESET_LIMIT
#define E2E_PKTCELL_RESET_LIMIT   0
#undef  E2E_MAXTIMER_DEFSEL
#define E2E_MAXTIMER_DEFSEL       0xb
#undef  E2E_MINTIMER_DEFSEL
#define E2E_MINTIMER_DEFSEL       1

#endif /* __BRD_E2E_OVERRIDES_H__ */
