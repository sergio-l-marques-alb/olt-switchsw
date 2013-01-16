/*
 * $Id: sysbrds.h,v 1.12.10.2 2003/10/02 03:17:51 brb Exp $
 * $Copyright: (c) 2002, 2003 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:        sysbrds.h
 * Purpose:
 */

#ifndef   _BCMSYS_SYSBRDS_H_
#define   _BCMSYS_SYSBRDS_H_

#include <soc/defs.h>
#include "hpc_db.h"
#include "dapi_db.h"

#define SOC_BOARD_DEV_MAX      SOC_MAX_NUM_DEVICES

/*
 * Enumerated type of all known boards
 */
typedef enum bcm_sys_board_ids_e {
    BCM_SYS_BOARD_INACTIVE = -1,

    BCM_SYS_BOARD_56014_48,
    BCM_SYS_BOARD_56214_48,
    BCM_SYS_BOARD_56214,
    BCM_SYS_BOARD_56218,

    BCM_SYS_BOARD_53718,
    BCM_SYS_BOARD_53724,
    BCM_SYS_BOARD_53314,
    BCM_SYS_BOARD_56024K24T,
    BCM_SYS_BOARD_56224,
  	BCM_SYS_BOARD_56224_48,
    BCM_SYS_BOARD_56228,
    BCM_SYS_BOARD_56304,
    BCM_SYS_BOARD_56304_P48,
    BCM_SYS_BOARD_56314,

    BCM_SYS_BOARD_56314_P48,
    BCM_SYS_BOARD_56504,
    BCM_SYS_BOARD_56504_P48,
    BCM_SYS_BOARD_56514,
    BCM_SYS_BOARD_56514_P48,

    BCM_SYS_BOARD_56624,
    BCM_SYS_BOARD_56680,
    BCM_SYS_BOARD_56685,    /* PTin added: new platform */
    BCM_SYS_BOARD_56689,    /* PTin added: new platform */
    BCM_SYS_BOARD_56800,
    BCM_SYS_BOARD_56820,
    BCM_SYS_BOARD_56843,    /* PTin added: new platform BCM56843 (trident) */
    BCM_SYS_BOARD_56844,    /* PTin added: new platform BCM56844 (trident) */
    BCM_SYS_BOARD_56524,
    BCM_SYS_BOARD_56634,
    BCM_SYS_BOARD_53115,        /* ROBO - Vulcan */
    BCM_SYS_BOARD_56636,
    BCM_SYS_BOARD_56538,
    BCM_SYS_BOARD_56334,    
BCM_SYS_BOARD_COUNT         /* Last please */
} bcm_sys_board_ids_t;

typedef struct bcm_sys_board_s bcm_sys_board_t;

/*
 * Typedef:
 *      bcm_sys_id_board_f
 * Purpose:
 *      Boolean function to identify a board
 */

typedef int (*bcm_sys_id_board_f)(const bcm_sys_board_t *brd);

/*
 * Typedef:
 *      bcm_sys_sa_init_f
 * Purpose:
 *      Board initialization  function for stand alone
 */

typedef int (*bcm_sys_sa_init_f)(const  bcm_sys_board_t *brd, int base);

/*
 * Typedef:
 *      bcm_sys_pre_stack_init_f
 * Purpose:
 *      Board initialization function for pre stacking
 */

typedef int (*bcm_sys_pre_stack_init_f)(const bcm_sys_board_t *brd);

/*
 * FLAGS for stack port connections
 *   LINK        Link is up for this connection.  Indicated by source CPU
 *   SIMPLEX     Simplex stacking indicated.  From top level configuration
 *   REDUNDANT   Link is redundant.  Should not be enabled on bringup.
 *               This is indicated by discovery/db processing
 */

/* This is a stackable port description */
typedef struct bcm_sys_port_s {
    int unit;        /* BCM unit number */
    int pport;       /* Physical port */
    uint32 flags;    /* Flags for this port */
#define BCM_STK_PORT_F_LINK          0x1
#define BCM_STK_PORT_F_SIMPLEX       0x2
#define BCM_STK_PORT_F_REDUNDANT     0x4
} bcm_sys_port_t;

#define BCM_STK_PORT_SIMPLEX(bsp)  ((bsp)->flags & BCM_STK_PORT_F_SIMPLEX)
#define BCM_STK_PORT_DUPLEX(bsp)   (!BCM_STK_PORT_SIMPLEX(bsp))
#define BCM_STK_PORT_LINK(bsp)     ((bsp)->flags & BCM_STK_PORT_F_LINK)

#define L7_12G_INTERCONNECT 12
#define L7_10G_INTERCONNECT 10
#define L7_MAX_CONNECTIONS_LIST 16

typedef struct {
  int from_unit;
  int from_port;
  int to_unit;
  int to_port;  /* just a convenience variable to show the symetry */
  int speed;
} l7_connections_t;

