/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_polocy_db_int.h
*
* @purpose    Internal db header file used by Synchronization module.
*
* @component  HAPI
*
* @comments   none
*
* @create     1/6/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_POLICY_DB_INT_H
#define L7_USL_POLICY_DB_INT_H


extern void                          *pUslPolicyDbSema;

extern BROAD_USL_POLICY_t             *pUslOperPolicyDB;

extern BROAD_USL_POLICY_t             *pUslShadowPolicyDB;

extern BROAD_USL_POLICY_t             *pUslPolicyDBHandle;

extern L7_BOOL                         uslPolicyDbActive;


#define USL_POLICY_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_POLICY_DB_ID,"POL",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslPolicyDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslPolicyDbSema); \
  } \
}

#define USL_POLICY_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_POLICY_DB_ID,"POL",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaGive(pUslPolicyDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslPolicyDbSema); \
  } \
}

#define USL_DEBUG_PRINT if (usl_debug != L7_FALSE) sysapiPrintf
extern L7_BOOL usl_debug;

/*********************************************************************
* @purpose  Get the size of Policy Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_policy_db_elem(void);

/*********************************************************************
* @purpose  Print the contents of a Policy Db element in specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_policy_db_elem(void *item, L7_uchar8 *buffer, L7_uint32 size);

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_policy_db_elem(USL_DB_TYPE_t     dbType, 
                                L7_uint32         flags,
                                uslDbElemInfo_t   searchElem,
                                uslDbElemInfo_t  *elemInfo);

/*********************************************************************
* @purpose  Delete a given element from Policy Db
*
* @params   dbType   {(input)} 
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_policy_db_elem(USL_DB_TYPE_t   dbType, 
                                   uslDbElemInfo_t elemInfo);

/*********************************************************************
* @purpose  Create a given element in Policy Db
*
* @params   dbItem {(input)}  Pointer to db item to be created
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
L7_int32 usl_create_policy_db_elem_bcm(void *dbItem);

/*********************************************************************
* @purpose  Delete a given element from Policy Db
*
* @params   dbItem {(input)}  Pointer to db item to be deleted
*
* @returns  BCM error code
*
* @notes    For Policy Db, we should never get into this case
*
* @end
*********************************************************************/
L7_int32 usl_delete_policy_db_elem_bcm(void *dbItem);

/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_policy_db_elem_bcm(void *shadowDbItem, void *operDbItem);

#endif
