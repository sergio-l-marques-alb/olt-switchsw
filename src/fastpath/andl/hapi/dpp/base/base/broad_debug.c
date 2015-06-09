/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_debug.c
*
* @purpose   This file contains all the routines for debug
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/
#include <string.h>
#include <ctype.h>

#include "l7_common.h"
#include "sysapi.h"
#include "simapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "commdefs.h"
#include "broad_common.h"
#include "broad_l2_std.h"
#include "broad_debug.h"
#include "hpc_db.h"
#include "support_api.h"
#include "broad_policy.h"

/* For Debug utils */
#include "soc/types.h"
#include "soc/phy.h"
#include "soc/drv.h"
#include "soc/arl.h"
#include "bcm/port.h"
#include "bcm/stat.h"
#include "soc/cmic.h"
#include "soc/error.h"
#include "soc/mem.h"
#include "soc/hash.h"
#include "soc/l2x.h"
/* PTin removed: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcm_int/esw/draco.h"
#endif
#include "bcm_int/control.h"
#include "appl/diag/diag.h"
#include "appl/diag/system.h"
#include "appl/diag/cmdlist.h"
#if (SDK_VERSION_IS >= SDK_VERSION(6,4,0,0))
#include "shared/bsl.h"
#elif (SDK_VERSION_IS >= SDK_VERSION(6,3,2,0))
#include "bcm_int/mdebug.h"
#else
#include "bcm/debug.h"
#endif
#include "osapi_trace.h"

/* needed for the initialization of the bcm shell functionality */
extern void init_symtab();

#ifdef L7_QOS_PACKAGE
  #include "broad_qos.h"
  #include "broad_qos_debug.h"
#endif

#ifdef L7_MCAST_PACKAGE
  #include "broad_l3_mcast_debug.h"
#endif

#include "bcmx/port.h"
#include "bcmx/lport.h"
#include "bcmx/stg.h"
#include "bcmx/tx.h"
#include "bcmx/rate.h"
#include "ibde.h"

#define  __C_CODE__

/*
 * Dump all of the SOC register addresses, and if do_values is true,
 * read and dump their values along with the addresses.
 */

#define DREG_ADR_SEL_MASK     0xf       /* Low order 4 bits */
#define DREG_ADDR               0       /* Address only */
#define DREG_RVAL               1       /* Address and reset default value */
#define DREG_REGS               2       /* Address and real value */
#define DREG_DIFF               3       /* Addr & real value if != default */

#define DREG_PORT_ALL -1
#define DREG_BLOCK_ALL -1

struct dreg_data
{
  int unit;
  int dreg_select;
  int only_port;    /* Select which port/block.  -1 ==> all */
  int only_block;
};

extern DAPI_t    *dapi_g;

int hapiBroadDebugPktReceiveEnabled = 0;
unsigned int hapiBroadDebugReceiveCounter = 0;
unsigned int hapiBroadDebugSendCounter = 0;

void hapiDebugSysInfoDumpRegister(void);
void hapiDebugRegisteredSysInfoDump(void);

extern void txStats();
extern void rxStats();

extern void    diag_shell(void);  /* from appl/diag/system.h */

#ifdef L7_ROUTING_PACKAGE
extern void hapiBroadL3StatsShow();
#endif

#ifdef L7_STACKING_PACKAGE
extern void bcm_rlink_dump();
extern void hpcDebugBcmCpudbShow();
#endif

#ifndef L7_BCM_SHELL_SUPPORT

/* The following MAC address routines were initially pulled in from
 * SDK/diag code. BCM Diag shell is now integrated into FastPath and
 * is controlled by the above Make flag.
 *
 * We need these routines only when BCM shell is not included.
 */

/*
 * Convert hex digit to hex character and vice-versa
 */

int i2xdigit(int digit)
{
  digit &= 0xf;
  return(digit > 9) ? digit - 10 + 'a' : digit + '0';
}

int xdigit2i(int digit)
{
  if (digit >= '0' && digit <= '9') return(digit - '0'     );
  if (digit >= 'a' && digit <= 'f') return(digit - 'a' + 10);
  if (digit >= 'A' && digit <= 'F') return(digit - 'A' + 10);
  return 0;
}

/*
 * parse_macaddr will take a string of the form H:H:H:H:H:H where each
 * H is one or two hex digits, or a string of the form 0xN where N may
 * consist of up to 12 hex digits.  The result is returned in a byte
 * array to avoid endian confusion.
 */

int parse_macaddr(char *str, mac_addr_t macaddr)
{
  char *s;
  int colon = FALSE;
  int i, c1, c2;

  if (strchr(str, ':'))
  {   /* Colon format */
    colon = TRUE;
  }
  else if (*str++ != '0' || tolower(*str++) != 'x')
  {
    return -1;
  }
  else
  {
    memset(macaddr, 0, 6);
  }
  /* Start at end and work back */
  s = str + strlen(str);
  for (i = 5; (i >= 0) && (s >= str); i--)
  {
    c2 = (s > str && isxdigit((unsigned) s[-1])) ? xdigit2i((unsigned) *--s) : 0;
    c1 = (s > str && isxdigit((unsigned) s[-1])) ? xdigit2i((unsigned) *--s) : 0;
    macaddr[i] = c1 * 16 + c2;
    if (colon && (s >= str) && (':' != *--s))
      break;
  }
  return(((s <= str) && (!colon || (i == 0))) ? 0 : -1);
}

/*
 * Format a long integer.  If the value is less than 10, generates
 * decimal, otherwise generates hex.
 *
 * val[0] is the least significant word.
 * nval is the number of uint32's in the value.
 */

void
format_long_integer(char *buf, uint32 *val, int nval)
{
  int i;

  for (i = nval - 1; i > 0; i--)
  {  /* Skip leading zeroes */
    if (val[i])
    {
      break;
    }
  }

  if (i == 0 && val[i] < 10)
  {  /* Only a single word < 10? */
    sprintf(buf, "%d", val[i]);
  }
  else
  {
    sprintf(buf, "0x%x", val[i]); /* Print first word */
  }

  while (--i >= 0)
  {      /* Print rest of words, if any */
    sprintf(buf + strlen(buf), "%08x", val[i]);
  }
}


/*
 * Format uint64
 * Endian handling is taken into account.
 */

void
format_uint64(char *buf, uint64 n)
{
  uint32    val[2];

  val[0] = COMPILER_64_LO(n);
  val[1] = COMPILER_64_HI(n);

  format_long_integer(buf, val, 2);
}
#endif

/*********************************************************************
* @purpose  Execute commands on a driver shell
*
* @param
*
* @returns
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t hapiBroadDebugShell(void *data)
{
  L7_RC_t              result  = L7_SUCCESS;

#ifdef L7_BCM_SHELL_SUPPORT
  L7_uchar8* str =     (L7_uchar8 *)data;
  sh_process_command(0, str);
#else
  printf("L7_BCM_SHELL_SUPPORT - is not available in this build.\n");
#endif

  fflush(stdout);

  return result;

}

/*********************************************************************
* @purpose  Initialize the Broadcom Shell
*
* @param    None
*
* @returns  Nothing
*
* @end
*********************************************************************/
void hapiBroadDebugShellInit()
{
#ifdef BROADCOM_DEBUG
  static L7_BOOL initted = L7_FALSE;

  if (initted == L7_FALSE)
  {
    initted = L7_TRUE;
#ifdef L7_BCM_SHELL_SUPPORT
    cmdlist_init();
    init_symtab();
#endif
  }
#endif
}

/*********************************************************************
*
* @purpose To do any debug setup required and get debug routines linked
*
* @param   none
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDebugInit(DAPI_t *dapi_g)
{
#ifdef BROADCOM_DEBUG
#ifdef BCM_ESW_SUPPORT
  hapiDebugSysInfoDumpRegister();
#endif
  /*BCM SHELL API INIT*/
#ifdef L7_BCM_SHELL_SUPPORT
  hapiBroadDebugShellInit();
#endif
  if (dapi_g != L7_NULL)
  {
    dapi_g->system->dbgDrivShell = hapiBroadDebugShell;
  }

#ifdef DEBUG_STARTUP
  PT_LOG_WARN(LOG_CTX_STARTUP,"debugk_select not being applied!");
  //debugk_select(DEBUG_STARTUP);
#endif

#ifdef L7_QOS_PACKAGE
  hapiBroadQosDebugInit();
#endif

#ifdef L7_MCAST_PACKAGE
  hapiBroadL3McastDebugInit();
#endif
#endif
}

/*********************************************************************
*
* @purpose Prints out some general information for each unit, slot and port
*          in the current configuration
*
* @param   none
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDebugHelp()
{
  printf ("\nList of hapiBroadDebug... functions ->\n\n");
  printf ("hapiBroadDebugIPAddressAdd(i,p) - Add IP address\n");
  printf ("hapiBroadDebugStatsGet(unitNum, slotNum, portNum) - Get s/w port stat\n");
  printf ("hapiBroadDebugSocRegDump(reg, unit, pbmp) - Get SOC register dump\n");
  printf ("hapiBroadDebugPhyRegDump(u, pbmp_t pbm) - Get PHY register dump\n");
  printf ("hapiBroadDebugPcimDump(unit, off_start, count,printZero) - \n");
  printf ("hapiBroadDebugDmaVecDump(addr) - \n");
  printf ("hapiBroadDebugMemoryDump(memtype, unit, copyno, addr, numwords, flags) - Get the memory value for memtype\n");
  printf ("hapiBroadDebugFindL2(unit, vidMacHi, macLo) - Find a L2 Table entry\n");
  printf ("hapiBroadDebugFindL3(unit, ipAddr) - Find a L3 Table entry\n");
  printf ("hapiBroadDebugStpList() - List MSTP instances\n");
  printf ("hapiBroadDebugStpVlanList(instNumber) - List Vlans in a MSTP\n");
  printf ("hapiBroadDebugStpPortList(instNumber) - List ports in a MSTP\n");
  printf ("hapiBroadDebugRegDump(unit) - Full dump of all registers (Huge output)\n");
  printf ("hapiBroadDebugVlanTable(unit) - List hardware VLAN table\n");
  printf ("hapiBroadDebugPortStat(unit) - List hardware Port Stats\n");
  printf ("hapiBroadDebugFrameMaxGet(unit, port) - Display maximum frame size for this unit/port\n");
  printf ("hapiBroadDebugDumpAll(mmuDump,printFabrics) - Dump all memories and regsiters in system\n");
  printf ("hapiBroadDebugMemoryDumpAll(unit,mmuDump) - Dump all memories for unit\n");
  printf ("hapiBroadDebugBcmPrint(enable) - Enables printing of the bcm logging to screen\n");
  printf ("hapiBroadDebugBcmTrace(enable) - Enables tracing of bcm logging to dapiTrace\n");

  fflush(stdout);
}

void hapiBroadDebugSpecial(L7_ushort16 unit)
{
  /* PTin added: new switch 56843 (Triumph3) */
  if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit) ||
      SOC_IS_TRIUMPH3(unit))
  {
      uint64 egr_val_64;
      uint32 bitmap;
      int rv;
      /*uint32 egr_val;*/

      rv = READ_ING_EN_EFILTER_BITMAP_64r(unit, &egr_val_64);
      if (BCM_SUCCESS(rv)) {
          /* Low bitmap */
          bitmap =
              soc_reg64_field32_get(unit,
                                    ING_EN_EFILTER_BITMAP_64r,
                                    egr_val_64, BITMAP_LOf);
          printf(" bitmap lo: %x \n", bitmap);
          soc_reg64_field32_set(unit, ING_EN_EFILTER_BITMAP_64r,
                                &egr_val_64, BITMAP_LOf, 0);

          /* High bitmap */
          bitmap =
              soc_reg64_field32_get(unit,
                                    ING_EN_EFILTER_BITMAP_64r,
                                    egr_val_64, BITMAP_HIf);
          printf(" bitmap hi: %x \n", bitmap);

          
          printf(" setting to zero..\n");
          soc_reg64_field32_set(unit, ING_EN_EFILTER_BITMAP_64r,
                                &egr_val_64, BITMAP_HIf, 0);

          rv = WRITE_ING_EN_EFILTER_BITMAP_64r(unit, egr_val_64);
      }

  }

  fflush(stdout);
}

