/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  bootos.c
*
* @purpose   Switch startup program.
*
* @component hardware
*
* @comments  none
*
* @create    02/05/2002
*
* @author    John W. Linville
*
* @end
*
*********************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"
#include "simapi.h"
#include "nvstoreapi.h"
#include "platform_cpu.h"
#include "l7_cnfgr_api.h"
#include "bspcpu_api.h"
#include "osapi_priv.h"
#include "bspapi.h"
#include "log_api.h"
#include "dim.h"
/* PTin added */
#include "ptin_globaldefs.h"
#include "logger.h"
#include "addrmap.h"
/* PTin end */

#ifdef L7_CLI_PACKAGE
extern void binsh(void);
#endif

#ifdef USE_LM75_MONITOR
extern void LM75MonitorTask(void);
#endif


#ifdef L7_TOOL_VALGRIND
L7_BOOL globValgrindRunning = L7_FALSE;
#endif


/*
 *These are usefull #defines used in tuning the
 *behavior.  A description precedes each one
 */

/*
 *define DISPLAY_BACKTRACE to enable the printing
 *of backtrace data
 */
#define DISPLAY_BACKTRACE

/*
 *define EXTRA_BACKTRACE_DETAIL to enable the
 *printing of the entire contents of each stack
 *frame.  If not defined only the stack link registers
 *will be printed
 */
#define EXTRA_BACKTRACE_DETAIL

/*
 *define BACKTRACE_DEPTH to any integer value to restrict
 *the recursion depth of the backtrace.  If undefined the
 *entire stack will be printed
 */
/*#define BACKTRACE_DEPTH 25*/

/*
 *define BACKTRACE_FILE = 1 to enable the
 *creation of a backtrace file
 */
#define BACKTRACE_FILE 1

/*
 *define BACKTRACE_SCREEN = 1 to enable the
 *display of a backtrace on the screen
 */
#define BACKTRACE_SCREEN 0

/*
 *this macro will conditionally print backtrace data to a
 *file or the screen or both.
 */
#define BT_PRINT(a, ...) \
  if ( BACKTRACE_FILE ) \
    if ( a != (FILE *)NULL ) \
      fprintf( a , __VA_ARGS__ ); \
  if ( BACKTRACE_SCREEN ) \
    printf( __VA_ARGS__ );

/*
*This variable should alias r1 to be the variable
*sigsegv_stack.  This lets us print a backtrace
*r1 is the stack pointer according to the SvR4
*linux ppc EABI so this obviously won't work
*elsewhere, but at the same time, this
*won't compile for other platforms so  we should
*know about by virtue of the compiler when we port
*to another processor
*/
#ifdef __powerpc__
register unsigned int *sigsegv_stack asm("r1");

/*
 *This structure provides us access to
 *The dark signal handler magic which is
 *otherwise hidden to the registered handler
 *This is powerpc specific and will need
 *re-implementation on other platforms
 */
struct sig_ctl_frame {
  siginfo_t info;
#ifdef L7_LINUX_24
  unsigned char pad[60]; /*Note: Linux 2.4 requires a pad of 60*/
#else
  unsigned char pad[192]; /*Note: Linux 2.6 requires a pad of 192*/
#endif
  struct pt_regs regs; /*The good stuff {|*/
};

/*
*This structure defines the start of our stack frame
*this is all I can put in a sturcure as the rest has
*variable length. The remainder is however, 4 byte aligned
*and so I can print it out as an array of ints. yea!
*/
struct stack_frame_intro_s
{
  unsigned int *backchain;
  unsigned int last_link_register;
};
#elif defined(__i386__)
/*
 * We need to define the right stuff here for an x86 backtrace.  For
 * now the functionality is disabled.  This should not be too bad, since
 * we can get a core dump and use gdb on it with current x86 platforms
 * (i.e. LinuxHost).
 */
#elif defined(__mips__)

register unsigned int *sigsegv_stack asm("$29");

#else
#error "the sigsev_stack variable is not defined for this processor.\n"\
"Please define one in accordance with your processor/compiler ABI.  Thank you"
#endif
/*NH*/

extern pthread_key_t osapi_name_key;
extern pthread_key_t osapi_signal_key;

extern void dtlNetInit(void);
extern void dg_blinkleds (void);

extern char **environ;
extern unsigned int ignore_service_port;

#define VOIDFUNCPTR void (*)(void)

pthread_mutex_t main_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t main_wait = PTHREAD_COND_INITIALIZER;

unsigned char ServicePortName[]
   = {'e', 't', 'h', '\0', '\0', '\0', '\0'};