#define NO_CONNECTION_LIST 0,{}
/*
 * Typedef
 *      bcm_stk_board_t
 * Purpose
 *      Stackable board description.
 * Notes
 *      Regarding module ids, if the CPU will receive packets thru
 *      a particular unit, that unit must have a module id (so that
 *      other chips in the system can send packets to that unit.)
 *      Thus, a Hercules doesn't need a mod id if it is on a board
 *      with Dracos (so packets can go from the Draco to the CPU).
 *      But a board with just a Hercules requires a mod id if the
 *      CPU on that board is to receive packets from front panel or
 *      more than a hop away.
 *
 *      In general, module ids will be successive in a given box.
 *      However, with hot swapping and the desire to keep existing
 *      module ids unchanged (so that tables don't need to be
 *      recalculate and rewritten) it's conceivable there will
 *      be jumps in mod ids.  Thus we keep mod_ids an array below.
 */

struct bcm_sys_board_s {
    bcm_sys_board_ids_t      sys_brd_id;     /* Board id for id funs */
    int                      num_units;
    int                      dev_id[SOC_BOARD_DEV_MAX];
    bcm_sys_id_board_f       brd_id;         /* Board ID function */
    bcm_sys_sa_init_f        sa_init;        /* Standalone init */
    bcm_sys_pre_stack_init_f stk_init;       /* Setup for stacking */
    L7_uint32                unit_id;
    L7_int32                 npd_id;
    L7_uint32                num_interconnects; /* num of elements in interconnect_list */
    l7_connections_t         interconnect_list[L7_MAX_CONNECTIONS_LIST]; /* mapping of interconnect */
};

/* Supported board list */
extern bcm_sys_board_t bcm_sys_boards[];

/* Board Discovery */
const bcm_sys_board_t* hpcBoardGet(void);
extern bcm_sys_board_t *hpcBoardFind(int id);

/* Board Configuration */
/*********************************************************************
* @purpose  Common driver configuration.
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t hpcConfigSet(void);

/*********************************************************************
* @purpose  Board specific driver configuration.
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t hpcConfigBoardSet(void);

L7_RC_t hpcConfigPhySet(void);

/*********************************************************************
* @purpose  Board specific PHY configuration
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments these ports are referred to as XPORTS
*
* @end
*********************************************************************/
L7_RC_t hpcConfigBoardPhySet(int portNo, L7_uint32 cardTypeId,
                             HAPI_CARD_SLOT_MAP_t *hapiSlotMapPtr);

/*********************************************************************
* @purpose  Board specific PHY Post Setup configuration
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments these ports are referred to as XPORTS
*
* @end
*********************************************************************/
L7_RC_t hpcConfigBoardPhyPostSet(int portNo, L7_uint32 cardTypeId,
                             HAPI_CARD_SLOT_MAP_t *hapiSlotMapPtr,L7_uint32 phyCap);


/*********************************************************************
* @purpose  Board specific plugin module detection.
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If applicable, this function should detect plugin
*           modules and call hpcStackPortSet() as appropriate.
*
* @end
*********************************************************************/
#if L7_FEAT_PLUGIN_STACKING_MODULE
L7_RC_t hpcBoardPluginModulesDetect();
#define HPC_BOARD_PLUGIN_MODULES_DETECT() hpcBoardPluginModulesDetect()
#else
#define HPC_BOARD_PLUGIN_MODULES_DETECT() (L7_SUCCESS)
#endif

/*********************************************************************
* @purpose  Board specific board ID detection to be used in bcm_sys_boards[].
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
int hpcBoardIdCheck(const bcm_sys_board_t *brd);



/* Default system board id function */
extern int bcm_sys_id_defl(const bcm_sys_board_t *brd);
extern int bcm_sys_sa_init_defl(const bcm_sys_board_t *brd, int base);
extern int bcm_sys_pre_stack_defl(const bcm_sys_board_t *brd);

/*********************************************************************
* @purpose  Tell the Broadcom driver which ports are HG vs XE ports
*
* @param    lclUnitDesc - The descriptor with the info for the board
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments these ports are referred to as XPORTS
*
* @end
*********************************************************************/
L7_RC_t hpcXeHgSetup(void);

/*********************************************************************
* @purpose  Tell the Broadcom driver which ports are HL vs GE ports
*
* @param    lclUnitDesc - The descriptor with the info for the board
*                                       
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t hpcGeHlSetup(void);

/*********************************************************************
* @purpose  Does box specific Phy settings
*
* @param
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @end
*********************************************************************/
#ifdef LVL7_DNI8541
extern L7_RC_t hpcBoardPhyInit(void);
#define HPC_BROAD_PHY_INIT()    hpcBoardPhyInit()
#else
#define HPC_BROAD_PHY_INIT()
#endif

#endif /* _BCMSYS_SYSBRDS_H_ */