L7_RC_t hapiBroadDebugFrameMaxGet(L7_ushort16 unit, int port)
{
  int rc;
  int size;

  rc = bcm_port_frame_max_get (unit, port, &size);

  printf("rc = %d, size = %d\n", rc, size);
  fflush(stdout);

  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose
*
* @param   L7_ushort16 i
* @param   L7_ushort16 p
*
* @returns L7_SUCCESS
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugIPAddressAdd(L7_ushort16 i,L7_ushort16 p)
{

  DAPI_USP_t        ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_ulong32        ipAddr;
  L7_ulong32        ipAddr1;
  L7_ushort16       ip[2]  = {i,p};
  L7_RC_t     result = L7_SUCCESS;


  ddUsp.unit = 0;
  ddUsp.slot = 0;
  ddUsp.port = 0;

  ipAddr1 = (L7_ulong32)(ip[0]);
  ipAddr = ((ipAddr1 << 16) | ((L7_ulong32)ip[1]));
  dapiCmd.cmdData.systemIpAddress.ipAddress = ipAddr;

  dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS, &dapiCmd);

  return result;

}



/*********************************************************************
*
* @purpose Manually configure a port to be down
*
* @param   L7_ushort16 unitNum      - the desired unit number
* @param   L7_ushort16 slotNum      - the desired slot number
* @param   L7_ushort16 portNum      - the desired port number
* @param   L7_BOOL entireSystem - if true(or 1), all ports will be
*                                     configured to be down
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugLinkDown(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum, L7_BOOL entireSystem)
{

  DAPI_USP_t    usp;
  L7_RC_t result = L7_SUCCESS;


  if (entireSystem == L7_TRUE)
  {
    for (usp.unit=0;usp.unit<dapi_g->system->totalNumOfUnits;usp.unit++)
    {
      for (usp.slot=0;usp.slot<dapi_g->unit[usp.unit]->numOfSlots;usp.slot++)
      {
        if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardId != L7_NULL)
        {
          for (usp.port=0;usp.port<dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
          {
            hapiBroadPortLinkDown(&usp,dapi_g);
          }
        }
      }

    }
  }
  else
  {
    usp.unit = unitNum;
    usp.slot = slotNum;
    usp.port = portNum;

    hapiBroadPortLinkDown(&usp,dapi_g);
  }

  return result;

}



/*********************************************************************
*
* @purpose Manually configures a port to be up
*
* @param   L7_ushort16 unitNum      - the desired unit number
* @param   L7_ushort16 slotNum      - the desired slot number
* @param   L7_ushort16 portNum      - the desired port number
* @param   L7_BOOL entireSystem - if set to true (or 1) then all
*                                     ports will be configured to be up
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugLinkUp(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum, L7_BOOL entireSystem)
{

  DAPI_USP_t    usp;
  L7_RC_t result = L7_SUCCESS;


  if (entireSystem == L7_TRUE)
  {
    for (usp.unit=0;usp.unit<dapi_g->system->totalNumOfUnits;usp.unit++)
    {
      for (usp.slot=0;usp.slot<dapi_g->unit[usp.unit]->numOfSlots;usp.slot++)
      {
        if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardId != L7_NULL)
        {
          for (usp.port=0;usp.port<dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
          {
            hapiBroadPortLinkUp(&usp,dapi_g);

          }
        }
      }

    }
  }
  else
  {
    usp.unit = unitNum;
    usp.slot = slotNum;
    usp.port = portNum;

    hapiBroadPortLinkUp(&usp,dapi_g);
  }

  return result;

}



/*********************************************************************
*
* @purpose Prints out the stats for the requested usp if it is up
*
* @param   L7_ushort16 unitNum - needs to be a valid unit number
* @param   L7_ushort16 slotNum - needs to be a valid slot number
* @param   L7_ushort16 portNum - needs to be a valid port number
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugStatsGet(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum)
{

  DAPI_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t           usp;
  L7_ulong64           value;
  L7_ushort16          i;
  L7_RC_t        result = L7_SUCCESS;

  usp.unit = unitNum;
  usp.slot = slotNum;
  usp.port = portNum;

  dapiCmd.cmdData.statistics.getOrSet = DAPI_CMD_GET;
  dapiCmd.cmdData.statistics.enable   = L7_TRUE;
  dapiCmd.cmdData.statistics.valuePtr = &value;

  SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS, "USP(%d,%d,%d)\n",usp.unit,usp.slot,usp.port);

  for (i=0;i<DAPI_STATS_NUM_OF_INTF_ENTRIES;i++)
  {
    dapiCmd.cmdData.statistics.counterId = i;

    result = dapiCtl(&usp, DAPI_CMD_INTF_STATISTICS, &dapiCmd);

    if (result != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (i != DAPI_STATS_NUM_OF_INTF_ENTRIES)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "stat#: %d, 0x%8.8x%8.8x, %d\n",i,value.high,value.low,(value.high+value.low));
    }
  }

  return result;

}

L7_RC_t hapiBroadDebugSocRegRead(L7_uint32 reg, L7_uint32 unit, soc_port_t port, L7_uint32 *val)
{
#ifdef BCM_ESW_SUPPORT
  int                r;
  soc_reg_info_t    *reginfo;

  if (!SOC_REG_IS_VALID(unit, reg))
    return L7_FAILURE;

  reginfo = &SOC_REG_INFO(unit, reg);

  switch (reginfo->regtype)
  {
  case soc_portreg:
    break;
  default:
    assert(0);
  }

  r = soc_reg32_read(unit, soc_reg_addr(unit, reg, port, 0), val);

  if (r < 0)
  {
    printf("ERROR: read from register failed: %s\n", soc_errmsg(r));
    fflush(stdout);
    return L7_FAILURE;
  }
#endif
  return L7_SUCCESS;
}

L7_RC_t hapiBroadDebugSocRegReadDebug(L7_uint32 reg, L7_uint32 unit, soc_port_t port)
{
  L7_uint32 val=0;

  hapiBroadDebugSocRegRead(reg, unit, port, &val);

  printf("\n val %d\n", val);
  fflush(stdout);

  return L7_SUCCESS;
}

L7_RC_t hapiBroadDebugSocRegDump(L7_uint32 reg, L7_uint32 unit, L7_uint32 pbmp)
{
#ifdef BCM_ESW_SUPPORT
  int                f, r;
  soc_reg_info_t    *reginfo;
  L7_uint32          val, j, fval;
  soc_port_t         port;
  soc_cos_t          cos;
  soc_block_t        block, blk;
  soc_regaddrlist_t  alist;
  soc_regaddrinfo_t *a;
  soc_regaddrinfo_t  ainfo[64];
  pbmp_t             new_pbmp;
  char              *fmt;
  char               buf[80];
  uint64             data64;

  if (!SOC_REG_IS_VALID(unit, reg))
    return L7_FAILURE;

  switch(pbmp)
  {
    case L7_BROAD_DEBUG_PBMP_FE_ALL: new_pbmp = PBMP_FE_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_GE_ALL: new_pbmp = PBMP_GE_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_XE_ALL: new_pbmp = PBMP_XE_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_HG_ALL: new_pbmp = PBMP_HG_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_HL_ALL: new_pbmp = PBMP_HL_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_ST_ALL: new_pbmp = PBMP_ST_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_GX_ALL: new_pbmp = PBMP_GX_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_XG_ALL: new_pbmp = PBMP_XG_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_E_ALL : new_pbmp = PBMP_E_ALL(unit);break;
    case L7_BROAD_DEBUG_PBMP_CMIC  : new_pbmp = PBMP_CMIC(unit);break;
    case L7_BROAD_DEBUG_PBMP_SPI   : new_pbmp = PBMP_SPI(unit);break;
    case L7_BROAD_DEBUG_PBMP_PORT_ALL: new_pbmp = PBMP_PORT_ALL(unit);break; /* This is the default case if pbmp not given */
    default: new_pbmp.pbits[0] = pbmp; break;
  }

  reginfo = &SOC_REG_INFO(unit, reg);

  printf("\nInfo for %s\n",SOC_REG_NAME(unit, reg));

  block = reginfo->block[0];  // PTin added: compilation fix for SDK 5.10.4
  alist.count = 0;
  alist.ainfo = ainfo;

  switch (reginfo->regtype)
  {
  case soc_portreg:
    SOC_PBMP_ITER(new_pbmp, port)
    {
      a = &alist.ainfo[alist.count++];
      a->addr = soc_reg_addr(unit, reg, port, 0);
      a->valid = 1;
      a->reg = reg;
      a->block = -1;
      a->port = port;
      a->field = INVALIDf;
      a->cos = -1;
    }
    break;
  case soc_cosreg:
    for (cos = 0; cos < NUM_COS(unit); cos++)
    {
      SOC_BLOCK_ITER(unit, blk, block)
      {
        a = &alist.ainfo[alist.count++];
        if (block & SOC_BLK_PORT)
        {
          a->addr = soc_reg_addr(unit, reg, cos, blk);
        }
        else
        {
          a->addr = soc_reg_addr(unit, reg, cos, 0);
        }
        a->valid = 1;
        a->reg = reg;
        a->port = -1;
        a->block = blk;
        a->field = INVALIDf;
        a->cos = cos;
      }
    }
    break;
  case soc_genreg:
    SOC_BLOCK_ITER(unit, blk, block)
    {
      a = &alist.ainfo[alist.count++];
      if (block & SOC_BLK_PORT)
      {
        a->addr = soc_reg_addr(unit, reg, SOC_BLOCK_PORT(unit, blk), 0);
      }
      else
      {
        a->addr = soc_reg_addr(unit, reg, 0, 0);
      }
      a->valid = 1;
      a->reg = reg;
      a->block = blk;
      a->port = -1;
      a->field = INVALIDf;
      a->cos = -1;
    }
    break;
  case soc_cpureg:
    ainfo[0].reg = reg;
    ainfo[0].addr = soc_reg_addr(unit, reg, 0, 0);
    (void)soc_anyreg_read(unit, &ainfo[0], &data64);
    printf("%x %x\n",COMPILER_64_HI(data64), COMPILER_64_LO(data64));
    fflush(stdout);
    return L7_SUCCESS;
  default:
    assert(0);
  }

  for (j = 0; j < (uint32)alist.count; j++)
  {
    r = soc_reg32_read(unit, alist.ainfo[j].addr, &val);
    reginfo = &SOC_REG_INFO(unit, alist.ainfo[j].reg);

    if (reginfo->regtype == soc_portreg)
    {
      printf("Port=%d ",alist.ainfo[j].port);
    }
    else if (reginfo->regtype == soc_cosreg)
    {
      printf("COS=%d ",alist.ainfo[j].cos);
    }

    if (r < 0)
    {
      soc_reg_sprint_addr(unit, buf, &alist.ainfo[j]);
      printf("ERROR: read from register %s failed: %s\n", buf, soc_errmsg(r));
      fflush(stdout);
      return L7_FAILURE;
    }
    else
    {
      soc_reg_sprint_addr(unit,  buf, &alist.ainfo[j]);
      printf(" <");

      for (f = reginfo->nFields - 1; f >= 0; f--)
      {
        soc_field_info_t *fld = &reginfo->fields[f];
        fval = soc_reg_field_get(unit, alist.ainfo[j].reg, val, fld->field);
        fmt = (fval < 10) ? "%s=%d%s" : "%s=0x%x%s";
#if !defined(SOC_NO_NAMES)
        printf(fmt, soc_fieldnames[fld->field], fval, f > 0 ? "," : "");
#else
        printf(fmt, "-", fval, f > 0 ? "," : "");
#endif
      }

      printf(">\n");
    }

  }
