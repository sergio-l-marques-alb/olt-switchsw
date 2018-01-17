/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: l7_getvar.c
*
* Purpose: Externalize the i_GetVar function
*
* Created by: Colin Verne, 07-17-01
*
* Component: SNMP
*
*********************************************************************/

/**********************************************************************
                       
**********************************************************************/
#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif /* HAVE_MEMORY_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "sr_snmp.h"
#include "diag.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "getvar.h"

#include <string.h>

#include "l7_getvar.h"


/*********************************************************************
* @purpose  Retrieve an unsigned integer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *val          unsigned integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an unsigned integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
L7GetVarUInt( L7_char8 *oid_buf, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  VarBind *vb_ptr = NULL;
  ContextInfo *cip = NULL;

  cip = AllocContextInfo(0);
  
  vb_ptr = GetVar(cip, EXACT, oid_buf);

  if (vb_ptr != NULL)
  {
    switch (vb_ptr->value.type)
    {
    case COUNTER_TYPE:
    case GAUGE_TYPE:
    case TIME_TICKS_TYPE:
      /* value is an unsigned integer */
      *val = vb_ptr->value.u.ulval;
      rc = L7_SUCCESS;
      break;

    case INTEGER_TYPE:
      /* value is a signed integer */
      *val = vb_ptr->value.u.slval;
      rc = L7_SUCCESS;
      break;

    default:
      rc = L7_ERROR;
    }

    FreeVarBind(vb_ptr);
  }

  FreeContextInfo(cip);

  return rc;
}

void
snmp_testL7GetVarUInt(L7_char8 *oid_buf)
{
  L7_uint32 val = 0;
  L7_RC_t rc;

  rc = L7GetVarUInt(oid_buf, &val);

  printf("rc = %d\nval = %d\n", rc, val);
}


/*********************************************************************
* @purpose  Retrieve a signed integer MIB object
*
* @parms    *oid_buf      integer buffer containing the OID to retreive
*           *val          signed integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not a signed integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
L7GetVarSInt( L7_char8 *oid_buf, L7_int32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  VarBind *vb_ptr = NULL;
  ContextInfo *cip = NULL;
  
  cip = AllocContextInfo(0);

  vb_ptr = GetVar(cip, EXACT, oid_buf);

  if (vb_ptr != NULL)
  {
    /* check that the value is a signed integer */
    if (vb_ptr->value.type == INTEGER_TYPE)
    {
      *val = vb_ptr->value.u.slval;
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_ERROR;
    }
    FreeVarBind(vb_ptr);
  }

  FreeContextInfo(cip);
  
  return rc;
}

void
snmp_testL7GetVarSInt(L7_char8 *oid_buf)
{
  L7_int32 val = 0;
  L7_RC_t rc;

  rc = L7GetVarSInt(oid_buf, &val);

  printf("rc = %d\nval = %d\n", rc, val);
}

/*********************************************************************
* @purpose  Retrieve an character buffer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *buf          character buffer to place the value into
*           *buf_len      unsigned integer to place the buffer length into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an octet string
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
L7GetVarCharBuf( L7_char8 *oid_buf, L7_char8 *buf, L7_uint32 *buf_len)
{
  L7_RC_t rc = L7_FAILURE;
  VarBind *vb_ptr = NULL;
  ContextInfo *cip = NULL;

  cip = AllocContextInfo(0);
  
  vb_ptr = GetVar(cip, EXACT, oid_buf);

  if (vb_ptr != NULL)
  {
    /* check that the value is an octet string */
    if (vb_ptr->value.type == OCTET_PRIM_TYPE)
    {
      memcpy(buf, vb_ptr->value.u.osval->octet_ptr, vb_ptr->value.u.osval->length);
      *buf_len = vb_ptr->value.u.osval->length;
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_ERROR;
    }
    FreeVarBind(vb_ptr);
  }

  FreeContextInfo(cip);
  
  return rc;
}

void
snmp_testL7GetVarCharBuf(L7_char8 *oid_buf, L7_BOOL force_hex)
{
  L7_RC_t rc;
  L7_char8 buf[256];
  L7_uint32 buf_len = 0;
  L7_BOOL printable = L7_TRUE;
  L7_uint32 i;

  memset(buf, 0, sizeof(buf));

  rc = L7GetVarCharBuf(oid_buf, buf, &buf_len);

  printf("rc = %d\n", rc);

  if (rc == L7_SUCCESS)
  {
    printf("buf = '");
    for (i = 0; i<buf_len; i++)
    {
      if (buf[i] < ' ' || buf[i] > '~')
      {
        printable = L7_FALSE;
        force_hex = L7_TRUE;
        printf(".");
      }
      else
      {
        printf("%c", buf[i]);
      }
    }
    printf("'\n");
    if (force_hex)
    {
      printf("buf = ");
  
      for (i = 0; i<buf_len; i++)
      {
        printf(" %02X", buf[i]);
      }
      printf("\n");
    }
  }
}

