/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_db.h
*
* @purpose   Multiple Spanning tree data structures
*
* @component dot1s
*
* @comments 
*
* @create    8/13/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_DB_H
#define INCLUDE_DOT1S_DB_H


typedef struct dot1s_usp_s
{
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
} DOT1S_USP_t;


/* Determines how many recurring state machine events can occur. A higher 
value can lead to stack corruption*/
#define DOT1S_MAX_STATEMACHINE_EVENTS_DEPTH  \
                (DOT1S_THRESHOLD_STATEMACHINE_EVENTS_DEPTH + \
                DOT1S_STATEMACHINE_EVENTS_BUF)

/*this is a 4 byte path cost value*/
typedef	L7_uint32	DOT1S_PATHCOST_t; 


/*the bridge id is a 8 byte value. the 4 most significant bits represent,
  the 4 most significant bits of the bridge priority(settable in jumps of 4096
  thus ensuring the the remaing 12 bits of the priority (a 16 bit value) are
  always 0s) , the next 12 bits represent the instance id, encoded as 0 for 
  the CIST and the MSTID for the MSTIs. the next 6 bytes represent the 
  mac address of the bridge*/
typedef struct
{
  L7_ushort16 	priInstId; /*4bits of priority and 12 bits of instance id*/ 
  L7_uchar8		macAddr[L7_MAC_ADDR_LEN];
}DOT1S_BRIDGEID_t;                         /*8 byte*/


/*the port id is a 2 byte value. the 4 most significant bits represent
 the 4 most significant bits of the port priority (a 8 bit  
 value that is settable in jumps of 16thus ensuring that the 4 remaining
 bits in the priority are always 0s, 
 the next 12 bits represent the port number of the port.*/
typedef	L7_ushort16	DOT1S_PORTID_t;


typedef struct dot1s_cist_priority_vector_s
{
  DOT1S_BRIDGEID_t	rootId;            /*8 byte*/
  DOT1S_PATHCOST_t	extRootPathCost;   /*4 byte*/
  DOT1S_BRIDGEID_t	regRootId;         /*8 byte*/
  DOT1S_PATHCOST_t	intRootPathCost;   /*4 byte*/
  DOT1S_BRIDGEID_t	dsgBridgeId;       /*8 byte*/
  DOT1S_PORTID_t	dsgPortId;         /*2 byte*/
  DOT1S_PORTID_t	rxPortId;          /*2 byte*/

}DOT1S_CIST_PRIORITY_VECTOR_t;             /*36 byte*/

typedef struct dot1s_msti_priority_vector_s
{
  DOT1S_BRIDGEID_t	regRootId;         /*8 byte*/
  DOT1S_PATHCOST_t	intRootPathCost;   /*4 byte*/     
  DOT1S_BRIDGEID_t	dsgBridgeId;       /*8 byte*/
  DOT1S_PORTID_t	dsgPortId;         /*2 byte*/
  DOT1S_PORTID_t	rxPortId;          /*2 byte*/

}DOT1S_MSTI_PRIORITY_VECTOR_t;              /*24 byte*/

  typedef struct dot1s_cist_bridge_times_s
{
  L7_uint32 	    bridgeFwdDelay;
  L7_uint32			bridgeHelloTime;
  L7_uint32			bridgeMaxAge;

  L7_uint32			msgAge;/*encode as zero*/
  L7_uint32 	    maxHops;


}DOT1S_CIST_BRIDGE_TIMES_t;                      /*20 byte*/

typedef struct dot1s_msti_bridge_times_s
{
  L7_uint32    		bridgeMaxHops;
                                           /*4 byte*/
}DOT1S_MSTI_BRIDGE_TIMES_t;

typedef struct dot1s_cist_times_s
{
  L7_uint32 		msgAge;
  L7_uint32			maxAge;
  L7_uint32			fwdDelay;
  L7_uint32			helloTime;
  L7_uint32			remHops;

}DOT1S_CIST_TIMES_t;					/*20 bytes*/