#endif

  fflush(stdout);

  return L7_SUCCESS;
}

static int hapiBroadDebugFindHerc(void)
{
  int i;

  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
    if (SOC_IS_XGS_FABRIC(i))
                  return i;
  }

  return -1;
}

void hapiBroadDebugHercCtrsDump(void)
{
  int herc_mod;

  herc_mod = hapiBroadDebugFindHerc();
  if (herc_mod < 0)
  {
    printf("Herc not found.\n");
    fflush(stdout);
    return;
  }

  hapiBroadDebugSocRegDump(ITPKTr, herc_mod, 0);
  hapiBroadDebugSocRegDump(IRPKTr, herc_mod, 0);
  hapiBroadDebugSocRegDump(IRFCSr, herc_mod, 0);
  hapiBroadDebugSocRegDump(IRERPKTr, herc_mod, 0);
  return;
}

#ifdef BCM_ESW_SUPPORT
int hapiBroadDebugHercCpuQueuesDump(void)
{
  uint32 val;
  int rv;
  uint32 reg;
  int herc_mod;

  herc_mod = hapiBroadDebugFindHerc();
  if (herc_mod < 0)
  {
    printf("Herc not found.\n");
    fflush(stdout);
    return -1;
  }

  for (reg = 0x101; reg <= 0x108; reg++)
  {
    rv = soc_reg32_read (herc_mod, reg, &val);
    if (rv != SOC_E_NONE)
    {
      printf("Error %d reading register 0x%x.\n",
             rv, reg);
    } else
    {
      printf("Register 0x%x is set to 0x%x\n",
             reg, val);
    }
  }

  fflush(stdout);
  return 0;
}
#endif

/* Dump PHY Register for the specified ports in the bitmap */
L7_int32 hapiBroadDebugPhyRegDump(L7_int32 u, L7_uint32 pbmp)
{
  L7_int32       p;
  L7_ushort16    phy_data, phy_reg;
  L7_uchar8      phy_id;
  L7_int32       rv = 0;
  pbmp_t         new_pbmp;
  L7_char8       drvName[64];

  memset((void *)&new_pbmp, 0, sizeof(pbmp_t));

  if (!pbmp) /* If 0 then all ports */
    new_pbmp = PBMP_PORT_ALL(u);
  else
    new_pbmp.pbits[0] = pbmp;

  PBMP_ITER(new_pbmp, p)
  {
    phy_id = PORT_TO_PHY_ADDR(u, p);
    if (bcm_port_phy_drv_name_get(u, p,drvName,64) != BCM_E_NONE)
    {
      osapiSnprintf(drvName, sizeof(drvName), "UNKNOWN:");
    }
    printf("Port %d (PHY ID %d): %s (%s)", p + 1, phy_id,
           soc_phy_name_get(u, p),
           drvName);
    for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG; phy_reg++)
    {
      if (IS_XE_PORT(u, p)) {
        rv = soc_miimc45_read(u, phy_id, PHY_C45_DEV_PMA_PMD,
                               phy_reg, &phy_data);
      } else {
        rv = soc_miim_read(u, phy_id, phy_reg, &phy_data);
      }
      if (rv < 0)
      {
        printf("\nError: Port %d: soc_miim_read failed: %s\n",
               p + 1, soc_errmsg(rv));
        goto done;
      }
      printf("%s\t0x%02x: 0x%04x",
             ((phy_reg % 4) == 0) ? "\n" : "",
             phy_reg, phy_data);
    }
    printf("\n");
  }

  done:
  fflush(stdout);
  return rv;
}

L7_int32 hapiBroadDebugPhyRegWrite(L7_int32 u, L7_uint32 port, L7_ushort16 phy_reg, L7_ushort16 value)
{
  L7_uchar8      phy_id;
  L7_int32       rv = 0;
  L7_char8       drvName[64];

  phy_id = PORT_TO_PHY_ADDR(u, port);
  if (bcm_port_phy_drv_name_get(u, port,drvName,64) != BCM_E_NONE)
  {
    osapiSnprintf(drvName, sizeof(drvName), "UNKNOWN:");
  }
  printf("Port %d (PHY ID %d): %s (%s)", port + 1, phy_id,
         soc_phy_name_get(u, port),
         drvName);

  rv = soc_miim_write(u, phy_id, phy_reg, value);
  if (rv < 0)
  {
    printf("\nError: Port %d: soc_miim_write failed: %s\n",
           port + 1, soc_errmsg(rv));
    goto done;
  }

  done:
  fflush(stdout);
  return rv;
}

#ifdef BCM_ESW_SUPPORT
/* Dump PCI Memory registers starting at the offset */
void hapiBroadDebugPcimDump(L7_int32 unit, L7_uint32 off_start, L7_uint32 count,L7_uint32 printZero)
{
  L7_uint32 off, val;

  if ((off_start & 3) != 0)
  {
    printf("dump_pcim ERROR: offset must be a multiple of 4\n");
    fflush(stdout);
    return;
  }

  for (off = off_start; count--; off += 4)
  {
    soc_pci_getreg(unit, off, &val);

    if ((val == 0) && (!printZero)) continue;

    printf("0x%04x %s: 0x%x\n", off, soc_pci_off2name(unit, off), val);
  }

  fflush(stdout);
}
#endif

#ifdef BCM_ESW_SUPPORT
void hapiBroadDebugDmaVecDump(L7_int32 addr)
{
  L7_int32     unit = 0;

  soc_dma_dump_dv(unit, " ", (void *)addr);
}
#endif

#ifdef BCM_ESW_SUPPORT
static int hapiBroadDebugIdentifiersValid[] = {
#ifdef VALIDf
  VALIDf,
#endif
#ifdef VALID0f
  VALID0f,
#endif
#ifdef VALID1f
  VALID1f,
#endif
#ifdef VALID_0f
  VALID_0f,
#endif
#ifdef VALID_1f
  VALID_1f,
#endif
#ifdef VALID_2f
  VALID_2f,
#endif
#ifdef VALID_3f
  VALID_3f,
#endif
#ifdef VALID_4f
  VALID_4f,
#endif
#ifdef VALID_5f
  VALID_5f,
#endif
#ifdef VALID_6f
  VALID_6f,
#endif
#ifdef VALID_7f
  VALID_7f,
#endif
#ifdef VALID_BITf
  VALID_BITf,
#endif
#ifdef L3_VALIDf
  L3_VALIDf,
#endif
INVALIDf
};
#endif

L7_RC_t hapiBroadDebugMemoryDump(L7_int32 memtype, L7_uint32 unit, L7_int32 copyno, L7_int32 addr, L7_int32 numwords, L7_int32 flags)
{
#ifdef BCM_ESW_SUPPORT
  int index, count;
  int k, i, j;
  uint32 entry[SOC_MAX_MEM_WORDS];
  int entry_dw;
  soc_mem_t  mem;
  int field,isValid,valField;

  mem = (soc_mem_t)memtype;

  if (!SOC_MEM_IS_VALID(unit, mem))
  {
    printf("Invalid memtype specified\n");
    fflush(stdout);
    return L7_FAILURE;
  }

#ifdef L7_MCAST_PACKAGE
  /* Accessing IPMC_VLAN and IPMC GROUP tables requires workaround due to B1 errata */
  if (((SOC_IS_FIREBOLT(unit) || SOC_IS_HELIX(unit))) &&
      ((mem == MMU_IPMC_VLAN_TBLm) ||
       ((mem >= MMU_IPMC_GROUP_TBL0m) && (mem <= MMU_IPMC_GROUP_TBL7m))))
  {
     extern int _bcm_fb_ipmc_repl_freeze(int unit);
     if (_bcm_fb_ipmc_repl_freeze(unit) != BCM_E_NONE)
     {
       return L7_FAILURE;
     }
  }
#endif

  if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno))
  {
    copyno = MEM_BLOCK_ANY;
  }

  if (addr)
  {
    index = addr;
    count = (numwords ? numwords : 1);
  }
  else if (numwords)
  {
    index = 0;
    count = numwords;
  }
  else
  {
    index = soc_mem_index_min(unit, mem);
    if (soc_mem_is_sorted(unit,mem))
      count = soc_mem_entries(unit, mem, copyno);
    else
      count = soc_mem_index_max(unit, mem) - index + 1;
  }