static int already_called = 0;
L7_RC_t   hpcDiagStart(void);
/******************************************************************
** NAME: bspapiSwitchReset
**
** This function resets the box.
******************************************************************/
void bspapiSwitchReset (void)
{
  fflush (0);
  printf("\n\nReference platform resetting ...\n");

  /* Tell the fault handler not to log this error */
  already_called = 1;

  if(0 > chdir("/")){}
  fflush(NULL);
  sync();
/*  kill(1, SIGTERM);*/

  exit (1);
}

void startupStatusTask(int argc, void *argv)
{
  int i = 0;
  char seq[] = {'|', '/', '-', '\\'};

  printf("\nFASTPATH starting...");

  while(1)
  {
    putchar(seq[i]);
    fflush(stdout);
    osapiSleepMSec(50);
    putchar('\b');
    i++;
    i %= sizeof(seq);
  }
}

/* PTin added */

/* FPGA map */
volatile st_fpga_map_t *fpga_map = MAP_FAILED;
volatile st_cpld_map_t *cpld_map = MAP_FAILED;


/**
 * Maps FPGA and PLD registers map
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t hapi_ptin_fpga_map(void)
{
#ifdef MAP_FPGA
  TAddrMap fpga_AddrMap;

  LOG_TRACE(LOG_CTX_MISC, "Going to map FPGA...");

  // Load FPGA
  if ((fpga_map = (volatile st_fpga_map_t *) AddrAlloc((void *) &fpga_AddrMap, (int) FPGA_BASE_ADDR, sizeof(st_fpga_map_t))) != MAP_FAILED)
  {
    /* If FPGA id is not valid, free FPGA map */
    if ((fpga_map->map[FPGA_ID1_REG] != FPGA_ID1_VAL) ||
        (fpga_map->map[FPGA_ID0_REG] != FPGA_ID0_VAL)) {

      LOG_ERR(LOG_CTX_MISC, "Invalid FPGA ID: 0x%02X%02X (expecting 0x%02X%02X)",
              fpga_map->map[FPGA_ID0_REG], fpga_map->map[FPGA_ID1_REG],
              FPGA_ID0_VAL, FPGA_ID1_VAL);

      AddrFree(&fpga_AddrMap);
      fpga_map = MAP_FAILED;
    }
    /* Otherwise, make some initializations */
    else
    {
#if (PTIN_BOARD == PTIN_BOARD_OLT7_8CH_B)
      /* Release External PHYs RESET */
      fpga_map->map[FPGA_EXTPHY_RESET] = 0xFF;
      /* Disable all TX */
      fpga_map->map[FPGA_TXDISABLE_REG] = 0x00;
#endif

      LOG_TRACE(LOG_CTX_MISC, "FPGA mapping ok");
      LOG_TRACE(LOG_CTX_MISC, "  FPGA Id:      0x%02X%02X", fpga_map->map[FPGA_ID0_REG], fpga_map->map[FPGA_ID1_REG]);
      LOG_TRACE(LOG_CTX_MISC, "  FPGA Version: %d", fpga_map->map[FPGA_VER_REG]);
    }
  }

  if ( fpga_map == MAP_FAILED )
    return L7_FAILURE;
#endif

#ifdef MAP_CPLD
  TAddrMap cpld_AddrMap;

  LOG_TRACE(LOG_CTX_MISC, "Going to map PLD...");

  // Load CPLD
  if ((cpld_map = (volatile st_cpld_map_t *) AddrAlloc((void *) &cpld_AddrMap, (int) CPLD_BASE_ADDR, sizeof(st_cpld_map_t))) != MAP_FAILED)
  {
    /* If CPLD id is not valid, free CPLD map */
    LOG_WARNING(LOG_CTX_MISC, "CPLD ID is not being validated");
//  if ((cpld_map->map[CPLD_ID1_REG] != CPLD_ID1_VAL) ||
//      (cpld_map->map[CPLD_ID0_REG] != CPLD_ID0_VAL)) {
//    AddrFree(&cpld_AddrMap);
//    cpld_map = MAP_FAILED;
//
//    LOG_ERR(LOG_CTX_MISC, "Invalid CPLD ID: 0x%02X%02X (expecting 0x%02X%02X)",
//            cpld_map->map[FPGA_ID0_REG], cpld_map->map[FPGA_ID1_REG],
//            CPLD_ID0_VAL, CPLD_ID1_VAL);
//  }
//  /* Otherwise, make some initializations */
//  else
    {
      LOG_TRACE(LOG_CTX_MISC, "CPLD mapping ok");
      LOG_TRACE(LOG_CTX_MISC, "  CPLD Id:      0x%02X%02X", cpld_map->map[CPLD_ID0_REG], cpld_map->map[CPLD_ID1_REG]);
      LOG_TRACE(LOG_CTX_MISC, "  CPLD Version: %d", cpld_map->map[CPLD_VER_REG]);
      LOG_TRACE(LOG_CTX_MISC, "  Hw Id:        %d", cpld_map->map[CPLD_HW_ID_REG]);
      LOG_TRACE(LOG_CTX_MISC, "  Chassis Id:   %d", cpld_map->map[CPLD_CHASSIS_ID_REG]);
      LOG_TRACE(LOG_CTX_MISC, "  Slot Id:      %d", cpld_map->map[CPLD_SLOT_ID_REG]);
      /* No initializations to be done */
    }
  }

  if ( cpld_map == MAP_FAILED )
    return L7_FAILURE;