typedef struct dot1s_msti_times_s
{
  L7_uint32			remHops;

}DOT1S_MSTI_TIMES_t;					 /*4 bytes*/


/* The size of this structure should be fixed
   This structure interacts with PDU which has fixed the configId field 
   length to DOT1S_BPDU_CONFIG_ID_LEN.
   The size of this structure should be 1 + DOT1S_BPDU_CONFIG_ID_LEN to account for padding .
*/
typedef struct dot1s_mst_config_id_s
{
  L7_uchar8			pad[1];/*SKTBD accomodate while transmitting*/
  L7_uchar8			formatSelector;
  L7_uchar8			configName[DOT1S_MAX_CONFIG_NAME_SIZE];
  L7_ushort16		revLevel;
  L7_uchar8			configDigest[DOT1S_CONFIG_DIGEST_SIZE];

}DOT1S_MST_CONFIG_ID_t;					  /*52 bytes*/


/*common bridge parms accross all STPs*/
typedef struct dot1s_bridge_s
{
 L7_BOOL 			Mode;
 DOT1S_FORCE_VERSION_t 			ForceVersion;
 L7_uint32 			FwdDelay;
 L7_uint32 	        TxHoldCount;
 L7_uint32 			MigrateTime;/*in secs*/
 L7_uint32 			MaxHops;
 L7_uint32			instanceCount;
 L7_uint32			enabledPortCount;

 L7_uint32 			Begin;
 DOT1S_MST_CONFIG_ID_t 			MstConfigId;

 L7_BOOL            bpduGuard;
 L7_BOOL            bpduFilter; /* global filter mode */
}DOT1S_BRIDGE_t;

/*parms that are for the cist alone*/
typedef struct dot1s_cist_s
{
 L7_uchar8 		        		cistid;/*should be zero*/
 
 DOT1S_BRIDGEID_t 	        	BridgeIdentifier;

 DOT1S_CIST_PRIORITY_VECTOR_t	CistBridgePriority;
 DOT1S_CIST_BRIDGE_TIMES_t 		CistBridgeTimes;
 DOT1S_PORTID_t				    cistRootPortId;
 DOT1S_CIST_PRIORITY_VECTOR_t	cistRootPriority;
 DOT1S_CIST_TIMES_t				cistRootTimes;

 L7_uint32                      allSyncedRequested;      /* intIfNum of the port which has requested allSynced. 
                                                            0 by default */
 L7_uint32                      syncedPending;      
 L7_uint32                      reRootedRequested;
 L7_uint32                      reRootedPending;

 L7_uchar8						prsState; /* dot1s_prs_states_t */
 L7_uint32                      tcWhileChange;/*counter*/
 L7_uint32                      tcWhileChangeTime;
                                            
}DOT1S_CIST_t;                              /*134 byte*/

/*parms that are for mstis only NOT for cist-- do not use index 0 if this typedef is to be an array*/
typedef struct dot1s_msti_s
{
 L7_uchar8 		        		mstid;

 DOT1S_BRIDGEID_t 				BridgeIdentifier;

 DOT1S_MSTI_PRIORITY_VECTOR_t	MstiBridgePriority;
 DOT1S_MSTI_BRIDGE_TIMES_t 		MstiBridgeTimes;
 DOT1S_PORTID_t				    mstiRootPortId;
 DOT1S_MSTI_PRIORITY_VECTOR_t	mstiRootPriority;
 DOT1S_MSTI_TIMES_t				mstiRootTimes;

 L7_uint32                      allSyncedRequested;      /* intIfNum of the port which has requested allSynced. 
                                                            0 by default */
 L7_uint32                      syncedPending;      
 L7_uint32                      reRootedRequested;
 L7_uint32                      reRootedPending;

 L7_uchar8						prsState; /* dot1s_prs_states_t */
 L7_uint32                      tcWhileChange;/*counter*/
 L7_uint32                      tcWhileChangeTime;

} DOT1S_MSTI_t;                             /*82 byte*/