/*   assert(copyno == 0); */

  entry_dw = soc_mem_entry_words(unit, mem);

  for (k = index; k < index + count; k++)
  {
    isValid = 0;
    valField = 0;
    memset(entry, 0, sizeof(entry));
    i = soc_mem_read(unit, mem, copyno, k, entry);
    if (i < 0)
    {
      printf("Read ERROR: table[%s.%d] index[0x%x]: %s\n",
             SOC_MEM_UFNAME(unit, mem), copyno, k, soc_errmsg(i));
      fflush(stdout);
      return L7_FAILURE;
    }

    /* if flags bit 2 is set, only print entries that have some non-zero data in them */
    if (flags & 0x04)
    {
      /* scan for valid fields */
      for (field = 0;field < (sizeof(hapiBroadDebugIdentifiersValid)/sizeof(int));field++)
      {
        if (soc_mem_field_length(unit, mem, hapiBroadDebugIdentifiersValid[field]) > 0)
        {
          valField = 1;
          if (soc_mem_field32_get(unit, mem, entry, hapiBroadDebugIdentifiersValid[field]))
          {
            isValid = 1;
            break; /* assume memory is valid */
          }
        }
      }

      /* scan the retrieved memory data for any non-zero data */
      for (j=0; j<entry_dw; j++)
      {
        if (entry[j] != 0)
        {
          /* non-zero data found, stop scanning */
          break;
        }
      }
      /* reached end of data without finding non-zero data, go to next memory entry */
      if (j == entry_dw)
      {
        continue;
      }

      if (valField && !isValid)
      {
        continue;
      }
    }

    if (flags & 0x01)
    {
      for (i = 0; i < entry_dw; i++)
        printf("%08x\n", entry[i]);
    }
    else
    {
      printf("%s.%d[0x%x]: ", SOC_MEM_UFNAME(unit, mem), copyno, k);

      if (flags & 0x02)
      {
        for (i = 0; i < entry_dw; i++)
          printf("0x%08x ", entry[i]);
      }
      else
        soc_mem_entry_dump(unit, mem, entry);

      printf("\n");
    }
  }

  fflush(stdout);

#ifdef L7_MCAST_PACKAGE
  if (((SOC_IS_FIREBOLT(unit) || SOC_IS_HELIX(unit))) &&
      ((mem == MMU_IPMC_VLAN_TBLm) ||
       ((mem >= MMU_IPMC_GROUP_TBL0m) && (mem <= MMU_IPMC_GROUP_TBL7m))))
  {
      extern int _bcm_fb_ipmc_repl_thaw(int unit);
      if (_bcm_fb_ipmc_repl_thaw(unit) != BCM_E_NONE)
      {
         return L7_FAILURE;
      }
  }
#endif

#endif
  return L7_SUCCESS;

}

L7_RC_t hapiBroadDebugUCMemDump(L7_int32 ingress_port)
{
  return(hapiBroadDebugMemoryDump(MEM_UCm, 0, SOC_PORT_BLOCK(0, ingress_port), 0, 0, 0));
}

#ifdef BCM_ESW_SUPPORT
/* The following functions are useful for XGS since table entries are hashed */
int hapiBroadDebugHash(L7_uint32 unit, L7_uint32 keyType, L7_uint32 key63_32, L7_uint32 key31_0)
{
  /* PTin removed: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  return L7_SUCCESS;
  #else
  hashinput_entry_t   hent;
  uint32              key[2];
  uint32              result = -1;

  key[0] = key31_0;
  key[1] = key63_32;

  keyType &= 0x03;

  soc_HASHINPUTm_field32_set(unit, &hent, KEY_TYPEf, keyType);
  soc_HASHINPUTm_field_set(unit, &hent, KEYf, key);

  SOC_IF_ERROR_RETURN(soc_mem_write(unit, HASHINPUTm, 0, 0, &hent));

  SOC_IF_ERROR_RETURN(READ_HASH_OUTPUTr(unit, &result));

  return(int)result;
  #endif
}

int hapiBroadDebugFindL2(L7_uint32 unit, L7_uint32 vidMacHi, L7_uint32 macLo)
{
  int rc;

  rc = hapiBroadDebugHash(unit, XGS_HASH_KEY_TYPE_L2, vidMacHi, macLo);

  if (rc > 0)
  {
    hapiBroadDebugMemoryDump(L2Xm, unit, 0, (rc << 3), 8, 0);
  }
  else
  {
    printf("Entry not found\n");
  }

  fflush(stdout);

  return 0;
}

int hapiBroadDebugFindL3(L7_uint32 unit, L7_uint32 ipAddr)
{
  /* PTin removed: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  /* Nothing done */
  #else
  int rc;

  rc = hapiBroadDebugHash(unit, XGS_HASH_KEY_TYPE_L3UC, 0, ipAddr);

  if (rc > 0)
  {
    hapiBroadDebugMemoryDump(L3Xm, unit, 0, (rc << 3), 8, 0);
  }
  else
  {
    printf("Entry not found\n");
  }
  fflush(stdout);
  #endif
  return 0;
}
#endif

/*********************************************************************
*
* @purpose Show the DOT1S details
*
*
* @returns L7_RC_t result
*
*********************************************************************/
L7_RC_t hapiBroadDebugStpList()
{
  bcm_stg_t     stg;
  L7_int32      stg_index, instNumber;

  /* Loop thru and print the instances */
  for (stg_index = 0; stg_index < HAPI_MAX_MULTIPLE_STP_INSTANCES; stg_index++)
  {
    hapiBroadDebugDot1sIndexMapGet(stg_index,&instNumber,&stg);
    printf ("Spanning Tree Instance [%d] : instNumber: %4d, stg: %4d\n", stg_index, instNumber, stg);
  }

  fflush(stdout);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Show the VLANs for a given Spanning Tree Instance
*
*
* @returns L7_RC_t result
*
*********************************************************************/
L7_RC_t hapiBroadDebugStpVlanList(L7_uint32 instNumber)
{
  bcm_vlan_t           *vlanList;
  bcm_stg_t             stg;
  L7_uint32             stg_index;
  L7_int32              numVlans, vlanIndex;
  L7_int32              rc;

  if (hapiBroadDot1sBcmStgMapGet(instNumber,&stg,&stg_index) == L7_FAILURE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadDot1sBcmStgMapGet' - FAILED : %d (Spanning Tree Instance not found)\n",
                   __FILE__, __LINE__, __FUNCTION__, instNumber);
    return(L7_FAILURE);
  }

  /* Get a list of vlans for the given spanning tree instance */
  rc = bcmx_stg_vlan_list (stg, &vlanList, &numVlans);
  if (BCM_E_NOT_FOUND == rc)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'bcmx_stg_vlan_list' - FAILED : %d (Spanning Tree Instance not found)\n",
                   __FILE__, __LINE__, __FUNCTION__, rc);
    return(L7_FAILURE);
  }
  else if (BCM_E_NONE != rc)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'bcmx_stg_vlan_list' - FAILED : %d\n",
                   __FILE__, __LINE__, __FUNCTION__, rc);
    return(L7_FAILURE);
  }


  /* Loop thru and print the instances */
  printf ("For Spanning Tree : %d\n", instNumber);
  for (vlanIndex = 0; vlanIndex < numVlans; vlanIndex++)
  {
    printf ("Vlan [%d] : %d\n", vlanIndex, vlanList [vlanIndex]);
  }

  fflush(stdout);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Show the VLANs for a given Spanning Tree Instance
*
*
* @returns L7_RC_t result
*
*********************************************************************/
L7_RC_t hapiBroadDebugStpPortList(L7_uint32 instNumber)
{
  DAPI_USP_t            usp;
  BROAD_PORT_t         *hapiPortPtr;
  bcm_stg_t             stg;
  L7_uint32             stg_index;
  L7_int32              rc;
  bcm_stg_stp_t         stgState;
  bcmx_lport_t          lport;


  if (hapiBroadDot1sBcmStgMapGet(instNumber,&stg,&stg_index) == L7_FAILURE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadDot1sBcmStgMapGet' - FAILED : %d (Spanning Tree Instance not found)\n",
                   __FILE__, __LINE__, __FUNCTION__, instNumber);
    return(L7_FAILURE);
  }

  for (usp.unit=0;usp.unit<dapi_g->system->totalNumOfUnits;usp.unit++)
  {
    for (usp.slot=0;usp.slot<dapi_g->unit[usp.unit]->numOfSlots;usp.slot++)
    {
      if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardId != L7_NULL)
      {
        for (usp.port=0;usp.port<dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
        {
          /* Get the logical BCMX port */
          hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
          lport = hapiPortPtr->bcmx_lport;

          /* Call BCMX to get the state for a port in a STG instance */
          rc = bcmx_stg_stp_get (lport, stg, (int*)&stgState);
          if (BCM_E_NOT_FOUND == rc)
          {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                           "\n%s %d: In %s call to 'bcmx_stg_stp_get' - FAILED : %d (Spanning Tree Instance not found)\n",
                           __FILE__, __LINE__, __FUNCTION__, rc);
            return(L7_FAILURE);
          }
          else if (BCM_E_NONE != rc)
          {
            SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                           "\n%s %d: In %s call to 'bcmx_stg_stp_get' - FAILED : %d\n",
                           __FILE__, __LINE__, __FUNCTION__, rc);
            return(L7_FAILURE);
          }



          /* Print the state informatino for the port */
          printf ("Unit : %d Port : %d \t\t State :", usp.unit, (usp.port + 1));
          switch (stgState)
          {
          case BCM_STG_STP_DISABLE:
            printf ("Disabled\n");
            break;
          case BCM_STG_STP_BLOCK:
            printf ("Discarding\n");
            break;
          case BCM_STG_STP_LISTEN:
            printf ("Listening\n");
            break;
          case BCM_STG_STP_LEARN :
            printf ("Learning\n");
            break;
          case BCM_STG_STP_FORWARD:
            printf ("Forwarding\n");
            break;
          case BCM_STG_STP_COUNT:
            printf ("Count\n");
            break;
          default:
            printf ("Unknown ??\n");
            break;
          }

        }
      }
    }
  }

  fflush(stdout);

  return L7_SUCCESS;
}

#ifdef BCM_ESW_SUPPORT
static int
dreg(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
  struct dreg_data *dd = data;
  uint32 value;
  uint64 val64, resetVal;
  char name[80];
  int is64, is_default, rv;
  int no_match = FALSE;  /* If specific port/block requested, turns true */
  char rval_str[20];

  /* Filter (set no_match) on ports and blocks if selected. */
  if (dd->only_port != DREG_PORT_ALL)
  {
    /* Only print ports that match */
    if (ainfo->port != dd->only_port)
    {
      no_match = TRUE;
    }
  }

  if (dd->only_block != DREG_BLOCK_ALL)
  {
    /* Only print blocks that match */
    if (ainfo->block != dd->only_block)
    {
      no_match = TRUE;
    }
    else
    { /* Match found; undo no_match */
      no_match = FALSE;
    }
  }

  if (no_match)
  {
    return 0;
  }

  soc_reg_sprint_addr(unit, name, ainfo);

  if (dd->dreg_select == DREG_ADDR)
  {
    printf("0x%08x %s\n", ainfo->addr, name);
    fflush(stdout);
    return 0;
  }

  SOC_REG_RST_VAL_GET(unit, ainfo->reg, resetVal);
  format_uint64(rval_str, resetVal);

  if (dd->dreg_select == DREG_RVAL)
  {
    printf("0x%08x %s = 0x%s\n", ainfo->addr, name, rval_str);
    fflush(stdout);
    return 0;
  }

  if (SOC_REG_INFO(unit, ainfo->reg).flags & SOC_REG_FLAG_WO)
  {
    printf("0x%08x %s = Write Only\n", ainfo->addr, name);
    fflush(stdout);
    return 0;
  }

  is64 = SOC_REG_IS_64(unit, ainfo->reg);

  if (is64)
  {
    rv = soc_reg64_read(dd->unit, ainfo->addr, &val64);
    is_default = COMPILER_64_EQ(val64, resetVal);
  }
  else
  {
    rv = soc_reg32_read(dd->unit, ainfo->addr, &value);
    is_default = (value == COMPILER_64_LO(resetVal));
  }

  if (rv < 0)
  {
    printf("0x%08x %s = ERROR\n", ainfo->addr, name);
    fflush(stdout);
    return 0;
  }

  if (dd->dreg_select == DREG_DIFF && is_default)
  {
    return 0;
  }

  if (is64)
  {
    printf("0x%08x %s = 0x%08x%08x\n",
           ainfo->addr, name,
           COMPILER_64_HI(val64), COMPILER_64_LO(val64));
  }
  else
  {
    printf("0x%08x %s = 0x%08x\n", ainfo->addr, name, value);
  }

  fflush(stdout);
  return 0;
}

