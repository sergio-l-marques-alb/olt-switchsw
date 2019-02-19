/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_data.h
*
* @purpose    Types and data global data for NIM internals
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#ifndef NIM_DATA_H
#define NIM_DATA_H

#include "nim.h"
#include "nimapi.h"
#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "avl_api.h"

#ifdef NIM_GLOBAL
	#define NIM_EXTRN
#else
	#define NIM_EXTRN extern
#endif    


/* Type for the configuration info that will be saved per port in the configuration file */
typedef struct
{
  nimConfigID_t   configInterfaceId;	/* This is the configuration interface ID 64 bit field */

  L7_uint32       configIdMaskOffset;   /* a mask offset to be used for Config files- equals value of intIfNum */
  
  nimIntfConfig_t cfgInfo;              /* provided by the creator of interface or defaults */  

} nimCfgPort_t;

/* Type for the header of the configuration file */
typedef struct
{
  L7_fileHdr_t   	cfgHdr;

  L7_uint32       numberOfInterfaces;	/* Number of created interfaces in the file */

} nimConfigHeaderData_t;

/* type for the format of the configuration file */
typedef struct
{
  L7_fileHdr_t      cfgHdr;
  
  L7_uint32         numOfInterfaces;	/* Number of created interfaces */

  NIM_INTF_MASK_t   configMaskBitmap;  /* a bitmap to determine the used maskoffsets */

  nimCfgPort_t   *cfgPort;		/* an array of the ports configuration */
  
  L7_uint32      *checkSum;		/* check sum of config file NOTE: must be last entry */

} nimConfigData_t;

/* Type for the interface change callback registration list */
typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID; 

  L7_RC_t (*notify_intf_change)(L7_uint32 port, L7_uint32 event,NIM_CORRELATOR_t correlator);	

} nimNotifyList_t;


/* Type for the NIM specific message format */
typedef struct
{

  NIM_CORRELATOR_t        correlator;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;

} nimMessage_t; 

/* Type for differentiating between either NIM or the CNFGR messages in the nimTask */
typedef enum 
{
  NIM_MSG = 1,

  CNFGR_MSG,

} nimMsgTypes_t;

/* Type for the NIM pdu, which can have a NIM payload or CNFGR payload */
typedef struct
{
  nimMsgTypes_t    msgType;

  union
  {
    nimMessage_t      message;  /* What interface and event to dispatch to registered apps */

    L7_CNFGR_CMD_DATA_t   pCmdData; /* The information passed by the CNFGR to NIM for state transitions */

  }data;

} nimPdu_t;

/* Type for the runtime information related to an interface, including port mapping information */
typedef struct
{
  L7_uint32      present;                            /* Interface is defined (plugged in or configured */

  L7_uint32      intfNo;                             /* internal port num., 1 to MAX_INTERFACE_COUNT */

  L7_uint32      runTimeMaskId;

  nimConfigID_t  configInterfaceId;                  /* This is the configuration interface ID 64 bit field */

  L7_uint32      sysIntfType;                        /* Intf Type for the system */

  L7_uint32      ifIndex;                            /* ifIndex   */

  L7_uint32      resetTime;                          /* time interface counters were reset */

  L7_uint32      linkChangeTime;                     /* system clock, in seconds, at last link change */

  nimUSP_t       usp;                                /* Unit, Slot, Port */

  L7_uint32      currentLoopbackState;               /* L7_PHY_LOOPBACK, L7_MAC_LOOPBACK, L7_NONE */

  nimCfgPort_t   configPort;                         /* run time config data */

  L7_INTF_STATES_t  intfState;

  nimIntfDescr_t  operInfo;

  nimIntfConfig_t defaultCfg;

  L7_BOOL         defaultCfgSupplied;
}nimIntf_t;    

/* Type used to tally the responses to Events */
typedef struct nimCorrelatorData_s
{

  L7_BOOL                 inUse;
  L7_uint32               time;
  NIM_CORRELATOR_t        correlator;
  NIM_EVENT_NOTIFY_INFO_t requestData;
  L7_RC_t                 response;
  L7_uint32               *remainingMask;
  L7_uint32               *failedMask;

}nimCorrelatorData_t;

