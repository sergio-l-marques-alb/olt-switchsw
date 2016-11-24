/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_sm.h
*
* @purpose   Multiple Spanning tree State Machine header file
*
* @component dot1s
*
* @comments 
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_STATES_H
#define INCLUDE_DOT1S_STATES_H

typedef enum dot1sEvents_s
{
  /***************************************************************/
  /* Events just for Port Information Machine 			PIM */
  /***************************************************************/
  /*1.0*/pimBegin = 0,									/*E1.pim*/
  /*1.1*/pimRcvdMsg,                                      /*E2*/
  /*1.2*/pimUpdtInfo,                                     /*E3*/ /*this event is obselete as per draft 15*/
  /*1.3*/pimSupDsgInfo,                                   /*E4*/
  /*1.4*/pimRepDsgInfo,                                   /*E5*/
  /*1.5*/pimInferiorDesignatedInfo,                       /*E5A*/
  /*1.6*/pimRootAlternateInfo,                            /*E6*/
  /*1.7*/pimOtherInfo,                                    /*E7*/
  /*1.8*/pimRcvdXstMsgNotUpdtXstInfo,                     /*E8*/
  /*1.9*/pimSelectedUpdtInfo,                             /*E9*/
  /*1.10*/pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo,/*E10*/
  /*1.11*/pimPortEnabled,			                        /*E11*/
  /*1.12*/pimPortDisabledInfoIsNotEqualDisabled,          /*E12*/

  /*1.13*/dot1sPimEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Port Role Selection Machine 		PRS */
  /***************************************************************/
  /*2.0*/prsBegin,										/*E1.prs*/
  /*2.1*/prsReselect,                                    /*E13*/
  
  /*2.2*/dot1sPrsEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Port Role Transition Machine 		PRT */
  /***************************************************************/



	/*------------------------------------*/
 /*3.0*/prtBegin,										/*E1.prt*/


/*3.1.1*/prtDisabledPortRoleNotEqualSelectedRole,
/*3.1.2*/prtDisabledNotLearningNotForwarding,
/*3.1.3*/prtDisabledSync,
/*3.1.4*/prtDisabledReroot,
/*3.1.5*/prtDisabledNotSynced,
/*3.1.6*/prtDisabledFdWhileNotMaxAge,

/*3.1.7*/dot1sPrtDisabledEvents,/*keep this last in disabled sub group*/

/*3.2.1*/prtMasterPortRoleNotEqualSelectedRole,          /*E25*/
/*3.2.2*/prtMasterProposedNotAgree,                            /*E26*/
/*3.2.3*/prtMasterProposedAgree,                            /*E26*/
/*3.2.4*/prtMasterAllSyncedNotAgree,                           /*E29*/
/*3.2.5*/prtMasterNotLrnNotFwdNotSynced,       /*E30*/
/*3.2.6*/prtMasterAgreedNotSynced,                             /*E31*/
/*3.2.7*/prtMasterEdgeNotSynced,                               /*E32*/
/*3.2.8*/prtMasterSyncSynced,                                  /*E33*/
/*3.2.9*/prtMasterReRootRrWhileZero,                           /*E37*/
/*3.2.10*/prtMasterListen,/*need more explicit events*/         /*E38*/
/*3.2.11*/prtMasterLearn, /*need more explicit events*/         /*E39*/
/*3.2.12*/prtMasterForward,/*need more explicit events*/        /*E40*/

/*3.2.13*/dot1sPrtMasterEvents,/*keep this last in Mater sub group*/
	 
/*3.3.1*/prtDesigPortRoleNotEqualSelectedRole,           /*E24*/
/*3.3.2*/prtDesigNotFwdNotAgNotPropNotEdge,      /*E27*/
/*3.3.3*/prtDesigAllSynced,                           /*E29*/
/*3.3.4*/prtDesigNotLrnNotFwdNotSynced,       /*E30*/
/*3.3.5*/prtDesigAgreedNotSynced,                             /*E31*/
/*3.3.6*/prtDesigEdgeNotSynced,                               /*E32*/
/*3.3.7*/prtDesigSyncSynced,                                  /*E33*/
/*3.3.8*/prtDesigReRootRrWhileZero,                           /*E37*/
/*3.3.9*/prtDesigListen,/*need more explicit events*/         /*E38*/
/*3.3.10*/prtDesigLearn, /*need more explicit events*/         /*E39*/
/*3.3.11*/prtDesigForward,/*need more explicit events*/        /*E40*/

/*3.3.12*/dot1sPrtDesigEvents,/*keep this last in desig sub group*/

/*3.4.1*/prtRootPortRoleNotEqualSelectedRole,            /*E23*/
/*3.4.2*/prtRootProposedNotAgree,                            /*E26*/
/*3.4.3*/prtRootProposedAgree,                               /*E26*/
/*3.4.4*/prtRootAllSyncedNotAgree,                           /*E29*/
/*3.4.5*/prtRootAgreedNotSynced,                             /*E31*/
/*3.4.6*/prtRootSyncSynced,                                  /*E33*/
/*3.4.7*/prtRootNotFwdNotReRoot,                             /*E34*/
/*3.4.8*/prtRootRrWhileNotEqualFwd,                          /*E35*/
/*3.4.9*/prtRootReRootFwd,                                   /*E36*/
/*3.4.10*/prtRootLearn, /*need more explicit events*/         /*E39*/
/*3.4.11*/prtRootForward,/*need more explicit events*/        /*E40*/

/*3.4.13*/dot1sPrtRootEvents,/*keep this last in root sub group*/

/*3.5.1*/prtAlternatePortRoleNotEqualSelectedRole,       /*E15*/
/*3.5.2*/prtBackupPortRoleNotEqualSelectedRole,          /*E16*/
/*3.5.3*/prtAltbkProposedNotAgree,                            /*E26*/
/*3.5.4*/prtAltbkAllSyncedNotAgree,                           /*E29*/
/*3.5.5*/prtAltbkProposedAgree,                            /*E26*/
/*3.5.6*/prtAltbkNotLrngNotFwdg,                            /*E26*/
/*3.5.7*/prtAltbkRoleBkupRbWhile,                            /*E26*/
/*3.5.8*/prtAltbkSync,
/*3.5.9*/prtAltbkReroot,
/*3.5.10*/prtAltbkNotSynced,
/*3.5.11*/prtAltbkFdWhileNotFwdDelay,



/*3.5.12*/dot1sPrtAltBackupEvents,/*keep this last in altbkup sub group*/



  /*3.1*/dot1sPrtEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Port Receive Machine 				PRX */
  /***************************************************************/
  /*4.0*/prxBegin,											/*E1.prx*/
  /*4.1*/prxRcvdBpduPortDisabled,                        /*E41*/
  /*4.2*/prxRcvdBpduPortEnabledNotRcvdAnyMsg,            /*E42*/
  
  /*4.3*/dot1sPrxEvents,/*keep this last in sub group*/

  /***************************************************************/
  /* Events just for Port State Transition Machine 		PST */
  /***************************************************************/
  /*5.0*/pstBegin,											/*E1.pst*/
  /*5.1*/pstLearn,                                       /*E43*/
  /*5.2*/pstNotLearn,                                    /*E44*/
  /*5.3*/pstForward,                                     /*E45*/
  /*5.4*/pstNotForward,                                  /*E46*/

  /*5.5*/dot1sPstEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Topology Change Machine 			TCM */
  /***************************************************************/
  /*6.0*/tcmBegin,											/*E1.tcm*/
  /*6.1*/tcmRcvdTc,                                      /*E47*/
  /*6.2*/tcmRcvdTcn,                                     /*E48*/
  /*6.3*/tcmRcvdTcAck,                                   /*E49*/
  /*6.4*/tcmTcProp,                                      /*E50*/
  /*6.5*/tcmRoleRootFwdNotEdge,                          /*E51*/
  /*6.6*/tcmRoleDesigFwdNotEdge,                         /*E52*/
  /*6.7*/tcmRoleMasterFwdNotEdge,                        /*E53*/
  /*6.8*//*tcmEdge,*//*this event is removed as per D14*//*E54*/
  /*6.9*/tcmRoleNotRootNotDesigNotMaster,                /*E55*/ 

  /*6.10*/dot1sTcmEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Port Protocol Migration Machine 	PPM */
  /***************************************************************/
  /*65*//*ppmBegin,									*/		/*E1.ppm*/
  /*66*//*ppmPortDisabledNotInitPm,                   */    /*E56*/
  /*67*//*ppmForceVer2PortEnabled,                    */    /*E57*/
  /*68*//*ppmMDelayWhileNotZeroRcvdStp,               */    /*E58*/
  /*69*//*ppmMDelayWhileNotZeroRcvdRstp,              */    /*E59*/
  /*70*//*ppmForceVer01PortEnabled,                   */    /*E60*/
  /*71*//*ppmMDelayWhileZeroRcvdRstp,                 */    /*E61*/
  /*72*//*ppmMCheck,                                  */    /*E62*/
  /*73*//*ppmForceVer01,                              */    /*E63*/
  /*74*//*ppmMDelayWhileZeroRcvdStp,                  */    /*E64*/
  
  /**************************************************************************/
  /******************state machine events for the PPM in 802.1y**************/
  /**************************************************************************/
  /*7.0*/ppmBegin,											/*E1.tcm*/
  /*7.1*/ppmNotPortEnabledmDelayWhileNotMigrateTime,         /*E56*/
  /*7.2*/ppmMDelayWhileZero,	    							/*E57*/
  /*7.3*/ppmPortDisabled,		    						/*E58*/
  /*7.4*/ppmMCheck,					    					/*E59*/
  /*7.5*/ppmSendRSTPRcvdStp,			    					/*E60*/
  /*7.6*/ppmRstpVersionNotSendRstpRcvdSTP,					/*E61*/
  /*Currently Events 62 to 64 are left unused for the implementaion in .1y*/
  /**************************************************************************/
  /**************************************************************************/

  /*7.7*/dot1sPpmEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Port Transmit Machine 				PTX */
  /***************************************************************/
  /*8.0*/ptxBegin,											/*E1.ptx*/
  /* all events in this sub group qualified by "&& selected && !updtInfo"       */
  /*8.1*/ptxHelloWhenZero,                               /*E65*/
  /*8.2*/ptxSendRstp,/*need more explicit events*/       /*E66*/
  /*8.3*/ptxSendTcn, /*need more explicit events*/       /*E67*/
  /*8.4*/ptxSendConfig,/*need more explicit events*/     /*E68*/
  
  /*8.5*/dot1sPtxEvents,/*keep this last in sub group*/

  /***************************************************************/
  /* Events just for Bridge Detection Machine (802.1t 18.2) BDM */
  /***************************************************************/
  /*9.0*/bdmBeginAdminEdge,                                       /*E1.bdm*/
  /*9.1*/bdmBeginNotAdminEdge,                                       /*E1.bdm*/
  /*9.2*/bdmNotPortEnabledNotAdminEdge,                  /*E69*/	/*link up/dn*/
  /*9.3*/bdmNotOperEdge,                                 /*E70*/
  /*9.4*/bdmNotPortEnabledAdminEdge,
  /*9.5*/bdmEdgeDelayWhileZero,
  
  /**************************************************************************/
  /******************state machine events for the BDM in 802.1y**************/
  /**************************************************************************/
  /**//*bdmBegin,*/											/*E1.bdm*/
  /*77*//*bdmBeginAdminEdge,  */	                        /*E69*/
  /*78*//*bdmBeginNotAdminEdge, */  	                    /*E70*/
  /*79*//*bdmPortDisabledNotAdminEdge,*/					/*E71*/
  /*80*//*bdmNotOperEdge,				*/					/*E72*/
  /*81*//*bdmNotMigrateTimeNotProposingEtc,*/				/*E73*/
  /*82*//*bdmNotPortEnabledAdminEgdeOredgeDelayZeroEtc,*//*E74*/
  /**************************************************************************/
  /**************************************************************************/

  /*9.5*/dot1sBdmEvents, /*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Port Timers Machine 				PTM */
  /***************************************************************/
  /*10.0*/ptmBegin,										     /*E1.ptm*/
  /*10.1*/ptmTick,                                         /*E75*/

  /*10.2*/dot1sPtmEvents, /*keep this last in sub group*/
          dot1sEvents_lastEvent /* Keep this last in the enum define */

}dot1sEvents_t;