L7_RC_t hapiBroadDebugRegDump(L7_uint32 unit)
{
  struct dreg_data dd;

  dd.unit = unit;
  dd.dreg_select = DREG_REGS;
  dd.only_port = DREG_PORT_ALL;
  dd.only_block = DREG_BLOCK_ALL;

  (void) soc_reg_iterate(unit, dreg, &dd);

  return L7_SUCCESS;

}

L7_RC_t hapiBroadDebugRegNonZeroDump(L7_uint32 unit)
{
  struct dreg_data dd;

  dd.unit = unit;
  dd.dreg_select = DREG_DIFF;
  dd.only_port = DREG_PORT_ALL;
  dd.only_block = DREG_BLOCK_ALL;

  (void) soc_reg_iterate(unit, dreg, &dd);

  return L7_SUCCESS;

}

L7_RC_t hapiBroadDebugRegPortDump(L7_uint32 unit, L7_uint32 port)
{
  struct dreg_data dd;

  dd.unit = unit;
  dd.dreg_select = DREG_REGS;
  dd.only_port = port;
  dd.only_block = DREG_BLOCK_ALL;

  (void) soc_reg_iterate(unit, dreg, &dd);

  return L7_SUCCESS;

}

L7_RC_t hapiBroadDebugRegNonZeroPortDump(L7_uint32 unit, L7_uint32 port)
{
  struct dreg_data dd;

  dd.unit = unit;
  dd.dreg_select = DREG_DIFF;
  dd.only_port = port;
  dd.only_block = DREG_BLOCK_ALL;

  (void) soc_reg_iterate(unit, dreg, &dd);

  return L7_SUCCESS;

}

void hapiBroadDebugVlanPortMemberRemove(L7_uint32 unit, L7_uint32 vid, L7_uint32 port)
{
  pbmp_t              pbmp;

  SOC_PBMP_CLEAR(pbmp);
  SOC_PBMP_PORT_SET(pbmp, port);
  bcm_vlan_port_remove(unit, vid, pbmp);

}

L7_RC_t hapiBroadDebugVlanTable(L7_uint32 unit)
{
  int     k, i;
  uint32    entry[SOC_MAX_MEM_WORDS];
  int     entry_dw;
  int                 copyno, index, count ;
  int                 mem = VLAN_TABm;

  /* This logic is for XGS */
  copyno = SOC_MEM_BLOCK_ANY(unit, mem);
  index = soc_mem_index_min(unit, mem);
  if (soc_mem_is_sorted (unit, mem))
    count = soc_mem_entries(unit, mem, copyno);
  else
    count = soc_mem_index_max(unit, mem) - index + 1;

  entry_dw = soc_mem_entry_words(unit, mem);

  for (k = index; k < index + count; k++)
  {
    if ((i = soc_mem_read(unit, mem, copyno, k, entry)) < 0)
    {
      printf("Read ERROR: table %s.%s[%d]: %s\n",
             SOC_MEM_UFNAME(unit, mem),
             SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i));
      fflush(stdout);
      return L7_FAILURE;
    }

    if (mem == VLAN_TABm && !soc_VLAN_TABm_field32_get(unit,
                                                       (vlan_tab_entry_t *) entry,
                                                       VALIDf))
    {
      continue;
    }

    printf("%s.%s[%d]: ",
           SOC_MEM_UFNAME(unit, mem),
           SOC_BLOCK_NAME(unit, copyno),
           k);

    for (i = 0; i < entry_dw; i++)
    {
      printf("0x%08x ", entry[i]);
    }
    printf("\n");

    soc_mem_entry_dump(unit, mem, entry);

    printf("\n");
  }

  fflush(stdout);
  return L7_SUCCESS;
}

/*
 * Function:
 *  hapiBroadDebugPortStat
 * Purpose:
 *  Table display of port information
 * Parameters:
 *  unit - SOC unit #
 * Returns:
 *  CMD_OK/CMD_FAIL
 */
L7_RC_t hapiBroadDebugPortStat(int unit)
{
  pbmp_t          pbm;
  bcm_port_info_t info_all[SOC_MAX_NUM_PORTS];
  soc_port_t      p;
  int             r;
  uint32          mask;

  BCM_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
  BCM_PBMP_AND(pbm, PBMP_PORT_ALL(unit));
  if (BCM_PBMP_IS_NULL(pbm))
  {
    printf("No ports specified.\n");
    fflush(stdout);
    return L7_FAILURE;
  }

  if (SOC_IS_HERCULES(unit))
  {
    mask = BCM_PORT_HERC_ATTRS;
  }
  else
  {
    mask = BCM_PORT_ATTR_ALL_MASK;
  }

  PBMP_ITER(pbm, p)
  {
    if (p < SOC_MAX_NUM_PORTS) {
      info_all[p].action_mask = mask;
      if ((r = bcm_port_selective_get(unit, p, &info_all[p])) < 0)
      {
        printf("Error: Could not get port %s information: %d\n",
               SOC_PORT_NAME(unit, p), r);
        fflush(stdout);
        return L7_FAILURE;
      }
    }
  }

#ifdef L7_BCM_SHELL_SUPPORT
  brief_port_info_header(unit);

  PBMP_ITER(pbm, p)
  {
    if (p < SOC_MAX_NUM_PORTS) {
      #if (SDK_VERSION_IS >= SDK_VERSION(6,4,0,0))
       brief_port_info(unit, SOC_PORT_NAME(unit, p), &info_all[p], mask);
      #else
       brief_port_info(SOC_PORT_NAME(unit, p), &info_all[p], mask);
      #endif
    }
  } /* for each port */
#endif

  return L7_SUCCESS;
}

/*
 * Function:
 *  hapiBroadDebugARLTableDump
 * Purpose:
 *  Table display of port information
 * Parameters:
 *  unit - SOC unit #
 * Returns:
 *  CMD_OK/CMD_FAIL
 */
L7_RC_t hapiBroadDebugARLTableDump(int unit)
{
  soc_control_t     *soc = SOC_CONTROL(unit);


  if (soc->arlShadow != NULL)
  {

    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);

    if (soc_feature(unit, soc_feature_arl_hashed))
    {
      shr_avl_traverse(soc->arlShadow, soc_l2x_entry_dump, NULL);
    }

    sal_mutex_give(soc->arlShadowMutex);
  }
  else
  {
    printf("No software ARL shadow table\n");
  }

  fflush(stdout);
  return L7_SUCCESS;
}

void hapiBroadDebugStatsDump(int unit, int port)
{
  int i;
  int rc;
  unsigned int val;


  for (i = 0; i < snmpValCount; i++)
  {
    rc = bcm_stat_get32(unit, port, i, &val);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      val = 0;
    }

    printf("stat[%d] == 0x%x\n",i,val);
  }

  fflush(stdout);
  return;
}

L7_RC_t hapiBroadDebugStpStateFwdSet(int unit, int slot, int port, L7_BOOL enable)
{
  DAPI_USP_t            usp;
  DAPI_INTF_MGMT_CMD_t  cmdInfo;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  cmdInfo.cmdData.stpState.getOrSet = DAPI_CMD_SET;

  cmdInfo.cmdData.dot1sState.instNumber = 0;

  if (enable == L7_TRUE)
    cmdInfo.cmdData.dot1sState.state = DAPI_PORT_DOT1S_FORWARDING;
  else
    cmdInfo.cmdData.dot1sState.state = DAPI_PORT_DOT1S_ADMIN_DISABLED;

  /* Create Logical CPU Card */
  return dapiCtl(&usp,DAPI_CMD_INTF_DOT1S_STATE,&cmdInfo);
}

/*
 * Function:
 *  hapiBroadDebugMmuSet
 * Purpose:
 *  Set the Draco Mmu registers
 * Parameters:
 *  unit - SOC unit #
 *  holcospktset
 *  holcoscellset
 *  holcoscellset_higig
 *  ibpcellset
 *  ibpdiscardset
 * Returns:
 *  CMD_OK/CMD_FAIL
 */
L7_RC_t hapiBroadDebugMmuSet(int unit,
                             int port_num,
                             int holcospktset,
                             int holcoscellset,
                             int holcoscellset_higig,
                             int ibpcellset,
                             int ibpdiscardset)
{
  /* PTin removed: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  /* Nothing done */
  #else
  soc_port_t            port;
  uint32                cos, val;

  /* if all ports */
  if (port_num == -1)
  {
    PBMP_ALL_ITER(unit, port)
    {
      for (cos = 0; cos < 8; cos++)
      {
        val = 0;
        soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val, PKTSETLIMITf,
                          holcospktset);
        SOC_IF_ERROR_RETURN(WRITE_HOLCOSPKTSETLIMITr(unit, port,
                                                     cos, val));
        val = 0;

        if (port == IPIC_PORT(unit))
          soc_reg_field_set(unit, HOLCOSCELLSETLIMITr, &val,
                            CELLSETLIMITf,
                            holcoscellset_higig);
        else
          soc_reg_field_set(unit, HOLCOSCELLSETLIMITr, &val,
                            CELLSETLIMITf,
                            holcoscellset);
        SOC_IF_ERROR_RETURN(WRITE_HOLCOSCELLSETLIMITr(unit, port,
                                                      cos, val));
      }
      val = 0;
      soc_reg_field_set(unit, IBPCELLSETLIMITr, &val,
                        CELLSETLIMITf, ibpcellset);
      SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, val));
      val = 0;
      soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &val,
                        DISCARDSETLIMITf, ibpdiscardset);
      SOC_IF_ERROR_RETURN(WRITE_IBPDISCARDSETLIMITr(unit, port, val));
    }
  }
  else
  {
    /* only this port */
    port = port_num;

    for (cos = 0; cos < 8; cos++)
    {
      val = 0;
      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val, PKTSETLIMITf,
                        holcospktset);
      SOC_IF_ERROR_RETURN(WRITE_HOLCOSPKTSETLIMITr(unit, port,
                                                   cos, val));
      val = 0;

      if (port == IPIC_PORT(unit))
        soc_reg_field_set(unit, HOLCOSCELLSETLIMITr, &val,
                          CELLSETLIMITf,
                          holcoscellset_higig);
      else
        soc_reg_field_set(unit, HOLCOSCELLSETLIMITr, &val,
                          CELLSETLIMITf,
                          holcoscellset);
      SOC_IF_ERROR_RETURN(WRITE_HOLCOSCELLSETLIMITr(unit, port,
                                                    cos, val));
    }
    val = 0;
    soc_reg_field_set(unit, IBPCELLSETLIMITr, &val,
                      CELLSETLIMITf, ibpcellset);
    SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, val));
    val = 0;
    soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &val,
                      DISCARDSETLIMITf, ibpdiscardset);
    SOC_IF_ERROR_RETURN(WRITE_IBPDISCARDSETLIMITr(unit, port, val));
  }
  #endif
  return L7_SUCCESS;
}


