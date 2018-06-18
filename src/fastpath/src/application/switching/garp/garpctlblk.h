/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garpctlblk.h
* @purpose     GARP API functions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/
#ifndef INCLUDE_GARP_CTL_BLK_H
#define INCLUDE_GARP_CTL_BLK_H


enum {garp_max_GPDU_len = 1540 };


/*******************/
/* GARP Attributes */
/*******************/


#define GARP_MAX_PORT_COUNT                   L7_MAX_INTERFACE_COUNT
#define GARP_MAX_VLAN_COUNT                   L7_MAX_VLANS
#define GARP_MAX_GMRP_COUNT                   1     /*SKTBD add gmrp*/

/* GARP command flags. */
#define GARP_GMRP_FWDALL                      0x00000001 /* "Forward All" for this GMRP */
#define GARP_GMRP_FWDUN                       0x00000002 /* "Forward Unregistered" for this GMRP */

/***************************************************************/
/* GARP command structure.  The given command/data is applied  */
/* to the application VLAN context on the given port.          */
/***************************************************************/
typedef struct
{
    GARPCommandNameTable    command;    /* GARP command type                        */
    GARPApplication         app;        /* GARP application type                    */
    L7_uchar8               data[8];    /* GARP command data (8 bytes)              */
    L7_uint32               vlan_id;    /* VLAN ID for command context (GMRP only)  */
    L7_uint32               port;       /* Port to apply the command to             */
    L7_uchar8               flags;      /* Flags (see descriptions below)           */

} GARPCommand;


/*---------------------------------------------------------------------------------*/
/* GARP control block     ****PRIVATE DATA ***********                             */
/* The Garp Control Block is used to communicate information between Garp and Xapi.*/
/*---------------------------------------------------------------------------------*/
typedef struct _dot1d_Garp_Control_Block_ {
    /* <Garp Control Data> */
    L7_BOOL GarpCB_garpReady;                    /* Garp ready indicator.        */
                                                 /*   True:  Garp is ready.      */
                                                 /*   False: Garp is not ready.  */
    void * GarpCB_initSyncSem;                   /* Syncronizes CCP and create   */
    L7_uint32 GarpCB_numPorts;                   /* Support up to this many port */

    /* Timers (time units in milliseconds */
    GARPTime GarpCB_joinTime;                    /* Controls the interval between*/
                                                 /*  transmitPDU events.         */
    GARPTime GarpCB_leaveTime;                   /* Controls the period of time  */
                                                 /*  that the Registar FSM will  */
                                                 /*  wait in the LV state.       */
    GARPTime GarpCB_leaveAllTime;                /* Controls the frequency with  */
                                                 /*  which the Leave All FSM     */
                                                 /*  generates Leave All PDUs.   */
    GARPTime GarpCB_leaveAllTimeCeiling;         /* Ceiling for leave all timeout*/

    /* Database */
    L7_uint32 GarpCB_gmrpMaxVars;                /* Maximun number of variables  */
                                                 /*  supported by GMRP.          */
    L7_uint32 GarpCB_gvrpMaxVars;                /* Maximun number of variables  */
                                                 /*  supported by GVRP.          */

    /* Garp Control Points */
    L7_int32 GarpCB_garpCCPTask;                 /* Garp CCP task handle         */

    /* Garp Application */
    L7_uint32 GarpCB_garpNumInstances;           /* Maximum number of Garp ins-  */
                                                 /*  tances.                     */
    L7_BOOL GarpCB_gmrpSupported;                /* Garp application support     */
    L7_BOOL GarpCB_gvrpSupported;                /*   True:  App. supported.     */
                                                 /*   False: App. not supported. */
    L7_BOOL GarpCB_gmrpEnabled;                  /* Garp application State       */
    L7_BOOL GarpCB_gvrpEnabled;                  /*   True:  App. Enabled.       */
                                                 /*   False: App. Disabled.      */
    /* <Garp Statistics method pointers> */
    void (*GarpCB_statsRxGvrpPdu)(L7_int32);          /* Num. GPDU received by GVRP   */
    void (*GarpCB_statsTxGvrpPdu)(L7_int32);          /* Num. GPDU xmitted by GVRP    */
    void (*GarpCB_statsRxGmrpPdu)(L7_int32);          /* Num. GPDU received by GMRP   */
    void (*GarpCB_statsTxGmrpPdu)(L7_int32);          /* Num. GPDU xmitted by GMRP    */
    void (*GarpCB_statsGvrpFailedReg)(L7_int32);      /* Num. GVRP failed registration*/
    void (*GarpCB_statsGmrpFailedReg)(L7_int32);      /* Num. GMRP failed registration*/
    void (*GarpCB_statsGpduDiscards)(L7_int32);       /* Num. GPDU discarded.         */
    void (*GarpCB_statsGarpCollisions)(void);         /* Num. Hash table collisions   */

    /* <Garp Methods Entry Points> */
                                                /* Create  Garp entry point     */
    GARPStatus (*GarpCB_create)(void *);
                                                /* STP informing its state ep   */
    GARPStatus (*GarpCB_linkStateChange)(GARPSpanState, GARPPort);

    /* L7_TRUE after NIM create startup is done */
    L7_BOOL createStartupDone;

    /* Bit mask of NIM events that Garp is registered to receive */
    PORTEVENT_MASK_t nimEventMask;

    /* Interface status change callback.
    */
    GARPStatus (*GarpCB_intfStateChange)(L7_uint32, L7_uint32, NIM_CORRELATOR_t);

                                                /* Xapi garp init done entry pt */
    void (*GarpCB_garpInitDone)(void);

} GARPControlBlock, *GARPCBptr;

#define GARP_GVRP_IS_ENABLED        GARPCB->GarpCB_gvrpEnabled
#define GARP_GMRP_IS_ENABLED        GARPCB->GarpCB_gmrpEnabled

#endif