/* Type for the USP to intIfNum lookup */
typedef struct
{

  nimUSP_t usp;
  L7_uint32 intIfNum;
  void *avlData;

} nimUspIntIfNumTreeData_t; 


/* ConfigId Tree Data structure */
typedef struct
{

  nimConfigID_t  configId;
  L7_uint32     intIfNum;
  void *avlData;

} nimConfigIdTreeData_t; 


/* Intf Type Data structure */
typedef struct
{

  L7_uint32     minIntIfNumber;
  L7_uint32     maxIntIfNumber;
  NIM_INTF_MASK_t intfMask;

} nimIntfTypeData_t; 

/* Type for the NIM control block */
typedef struct  nimSystemData_s
{
  osapiRWLock_t     rwLock;          /* semaphore for syncronizing NIM */

  L7_CNFGR_STATE_t  nimPhaseStatus;	  /* present configurator state of NIM */

  nimNotifyList_t   *nimNotifyList;		/* an array of the routines registered for events by other components */

  L7_uint32         taskId;           /* the task ID for the nimTask */

  L7_uint32   		  ifTableLastChange;/* ifTableLastChange */

  L7_uint32   		  ifNumber;         /* Number of External interfaces created */

  NIM_INTF_MASK_t   linkStateMask;    /* mask of port link up/down */
 
  NIM_INTF_MASK_t   forwardStateMask;	/* mask of port forwarding or not */   /* WPJ: Revisit active mask being removed */

  nimIntf_t         *nimPorts;				/* An array of internal interfaces that serves as the primary mapping data */ 

  void 				*nimMsgQueue;			      /* the message queue for CNFGR state changes and port event notifications */

  L7_uint32  		*nimNumberOfPortsPerUnit;	/* an array of number of ports per unit */

  nimUSP_t    		*nimVlanSlotsInfo;	/* an array of the vlan interfaces usp  WPJ_TBD This should be obsolete*/

  L7_uint32   		nimHighestIntfNumber; 	 /* WPJ: Is this useful */   		

  L7_uint32   		maxNumOfUnits;        

  L7_uint32   		maxNumOfSlotsPerUnit; 

  L7_uint32   		maxNumOfPhysicalPortsPerSlot; 

  L7_uint32   		*numberOfInterfacesByType;	/* an array to hold the number of interfaces by type */

  nimIntfTypeData_t *intfTypeData;	/* an array to data specific to an interface type */

  L7_char8    		*nimFileName;	

  nvStoreFunctionList_t notifyFunctionList;	/* NVRAM storage routines */

  nimConfigData_t  *nimConfigData;  

  nimCorrelatorData_t *pCorrelator_table; 

  NIM_INTF_MASK_t createdMask;

  NIM_INTF_MASK_t physicalIntfMask;

  NIM_INTF_MASK_t lagIntfMask;      /* WPJ: Remove when nimIntfTypeData_t is genericized */

  NIM_INTF_MASK_t cpuIntfMask;

  NIM_INTF_MASK_t vlanIntfMask;
  NIM_INTF_MASK_t l2tnnlIntfMask;

  NIM_INTF_MASK_t presentMask;

  avlTreeTables_t        *nimUspTreeHeap;
  nimUspIntIfNumTreeData_t   *nimUspDataHeap;
  avlTree_t              nimUspTreeData;

  avlTreeTables_t        *nimConfigIdTreeHeap;
  nimConfigIdTreeData_t   *nimConfigIdDataHeap;
  avlTree_t              nimConfigIdTreeData;       

  L7_uint32       nimPrintLog;
}nimSystemData_t;

 

/* Global Control block for all of NIMs data */
#ifndef NIM_GLOBAL
NIM_EXTRN nimSystemData_t *nimCtlBlk_g;
#endif

#endif /* NIM_DATA_H */