int hapiBroadDebugPktSendEnabled = 0;
int hapiBroadDebugPktSendVlanId = 1;
int hapiBroadDebugPktSendPktCount = 0;
BROAD_PORT_t   *hapiPortPtr;

L7_uint32 maxTxTime = 0;
void printMaxTxTime(int reset)
{
  printf("maxTxTime %d\n", maxTxTime);
  if (reset)
  {
      maxTxTime = 0;
  }
  fflush(stdout);
}
void hapiBroadDebugPktSendTask(L7_uint32 numArgs, DAPI_t *dapi_g)
{
  bcm_pkt_t                pkt;
  bcm_pkt_blk_t            pkt_blk;
  unsigned int             pkt_count;
  int                      i;
  L7_uint64                t1, t2;

  memset (&pkt, 0, sizeof(pkt));
  pkt.pkt_data  = &pkt_blk;
  pkt.blk_count = 1;
  pkt.call_back = L7_NULLPTR;  /* only used for async responses which we do not use */
  pkt.pkt_data->data = sal_dma_alloc(64, "hapiBuf");

  pkt.cos = HAPI_BROAD_EGRESS_NORMAL_PRIORITY_COS;

  pkt.pkt_data->len = 64;
  pkt.flags = BCM_TX_CRC_APPEND | BCM_TX_NO_PAD;

  if (pkt.pkt_data->data == NULL)
  {
    return;
  }

  for (i = 0; i < 64; i++)
  {
    pkt.pkt_data->data[i] = i | (i << 4);
  }

  *(L7_ushort16 *)&pkt.pkt_data->data[12] = osapiHtons(L7_ETYPE_8021Q);
  *(L7_ushort16 *)&pkt.pkt_data->data[14] = osapiHtons(BCM_VLAN_CTRL(0, 0, hapiBroadDebugPktSendVlanId));

  while (1)
  {
    if (hapiBroadDebugPktSendEnabled)
    {
      hapiBroadDebugPktReceiveEnabled = 1;
      if (hapiBroadDebugPktSendPktCount == 0)
      {
        pkt_count = 0xffffffff;
      }
      else
      {
        pkt_count = hapiBroadDebugPktSendPktCount;
      }

      for (i = 0; i < pkt_count; i++)
      {
        osapiTaskYield();
        bcmx_tx_pkt_untagged_set(&pkt,L7_FALSE);
        t1 = osapiTimeMillisecondsGet64();
        bcmx_tx_uc(&pkt, hapiPortPtr->bcmx_lport,BCMX_TX_F_CPU_TUNNEL);
        hapiBroadDebugSendCounter++;

        t2 = osapiTimeMillisecondsGet64();

        if ((t2 - t1) > maxTxTime)
        {
          maxTxTime = t2 - t1;
          if ((t2 - t1) > 50)
          {
            OSAPI_TRACE_EVENT (L7_TRACE_EVENT_DRIVER_TX,
                               (L7_uchar8 *)L7_NULLPTR, 0 );
  
          }
        }

        if (hapiBroadDebugPktSendEnabled == 0)
        {
          break;
        }
      }

      hapiBroadDebugPktSendEnabled = 0;
      osapiSleep(3);
      hapiBroadDebugPktReceiveEnabled = 0;

      /* generate report */
      printf("\r\n=============================================================\r\n");
      printf("                     Test Report \r\n");
      printf("\r\n=============================================================\r\n");
      if(hapiBroadDebugSendCounter == hapiBroadDebugReceiveCounter)
      {
        printf("\nSent %u  packets and received %u packets and the test is successful",
               hapiBroadDebugSendCounter,hapiBroadDebugReceiveCounter);
      }
      else
      {
        printf("\nSent %u packets and received %u packets and the test is unsuccessful",
               hapiBroadDebugSendCounter,hapiBroadDebugReceiveCounter);
      }
      printf("\r\n\r\n=======================END===================================\r\n");

      fflush(stdout);

      /* reset the counters */
      hapiBroadDebugSendCounter = hapiBroadDebugReceiveCounter = 0;
    }
    else
    {
      osapiSleep(2);
    }
  }
}

void hapiBroadDebugPktSend(int unit,
                           int slot,
                           int port,
                           int vlan_id,
                           int pkt_size,
                           int pkt_count)
{
  bcm_pkt_t                pkt;
  bcm_pkt_blk_t            pkt_blk;
  DAPI_USP_t hapiBroadDebugPktSendUSP;

  hapiBroadDebugPktSendUSP.unit = unit;
  hapiBroadDebugPktSendUSP.slot = slot;
  hapiBroadDebugPktSendUSP.port = port;
  hapiBroadDebugPktSendVlanId = vlan_id;
  hapiBroadDebugPktSendPktCount = pkt_count;

  hapiPortPtr = HAPI_PORT_GET(&hapiBroadDebugPktSendUSP, dapi_g);

  memset (&pkt, 0, sizeof (pkt));
  pkt.pkt_data  = &pkt_blk;
  pkt.blk_count = 1;
  pkt.call_back = L7_NULLPTR;  /* only used for async responses which we do not use */
  pkt.pkt_data->data = sal_dma_alloc(pkt_size, "hapiBuf");

  pkt.cos = 0;

  pkt.pkt_data->len = pkt_size;
  pkt.flags = BCM_TX_CRC_APPEND | BCM_TX_NO_PAD;

  if (pkt.pkt_data->data == NULL)
  {
    printf("\nSending packet failed due to the failure of dma alloc\n");
    fflush(stdout);
    return;
  }

  *(L7_ushort16 *)&pkt.pkt_data->data[12] = osapiHtons(L7_ETYPE_8021Q);
  *(L7_ushort16 *)&pkt.pkt_data->data[14] = osapiHtons(BCM_VLAN_CTRL(0, 0, hapiBroadDebugPktSendVlanId));

  while (pkt_count)
  {
    osapiTaskYield();
    printf("\nSending packet[%d] to %d.%d.%d (lport %d)of size %d \n",pkt_count,unit,slot,port,hapiPortPtr->bcmx_lport,pkt_size);
    bcmx_tx_pkt_untagged_set(&pkt,L7_FALSE);
    bcmx_tx_uc(&pkt, hapiPortPtr->bcmx_lport,BCMX_TX_F_CPU_TUNNEL);
    pkt_count--;
    osapiSleep(2);
  }
  printf("\nPacket sending process to %d.%d.%d of size %d is OVER \n",unit,slot,port,pkt_size);
  fflush(stdout);
}

int hapiBroadDebugPktSendStart(int unit, int slot, int port, int vlan_id, int pkt_count)
{

  static int first_time = 1;
  DAPI_USP_t hapiBroadDebugPktSendUSP;

  hapiBroadDebugPktSendUSP.unit = unit;
  hapiBroadDebugPktSendUSP.slot = slot;
  hapiBroadDebugPktSendUSP.port = port;
  hapiBroadDebugPktSendVlanId = vlan_id;
  hapiBroadDebugPktSendPktCount = pkt_count;

  hapiPortPtr = HAPI_PORT_GET(&hapiBroadDebugPktSendUSP, dapi_g);

  hapiBroadDebugPktSendEnabled = 1;

  if (first_time)
  {
    if (osapiTaskCreate("hapiTxTask",hapiBroadDebugPktSendTask,1,dapi_g, L7_DEFAULT_STACK_SIZE,
                        L7_DEFAULT_TASK_PRIORITY,L7_DEFAULT_TASK_SLICE) == L7_ERROR)
    {
        L7_LOG_ERROR(0);
    }
  }

  first_time = 0;

  return 0;
}


int hapiBroadDebugPktSendStop()
{
  hapiBroadDebugPktSendEnabled = 0;

  return 0;
}


/* port is 1 based unlike most hapiBroadDebugFunctions */
int hapiBroadDebugDuplexAdvertSet(int unit,int slot,int port,int duplex)
{
  BROAD_PORT_t    *hapiPortPtr;
  DAPI_USP_t       usp;
  int              rv;
  bcm_port_abil_t  ability_mask;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port-1;

  hapiPortPtr = HAPI_PORT_GET(&usp,dapi_g);

  rv = bcmx_port_advert_get(hapiPortPtr->bcmx_lport, &ability_mask);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    return rv;
  }

  if (duplex == 0)
  {
    ability_mask &= ~(BCM_PORT_ABIL_10MB_FD | BCM_PORT_ABIL_100MB_FD | BCM_PORT_ABIL_1000MB_FD);
    ability_mask |=  (BCM_PORT_ABIL_10MB_HD | BCM_PORT_ABIL_100MB_HD | BCM_PORT_ABIL_1000MB_HD);
  }
  else
  {
    ability_mask &= ~(BCM_PORT_ABIL_10MB_HD | BCM_PORT_ABIL_100MB_HD | BCM_PORT_ABIL_1000MB_HD);
    ability_mask |=  (BCM_PORT_ABIL_10MB_FD | BCM_PORT_ABIL_100MB_FD | BCM_PORT_ABIL_1000MB_FD);
  }

  rv = bcmx_port_advert_set(hapiPortPtr->bcmx_lport, ability_mask);

  return rv;
}

/* Test Filter Reinstall Functionality.
*/
int hapiBroadDebugFltInst1 (void)
{
  /* PTin removed: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  /* Nothing done */
  #else
  bcm_filterid_t test_flt;
  int rv;
  int unit;

  unit = 0;

  rv = bcm_filter_create(unit, &test_flt);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_qualify_data16(unit, test_flt, 0, 0xffff, 0xffff);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_action_match(unit, test_flt, bcmActionSetPrio, 7);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_install(unit, test_flt);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_destroy(unit, test_flt);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }
  #endif
  return 0;
}