#define	DOT1S_VERSION_3_OFFSET	36
#define	DOT1S_VERSION_1_OFFSET	35
typedef enum
{
  PIM_DISABLED = 0,
  PIM_AGED,
  PIM_UPDATE,
  PIM_CURRENT,
  PIM_OTHER,
  PIM_NOT_DESIG,
  PIM_REP_DESIG,
  PIM_SUP_DESIG,
  PIM_RECEIVE,
  PIM_INFERIOR_DESIG,
  PIM_STATES


}dot1s_pim_states_t;

typedef enum
{
  PRS_INIT = 0,
  PRS_RECEIVE,
  PRS_STATES

}dot1s_prs_states_t;

typedef enum
{
  PRT_INIT_PORT = 0,
  PRT_DISABLE_PORT,
  PRT_DISABLED_PORT,
  PRT_DISABLED_STATES

}dot1s_prt_disabledStates_t;

typedef enum
{
  PRT_MASTER_FIRST = 10,
  PRT_MASTER_PROPOSED,
  PRT_MASTER_AGREED,
  PRT_MASTER_SYNCED,
  PRT_MASTER_RETIRED,
  PRT_MASTER_PORT,
  PRT_MASTER_DISCARD,
  PRT_MASTER_LEARN,
  PRT_MASTER_FORWARD,
  PRT_MASTER_STATES

}dot1s_prt_masterStates_t;


