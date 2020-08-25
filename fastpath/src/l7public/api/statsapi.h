/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename statsapi.h
*
* @purpose The purpose of this file is to enumerate the statistics 
*		   counters used in the stats manager, and also define the
*		   function prototypes for the user interface functions.
*
* @component Stats Manager
*
* @comments none
*
* @create 10/06/2000
*
* @author Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_STATS_API_H
#define INCLUDE_STATS_API_H

/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/
#define STATS_COMMON_MSIZE 16
#ifdef __cplusplus
// This represents the maximum number of mutlings allowable
// for a mutant counter
const L7_ushort16 STATS_MAX_MUTLING = STATS_COMMON_MSIZE;	 
#else
#define STATS_MAX_MUTLING STATS_COMMON_MSIZE
#endif

/*
***********************************************************************
*                           USER-DEFINED DATA TYPES
***********************************************************************
*/


/* The following is a user-defined type for counter id  */
typedef L7_uint32 counter_id_t;


/* A counter may have a positive or negative value associated with it when it  */
/* is a component counter of a MutantCounter                                   */
typedef enum {cMINUS = -1, cINVALID = 0, cPLUS = 1} counter_sign_t;


/* A counter may be of the following 3 types:                 */
/*     LocalCounter                                           */
/*     ExternalCounter                                        */
/*     MutantCounter                                          */
/* The type of the counter is specified using this structure  */
typedef enum { LocalCtr = 1, ExternalCtr , MutantCtr } counter_type_t;

/* Tells the external source if this is a get or a set */
typedef enum { getMode = 1, setMode} cmd_mode_t;

/* Tells us what the size type of the counter is, it may be an unsigned      */
/* short (16 bit), unsigned long (32 bit) or an unsigned long long(64 bits)  */
/* Note: These values *must* be 0, 1 and 2 as the counter_size_t is used to  */
/* index into the respective size masks                                      */
typedef enum { C16_BITS = 0, C32_BITS = 1, C64_BITS = 2 } counter_size_t;

/* A mutling (component counter of a MutantCounter) is associated with a  */
/* sign and an id                                                         */
typedef struct {
	counter_sign_t cSign;
	L7_uint32      cKey;
	counter_id_t   cId;
} mutling_id_t, *pMutling_id_t;


/* The following structure defines the parameter list received by the  */
/* Collector to create a MutantCounter                                 */
typedef struct {
	L7_ushort16 listSize;
	pMutling_id_t pMutlingParms;   
} mutling_parm_list_t, *pMutling_parm_list_t;

/* This structure stores the information required by the Collector to get the  */
/* value of a Counter. It also provides space to store the value and status    */
/* of the counter after the operation                                          */
typedef struct {
	cmd_mode_t cMode;           /* will be specified by Collector depending on
                                   whether the operation is a get or set          */
	L7_uint32 cKey;             /* input: counter key */
	counter_id_t cId;           /* input: counter id  */
    L7_BOOL raw;                /* input: specify true for raw (hardware) value   */
	counter_size_t cSize;       /* output: counter size will be returned by the 
                                   Stats Mgr                                      */
	L7_RC_t status;             /* output: Stats Mgr sets it to L7_SUCCESS if the
                                   operation was a success                        */
	L7_ulong64 cValue;          /* output: used to return the requested value 
                                   for get                                        */
} counterValue_entry_t, *pCounterValue_entry_t;



/* This structure stores the information required by the Collector to  */
/* create a Counter                                                    */
typedef struct {
	L7_RC_t status;         /* output: Stats Mgr returns L7_SUCCESS if the operation
                               was successful                                      */
	counter_type_t cType;   /* input: specify counter type i.e Local, External or
                               Mutant                                              */
	L7_uint32 cKey;         /* input: specify counter key                          */
	counter_id_t cId;       /* input: specify counter id                           */
	counter_size_t cSize;   /* input: specify counter size                         */
	L7_BOOL isResettable;   /* input: specify if counter is resettable
                                      true: if the value can be reset by the user
                                      false: if the value is not to be reset by the 
                                             user                                  */
	L7_VOIDFUNCPTR_t pSource;               /* input: specify source for External 
                                                      counter                      */
	pMutling_parm_list_t pMutlingsParmList; /* input: specify mutling params for
                                                      Mutant Counter               */
} statsParm_entry_t, *pStatsParm_entry_t;   


/* The statsCreate API function receives an array of statsParm_entry_t and it  */
/* loops through the array until all the user specified counters have been     */
/* created                                                                     */
typedef pStatsParm_entry_t pStatsParm_list_t;

/* The statsGet API function receives an array of counterValue_entry_t and it  */
/* loops through the array until all the user specified counters have been     */
/* recovered (or if they could not be recovered the status changed to          */
/* "failure")                                                                  */
typedef pCounterValue_entry_t pCounterValue_list_t;


/*
********************************************************************
*                           FUNCTION PROTOTYPES
********************************************************************
*/

/*
/------------------------------------------------------------------\
*                            ACCESS                                *
\------------------------------------------------------------------/
*/


/*
**********************************************************************
*
* @function     statsCfgInit
*
* @purpose      This function initializes the Statistics Manager and its 
*		        three major packages the Presenter, the MutantRegistry &
*		        the Collector.  
*          
* @parameter    none
*
* @return       L7_SUCCESS     The Statistics Manager is initialized
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     The Statistics Manager could not be initialized
*
* @note         Implemented in src\application\stats\public\statsapi.c
*
* @end
**********************************************************************
*/
EXT_API L7_RC_t statsCfgInit (void);