/* Reinstalls the same filter.
*/
int hapiBroadDebugFltReInst1 (void)
{
  /* PTin removed: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  /* Nothing done */
  #else
  bcm_filterid_t test_flt;
  int rv;
  int unit;
  pbmp_t port_list;

  BCM_PBMP_CLEAR (port_list);

  BCM_PBMP_PORT_ADD (port_list, 24); /* Front panel port 25 */

  unit = 0;

  rv = bcm_filter_create(unit, &test_flt);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_qualify_data16(unit, test_flt, 0, 0xffff, 0xffff);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_qualify_ingress(unit, test_flt, port_list);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_action_match(unit, test_flt, bcmActionSetPrio, 6);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_reinstall(unit, test_flt);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }

  rv = bcm_filter_destroy(unit, test_flt);
  if (rv != BCM_E_NONE)
  {
    L7_LOG_ERROR (rv);
  }
  #endif
  return 0;
}

/*****************************************************
** Test timed osapiSemaTake and osapiMessageReceive
*****************************************************/
int hapiBroadDebugSemTest (int time_ms)
{
  void *sem;
  L7_RC_t rc;


  sem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (sem != L7_NULLPTR) {
    rc = osapiSemaTake (sem, time_ms);

    printf("rc = %d\n", rc);

    osapiSemaDelete (sem);
  }

  fflush(stdout);
  return 0;
}

int hapiBroadDebugQueueTest (int time_ms)
{
  void *queue;
  L7_RC_t rc;
  char msg;


  queue = osapiMsgQueueCreate ("test_q", 1, 1);
  if (queue != L7_NULLPTR) {
    rc = osapiMessageReceive (queue, &msg, 1, time_ms);

    printf("rc = %d\n", rc);

    osapiMsgQueueDelete (queue);
  }

  fflush(stdout);
  return 0;
}

void hapiBroadDebugMemoryDumpAll(int unit, int mmuDump)
{
  int mem,block;

  for (mem = 0; mem < NUM_SOC_MEM;mem++)
  {
    if (SOC_MEM_IS_VALID(unit,mem))
    {
      SOC_MEM_BLOCK_ITER(unit,mem,block){
        /* only print MMU Tables if asked */
        if ((MMU_BLOCK(unit) == block) && (!mmuDump)) continue;

        printf("******** Unit = %d , %s (%d), %s *********\n",
              unit,SOC_MEM_UFNAME(unit, mem),mem,SOC_BLOCK_NAME(unit, block));

        hapiBroadDebugMemoryDump(mem, unit, block, 0, 0, 4);
      }
    }
  }

  fflush(stdout);
  return;
}

void hapiBroadDebugDumpAll(int mmuDump)
{
  int unit;
  soc_control_t       *soc;

  for (unit = 0; unit < soc_ndev; unit++)
  {
    if (!SOC_UNIT_VALID(unit)) continue;

    soc = SOC_CONTROL(unit);

    printf("********  BEGIN BCM_UNIT %d ***********\n",unit);
    printf("soc_dump(%d)\n",unit);
    soc_dump(unit," ");

    printf("soc_chip_dump(%d,0x%x)\n",unit,(L7_uint32)soc->chip_driver);
    soc_chip_dump(unit,soc->chip_driver);

    printf("********  CMIC PCI Reg DUMPS ***********\n");
    printf("hapiBroadDebugPcimDump(%d,%d,%d)\n",unit,CMIC_OFFSET_MIN,CMIC_OFFSET_MAX-CMIC_OFFSET_MIN);
    hapiBroadDebugPcimDump(unit,CMIC_OFFSET_MIN,CMIC_OFFSET_MAX-CMIC_OFFSET_MIN,0);

    printf("********  Reg DUMPS (non-zero)***********\n");
    printf("hapiBroadDebugRegNonZeroDump(%d)\n",unit);;
    hapiBroadDebugRegNonZeroDump(unit);

    printf("********  Memory DUMPS ***********\n");
    printf("hapiBroadDebugMemoryDumpAll(%d,%d)\n",unit,mmuDump);
    hapiBroadDebugMemoryDumpAll(unit,mmuDump);

    printf("********  END BCM_UNIT %d ***********\n",unit);
  }

  fflush(stdout);
}

/* Dump function for BCAST, MCAST, DLF storm control registers */
void hapiBroadDebugStormCtrlRegDump(int bcm_unit, int bcm_port)
{
  int pps,flags,enable;
  printf("\n-------------------------------------------------------------");
  printf("\n\t\tEnable\t\tThreshold");
  printf("\n-------------------------------------------------------------");

  bcm_rate_bcast_get(bcm_unit, &pps,&flags,bcm_port);
  enable = (flags & BCM_RATE_BCAST) ? 1 : 0;
  printf("\nBCAST CTRL \t %d \t\t %d",enable,pps);

  bcm_rate_mcast_get(bcm_unit, &pps,&flags,bcm_port);
  enable = (flags & BCM_RATE_MCAST) ? 1 : 0;
  printf("\nMCAST CTRL \t %d \t\t %d",enable,pps);

  bcm_rate_dlfbc_get(bcm_unit, &pps,&flags,bcm_port);
  enable = (flags & BCM_RATE_DLF) ? 1 : 0;
  printf("\nDLFCAST CTRL \t %d \t\t %d",enable,pps);
  printf("\n-------------------------------------------------------------");

  fflush(stdout);
  return ;
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
void hapiDebugSysInfoDumpRegister(void)
{
  supportDebugCategory_t supportDebugCategory;

  memset(&supportDebugCategory, 0x00, sizeof(supportDebugCategory));
  supportDebugCategory.componentId = L7_DRIVER_COMPONENT_ID;

  /*-----------------------------*/
  /* Register sysInfo debug dump */
  /*-----------------------------*/
  supportDebugCategory.category = SUPPORT_CATEGORY_SYSTEM;
  supportDebugCategory.info.supportRoutine = hapiDebugRegisteredSysInfoDump;
  strncpy(supportDebugCategory.info.supportRoutineName, "hapiDebugRegisteredSysInfoDump",
          SUPPORT_DEBUG_HELP_NAME_SIZE);
  (void) supportDebugCategoryRegister(supportDebugCategory);
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
void hapiDebugRegisteredSysInfoDump(void)
{
  int unit;

  sysapiPrintf("\r\n");
  sysapiPrintf("/*=====================================================================*/\n");
  sysapiPrintf("/*                  DRIVER  INFORMATION                                   */\n");
  sysapiPrintf("/*=====================================================================*/\n");

#ifdef L7_ROUTING_PACKAGE
  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("/* Output of hapiBroadL3StatsShow():     */\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("\r\n");

  hapiBroadL3StatsShow();

#endif

  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("/* Output of bcm_rx_show():     */\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("\r\n");

  for (unit = 0; unit < soc_ndev; unit++)
  {
    if (!SOC_UNIT_VALID(unit)) continue;
    sysapiPrintf("\n******** bcm_rx_show Unit(%d)********\n\n",unit);
    bcm_rx_show(unit);
  }

#ifdef L7_STACKING_PACKAGE
  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("/* Output of bcm_rlink_dump():     */\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("\r\n");

  bcm_rlink_dump();

  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("/* Output of hpcDebugBcmCpudbShow():     */\n");
  sysapiPrintf("/*---------------------------------------*/\n");
  sysapiPrintf("\r\n");

  /*hpcDebugBcmCpudbShow();*/
#endif

  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("/* Output of rxStats():            */\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("\r\n");

  rxStats();

  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("/* Output of txStats():            */\n");
  sysapiPrintf("/*---------------------------------*/\n");
  sysapiPrintf("\r\n");

  txStats();

  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*--------------------------------------------------*/\n");
  sysapiPrintf("/* Output of hapiBroadDebugPhyRegDump():            */\n");
  sysapiPrintf("/*--------------------------------------------------*/\n");
  sysapiPrintf("\r\n");

  for (unit = 0; unit < soc_ndev; unit++)
  {
    if (!SOC_UNIT_VALID(unit)) continue;
    sysapiPrintf("\n******** hapiBroadDebugPhyRegDump(%d)********\n",unit);
    hapiBroadDebugPhyRegDump(unit,0);
  }


  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
  sysapiPrintf("/*----------------------------------------*/\n");
  sysapiPrintf("/* Output of hapiBroadDebugDumpAll():     */\n");
  sysapiPrintf("/*----------------------------------------*/\n");
  sysapiPrintf("\r\n");

  hapiBroadDebugDumpAll(0);

  sysapiPrintf("\r\n");
  sysapiPrintf("\n");
}

/* This routine will dis/en able trapping of BPDUs to CPU on xgs3 platforms
 * and dis/en able flooding of BDPU */
void hapiBroadDebugModifyBpduTrap(int enable)
{
  BROAD_SYSTEM_t     *hapiSystem;
  L7_RC_t             result;
  L7_uchar8           res_macda[]   = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};
  L7_uchar8           zero_mac_addr[]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  L7_uchar8           res_macmask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, 0xC0};
  L7_uchar8           exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_POLICY_t      sysId2;
  BROAD_POLICY_RULE_t ruleId;
  bcm_chip_family_t board_family;
  L7_uchar8 inValid = -1;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (enable == L7_FALSE)
  {
    if (hapiSystem->sysId2 == inValid)
    {
       printf("\nTrap BPDU to CPU: Disabled Successfully\n");
       fflush(stdout);
       return;
    }
    result = hapiBroadPolicyDelete(hapiSystem->sysId2);

    if (result == L7_SUCCESS)
    {
       hapiSystem->sysId2 = inValid;
       printf("\nTrap BPDU to CPU: Disabled Successfully\n");
       fflush(stdout);
       return;
    }
    else
    {
      printf("\nTrap BPDU to CPU: Disabled Failed\n");
      fflush(stdout);
      return;
    }
  }

  if (hapiSystem->sysId2 != inValid)
  {
     printf("\nTrap BPDU to CPU: Enabled Successfully\n");
     fflush(stdout);
     return;
  }

  /* Create policy for L2 specific information, e.g. BPDUs */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);   /* Policy 2 of 2 */

  /* give BPDUs high priority */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, res_macda, res_macmask);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_BPDU_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

  /* Drop packets with all-zero source or destination mac addresses.
   * Restricted to XGS3 due to resource constraints on XGS2. */
  hapiBroadGetSystemBoardFamily(&board_family);
  if (board_family == BCM_FAMILY_FIREBOLT)
  {
    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACSA, zero_mac_addr, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);

    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, zero_mac_addr, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  }

#ifdef L7_STACKING_PACKAGE
  hapiBroadPolicyEnableFPS(); /* Enable on FPS ports, if applicable */
#endif

  result = hapiBroadPolicyCommit(&sysId2);

  if (L7_SUCCESS == result)
  {
    hapiSystem->sysId2 = sysId2;
    printf("\nTrap BPDU to CPU: Enabled Successfully\n");
    fflush(stdout);
  }
  return;
}
#endif

/*********************************************************************
* @purpose  Pass-thru function to invoke the Broadcom SDK diagnostic shell
*
* @param    void
*
* @returns  void
*
* @notes    This is here to provide the BSP with a way to start the
*           vendor SDK diag_shell() without making a direct reference.
*
* @end
*********************************************************************/
void hapiBroadDebugDiagShell(void)
{
#ifdef BCM_ESW_SUPPORT
#ifdef L7_BCM_SHELL_SUPPORT
  diag_shell();
#endif
#endif
}

#if (SDK_VERSION_IS < SDK_VERSION(6,3,2,0))
/* These routines sets up the debug level for BCM APIs and previously
 * resided in SDK code. As part of making SDK fixup-free, these routines
 * are moved here. With BCM diag shell, we should be able to enable
 * debug levels, but diag shell may not exist for some platforms.
 */
#ifdef BROADCOM_DEBUG
extern unsigned int bcm_debug_level;
extern char *bcm_debug_names[];
L7_uint32 bcm_debug_level_set(L7_uint32 lvl)
{
   L7_uint32 olvl = bcm_debug_level;
   bcm_debug_level = lvl;
   return olvl;
}

L7_uint32 bcm_debug_level_enable(L7_uint32 lvl)
{
   L7_uint32 olvl = bcm_debug_level;
   bcm_debug_level |= lvl;
   return olvl;
}

L7_uint32 bcm_debug_level_disable(L7_uint32 lvl)
{
   L7_uint32 olvl = bcm_debug_level;
   bcm_debug_level &= ~lvl;
   return olvl;
}

void bcm_debug_names_print()
{
   L7_uint32 i;
   for (i =0;i < BCM_DBG_COUNT;i++)
   {
       soc_cm_print("%-20s = 0x%x = %d\n",bcm_debug_names[i],1<<i,1<<i);
   }
}

L7_uint32 bcm_debug_level_print()
{
   L7_uint32 i;
   for (i =0;i < BCM_DBG_COUNT;i++)
   {
      if (bcm_debug_level & (1 << i))
          soc_cm_print("%-20s = 0x%x = %d\n",bcm_debug_names[i],1<<i,1<<i);
   }
   soc_cm_print("bcm_debug_level = 0x%x\n",bcm_debug_level);
   return bcm_debug_level;
}

extern L7_uint32 tks_debug_level;
L7_uint32  tks_level_set(L7_uint32 val)
{
   L7_uint32 oldval = tks_debug_level;
   tks_debug_level = (val);
   return oldval;
}

L7_uint32  tks_level_enable(L7_uint32 val)
{
   L7_uint32 oldval = tks_debug_level;
   tks_debug_level |= (val);
   return oldval;
}

L7_uint32  tks_level_disable(L7_uint32 val)
{
   L7_uint32 oldval = tks_debug_level;
   tks_debug_level &= (~val);
   return oldval;
}

void   tks_debug_vals()
{
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_NORMAL=0x%lx   /* Normal Output */\n",TKS_DBG_NORMAL));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_ERR=0x%lx      /* Print errors */\n", TKS_DBG_ERR));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_WARN=0x%lx     /* Print warnings */\n", TKS_DBG_WARN));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_VERBOSE=0x%lx  /* General verbose output */\n", TKS_DBG_VERBOSE));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_VVERBOSE=0x%lx /* Very verbose output */\n", TKS_DBG_VVERBOSE));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_DISCOVER=0x%lx /* Discovery/stacking */\n", TKS_DBG_DISCOVER));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_DISCV=0x%lx    /* Discovery verbose */\n", TKS_DBG_DISCV));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_TOPOLOGY=0x%lx /* Topology */\n", TKS_DBG_TOPOLOGY));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_TOPOV=0x%lx    /* Topology verbose */\n", TKS_DBG_TOPOV));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_STKTASK=0x%lx  /* Stack task */\n", TKS_DBG_STKTASK));
#if (SDK_VERSION_IS >= SDK_VERSION(6,3,2,0))
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_STKV=0x%lx     /* Stack task verbose */\n", TKS_DBG_STKV));
#else
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_STV=0x%lx      /* Stack task verbose */\n", TKS_DBG_STV));
#endif
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_TX=0x%lx       /* TX */\n", TKS_DBG_TX));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_NH=0x%lx       /* Next Hop */\n", TKS_DBG_NH));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_NHV=0x%lx      /* Next Hop Verbose */\n", TKS_DBG_NHV));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_ATP=0x%lx      /* ATP */\n", TKS_DBG_ATP));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_ATPV=0x%lx     /* ATP Verbose */\n", TKS_DBG_ATPV));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_C2C=0x%lx      /* CPU-to-CPU */\n", TKS_DBG_C2C));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_C2CV=0x%lx     /* CPU-to-CPU Verbose */\n", TKS_DBG_C2CV));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_CTPKT=0x%lx    /* CPUTRANS packet */\n", TKS_DBG_CTPKT));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_CTPKTV=0x%lx   /* CPUTRANS packet verbose */\n", TKS_DBG_CTPKTV));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_TUNNEL=0x%lx   /* Tunnel related */\n", TKS_DBG_TUNNEL));
TKS_DEBUG(TKS_DBG_ERR,("TKS_DBG_TUNNELV=0x%lx  /* Tunnel verbose */\n", TKS_DBG_TUNNELV));
}
#endif
#endif