#endif

  return L7_SUCCESS;
}
/* PTin end */


int fp_main(int argc, char *argv[])
{
  L7_CNFGR_CMD_DATA_t cmdData,
                      *pCmdData = &cmdData;
  L7_int32 startupStatusTaskID;
  L7_RC_t rc;

  /* PTin added: Clock */
  #if 1
  extern pthread_cond_t osapiTimerCond;

  pthread_condattr_t attr;

  /* Set up the condvar attributes to use CLOCK_MONOTONIC. */
  pthread_condattr_init( &attr);
  pthread_condattr_setclock( &attr, CLOCK_MONOTONIC);
  pthread_cond_init( &osapiTimerCond, &attr);
  #endif

  LOG_INFO(LOG_CTX_MISC,"Hello World!\r\n");

  /* Set up platform-specific functions for storing and retrieving the
  ** error log information.
  ** The logging utility is initialized inside sysapiInit ().
  */
  loggingCalloutsInit(logNvStore, logNvRetrieve);

  printf("%s: %s(%d) ...\r\n",__FILE__,__FUNCTION__,__LINE__);

  /* Initialize internal DIM structures */
  if (dimInitialize() < 0)
  {
    printf("Dual Image Manager Initialization failed. \n\n");
  }

  printf("%s: %s(%d) ...\r\n",__FILE__,__FUNCTION__,__LINE__);

  /* PTin added: CPLD and FPGA mapping */
  rc = hapi_ptin_fpga_map();
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  printf("%s: %s(%d) ...\r\n",__FILE__,__FUNCTION__,__LINE__);

  /* Initialize file system, timers, event log, HPC, and other
  ** system services.
  */
  sysapiInit (bspapiSwitchReset);

  printf("%s: %s(%d) ...\r\n",__FILE__,__FUNCTION__,__LINE__);

  /*
  ** We will try to change the directory to the configuration
  ** filesystem, which should reside on persistent storage.
  ** If it does not exist then we will stay in the current.
  */
  printf("\n");


  rc = 10;
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
         "Starting code... BSP initialization complete, "
               "starting FastPath application.");
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "rc = %d"
            " Second message logged at bootup, right "
            "after \'Starting code...\'. Always logged.", rc);

  printf("%s: %s(%d) ...\r\n",__FILE__,__FUNCTION__,__LINE__);

  /*
   * Start the Startup Status task
   */
  startupStatusTaskID = osapiTaskCreate("startupStatus",
                        (VOIDFUNCPTR)startupStatusTask,
                        0,
                        L7_NULLPTR,
                        L7_DEFAULT_STACK_SIZE,
                        L7_TASK_PRIORITY_LEVEL(0),
                        L7_DEFAULT_TASK_SLICE);

  /* diagnostics? */

  printf("%s: %s(%d) ...\r\n",__FILE__,__FUNCTION__,__LINE__);

/* PTin commented... */
//  LOG_EVENT (0xaaaaaaaa);    /* Start of new execution. */

#ifdef USE_LM75_MONITOR
  /*
   * Start the LM75 monitoring task
   */
  osapiTaskCreate( "LM75monitor",
                   ( VOIDFUNCPTR )LM75MonitorTask,
                   0,
                   L7_NULLPTR,
                   L7_DEFAULT_STACK_SIZE,
                   L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                   L7_DEFAULT_TASK_SLICE);
#endif

  osapiNetworkInit(); /* Initializes the IP Network */

  sysapiIfNetInit(); /* Initialize the sysapi network support layer */

  dtlNetInit(); /* Initialize DTL interface */

  /* Startup networking applications ----
   * Call the configurator to bring up the net application software */
  /* 1. Initialize the configurator */
  if ( cnfgrApiInit( L7_NULLPTR ) == L7_SUCCESS ) {

      /* At this point the configurator is initialized and ready
       * to receive commands.
       * 2. Prepare configurator command to start the applications
      */
      pCmdData->cbHandle         = L7_CNFGR_NO_HANDLE;
      pCmdData->command          = L7_CNFGR_CMD_INITIALIZE;
      pCmdData->correlator       = L7_NULL;
      pCmdData->type             = L7_CNFGR_EVNT;
      pCmdData->u.evntData.event = L7_CNFGR_EVNT_I_START;
      pCmdData->u.evntData.data  = L7_LAST_COMPONENT_ID;

      if ( cnfgrApiCommand( pCmdData ) != L7_SUCCESS)
          printf(" Configurator: Applications could not be started... exiting \n");

  } else {

      printf(" Configurator did not initialized... exiting \n");

  } /* endif 1. initialize configurator */

  /* kill startup status task */
  osapiTaskDelete(startupStatusTaskID);

  printf("\bstarted!\n");

  return(0);

}