/*
**********************************************************************
*
* @function     statsCreate
*
* @purpose      This function allows the client to create one or more 
*		        counters  
*          
* @parameter    L7_uint32          listSize
*               pStatsParm_list_t  pStatsParmList
* @return       L7_SUCCESS    All the specified counters were successfully 
*					          created 
* @return       L7_ERROR      Not Applicable
* @return       L7_FAILURE    Counters could not be created
*
* @note         Implemented in src\application\stats\public\statsapi.c
*
* @end
*
**********************************************************************
*/

EXT_API L7_RC_t statsCreate ( L7_uint32 listSize , pStatsParm_list_t pStatsParmList);


/*
**********************************************************************
*
* @function     statsGet
*
* @purpose      This function allows the client to access counter information
*		        from the Statistics Manager
*          
* @parameter    L7_uint32               listSize
* @parameter    pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS      If all the counter(s) could be successfully 
*					            found and stored in the space provided by the
*					            client 
* @return       L7_ERROR        If the counter requested by the client does not 
*					            exist in the CounterFlyweight
* @return       L7_FAILURE      Catastrophic failure i.e. all the returned 
*					            values are invalid
*
* @note         Implemented in src\application\stats\public\statsapi.c
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsGet ( L7_uint32 listSize, 
				   pCounterValue_list_t pCounterValueList);

/*
**********************************************************************
*
* @function     statsReset
*
* @purpose      This function allows the client to reset counter information
*		        in the Statistics Manager
*          
* @parameter    L7_uint32          listSize
*               pCounterValue_list_t pCounterValueList
*
* @return       L7_SUCCESS      If all the specified counters could be 
*					            successfully reset
* @return       L7_ERROR        If some counters could not be reset
* @return       L7_FAILURE      Catastrophic failure i.e. if none of the
*						        counters could be reset
*
* @note         Implemented in src\application\stats\public\statsapi.c
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsReset ( L7_uint32 listSize, 
					 pCounterValue_list_t pCounterValueList);


/*
**********************************************************************
*
* @function     statsIncrement
*
* @purpose      This function allows the client to increment the values of
*		        LocalCounters
*          
* @parameter    L7_uint32               listSize
*               pCounterValue_list_t    pCounterValueList
*
* @return       L7_SUCCESS      If the specified counter(s) was successfully
*						        incremented 
* @return       L7_ERROR        Not Applicable
* @return       L7_FAILURE      If the specified counter could not be 
*						        incremented
*
* @note         Implemented in src\application\stats\public\statsapi.c
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsIncrement ( L7_uint32  listSize, 
						 pCounterValue_list_t  pCounterValueList);



/*
**********************************************************************
*
* @function     statsDecrement
*                       
* @purpose      This function allows the client to decrement the values of
*		        LocalCounters 
*          
* @parameter    L7_uint32        listSize
* @parameter    pCounterValue_list_t pCounterValueList
*
* @return       L7_SUCCESS      If the specified counter(s) was successfully
*       		                decremented 
* @return       L7_ERROR        Not Applicable
* @return       L7_FAILURE      If the specified counter could not be 
*						        decremented
*
* @note         Implemented in src\application\stats\public\statsapi.c
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsDecrement ( L7_uint32 listSize, 
						 pCounterValue_list_t pCounterValueList );

/*
**********************************************************************
*
* @function     statsAddMutlings
*                       
* @purpose      This function allows the client to dynamically add
*               mutlings to a mutant counter 
*          
* @parameter    L7_uint32               listSize
* @parameter    pStatsParm_list_t       pStatsParmList
*
* @return       L7_SUCCESS      If all the specified counter(s) were successfully
*						        added to the mutant set 
* @return       L7_ERROR        Not Applicable
* @return       L7_FAILURE      If at least one of the specified counters could
*                               not be added to the mutant set.
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsMutlingsAdd (L7_uint32 listSize, 
                                  pStatsParm_list_t  pStatsParmList);


/*
**********************************************************************
*
* @function     statsDelMutlings
*                       
* @purpose      This function allows the client to dynamically delete
*               mutlings to a mutant counter 
*          
* @parameter    L7_uint32        listSize
* @parameter    pStatsParm_list_t pStatsParmList
*
* @return       L7_SUCCESS      If all the specified counter(s) were successfully
* @return				        deleted from the mutant set 
* @return       L7_ERROR        Not Applicable
* @return       L7_FAILURE      If at least one of the specified counters could
*                               not be deleted from the mutant set.
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsMutlingsDel (L7_uint32 listSize, 
                                  pStatsParm_list_t  pStatsParmList);

/*
**********************************************************************
*
* @function     statsCreate
*
* @purpose      This function allows the client to create one or more 
*		        counters  
*          
* @parameter    L7_uint32          listSize
*               pStatsParm_list_t  pStatsParmList
* @return       L7_SUCCESS     All the specified counters were successfully 
*					           created 
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     Counters could not be created
*
* @end
*
**********************************************************************
*/
EXT_API L7_RC_t statsDelete ( L7_uint32 listSize, 
					  pStatsParm_list_t pStatsParmList);

#endif   /* INCLUDE_STATS_API_H */