typedef enum
{
  INFO_RECEIVED = 0,
  INFO_MINE,
  INFO_AGED,
  INFO_DISABLED

}DOT1S_INFO_t;

typedef enum
{
  SAME = 0,
  BETTER,
  WORSE
}DOT1S_PRI_COMP_t;

typedef enum
{
  SuperiorDesignatedInfo = 0,
  RepeatedDesignatedInfo,
  InferiorDesignatedInfo,
  InferiorRootAlternateInfo,
  OtherInfo

}DOT1S_RCVD_INFO_t;

/* Port Roles
 * Note: Any modifications to this will also require
 * modifications to the L7_DOT1S_PORT_ROLE_t enum in
 * commdefs.h
 */
typedef enum
{
  ROLE_DISABLED = 0,
  ROLE_ROOT,
  ROLE_DESIGNATED,
  ROLE_ALTERNATE,
  ROLE_BACKUP,
  ROLE_MASTER

}DOT1S_PORT_ROLE_t;



/*this structure has an instance per CIST and each of the MSTIs*/
typedef struct dot1s_port_s
{
  L7_uint32 				stPortNum; /*Spanning Tree port Number*/
  /*per port per instance timer*/
  L7_uint32				fdWhile;/*counter*/
  L7_uint32				rrWhile;/*counter*/
  L7_uint32				rbWhile;/*counter*/
  L7_uint32				tcWhile;/*counter*/
  L7_uint32				rcvdInfoWhile;/*counter*/

  /*per port defined for CIST and All MISTs*/
  L7_uint32 			InternalPortPathCost;
  DOT1S_INFO_t			infoIs;
  L7_uint32				ExternalPortPathCost;

  /*per port defined only for the CIST*/
  L7_uchar8             autoExternalPortPathCost:1;
  L7_uchar8             autoInternalPortPathCost:1;

  L7_uchar8 			forward:1;   /*admin*/
  L7_uchar8				forwarding:1;/*oper */
  L7_uchar8 			learn:1;   /*admin*/
  L7_uchar8				learning:1;/*oper */
  L7_uchar8				proposed:1;
  L7_uchar8				proposing:1;
  L7_uchar8 			rcvdTc:1;
  L7_uchar8				reRoot:1;
  L7_uchar8				reselect:1;
  L7_uchar8				selected:1;
  L7_uchar8				tcProp:1;
  L7_uchar8				updtInfo:1;

  L7_uchar8				agreed:1;
  L7_uchar8				sync:1;
  L7_uchar8				synced:1;

  L7_uchar8				disputed:1;

  
  L7_uchar8 			agree:1;
  L7_uchar8				changedMaster:1;
  L7_uchar8				rcvdMsg:1;

  /*dot1s/driver async task parameters*/
  L7_uchar8 			stateChangeInProgress:1;

  L7_uchar8				portState; /*DISCARDING, LEARNING, FORWARDING*/
    
  DOT1S_PORTID_t		portId;
  DOT1S_RCVD_INFO_t		rcvdInfo;
  DOT1S_PORT_ROLE_t		role;
  DOT1S_PORT_ROLE_t		selectedRole;
  union
  {
   struct
   {
	 DOT1S_CIST_PRIORITY_VECTOR_t	cistDesignatedPriority;
	 DOT1S_CIST_TIMES_t			cistDesignatedTimes;
	 DOT1S_CIST_PRIORITY_VECTOR_t	cistMsgPriority;
	 DOT1S_CIST_TIMES_t			cistMsgTimes;
	 DOT1S_CIST_PRIORITY_VECTOR_t	cistPortPriority;
	 DOT1S_CIST_TIMES_t			cistPortTimes;
   }cist; /*168 bytes*/
   struct
   {
	 DOT1S_MSTI_PRIORITY_VECTOR_t 	mstiDesignatedPriority;
	 DOT1S_MSTI_TIMES_t				mstiDesignatedTimes;
	 DOT1S_MSTI_PRIORITY_VECTOR_t	mstiMsgPriority;
	 DOT1S_MSTI_TIMES_t				mstiMsgTimes;
	 DOT1S_MSTI_PRIORITY_VECTOR_t	mstiPortPriority;
	 DOT1S_MSTI_TIMES_t				mstiPortTimes;
	 L7_uchar8						mstiMaster;
	 L7_uchar8						mstiMastered;
   }msti; /*92 bytes*/

  }inst;

  L7_uchar8             rcvdBpduFlags; 

  /*state variables that are per port per instance*/

  L7_uchar8	            pimState; /* dot1s_pim_states_t */
  L7_uchar8	            prtState; /* dot1s_prt_states_t */
  L7_uchar8	            pstState; /* dot1s_pst_states_t */
  L7_uchar8	            tcmState; /* dot1s_tcm_states_t */


}DOT1S_PORT_t;   /*(39 * 4 +max(168, 92)) = 332 bytes*/