/****************************************************************
** NAME: print_backtrace
**
** This function will dump information regarding the  call stack
** of the offending pid which received a fatal signal
** Processor architecture specific.
*****************************************************************/
#ifdef __powerpc__
void print_backtrace(siginfo_t *info, unsigned int *raw_stack_pointer, FILE *logfd)
{
  unsigned int trace_depth;
  struct stack_frame_intro_s *frame_info, *old_frame_info;
  unsigned int *extra_frame_data;
  struct sig_ctl_frame *hidden_signal_data;
  int i;

#ifndef DISPLAY_BACKTRACE
  return;
#endif

#ifdef BACKTRACE_DEPTH
  /*here we need to first determine our call depth*/
  trace_depth = BACKTRACE_DEPTH;
#else
  trace_depth = 0xffffffff; /*trace as far as we can*/
#endif
  hidden_signal_data = (struct sig_ctl_frame *)info;

  old_frame_info = NULL;

  BT_PRINT(logfd, "\n------------------------------------------------------------\n");
  BT_PRINT(logfd, "Analysis of Failure:\n\n");
  BT_PRINT(logfd, "  Software Failed on instruction located at: 0x%x\n\n",(unsigned int)hidden_signal_data->regs.nip);
  BT_PRINT(logfd, "  This signal was generated in response to a software\n");
  BT_PRINT(logfd, "    attempt to decode address: 0x%x\n\n",(unsigned int)hidden_signal_data->regs.dar);
  BT_PRINT(logfd, "  This error caused trap number 0x%x to be generated.\n",(unsigned int)hidden_signal_data->regs.trap);

  /*now that we know how far to trace...lets do it*/
  for(frame_info = (struct stack_frame_intro_s *)hidden_signal_data->regs.gpr[1];
      ((trace_depth >= 0) &&
       ((int)frame_info != (int)NULL));
      trace_depth--)
  {

    BT_PRINT(logfd, "\n------------------------------------------------------------\n\n");
    BT_PRINT(logfd, "frame location       = 0x0%x\n",(unsigned int)frame_info);
    BT_PRINT(logfd, "frame link register  = 0x%x\n",frame_info->last_link_register);
    BT_PRINT(logfd, "next frame backchain = 0x%x\n",(int)frame_info->backchain);

#ifdef EXTRA_BACKTRACE_DETAIL
    if(old_frame_info != NULL)
    {

    extra_frame_data = (unsigned int*)frame_info;
    i = 1;
    BT_PRINT(logfd, "This stack frame contains localized data as follows:\n");

    while((int)extra_frame_data > (int)old_frame_info+1)
      {
      BT_PRINT(logfd, "0x%08x\t", *extra_frame_data);
      extra_frame_data--;
      if(i%4 == 0)
      {
        BT_PRINT(logfd, "\n");
      }
      i++;/*increment the counter for formatting purposes*/
      }/*while loop*/

    }/*if != NULL*/
#endif

    trace_depth--; /*reduce the trace depth by 1 for the next loop iteration*/
    /*and select the next stack frame*/
    old_frame_info = frame_info;

    frame_info = (struct stack_frame_intro_s *)frame_info->backchain;

  }/*end for loop*/
}
#endif /* __powerpc__ */

/* Backtrace printing for MIPS */
/* Assumes 32 bit */
#ifdef __mips__
struct sigstuff { /* Interesting fields of kernel's struct sigcontext */
  unsigned char pad1[128]; /* Documented struct siginfo */
  unsigned char pad2[32];
  unsigned long long return_pc;
  unsigned long long reg[32];
  unsigned char pad3[296];
  unsigned long exception_cause;
  unsigned long bad_address;
};