extern _bcmx_port_t *_bcmx_port;
extern int bcmx_lport_max;
void debug_bcmx_db_print(void)
{
    int i=0;
    for (i = 0; i < bcmx_lport_max; i++) {
      if (BCMX_LPORT_VALID(i)) {
        sal_printf("\n***********************\n");
        sal_printf("modid = %u \n", _bcmx_port[i].modid);
        sal_printf("unit = %u \n", _bcmx_port[i].bcm_unit);
        sal_printf("modport = %u \n", _bcmx_port[i].modport);
        sal_printf("port = %u \n", _bcmx_port[i].bcm_port);
      }
    }

}

extern int _bcmx_port_info_dump(void);
void hapiBroadDebugBcmxInfoDump(void)
{
  _bcmx_port_info_dump();
}

/* Dump specified PHY Register for the specified port */
L7_int32 hapiBroadDebugSinglePhyRegDump(L7_int32 u, L7_uint32 p, L7_short16 phy_reg)
{
	L7_ushort16    phy_data;
	L7_uchar8      phy_id;
	L7_int32       rv = 0;

	if ((phy_reg < PHY_MIN_REG) || (phy_reg > PHY_MAX_REG))
		printf("\n invalid phy_reg value (enter 0 to 16)");

	phy_id = PORT_TO_PHY_ADDR(u, p);

	if (IS_XE_PORT(u, p)) {
	rv = soc_miimc45_read(u, phy_id, PHY_C45_DEV_PMA_PMD,
	                       phy_reg, &phy_data);
	} else {
	rv = soc_miim_read(u, phy_id, phy_reg, &phy_data);
	}
	if (rv < 0)
	{
		printf("\nError: Port %d: soc_miim_read failed: %s\n",
		       p , soc_errmsg(rv));
        fflush(stdout);
		return rv;
	}
	printf("0x%02x: 0x%04x", phy_reg, phy_data);

    fflush(stdout);

	return rv;
}


/*********************************************************************
*
* @purpose  Add support to load "sdk-preinit.soc", "sdk-postinit.soc"
*           and "sdk-postboot.soc" during bootup.
*
* @end
*
*********************************************************************/
char *_build_release = "unknown";
char *_build_user = "unknown";
char *_build_host = "unknown";
char *_build_date = "no date";
char *_build_datestamp = "no datestamp";
char *_build_tree = "unknown";
char *_build_arch = "unknown";
char *_build_os = "unknown";

#include "l3_addrdefs.h"
#include "osapi_support.h"

extern L7_uint32 upd_get_socscript_via_tftp (L7_char8 * file_name, L7_inet_addr_t * inet_address);
extern L7_uint32 upd_put_socscript_via_tftp (L7_char8 * file_name, L7_inet_addr_t * inet_address);

L7_uint32 hapiBroadSocFileGet (L7_char8 * file_name,
                               L7_char8 * ip_address)
{
  L7_int32 rc;
  L7_inet_addr_t inet_address;

  inet_address.family = L7_AF_INET;

  rc = osapiInetPton((L7_uint32)(inet_address.family),
                       ip_address, (L7_uchar8 *)(&(inet_address.addr)));

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Attempting to tftp %s from 0x%X ... ", file_name, inet_address.addr.ipv4.s_addr);

  rc = upd_get_socscript_via_tftp(file_name,&inet_address);

  if (6 == rc)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Success! Read %s\n", file_name);
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Failed! Could not read %s\n", file_name);
  }

  return(rc);
};

L7_uint32 hapiBroadSocFilePut (L7_char8 * file_name,
                               L7_char8 * ip_address)
{
  L7_int32 rc;
  L7_inet_addr_t inet_address;

  inet_address.family = L7_AF_INET;

  rc = osapiInetPton((L7_uint32)(inet_address.family),
                       ip_address, (L7_uchar8 *)(&(inet_address.addr)));

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Attempting to tftp %s to 0x%X ... ", file_name, inet_address.addr.ipv4.s_addr);

  rc = upd_put_socscript_via_tftp(file_name, &inet_address);

  if (6 == rc)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Success! Wrote %s\n", file_name);
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Failed! Could not write %s\n", file_name);
  }

  return(rc);
};

void hapiBroadSocFileLoad(char *file_name, L7_BOOL suppressFileNotAvail)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 filesize;
  L7_char8  command[256];

  hapiBroadDebugShellInit();

  rc =  osapiFsFileSizeGet (file_name, &filesize);

  if (rc != L7_ERROR && filesize != 0)
  {
    if (osapiSnprintf( command, sizeof(command),"rcload %s",file_name) > 0)
    {
#ifdef L7_BCM_SHELL_SUPPORT
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"sh_process_command(0,%s) returns %d\n", command, sh_process_command(0,command));
#else
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"sh_process_command(0,%s) is not compiled into this code.\n", command);
#endif
    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS, "rcload %s command construction failed\n",file_name);
    }
  }
  else if (L7_TRUE != suppressFileNotAvail)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS, "%s: does not exist\n",file_name);
  }

  return;
}

#ifdef L7_TRACE_ENABLED
#include <trgLib.h>
#include <wvLib.h>
void driverWvTraceSet(void)
{

  /* Configure Windview trace buffer */
  osapiWvOn(WV_CLASS_2);

  /* Configure triggers for warm restart */
  trgOff();
  trgAdd(L7_TRACE_TRIGGER_DRIVER_UNIT_FAIL_START, TRG_ENABLE, TRG_CTX_ANY, 0, 0,
          TRIGGER_COND_NO, 0, 0, 0, 0,
          1, NULL, TRG_ACT_WV_START, (L7_FUNCPTR)wvEvtLogStart, TRUE, 0);

  trgAdd(L7_TRACE_TRIGGER_DRIVER_UNIT_FAIL_END, TRG_ENABLE, TRG_CTX_ANY, 0, 0,
          TRIGGER_COND_NO, 0, 0, 0, 0,
          1, NULL, TRG_ACT_WV_STOP, (L7_FUNCPTR)wvEvtLogStop, TRUE, 0);
  trgOn();
}

void driverWvUnitFailTraceTriggerStart()
{
  OSAPI_TRACE_TRIGGER (L7_TRACE_TRIGGER_DRIVER_UNIT_FAIL_START);
}
#endif

