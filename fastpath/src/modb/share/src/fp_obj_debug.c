/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename fp_obj_debug.c
 *
 * @purpose
 *
 * @component object handlers
 *
 * @comments
 *
 * @create  30 May 2007
 *
 * @author  Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include "fpobj_util.h"

static int fpObjDebugTraceLevel = 0;

int fpObjDebugTraceSet(int val)
{
  int old = fpObjDebugTraceLevel;
  fpObjDebugTraceLevel = val;
  return old;
}

void fpObjDebugTrace (fpObjTraceType_t which, void *bufp, void *owcp, int *indx, int len, const char *func, int line)
{
  if(fpObjDebugTraceLevel == 0)
  {
    return;
  }

  if (which == FPOBJ_TRACE_TYPE_ENTER)
  {
    fpObjLibBuf_t *arg = (fpObjLibBuf_t *) bufp;
    int i;
    printf ("[%s-%d]: Enter TRACE_ENTER Macro\r\n", func, line);
    if (arg->valid)
    {
      printf ("\r\narg value is  ");
      for (i = 0; i < arg->valen; i++)
      {
        printf ("%x ", *((arg->value) + i));
      }
      printf ("\r\n");
      printf ("\r\narg oid is %u\r\n", arg->oid);
    }
    printf ("\r\nExit TRACE_ENTER Macro\r\n");
  }
  else if (which == FPOBJ_TRACE_TYPE_EXIT)
  {
    fpObjLibBuf_t *arg = (fpObjLibBuf_t *) bufp;
    fpObjWa_t *owb = (fpObjWa_t *) owcp;
    int i;
    printf ("[%s-%d]: Enter TRACE_EXIT Macro\r\n", func, line);
    if (arg->valid)
    {
      printf ("\r\narg value is  ");
      for (i = 0; i < arg->valen; i++)
      {
        printf ("%x ", *((arg->value) + i));
      }
      printf ("\r\n");
      printf ("\r\narg oid is %u\r\n", arg->oid);
    }
    printf ("\r\nowb l7rc = %d rc = 0x%x length = %hu\r\n", owb->l7rc, owb->rc, owb->len);
    printf ("\r\nExit TRACE_EXIT Macro\r\n");
  }
  else if (which == FPOBJ_TRACE_TYPE_CURRENT_KEY)
  {
    fpObjLibBuf_t *arg;
    arg = (fpObjLibBuf_t *) bufp;
    printf ("[%s-%d]: Enter TRACE_CURRENT_KEY Macro\r\n", func, line);
    int i;
    if (arg && arg->valid)
    {
      printf ("arg value is  ");
      for (i = 0; i < arg->valen; i++)
      {
        printf ("%x ", (*(arg->value) + i));
      }
      printf ("\r\n");
      printf ("\r\narg oid is %u\r\n", arg->oid);
    }
    if ((indx))
    {
      if (sizeof (*indx) == 4)
      {
        int *val;
        val = (int *) indx;
        printf ("\r\nindex feild is %d\r\n", *val);
      }
      else
      {
        char *str;
        str = (char *) indx;
        printf ("\r\nindex feild is %s\r\n", str);
      }
    }
    else
    {
      printf ("\r\nfunction called with NULL\r\n");
    }
    printf ("\r\nlen is %hu\r\n", len);
    printf ("\r\n[%s-%d]: Exit TRACE_CURRENT_KEY Macro\r\n", func, line);
  }

  else if (which == FPOBJ_TRACE_TYPE_NEW_KEY)
  {
    fpObjLibBuf_t *arg = (fpObjLibBuf_t *) bufp;
    printf ("[%s-%d]: Enter TRACE_NEW_KEY Macro\r\n", func, line);
    int i;
    if (arg && arg->valid)
    {
      printf ("arg value is  ");
      for (i = 0; i < arg->valen; i++)
      {
        printf ("%x ", *((arg->value) + i));
      }
      printf ("\r\n");
      printf ("\r\narg oid is %u\r\n", arg->oid);
    }
    if ((indx))
    {
      if (sizeof (*indx) == 4)
      {
        int *val;
        val = (int *) indx;
        printf ("\r\nindex feild is %d\r\n", *val);
      }
      else
      {
        char *str;
        str = (char *) indx;
        printf ("\r\nindex feild is %s\r\n", str);
      }
    }
    else
    {
      printf ("\r\nfunction called with NULL\r\n");
    }
    printf ("\r\nlen is %hu\r\n", len);
    printf ("\r\n[%s-%d]: Exit TRACE_NEW_KEY Macro\r\n", func, line);
  }
}