unsigned long *mips_backtrace(unsigned long *pc, unsigned long **sp,
                              FILE *logfd)
{
unsigned long *cur_sp = *sp;
  volatile unsigned long *code_search_point = pc;
  unsigned int cur_frame_size, cur_ra_offset, cur_epilog_frame_size;
  unsigned long cur_inst, r1, r2, r3, immed, op, dest_reg;
  unsigned long *cur_frame_addr;
  int i, num_inst_scanned;

    /* Figure out from the code how large the stack frame is, and
       where the return-address register R31 is stored */
    cur_frame_size = 0;
    cur_ra_offset = 0;
    num_inst_scanned = 0; /* Protection against hanging for a long time if we
                 go off into the weeds */
    cur_epilog_frame_size = 0;
    while (((cur_frame_size == 0) || (cur_ra_offset == 0)) &&
       (num_inst_scanned < 100000)) {
      cur_inst = *code_search_point;
      num_inst_scanned++;
      op = (cur_inst & 0xFC000000UL)>>26;
      r1 = (cur_inst & 0x03E00000UL)>>21;
      r2 = (cur_inst & 0x001F0000UL)>>16;
      r3 = (cur_inst & 0x0000F800UL)>>11;
      immed = cur_inst & 0x0000FFFFUL;
      /* Don't analyze instructions that don't alter or store
     registers, to avoid false positives */
      if ((op == 1) || (op == 4) || (op == 2) || (op == 3) ||
      (op == 5) || (cur_inst == 0) /* noop */ ||
      ((cur_inst & 0xF81FFFFF) == 8) /* jr */ ||
      ((cur_inst & 0xFC00003F) == 0xC) /* syscall */ ||
      ((cur_inst & 0xF800FFF8) == 0x0018) /* mult/div */ ) {
    code_search_point--;
    continue;
      }
      /* Which register stores the result of this instruction? */
      if (op == 0) {
    dest_reg = r3;
      } else if (op == 0x2B) { /* sw */
    dest_reg = 0;
      } else {
    dest_reg = r2;
      }
      if ((op == 0x2B) && (r1 == 29)) {
    /* sw {r2}, {immed}(sp) */
    if (r2 == 31) { /* ra */
      if ((immed & 0x8000UL) != 0) {
        BT_PRINT(logfd, "At code addr 0x%08lX found code 0x%08lX that stores\n",
             (unsigned long)code_search_point, cur_inst);
        BT_PRINT(logfd, "RA at negative offset to SP. Giving up.\n");
        return 0;
      }
      cur_ra_offset = immed;
    }
      } else if ((cur_inst & 0xFBFF0000UL) == 0x23BD0000UL) {
    /* addi/addiu sp,sp,{immed} */
    if ((immed & 0x8000UL) == 0) {
      /* Positive offset - this is either the epilog code at the end of
         the previous function, or this function has some nonlinear
         control flow with the pop and return in the middle. For the
         latter case, we'll only see one epilog, of the same size as
         the prolog. */
      if (cur_epilog_frame_size == 0) {
        cur_epilog_frame_size = immed;
      } else {
        BT_PRINT(logfd, "At code addr 0x%08lX found function epilog code 0x%08lX\n",
           (unsigned long)code_search_point, cur_inst);
        BT_PRINT(logfd, "but had not yet found prolog code. Giving up.\n");
        BT_PRINT(logfd, "This might mean I hit the top of stack.\n");
        return 0;
      }
    } else {
      cur_frame_size = (((~immed) & 0xFFFFUL)+1); /* -immed */
      if ((cur_epilog_frame_size != 0) &&
          (cur_epilog_frame_size != cur_frame_size)) {
        BT_PRINT(logfd, "Found prolog at code addr 0x%08lX, but had seen epilog of different frame size %d on the way. Giving up.\n", (unsigned long)code_search_point, cur_epilog_frame_size);
        return 0;
      }
    }
      } else if ((dest_reg == 29) && (cur_frame_size == 0)) {
    BT_PRINT(logfd, "At code addr 0x%08lX the code 0x%08lX alters SP, \n",
         (unsigned long)code_search_point, cur_inst);
    BT_PRINT(logfd, "but had not yet found frame size. Giving up.\n");
    return 0;
      }
      code_search_point--;
    } /* End while looking for ra_offset and frame_size */
    BT_PRINT(logfd, "Current frame at 0x%08lX, size %d bytes, retaddr offset %d bytes\n",
         (unsigned long)cur_sp, cur_frame_size, cur_ra_offset);
    code_search_point = (unsigned long *)(*(cur_sp + (cur_ra_offset / 4)));
    BT_PRINT(logfd, "Return address: 0x%08lX\n\n", (unsigned long)code_search_point);

#ifdef EXTRA_BACKTRACE_DETAIL
  BT_PRINT(logfd, "Stack frame dump:\n");
  cur_frame_addr = (unsigned long *)cur_sp;
  for(i=0;i<(cur_frame_size/4);i++) {
    if (i%6 == 0) {
      BT_PRINT(logfd, "\n%08lX: ", (unsigned long)cur_frame_addr);
    }
    BT_PRINT(logfd, "%08lX ", *cur_frame_addr);
    cur_frame_addr++;
  }
  BT_PRINT(logfd, "\n");
#endif
    if (((((unsigned long)code_search_point) & 0xF0000000UL) > 0x20000000UL) ||
    ((unsigned long)code_search_point < 0x50000UL)) {
      BT_PRINT(logfd, "That return address doesn't look like userspace code-space. Giving up.\n");
      return 0;
    }
    cur_sp += (cur_frame_size / 4);
    BT_PRINT(logfd, "\n");
  /* Update stack pointer to be returned to caller. */
  *sp = cur_sp;
  return (unsigned long *)code_search_point;
}

