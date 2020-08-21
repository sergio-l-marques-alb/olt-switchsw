/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_debug.c
*
* @purpose    Handle debug facilities for usl
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#include "l7_common.h"
#include <string.h>
#include "sysapi.h"
#include "log.h"
#include "l7_usl_common.h"
#include "l7utils_api.h"
#include "l7_usl_bcmx_l2.h"
#ifdef L7_ROUTING_PACKAGE
#include "l7_usl_l3_db.h"
#endif
#include "platform_config.h"
#include "broad_common.h"

static L7_BOOL uslPrintLog = L7_FALSE;

/* always allow USL errors to go to the log */
static L7_uint32 uslAllowedLogTypes = USL_E_LOG | USL_BCM_E_LOG;

void usl_log_msg_fmt(L7_char8 * file, L7_ulong32 line, L7_char8 * msg,L7_BOOL overRide)
{
  L7_int32 len;
  L7_uchar8 log_buf[LOG_MSG_MAX_MSG_SIZE];

  if ((uslPrintLog == L7_TRUE) || (overRide == L7_TRUE))
  {
    bzero(log_buf,sizeof(log_buf));

    l7utilsFilenameStrip(&file);

    osapiSnprintf (log_buf, LOG_MSG_MAX_MSG_SIZE, "%s:%d : ", file, (L7_int32)line);

    len = (L7_int32)strlen(log_buf);

    if (len < LOG_MSG_MAX_MSG_SIZE)
    {
      strncat(log_buf,msg,(size_t)(LOG_MSG_MAX_MSG_SIZE - len));
    }

    sysapiPrintf(log_buf);
  }
} 

void usl_print_log(L7_BOOL set)
{
  uslPrintLog = (set == L7_TRUE)?L7_TRUE:L7_FALSE;
}


void usl_gplist_print(bcmy_gplist_t *list)
{
  int i; 
  bcm_gport_t gport;

  if (list == L7_NULLPTR)
    return;

  /* get the first port in the list */

  for (i=0; i <= list->gp_last; i++)
  {
    gport = bcmy_gplist_index(list, i);
    if (gport == BCM_GPORT_INVALID)
       break;

    sysapiPrintf("0x%x , ",gport);
  }
}

L7_uint32 usl_allowed_log_types_get()
{
  return uslAllowedLogTypes;
}

void usl_allowed_log_types_set(L7_uint32 types)
{
  uslAllowedLogTypes = types;
}

void usl_gport_to_unit_port(bcm_gport_t gport)
{
  int unit,port,modid;
  DAPI_USP_t usp;
  unit = port = modid = ~0;

  if (bcmy_lut_gport_to_unit_port_get(gport, &unit, &port) != BCMY_E_NONE)
  {
    printf("Error converting gport 0x%x to unit/port",  gport);
    return;
  }
  if (bcmy_lut_gport_to_usp_get(gport, &usp) != BCMY_E_NONE)
  {
    printf("Error converting gport 0x%x to usp",  gport);
    return;
  }
  modid = bcmy_gport_modid_get(gport);

  sysapiPrintf("gport(%d) modid=%d , unit=%d, port=%d, USP=%d.%d.%d\n\n",
               gport, modid, unit, port, usp.unit, usp.slot, usp.port);
}

/***
** Test the USL Logging function.
***/
int usl_log_msg_test (int msg_size)
{
  unsigned char test_msg [LOG_MSG_MAX_MSG_SIZE * 2];
  int old_log_mask;
  int i;

  if (msg_size > sizeof (test_msg))
  {
    msg_size = sizeof (test_msg);
  }
  if (msg_size < 2)
  {
    msg_size = 0;
  }

  old_log_mask = usl_allowed_log_types_get();
  usl_allowed_log_types_set(USL_INFO_LOG);

  memset (test_msg, 0, sizeof (test_msg));
  for (i = 0; i < (msg_size - 2); i++)
  {
    test_msg[i] = 'a' + (char) (i % 26);
  }

  sysapiPrintf("Testing the USL_INFO_LOG going to DEBUG in log\n");
  USL_LOG_MSG (USL_INFO_LOG, "%s", test_msg);

  usl_allowed_log_types_set(old_log_mask);
  
  
  sysapiPrintf("Testing the USL_E_LOG going to ERROR in log\n");
  USL_LOG_MSG (USL_E_LOG, "%s", test_msg);

  sysapiPrintf("Testing the USL_BCM_E_LOG going to ERROR in log\n");
  USL_LOG_MSG (USL_BCM_E_LOG, "%s", test_msg);

  return 0;
}