/*parms that are per port but common to cist and mstis*/
typedef struct dot1s_port_common_s
{
 L7_ushort16				portNum;

 /* In order to optimize certain loops, we define the portUpEnabledNum to be 
 ** equal to portNum when both, portEnabled == L7_TRUE and portLinkState == L7_UP.
 ** Otherwise the portUpEnabledNum is set to zero.
 */
 L7_ushort16        portUpEnabledNum;

 /*parms that are common to cist and all mstis*/
 L7_uint32 				tick;
 L7_uchar8 				txCount;


 L7_uchar8 				operEdge:1;
 L7_uchar8 				adminEdge:1;
 L7_uchar8 				autoEdge:1;

 L7_uchar8 				portEnabled:1;
 L7_uchar8				portAdminMode:1;
 /*parms that are common to cist and or all mstis*/
 L7_uchar8 				infoInternal:1;
 L7_uchar8 				newInfoCist:1;
 L7_uchar8 				newInfoMist:1;
 L7_uchar8 				rcvdInternal:1;
 L7_uchar8 				prevRcvdInternal:1;
 /*parms that are used by the state machines*/
 L7_uchar8 				initPm:1;
 L7_uchar8				rcvdBpdu:1; /* 802.1s/D14 modification */
 L7_uchar8				rcvdRSTP:1;
 L7_uchar8				rcvdSTP:1;
 L7_uchar8                rcvdMSTP:1;
 L7_uchar8				rcvdTcAck:1;
 L7_uchar8				rcvdTcn:1;
 L7_uchar8				sendRSTP:1;
 L7_uchar8				tcAck:1;
 
 /*parms used by the bridge detection machine*/
 L7_uchar8				bdmBpduRcvd:1;

 L7_uchar8                restrictedRole:1;
 L7_uchar8                loopGuard:1; 
 L7_uchar8                restrictedTcn:1;
 L7_uchar8				mcheck:1;

 L7_uint32              portLinkState; /* Link up/down indication */
 L7_uchar8                notParticipating:1; /* Not currently participating in STP */
 L7_uchar8              ignoreLinkStateChanges:1; /* Set when intf acquired for trunk and
                                                   set for similar events */
 L7_uchar8                bpduFilter:1;
 L7_uchar8                bpduFlood:1;

 L7_uchar8                bpduGuardEffect:1;
 L7_uchar8             loopInconsistent:1;

 /* dot1s/driver async task parameters */
 L7_uchar8  				diagnosticDisable:1;
 L7_uint32				lastNotificationSent;

  /*state variables for the state machine that are per port*/
 L7_uchar8              prxState; /* dot1s_prx_states_t */
 L7_uchar8              ppmState; /* dot1s_ppm_states_t */
 L7_uchar8              ptxState; /* dot1s_ptx_states_t */
 L7_uchar8              bdmState; /* dot1s_bdm_states_t */


 L7_uint32				mdelayWhile;/*counter*/
 L7_uint32				helloWhen;/*counter*/
 L7_uint32				edgeDelayWhile;/*counter*/

 AcquiredMask           acquiredList; /* Mask of components "acquiring"
                                         an interface */

 L7_uchar8 				HelloTime;

 /*index 0 will represent the CIST*/
 DOT1S_PORT_t			portInstInfo[L7_MAX_MULTIPLE_STP_INSTANCES + 1];





}DOT1S_PORT_COMMON_t;                   /*(116 +  ((L7_MAX_MULTIPLE_STP_INSTANCES + 1)*328)) byte*/