void print_backtrace(siginfo_t *info, unsigned int *raw_stack_pointer, FILE *logfd)
{
  unsigned long trace_depth, cur_depth;
  struct sigstuff *ss;
  int i;
  unsigned long *cur_pc;
  unsigned long *cur_sp;

#ifndef DISPLAY_BACKTRACE
  return;
#endif

#ifdef BACKTRACE_DEPTH
  /*here we need to first determine our call depth*/
  trace_depth = BACKTRACE_DEPTH;
#else
  trace_depth = 0x0fffffffUL; /*trace as far as we can*/
#endif

  ss = (struct sigstuff *)info;
  cur_sp = (unsigned long *) ((unsigned long) (ss->reg[29]));
  BT_PRINT(logfd, "Stack pointer before signal: 0x%08lX\n", (unsigned long)cur_sp);
  BT_PRINT(logfd, "Offending instruction at address 0x%08lX\n", (unsigned long)ss->return_pc);
  BT_PRINT(logfd, "tried to access address 0x%08lX\n", ss->bad_address);
  BT_PRINT(logfd, "CPU's exception-cause code: 0x%08lX\n", ss->exception_cause);
  BT_PRINT(logfd, "Registers (hex) at time of fault:\n              ");
  for(i=1;i<=31;i++) {
    if(i%5 == 0) {
      BT_PRINT(logfd, "\n");
    }
    BT_PRINT(logfd, "r%02d: %08lX ", i, (unsigned long)ss->reg[i]);
  }
  BT_PRINT(logfd, "\n\n");

  cur_pc = (unsigned long *)(unsigned long)ss->return_pc;
  cur_depth = 0;
  while ((cur_pc) && (cur_depth < trace_depth))
  {
    cur_pc = mips_backtrace(cur_pc, &cur_sp, logfd);
    cur_depth++;
  }

}
#endif /* __mips__ */

/****************************************************************
** NAME: print_msg_log
**
** This function will dump information regarding the  call stack
** of the offending pid which received a fatal signal
*****************************************************************/
void print_msg_log(FILE *logfd)
{
  extern L7_RC_t  logServerLogMsgPersistentGetNext(L7_uint32, L7_uint32,
                                        L7_char8 *, L7_uint32 *);
  L7_uint32 ndx = 0;
  L7_char8 msg[LOG_MSG_MAX_MSG_SIZE*2];
  L7_RC_t rc;

  BT_PRINT(logfd, "\n");

  do
  {
    rc = logServerLogMsgPersistentGetNext(1, ndx, msg, &ndx);
    BT_PRINT(logfd, "%s\n", msg);
  } while (rc == L7_SUCCESS);
}

