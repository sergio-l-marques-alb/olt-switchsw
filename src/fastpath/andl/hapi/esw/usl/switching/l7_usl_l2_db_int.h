/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_l2_db_int.h
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

#ifndef L7_USL_L2_DB_INT_H
#define L7_USL_L2_DB_INT_H



#define USL_SYSTEM_DB_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_SYSTEM_DB_ID,"SDB",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslSystemDbSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema take failed, semId %x\n",  \
                pUslSystemDbSema); \
  } \
}

#define USL_SYSTEM_DB_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_SYSTEM_DB_ID,"SDB",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslSystemDbSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema give failed, semId %x\n",  \
                pUslSystemDbSema); \
  } \
}

#endif /* L7_USL_L2_DB_INT_H */