typedef enum
{
  PRT_DESIG_FIRST = 20,
  PRT_DESIG_PROPOSE,
  PRT_DESIG_AGREED,
  PRT_DESIG_SYNCED,
  PRT_DESIG_RETIRED,
  PRT_DESIG_PORT,
  PRT_DESIG_DISCARD,
  PRT_DESIG_LEARN,
  PRT_DESIG_FORWARD,
  PRT_DESIG_STATES

}dot1s_prt_desigStates_t;


typedef enum
{
  PRT_ROOT_FIRST = 30,
  PRT_ROOT_PROPOSED,
  PRT_ROOT_AGREED,
  PRT_ROOT_SYNCED,
  PRT_REROOT,
  PRT_ROOT_PORT,
  PRT_REROOTED,
  PRT_ROOT_LEARN,
  PRT_ROOT_FORWARD,
  PRT_ROOT_STATES

}dot1s_prt_rootStates_t;

typedef enum
{
  PRT_ALTBKUP_FIRST = 40,
  PRT_ALTERNATE_PROPOSED,
  PRT_ALTERNATE_AGREED,
  PRT_BLOCK_PORT,
  PRT_BACKUP_PORT,
  PRT_ALTERNATE_PORT,
  PRT_ALTBKUP_STATES

}dot1s_prt_altBkupStates_t;