/****************************************************************
** NAME: sigsegv_handler
**
** This function prints out the information about segmentation
** faults.
*****************************************************************/
void sigsegv_handler (int sig, siginfo_t * info, void * v)
{
  pthread_t self;
#if defined (__powerpc__) || defined(__mips__)
  unsigned int *pid_stack;
#endif /* __powerpc__ || __mips__ */
  char *taskName;
  FILE *bfd;
  char logfilename[20];
  int *sigsegv_recursion_guard;
  L7_RC_t rc;
  L7_localtime lt;

  /*
   * To minimize dumping of lots of sigsegv files, we will set
   * a static variable to handle only the first sigsegv
   */
  if ((already_called == 1) && (sig == SIGSEGV)) {
      exit(0);
  }

  /*
   *become the highest priority task
   *so that we don't get preempted
   */
  if(0 > nice(-19)){}
  /*
   *before we do anything dangerous
   *lets make sure we havent been here
   *before
   */
   sigsegv_recursion_guard = pthread_getspecific(osapi_signal_key);

   if(*sigsegv_recursion_guard)
   {
      /*
       *The recusrion guard word is set
       *we got a SIGSEGV handling a
       *SIGSEGV!!!
       */
     printf("\nLVL7 SIGSEGV Handler\n");
       printf("Warning!!! an error occured while handling a SIGSEGV\n");
       printf("  Note: The most likely cause of this error was stack corruption\n");
       printf("in or about the last stackframe in the file.\n");
       exit(0);
   }


  /*
   *if we make it to here, then the guard word should be set
   *to indicate that we are in a seg fault handler
   */
  *sigsegv_recursion_guard = 1;

  self = pthread_self();
  taskName = (char *)pthread_getspecific(osapi_name_key);

  if (BACKTRACE_FILE)
  {
    sprintf(&logfilename[0],"%serror.%ld",CONFIG_PATH,osapiGetpid());
    bfd = fopen(&logfilename[0],"w+");
    if(bfd == NULL)
      printf("error opening dump log %s\n", &logfilename[0]);
  }
  else
    bfd = NULL;

  BT_PRINT(bfd, "\n************************************************************\n");
  BT_PRINT(bfd, "*                Start LVL7 Stack Information              *\n");
  BT_PRINT(bfd, "************************************************************\n");
  BT_PRINT(bfd, "pid:      \t%ld\n", osapiGetpid());
  BT_PRINT(bfd, "TID:      \t%d\n", (int)self);
  BT_PRINT(bfd, "Task Name:\t%s\n", taskName ? taskName : "unknown!");
  BT_PRINT(bfd, "si_signo: \t%d\n", info->si_signo);
  BT_PRINT(bfd, "si_errno: \t%d\n", info->si_errno);
  BT_PRINT(bfd, "si_code:  \t%d\n", info->si_code);
  BT_PRINT(bfd, "si_addr:  \t0x%x\n", (int)info->si_addr);

  if (osapiLocalTime(osapiUTCTimeNow(), &lt) == L7_SUCCESS)
  {
    BT_PRINT(bfd, "Date/Time:\t%d/%d/%d %d:%02d:%02d\n",
         lt.L7_mon+1, lt.L7_mday, lt.L7_year+1900, lt.L7_hour,
         lt.L7_min, lt.L7_sec);
  } else
  {
    BT_PRINT(bfd, "Date/Time:\tUnknown\n");
  }
  if (sysapiRegistryGet(SW_VERSION, STR_ENTRY, logfilename) == L7_SUCCESS)
  {
    BT_PRINT(bfd, "SW ver:   \t%s\n", logfilename);
  } else
  {
    BT_PRINT(bfd, "SW Ver:   \tUnknown\n");
  }
#if defined(__powerpc__) || defined(__mips__)
  pid_stack = sigsegv_stack; /*get the stack pointer*/
  print_backtrace(info, pid_stack, bfd);
#endif /* __powerpc__ || __mips__ */

  BT_PRINT(bfd, "\n\n************************************************************\n");
  BT_PRINT(bfd, "*                 End LVL7 Stack Information               *\n");
  BT_PRINT(bfd, "************************************************************\n");

  /*
  *also print contents of message log
  */
  BT_PRINT(bfd, "\n\n************************************************************\n");
  BT_PRINT(bfd, "*                 Start LVL7 Message Log                   *\n");
  BT_PRINT(bfd, "************************************************************\n");

  print_msg_log(bfd);

  BT_PRINT(bfd, "\n\n************************************************************\n");
  BT_PRINT(bfd, "*                 End LVL7 Message Log                     *\n");
  BT_PRINT(bfd, "************************************************************\n");

  /* Figure out if this segfault is a stack overflow, and, if so, which
     task it's from. */
  rc = osapiWhichStack((L7_uint32)(info->si_addr), logfilename, 20);
  if (rc == L7_SUCCESS)
  {
    BT_PRINT(bfd, "\n\nThis appears to be a stack overflow, of task %s.\n\n",
         logfilename);
  }

  /*
  *once we are done with our sigsegv handler, we can clear the
  *the guardword to allow other sigsegv's to be processes
  */
  *sigsegv_recursion_guard = 0;

  if (BACKTRACE_FILE)
  {
    printf("\nLVL7 SIGSEGV Handler\n");
    printf("This build was configured to copy this crash information to\n");
    printf("  a file.\n");
  }

  already_called = 1;

  exit (0);
}

