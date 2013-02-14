/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_cfg.h
*
* @purpose This header file contains definitions to support the       
*          Device Transformation Layer configuration.      
*
* @component Device Transformation Layer
*
* @comments none
*
* @author Shekhar Kalyanam 3/14/2001
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTLCTRL_CFG_H
#define INCLUDE_DTLCTRL_CFG_H
/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/
#ifndef DTLCFG_H

  #define DTLCFG_H

  #define DTL_QUEUE               "dtlqueue"
  #define DTL_TASK_PRIORITY       9
  #define DTL_MSG_SIZE            2

  #define DTL_ADDR_QUEUE          "dtlAddrQueue"
#endif  /* DTLCFG_H */


/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/
/*********************************************************************
* @purpose  Message queue to maintain callback info to facilitate the 
* @purpose  the context switch from driver to application
*
* @notes    none     
* 
*********************************************************************/
typedef struct  DTL_MSG_s
{

  DAPI_USP_t     ddusp;
  DAPI_FAMILY_t  cmdFamilyType;
  DAPI_CMD_t     cmdType;
  DAPI_EVENT_t   eventType;
  union
  {
    DAPI_INTF_MGMT_CMD_t  dapiIntfEventInfo;

    DAPI_ADDR_MGMT_CMD_t  dapiAddrEventInfo;

    DAPI_FRAME_CMD_t      dapiFrameEventInfo;

  }cmdData;


}DTL_MSG_t;

typedef struct DTL_FUNC_REG_s
{
  DTL_EVENT_t             event;
  L7_FUNCPTR_t            funcPtr;
  struct DTL_FUNC_REG_s   *prev;
  struct DTL_FUNC_REG_s   *next;

}DTL_FUNC_REG_t;

typedef struct DTL_REG_CTRL_s
{
  DTL_FUNC_REG_t  *first;
  DTL_FUNC_REG_t  *last;
  L7_uint32       count;
}DTL_REG_CTRL_t;





/****************************************
*
*  DTL Configuration Data                    
*
*****************************************/

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  L7_uint32      checkSum;

} dtlCfgData_t;


/****************************************
*
*  DTL Information Data                    
*
*****************************************/

typedef struct
{
  L7_uint32      routeEntryCount;
  void			 *routeEntrySem;
  L7_uint32      localMcastAddrEntryCount;

} dtlInfo_t;



/*
**********************************************************************
*                           GLOBAL DATA
**********************************************************************
*/


#ifdef  DTLCTRL_CFG_GLOBALS
  #define DTLCTRL_CFG_EXT
#else 
  #define DTLCTRL_CFG_EXT extern
#endif  /* DTLCTRL_CFG_GLOBALS */

DTLCTRL_CFG_EXT DTL_MSG_t    *dtlMsg;


DTLCTRL_CFG_EXT L7_ushort16    dtlMsgTail;
DTLCTRL_CFG_EXT L7_ushort16    dtlMsgHead;

DTLCTRL_CFG_EXT void *dtlMsgQueue;
DTLCTRL_CFG_EXT void *dtlAddrMsgQueue;

DTLCTRL_CFG_EXT DTL_REG_CTRL_t *dtlEventHandle[DTL_NUM_EVENT];



/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DTL INTERNAL USE ONLY
***********************************************************************
*/
/*********************************************************************
* @purpose  Unlocks tasks and enables interrupts
*          
* @param 		LockKey handle to enable interrupts
*	
* @returns  void
*
* @notes none
*
* @end
*
*********************************************************************/
void dtlExitCS(void);
/*********************************************************************
* @purpose  Disables interrupts and locks tasks
*          
* @param 		
*	
* @returns  LockKey - a handle to enable the interrupts
*
* @notes none
*
* @end
*
*********************************************************************/
void dtlEnterCS(void);

/*********************************************************************
* @purpose  creates the first node in the registry
*          
* @param 		none
*	
* @returns  pointer to the first node on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_FUNC_REG_t *dtlRegistryNodeFirstGet();


/*********************************************************************
* @purpose  allocates a new node entry in the registry
*          
* @param 		*lastRegNodePtr   @b{(input)} pointer tot he last node in the registry 
*	
* @returns  pointer to new node on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_FUNC_REG_t *dtlRegistryNodeNextGet(DTL_FUNC_REG_t *lastRegNodePtr);


/*********************************************************************
* @purpose  Removes a node entry from the registry
*          
* @param 		*handle       @b{(input)} Handle to registry control block
* @param 		*regNodePtr   @b{(input)} node pointer to remove
*	
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlRegistryNodeDelete(DTL_REG_CTRL_t *handle, 
                              DTL_FUNC_REG_t *regNodePtr);


/*********************************************************************
* @purpose  Creates a registry control block in the registry
*          
* @param 		none
*	
* @returns  pointer to the control block on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_REG_CTRL_t *dtlRegistryCtrlCreate();


/*********************************************************************
* @purpose  Removes control block from the registry
*          
* @param 		*handle   @b{(input)} Handle to registry control block
*	
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlRegistryCtrlDelete(DTL_REG_CTRL_t *handle);


/*********************************************************************
* @purpose  adds a node entry from the registry
*          
* @param 		*handle   @b{(input)} Handle to registry control block
* @param 		*funcPtr  @b{(input)} Function pointer to add
*	
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlRegistryNodeAdd(DTL_REG_CTRL_t *handle, 
                           L7_FUNCPTR_t funcPtr);


/*********************************************************************
* @purpose  Finds a node entry from the registry
*          
* @param 		*handle   @b{(input)} Handle to registry control block
* @param 		*funcPtr  @b{(input)} Function pointer to locate
*	
* @returns  registry entry pointer on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
DTL_FUNC_REG_t *dtlRegistryNodeFind(DTL_REG_CTRL_t *handle, 
                                    L7_FUNCPTR_t funcPtr);







/*********************************************************************
* @purpose  Receive callback from the driver and based on registrations with DTL 
* @purpose  call the function/s registered for the event
*
* @param    *ddusp         @b{(input)} Device driver reference to unit slot and port
* @param	  family         @b{(input)} Device Driver family type 
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    event          @b{(input)} Event of type DAPI_EVENT_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlGenericCallback(DAPI_USP_t *ddusp, 
                                 DAPI_FAMILY_t family, 
                                 DAPI_CMD_t cmd, 
                                 DAPI_EVENT_t event,
                                 void *dapiEventInfo);





/*********************************************************************
* @purpose  create the message queue and wait for messages 
*          
* @param 		@b{(void)}
*
* @returns  L7_FAILURE - if unable to create the OS message queue
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlTask();


/*********************************************************************
* @purpose   Create the dtl task
*          
* @param 	  taskId    {(output)}  The task id for the dtl task
*	
* @returns      L7_SUCCESS  on successful Task creation
* @returns      L7_ERROR    on unsuccessful Task creation
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStartTask(L7_uint32 *taskId);

#endif    /* INCLUDE_DTLCTRL_CFG_H */