/*parms specific to an instance of STP including CIST*/
typedef struct dot1s_instance_s
{
 DOT1S_CIST_t 		cist;

 DOT1S_MSTI_t		msti[L7_MAX_MULTIPLE_STP_INSTANCES + 1]; /*do not use index 0*/

}DOT1S_INSTANCE_INFO_t;

typedef struct
{
  L7_BOOL	inUse;
  L7_uint32	instNumber;

}DOT1S_INSTANCE_MAP_t;

typedef struct dot1s_portstate_stat_s
{
  L7_uint32 successStateSet;
  L7_uint32 failureStateSet;

}DOT1S_PORTSTATE_STAT_t;

typedef struct port_stat_s
{
 L7_uint32 	stpRx;
 L7_uint32	rstpRx;
 L7_uint32 	mstpRx;

 L7_uint32  stpTx;
 L7_uint32  rstpTx;
 L7_uint32  mstpTx;

 L7_uint32  rxDiscards;
 L7_uint32  txDiscards;

 DOT1S_PORTSTATE_STAT_t stateStatsInst[L7_MAX_MULTIPLE_STP_INSTANCES +1];

 L7_uint32              transitionsIntoLoopInconsistentState;
 L7_uint32              transitionsOutOfLoopInconsistentState;
 
}DOT1S_PORT_STATS_t;

/*The following is the VID to MSTID mapping that is neccessary for the 
configuration ID signature*/
typedef struct inst_vlan_s
{
  L7_ushort16	instNumber;
}DOT1S_INST_VLAN_MAP_t;

/*  Dot1s Registrar Data */
typedef struct
{
  L7_uint32 registrar_ID; 
  L7_RC_t (*notify_intf_change)(L7_uint32 mstID, L7_uint32 port, L7_uint32 event);
} dot1sNotifyList_t;

/* dot1s/driver async task parameters */
typedef struct dot1sStateCirBuf_s
{
  L7_ushort16 intIfNum;
}dot1sStateCirBuf_t;


extern	DOT1S_BRIDGE_t				*dot1sBridge;    /*40 byte*/
extern	DOT1S_INSTANCE_INFO_t		*dot1sInstance;  /*(114 + ((L7_MAX_MULTIPLE_STP_INSTANCES + 1)*74)) byte*/

/*below we index into the port structure using internal interface numbers*/
extern	DOT1S_PORT_COMMON_t			*dot1sPort; /* L7_MAX_INTERFACE_COUNT * (116 +  ((L7_MAX_MULTIPLE_STP_INSTANCES + 1)*328)) bytes*/
extern  DOT1S_PORT_STATS_t			*dot1sPortStats;

/*index 0 represents the CIST it is left unused and is always initialized to inUse L7_TRUE
and instNumber of 0*/
extern	DOT1S_INSTANCE_MAP_t		*dot1sInstanceMap;
extern  DOT1S_INST_VLAN_MAP_t		*dot1sInstVlanMap; /*4094 +2*/

/* provides intf->array pointer map */
/*extern  DOT1S_PORT_COMMON_t         *dot1sPortIdx;*/

/* array of registred components */
extern  dot1sNotifyList_t           *dot1sNotifyList;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */

DOT1S_PORT_COMMON_t *dot1sIntfFind(L7_uint32 intIfNum);
L7_RC_t	dot1sInstIndexFind(L7_uint32 instNumber, L7_uint32	*index);
L7_RC_t	dot1sInstNumFind(L7_uint32 index, L7_uint32 *instNumber);
DOT1S_BRIDGE_t *dot1sBridgeParmsFind();
DOT1S_INSTANCE_INFO_t *dot1sInstFind();

#endif /*INCLUDE_DOT1S_DB_H*/