int main(int argc, char *argv[], char *envp[])
{
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  struct termios tio;
  speed_t speed;
  L7_uint32 baudrate_val;
#endif
  sigset_t  BlockedSigs;

#ifndef PC_LINUX_HOST
  struct sigaction sig_act;
  int rc;
  stack_t sig_stack;
#endif

  environ = envp;

  if (chdir (CONFIG_PATH) != 0) {

    perror("chdir " CONFIG_PATH);

  }

  if ((argc <= 1) || (strcmp(argv[1], "boot") != 0)) {
    sprintf (ServicePortName, "eth%d:", bspapiServicePortUnitGet());
  }

  sigemptyset(&BlockedSigs);

#ifndef PC_LINUX_HOST
  /* Set up handler for segmentation faults.
  */
  sig_act.sa_sigaction = sigsegv_handler;
  sig_act.sa_mask = BlockedSigs;
  sig_act.sa_flags = SA_SIGINFO | SA_ONSTACK;

  rc = sigaction (SIGSEGV, &sig_act, 0);
  rc = sigaction (SIGBUS, &sig_act, 0);
  rc = sigaction (SIGHUP, &sig_act, 0);
  rc = sigaction (SIGILL, &sig_act, 0);
  rc = sigaction (SIGABRT, &sig_act, 0);
  rc = sigaction (SIGIOT, &sig_act, 0);
  rc = sigaction (SIGFPE, &sig_act, 0);
#ifdef SIGSTKFLT
  rc = sigaction (SIGSTKFLT, &sig_act, 0);
#endif
  rc = sigaction (SIGXCPU, &sig_act, 0);
  rc = sigaction (SIGPWR, &sig_act, 0);
  rc = sigaction (SIGSYS, &sig_act, 0);
  rc = sigaction (SIGINT, &sig_act, 0);
  rc = sigaction (SIGTERM, &sig_act, 0);
  rc = sigaction (SIGPIPE, &sig_act, 0);
  rc = sigaction (SIGQUIT, &sig_act, 0);
  rc = sigaction (SIGCLD, &sig_act, 0);
  /* Set up an alternate stack for the signal handler to use.
     Otherwise, when a stack overflows, the signal handler can't run,
     and we get no debug output. */
  sig_stack.ss_sp = malloc(32768 + MINSIGSTKSZ);
  if (sig_stack.ss_sp == NULL)
  {
    printf("\n*** Could not allocate alt stack memory for signal handler!\n");
  }
  else
  {
    sig_stack.ss_size = 32768 + MINSIGSTKSZ;
    sig_stack.ss_flags = 0;
    if (sigaltstack(&sig_stack, NULL) < 0)
    {
      perror("\n*** Error setting up alt stack for signal handler");
      free(sig_stack.ss_sp);
    }
  }
#endif /* not PC_LINUX_HOST */

  /*
  block certain signals to keep switchdrvr from stopping
  */

  sigemptyset(&BlockedSigs);
  sigaddset(&BlockedSigs, SIGPIPE);
  sigaddset(&BlockedSigs, SIGQUIT);
  sigaddset(&BlockedSigs, SIGCLD);

  if ((argc > 1) && (strcmp(argv[1], "boot") == 0)) {

    sigaddset(&BlockedSigs, SIGTERM);
    sigaddset(&BlockedSigs, SIGINT);

  }

  /*
  Add above signal list to other signals already blocked
  */
  if (sigprocmask(SIG_BLOCK, &BlockedSigs, NULL) < 0) {
    perror("Error blocking signals");
  }

  sysapiRegistryInit();
  bspCpuInit();  /* Perform CPU specific initialization */

/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  tcgetattr(0, &tio);
  /* Do not send a SIGINT if we see a break on the serial line */
  tio.c_iflag &= ~(BRKINT);
  /* Do not send signals when e.g. SUSP character is seen */
  /* except on LiNe, where we want to be able to ^C */
#ifndef PC_LINUX_HOST
  tio.c_lflag &= ~(ISIG);
#endif
  tcsetattr(0, TCSANOW, &tio);
  speed = cfgetospeed(&tio);

  switch (speed) {

    case B1200:
      baudrate_val = L7_BAUDRATE_1200;
      break;

    case B2400:
      baudrate_val = L7_BAUDRATE_2400;
      break;

    case B4800:
      baudrate_val = L7_BAUDRATE_4800;
      break;

    case B9600:
      baudrate_val = L7_BAUDRATE_9600;
      break;

    case B19200:
      baudrate_val = L7_BAUDRATE_19200;
      break;

    case B38400:
      baudrate_val = L7_BAUDRATE_38400;
      break;

    case B57600:
      baudrate_val = L7_BAUDRATE_57600;
      break;

    case B115200:
      baudrate_val = L7_BAUDRATE_115200;
      break;

    default:
      printf("unknown speed (%d)!\n", speed);
      exit(-1);
      break;
  }

  /* Call a SIM function to tell it to override the configured baud rate.
  */
  simSerialBaudOverride (baudrate_val);
#endif

  if ((argc == 2) && (strcmp(argv[1], "diag") == 0)) {
    hpcDiagStart();
  }

  else
  {
#ifdef L7_TOOL_VALGRIND
  if ((argc == 2) && (strcmp(argv[1], "valgrind") == 0)) {
    globValgrindRunning = L7_TRUE;
  }
#endif
     osapiTaskCreate ("fp_main_task",
                        ( VOIDFUNCPTR )fp_main,
                        0,
                        L7_NULLPTR,
                        L7_DEFAULT_STACK_SIZE,
                        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                        L7_DEFAULT_TASK_SLICE);
  }

  pthread_mutex_lock(&main_lock);
  pthread_cond_wait(&main_wait, &main_lock);

  /* We will never get here.
  */

  /* Reference binsh() to ensure linking...
  */
#ifdef L7_CLI_PACKAGE
  binsh();
#endif

  return(0);

}

/* This is for Persistent Log Enhancement */
extern L7_RC_t logMsgToFlash(void);

