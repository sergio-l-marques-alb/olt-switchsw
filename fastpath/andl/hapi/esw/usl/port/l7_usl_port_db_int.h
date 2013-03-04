
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_port_db_int.h
*
* @purpose    Internal db header file used by Synchronization module.
*
* @component  USL
*
* @comments   none
*
* @create     11/21/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_PORT_DB_INT_H
#define L7_USL_PORT_DB_INT_H

extern L7_BOOL    uslPortDbActive;
extern void      *uslPortDbSema;
extern avlTree_t  uslOperPortDbTree;
extern avlTree_t  uslShadowPortDbTree;
extern avlTree_t *uslPortDbTreeHandle;

#define USL_PORT_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_PORT_DB_ID,"PORT",osapiTaskIdSelf(),\
                __LINE__,L7_TRUE); \
  if (osapiSemaTake(uslPortDbSema, L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n",  \
                uslPortDbSema); \
  } \
}

#define USL_PORT_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_PORT_DB_ID,"PORT",osapiTaskIdSelf(), \
                 __LINE__,L7_FALSE); \
  if (osapiSemaGive(uslPortDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", \
                uslPortDbSema); \
  } \
}

#endif /* L7_USL_PORT_DB_INT_H */
