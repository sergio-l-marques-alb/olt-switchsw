/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_ipmcast_db_int.h
*
* @purpose    Internal db header file used by Synchronization module.
*
* @component  HAPI
*
* @comments   none
*
* @create     12/2/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_IPMCAST_DB_INT_H
#define L7_USL_IPMCAST_DB_INT_H

extern void         *pUslIpMcastDbSema;
extern avlTree_t     uslOperIpMcastTreeData;
extern avlTree_t     uslShadowIpMcastTreeData;
extern avlTree_t    *pUslIpMcastTreeHandle;
extern L7_BOOL       uslIpMcDbActive;


#define USL_IPMC_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_IPMC_ROUTE_DB_ID,"IPM",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslIpMcastDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslIpMcastDbSema); \
  } \
}

#define USL_IPMC_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_IPMC_ROUTE_DB_ID,"IPM",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslIpMcastDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslIpMcastDbSema); \
  } \
}

/*********************************************************************
* @purpose  Get the size of IPMC Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_ipmc_db_elem(void);

/*********************************************************************
* @purpose  Get the size of IPMC Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_ipmc_port_db_elem(void);

/*********************************************************************
* @purpose  Print the contents of a IPMC Db element in specified buffer
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
void usl_print_ipmc_db_elem(void *item, L7_uchar8 *buffer, L7_uint32 size);

/*********************************************************************
* @purpose  Print the contents of a IPMC Db element in specified buffer
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
void usl_print_ipmc_port_db_elem(void *item, L7_uchar8 *buffer, L7_uint32 size);

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
L7_int32 usl_get_ipmc_db_elem(USL_DB_TYPE_t     dbType, 
                              L7_uint32         flags,
                              uslDbElemInfo_t   searchElem,
                              uslDbElemInfo_t  *elemInfo);

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
L7_int32 usl_get_ipmc_port_db_elem(USL_DB_TYPE_t     dbType, 
                                   L7_uint32         flags,
                                   uslDbElemInfo_t   searchElem,
                                   uslDbElemInfo_t  *elemInfo);

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
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
L7_int32 usl_delete_ipmc_db_elem(USL_DB_TYPE_t   dbType, 
                                 uslDbElemInfo_t elemInfo);

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
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
L7_int32 usl_delete_ipmc_port_db_elem(USL_DB_TYPE_t   dbType, 
                                      uslDbElemInfo_t elemInfo);

/*********************************************************************
* @purpose  Create a given element in IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be created
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
L7_int32 usl_create_ipmc_db_elem_bcm(void *dbItem);

/*********************************************************************
* @purpose  Create a given element in IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be created
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
L7_int32 usl_create_ipmc_port_db_elem_bcm(void *dbItem);

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be deleted
*
* @returns  BCM error code
*
* @notes    For IPMC Db, we should never get into this case
*
* @end
*********************************************************************/
L7_int32 usl_delete_ipmc_db_elem_bcm(void *dbItem);

/*********************************************************************
* @purpose  Delete a given element from IPMC Db
*
* @params   dbItem {(input)}  Pointer to db item to be deleted
*
* @returns  BCM error code
*
* @notes    For IPMC Db, we should never get into this case
*
* @end
*********************************************************************/
L7_int32 usl_delete_ipmc_port_db_elem_bcm(void *dbItem);

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
L7_int32 usl_update_ipmc_db_elem_bcm(void *shadowDbItem, void *operDbItem);

/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.
*
* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_ipmc_port_db_elem_bcm(void *shadowDbItem, void *operDbItem);

#endif