typedef enum
{
  PRX_DISCARD = 0,
  PRX_RECEIVE,
  PRX_STATES


}dot1s_prx_states_t;

typedef enum
{

  PST_DISCARDING = 0,
  PST_LEARNING,
  PST_FORWARDING,
  PST_STATES

}dot1s_pst_states_t;

typedef enum
{
  TCM_INACTIVE = 0,
  TCM_LEARNING,
  TCM_DETECTED,
  TCM_ACTIVE,
  TCM_NOTIFIED_TCN,
  TCM_NOTIFIED_TC,
  TCM_PROPAGATING,
  TCM_ACKNOWLEDGED,
  TCM_STATES


}dot1s_tcm_states_t;

/*the states below are being redefined in IEEE 802.1y*/
/*
typedef enum
{
  PPM_INIT = 0,
  PPM_SEND_RSTP,
  PPM_SENDING_RSTP,
  PPM_SEND_STP,
  PPM_SENDING_STP,
  PPM_STATES

}dot1s_ppm_states_t;
*/

typedef enum
{
  PPM_CHECKING_RSTP = 0,
  PPM_SELECTING_STP,
  PPM_SENSING,
  PPM_STATES

}dot1s_ppm_states_t;



typedef enum
{
 PTX_INIT = 0,
 PTX_PERIODIC,
 PTX_IDLE,
 PTX_RSTP,
 PTX_TCN,
 PTX_CONFIG,
 PTX_STATES

}dot1s_ptx_states_t;

/*the states below are being redefined in IEEE 802.1y*/
typedef enum
{
  BDM_EDGE = 0,
  BDM_NOT_EDGE,
  BDM_STATES
}dot1s_bdm_states_t;

/*The new states in IEEE 802.1y are the following*/
/*
typedef enum
{
  BDM_EDGE = 0,
  BDM_NOT_EDGE,
  BDM_STATES

}dot1s_bdm_states_t;
*/
typedef enum
{
 PTI_TICK = 0,
 PTI_ONE_SECOND,
 PTI_STATES

}dot1s_pti_states_t;

typedef enum
{
  BPDU_TYPE_CONFIG = 0,
  BPDU_TYPE_TCN,
  BPDU_TYPE_RSTP,
  BPDU_TYPE_MSTP,
  BPDU_TYPE_INVALID   /* Keep this last : also used for BPDU type not supported*/
}dot1s_bpdu_types_t;


#define DOT1S_DEBUG_STATE_PRINT(__dot1sEvent, __p, __instIndex, __depth) SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s(%d) : Port %d inst(%d) prx state (%d) depth %d\n",#__dot1sEvent, __dot1sEvent, (__p)->portNum, __instIndex, (__p)->prxState, __depth) 

#endif /*INCLUDE_DOT1S_STATES_H*/
