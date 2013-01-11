/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  broad_topo.h
*
* @purpose   This file is for FASTPATH generic topology code
*
* @component hapi
*
* @comments
*
* @create    6/1/2008
*
* @author    mbaucom
*
* @end
*
**********************************************************************/
#ifndef _BROAD_TOPO_H
#define _BROAD_TOPO_H

#include <appl/stktask/topology.h>
#include <appl/cpudb/cpudb.h>
#include <soc/types.h>


typedef int (*cust_topo_brd_f)(cpudb_ref_t db_ref, topo_cpu_t *topo_cpu,int *rv);


/*
 * Function:
 *      topo_board_program_register
 * Purpose:
 *      Customer can register a topo routine to handle the board topology programming 
 *      
 * Parameters:
 *      func      - the function to be registered.  It must set the return code in rv
 * 
 * Returns:  N/A
 * Notes: The registered function must set the rv and 
 *        return either 1 for handled or 0 for unhandled 
 *        refer to lvl7_topo_board_program for an example
 */
extern void topo_board_program_register(cust_topo_brd_f func);

/*
 * Function:
 *      lvl7_topo_board_program
 * Purpose:
 *      Program a board given local topology info
 *      Should work simular to the code in topo_board_program
 * Parameters:
 *      db_ref    - Data base of current configuration
 *      topo_cpu  - Info for local board programming
 *      rv - return code (BCM_E_XXX)
 * Returns:
 *      0 - If no board found
 *      1 - If the board was handled
 * Notes:
 */
extern int lvl7_topo_board_program(cpudb_ref_t db_ref, topo_cpu_t *topo_cpu,int *rv);


/*
 * Function:
 *      lvl7_topo_interconnect
 * Purpose:
 *      Remove internal connections for the portmap and set the config parms for speed
 *      
 * Parameters:
 *      pbmp_xport_xe    - IN/OUT - the 10g port bitmap to be modified based on interconnect
 * Returns: 
 *      Void
 */
extern void  lvl7_topo_interconnect(pbmp_t  *pbmp_xport_xe);

#endif
