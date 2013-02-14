/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_devshell.c
*
* @purpose   OSAPI calls to get OS information. 
*
* @component osapi
*
* @comments 
*
* @create    02/05/2002
*
* @author    John W. Linville
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "l7_common.h"
#include "flex.h"
#include "usmdb_user_mgmt_api.h"
#include "osapi.h"
#include "bspapi.h"

#ifdef L7_CLI_PACKAGE
#include "cliapi.h"
#endif

static L7_char8 *symbols = NULL;
static L7_uint32 *addresses = NULL;
static L7_uint32 num_symbols = 0;

#ifndef U_IDX
#define U_IDX   1
#endif

static L7_uint32 (*cliDevshellFuncPtr)(L7_char8 *cmd) = L7_NULLPTR;

void osapiDevShellSet(L7_uint32 (*func)(L7_char8 *))
{
  cliDevshellFuncPtr = func;
}

L7_uint32 osapiDevShellExec(L7_char8 *cmd)
{
#ifdef L7_CLI_PACKAGE
  return cliDevShell(cmd);
#else
  if(cliDevshellFuncPtr == NULL)
    return -1;

  return cliDevshellFuncPtr(cmd);
#endif
}

/**************************************************************************
*
* @purpose  Fork a /bin/sh process running on the system console
*
* @param    none
*
* @returns  none
*
* @comments none.
*
* @end
*
*************************************************************************/
void binsh(void) {
   int pid;
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
   struct termios term;
#endif
   int orig_console_fd;

   /* No looking for console breakins during binsh */
   orig_console_fd = bspapiConsoleFdGet();
   bspapiConsoleFdSet(0);
   /* PTin removed: serial port */
   #if (L7_SERIAL_COM_ATTR)
   if (tcgetattr(fileno(stdin), &term) == -1) {
      perror("tcgetattr");
      return;
   }
   #endif
   pid = fork();
   if (pid < 0) {
      perror("fork");
      return;
   }
   if (pid == 0) {
      if(0 > system("stty sane")){}
      if (execl("/bin/sh", "/bin/sh", NULL) == -1) perror("execl");
      exit(0);
   }
#ifdef L7_CLI_PACKAGE
   else {
     int rc;
     while((rc = waitpid(pid, 0, WNOHANG)) == 0) {
       if (usmDbLoginSessionUpdateActivityTime(U_IDX,
        cliLoginSessionIndexGet())
        != L7_SUCCESS) break;

       sleep(1);
     }
     if (rc == -1) perror("waitpid");
   /* PTin removed: serial port */
   #if (L7_SERIAL_COM_ATTR)
     if (tcsetattr(fileno(stdin), TCSAFLUSH, &term) == -1) {
       perror("tcsetattr");
     }
   #endif
   }
#endif
   bspapiConsoleFdSet(orig_console_fd);
}

L7_int32 osapiDevshell(L7_char8 *name, L7_int32 *rc,
                       L7_int32 arg0,  L7_int32 arg1,  L7_int32 arg2,
                       L7_int32 arg3,  L7_int32 arg4,  L7_int32 arg5,
                       L7_int32 arg6,  L7_int32 arg7,  L7_int32 arg8,
                       L7_int32 arg9,  L7_int32 arg10, L7_int32 arg11,
                       L7_int32 arg12, L7_int32 arg13, L7_int32 arg14,
                       L7_int32 arg15, L7_int32 arg16, L7_int32 arg17,
                       L7_int32 arg18, L7_int32 arg19, L7_int32 arg20,
                       L7_int32 arg21, L7_int32 arg22, L7_int32 arg23,
                       L7_int32 arg24
		       ) {
#ifdef OSAPI_DEVSHELL
   return (OSAPI_DEVSHELL(name, rc,
                          arg0,arg1,arg2,arg3,arg4,
                          arg5,arg6,arg7,arg8,arg9,
                          arg10,arg11,arg12,arg13,arg14,
                          arg15,arg16,arg17,arg18,arg19,
			  arg20,arg21,arg22,arg23,arg24));
#endif
   return (-1);
}
/**************************************************************************
*
* @purpose  Support a debug shell to process devshell commands.
*
* @param    none
*
* @returns  none
*
* @comments none.
*
* @end
*
*************************************************************************/
void osapiDebugShell(void) {
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
   struct termios term;
#endif
   char   cmd[80];
   int    crPos;

/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
   if (tcgetattr(fileno(stdin), &term) == -1) {
     perror("tcgetattr");
     return;
   }
   if(0 > system("stty sane")){}
#endif
   printf("\n\nEntering devShell, type 'exit' to return\n");

   while (1) {
     printf("devShell->");
     memset(cmd, 0, sizeof(cmd));
     if(0 > read(STDIN_FILENO, cmd, sizeof(cmd)-1)){}

     /* Remove CR from command line before passing command to cliDevShell */
     crPos = strlen(cmd);
     cmd[crPos-1] = '\0';
     if (!strcmp(cmd, "exit")) {
       break;
     }
     osapiDevShellExec(cmd);
   }
   printf("Exiting devShell....\n");

/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
   if (tcsetattr(fileno(stdin), TCSAFLUSH, &term) == -1) {
      perror("tcsetattr");
   }
#endif
}

/*********************************************************************
 * @purpose  Returns the address of a given function
 *
 * @param    funcName, a string
 *
 * @returns  L7_uint32 with address of function if it succeeds
 *           0 if it fails
 *
 * @comments
 *
 * @end
 *********************************************************************/
#define OSAPI_ADDRESS_LOOKUP_CACHE_SIZE 8
#define OSAPI_ADDRESS_LOOKUP_NAME_SIZE 64
#define OSAPI_ADDRESS_LOOKUP_FMT_STRING "%8x%64s"
L7_uint32 osapiAddressLookup(L7_char8 *funcName) {
  int retval = 0;
  FILE *f;
  char *buf, *cachep;
  static char *name_cache = NULL;
  static L7_uint32 *addr_cache = NULL;
  static int cache_open_slot = 0;
  int rc, i;
  L7_uint32 sym_addr = 0;

  /* If first time through, allocate cache, never to be freed */
  if (name_cache == NULL) {
    name_cache = osapiMalloc(L7_OSAPI_COMPONENT_ID, 
        OSAPI_ADDRESS_LOOKUP_CACHE_SIZE*OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
    if (name_cache != NULL) {
      memset(name_cache, 0, 
        OSAPI_ADDRESS_LOOKUP_CACHE_SIZE*OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
    }
  }
  if (addr_cache == NULL) {
    addr_cache = osapiMalloc(L7_OSAPI_COMPONENT_ID, 
        OSAPI_ADDRESS_LOOKUP_CACHE_SIZE*sizeof(*addr_cache));
    if (addr_cache != NULL) {
      memset(addr_cache, 0, OSAPI_ADDRESS_LOOKUP_CACHE_SIZE*sizeof(*addr_cache));
    }
  }
  for(i=0;((name_cache != NULL) && (addr_cache != NULL) && 
        (i<OSAPI_ADDRESS_LOOKUP_CACHE_SIZE));i++) {
    cachep = name_cache + OSAPI_ADDRESS_LOOKUP_NAME_SIZE*i;
    if (cachep[0] == 0) {
      break; /* End of populated cache entries */
    }
    if (strcmp(funcName, cachep) == 0) {
      return addr_cache[i];
    }
  }
  /* Not in cache. Look through the compressed file. */
  /* If user loaded symbols into memory, look through that. */
  if (num_symbols != 0) {
    for(i=0;i<num_symbols;i++) {
      cachep = symbols + i*OSAPI_ADDRESS_LOOKUP_NAME_SIZE;
      if (strcmp(funcName, cachep) == 0) {
	return addresses[i];
      }
    }
    return 0;
  }
  /* Look through file */
  buf = osapiMalloc(L7_OSAPI_COMPONENT_ID, OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
  if (buf == NULL) {
    perror("malloc failed");
    return 0;
  }
#if defined(RAPTOR) && defined(L7_LINUX_24)


   /* The popen call does not work correctly in the mips32 2.4 library */
  /* so use fopen in this case.                                       */
  /* PTin modified: paths */
  if (WEXITSTATUS(system("zcat /usr/local/ptin/sbin/devshell_symbols.gz > /usr/local/ptin/sbin/devshell_symbols.txt")) != 0)
  {
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    perror("zcat failed");
    return 0;
  }
  f = fopen("/usr/local/ptin/sbin/devshell_symbols.txt", "r");                      /* PTin modified: paths */
#else

/* zcat does not work correctly under valgrind*/
#if defined(L7_TOOL_VALGRIND)
  f = fopen("/usr/local/ptin/sbin/devshell_symbols", "r");                          /* PTin modified: paths */
  if (f == NULL) {
      /* f is NULL; this may be possible if valgrind is not running, try the zcat */
      f = popen("zcat /usr/local/ptin/sbin/devshell_symbols.gz 2>/dev/null", "r");  /* PTin modified: paths */
  }
#else
  f = popen("zcat /usr/local/ptin/sbin/devshell_symbols.gz 2>/dev/null", "r");      /* PTin modified: paths */
#endif
#endif
  if (f == NULL) {
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    perror("popen failed");
    return 0;
  }
  while (!feof(f)) {
    if(0 > fscanf(f, OSAPI_ADDRESS_LOOKUP_FMT_STRING, &sym_addr, buf)){}
    buf[OSAPI_ADDRESS_LOOKUP_NAME_SIZE-1] = 0;
    rc = strcmp(buf, funcName);
    if (rc == 0) {
#if defined(RAPTOR) && defined(L7_LINUX_24)
      fclose(f);
#else
      pclose(f);
#endif
      osapiFree(L7_OSAPI_COMPONENT_ID, buf);
      if ((addr_cache != NULL) && (name_cache != NULL)) {
        cachep = name_cache + OSAPI_ADDRESS_LOOKUP_NAME_SIZE*cache_open_slot;
        addr_cache[cache_open_slot] = sym_addr;
        strncpy(cachep, funcName, OSAPI_ADDRESS_LOOKUP_NAME_SIZE-1);
        cachep[OSAPI_ADDRESS_LOOKUP_NAME_SIZE-1] = 0;
        cache_open_slot++;
        cache_open_slot %= OSAPI_ADDRESS_LOOKUP_CACHE_SIZE; /* Most-recently-used */
      }
      return sym_addr;
    }
  }
#if defined(RAPTOR) && defined(L7_LINUX_24)
  fclose(f);
#else
  pclose(f);
#endif
  osapiFree(L7_OSAPI_COMPONENT_ID, buf);  
  return retval;
}



/*********************************************************************
 * @purpose  Returns the function containing a given address
 *
 * @param    addr, L7_uint32
 *           funcName, L7_char8* buffer allocated by caller
 *           funcNameLen, L7_uint32 length of name buffer
 *           offset, L7_uint32* pointer to L7_uint32 allocated by caller.
 *                   Address's offset from beginning of function stored there.
 *
 * @returns  L7_SUCCESS if function found, name printed into funcName buffer,
 *                      offset set into *offset.
 *           L7_FAILURE if function not found, buffers untouched.
 *
 * @comments 
 *
 * @end
 *********************************************************************/
L7_RC_t osapiFunctionLookup(L7_uint32 addr, L7_char8 *funcName,
       L7_uint32 funcNameLen, L7_uint32 *offset) {
  FILE *f;
  L7_char8 *buf, *candidate_name;
  L7_uint32 cur_addr, candidate_addr = 0;
  static int first_time = 1;
  static L7_uint32 lowest_addr = 0xFFFFFFFFUL, highest_addr = 0;
  int i;

  if (((addr < lowest_addr) || (addr > highest_addr)) && (!first_time)) {
    return L7_FAILURE;
  }
  if (num_symbols != 0) {
    for(i=1;i<(num_symbols);i++) {
      if (addresses[i] > addr) {
	if (addresses[i-1] < addr) {
	  if (funcName != NULL) {
	    strncpy(funcName, symbols+(i-1)*OSAPI_ADDRESS_LOOKUP_NAME_SIZE, funcNameLen);
	    funcName[funcNameLen-1] = 0;
	  }
	  if (offset != NULL) {
	    *offset = addr - addresses[i-1];
	  }
	  return(L7_SUCCESS);
	} else {
	  return(L7_FAILURE);
	}
      }
    }
    return(L7_FAILURE);
  }
  buf = osapiMalloc(L7_OSAPI_COMPONENT_ID, OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
  if (buf == NULL) {
    perror("malloc failed");
    return L7_FAILURE;
  }
  candidate_name = osapiMalloc(L7_OSAPI_COMPONENT_ID, OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
  if (candidate_name == NULL) {
    perror("malloc failed");
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    return L7_FAILURE;
  }
  f = popen("zcat /usr/local/ptin/sbin/devshell_symbols.gz 2>/dev/null", "r");      /* PTin modified: paths */
  if (f == NULL) {
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    osapiFree(L7_OSAPI_COMPONENT_ID, candidate_name);
    perror("popen failed");
    return L7_FAILURE;
  }

  while (!feof(f)) {
    if(0 > fscanf(f, OSAPI_ADDRESS_LOOKUP_FMT_STRING, &cur_addr, buf)){}
    if (cur_addr <= addr) {
      candidate_addr = cur_addr;
      strncpy(candidate_name, buf, OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
    }
    if (first_time) {
      if (cur_addr < lowest_addr) {
	lowest_addr = cur_addr;
      }
      if (cur_addr > highest_addr) {
	highest_addr = cur_addr;
      }
    }
    /* Go all the way through on the first invocation of this
       function, to get highest_addr set. On others, short-circuit 
       once we've been past the target address (the file is sorted). */
    if ((cur_addr > addr) && (!first_time)) {
      if (candidate_addr == 0) {
        pclose(f);
        osapiFree(L7_OSAPI_COMPONENT_ID, buf);
        osapiFree(L7_OSAPI_COMPONENT_ID, candidate_name);
        return L7_FAILURE;
      } else {
	break;
      }
    }
  }
  pclose(f);
  if (first_time) {
    first_time = 0;
  }
  if (candidate_addr != 0) {
    candidate_name[OSAPI_ADDRESS_LOOKUP_NAME_SIZE-1] = 0;
    if (funcName != NULL) {
      strncpy(funcName, candidate_name, funcNameLen);
      funcName[funcNameLen-1] = 0;
    }
    if (offset != NULL) {
      *offset = (addr - candidate_addr);
    }
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    osapiFree(L7_OSAPI_COMPONENT_ID, candidate_name);
    return L7_SUCCESS;      
  }
  osapiFree(L7_OSAPI_COMPONENT_ID, buf);
  osapiFree(L7_OSAPI_COMPONENT_ID, candidate_name);
  return L7_FAILURE;      
}

int osapiDebugFunctionLookupTest(unsigned long addr) {
  char *name_buf;
  L7_uint32 offset;
  L7_RC_t rc;

  name_buf = osapiMalloc(L7_OSAPI_COMPONENT_ID, 48);
  if (name_buf == NULL) {
    perror("malloc failed");
    return 1;
  }
  rc = osapiFunctionLookup(addr, name_buf, 48, &offset);
  if (rc == L7_FAILURE) {
    printf("Lookup failed.\n");
    return 1;
  }
  printf("Address %08lX is %d bytes into function %s\n", 
        addr, offset, name_buf);
  return 0;
}

/*********************************************************************
 *
 * @purpose The purpose of this function is to add a lookup capability
 *          to the Fastpath command prompt to search through all the
 *          available functions
 *
 * @param char *search_str - the name of the function (case sensitive)
 *
 * @returns void
 *
 * @notes there are 4 ways to use this:
 *               devshell "lkup '*function_name'" - ends with function_name
 *               devshell "lkup 'function_name*'" - starts with function_name
 *               devshell "lkup 'function_name'"  - contains function_name
 *               devshell "lkup '*function_name*'"- contains function_name
 *
 *               this is case sensitive
 *               do not use spaces
 *
 * @end
 *********************************************************************/
void osapiLkup(L7_char8 *search_str) {
  int begins_with = 0, ends_with = 0, contains = 0;
  int search_len, buf_len;
  FILE *f;
  L7_char8 *buf;
  L7_uint32 sym_addr;

  search_len = strlen(search_str);
  if (search_str[0] == '*') {
    search_str++;
    search_len--;
    ends_with = 1;
  }
  if (search_str[search_len-1] == '*') {
    search_str[search_len-1] = 0;
    search_len--;
    begins_with = 1;
  }
  if ((begins_with && ends_with) || ((!begins_with) && (!ends_with))) {
    contains = 1;
    begins_with = 0;
    ends_with = 0;
  }
  printf("Functions ");
  if (contains) {
    printf("containing \"");
  }
  if (begins_with) {
    printf("beginning with \"");
  }
  if (ends_with) {
    printf("ending with \"");
  }
  printf("%s\": \n\n", search_str);
  buf = osapiMalloc(L7_OSAPI_COMPONENT_ID, OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
  if (buf == NULL) {
    perror("malloc failed");
    return;
  }
  f = popen("zcat /usr/local/ptin/sbin/devshell_symbols.gz 2>/dev/null", "r");        /* PTin modified: paths */
  if (f == NULL) {
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    perror("popen failed");
    return;
  }
  while (!feof(f)) {
    if(0 > fscanf(f, OSAPI_ADDRESS_LOOKUP_FMT_STRING, &sym_addr, buf)){}
    buf[OSAPI_ADDRESS_LOOKUP_NAME_SIZE-1] = 0;
    if ((contains && strstr(buf, search_str)) ||
        (begins_with && (!strncmp(search_str, buf, search_len)))) {
      printf("%s at 0x%lX\n", buf, (unsigned long)sym_addr);
    }
    if (ends_with) {
      buf_len = strlen(buf);
      if (buf_len < search_len) {
        continue;
      }
      if (!strncmp(search_str, (buf+buf_len-search_len), search_len)) {
        printf("%s at 0x%lX\n", buf, (unsigned long)sym_addr);
      }
    }
  }
  pclose(f);
  osapiFree(L7_OSAPI_COMPONENT_ID, buf);
  printf("\nSearch complete.\n");
}

/* Load symbols from the file into memory. */
int osapiDevshellSymbolsLoad(void) {
  FILE *f;
  L7_uint32 cur_addr;
  int i;
  char *cur_sym, *buf;

  if (num_symbols != 0) {
    printf("Symbols already loaded.\n");
    return(1);
  }
  printf("Scanning devshell symbols file...\n");
  buf = osapiMalloc(L7_OSAPI_COMPONENT_ID, OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
  if (buf == NULL) {
    perror("malloc failed");
    return L7_FAILURE;
  }
  f = popen("zcat /usr/local/ptin/sbin/devshell_symbols.gz 2>/dev/null", "r");        /* PTin modified: paths */
  if (f == NULL) {
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    perror("popen failed");
    return(1);
  }
  while (!feof(f)) {
    if(0 > fscanf(f, OSAPI_ADDRESS_LOOKUP_FMT_STRING, &cur_addr, buf)){}
    num_symbols++;
  }
  pclose(f);
  printf("%d symbols, loading...\n", num_symbols);
  symbols = osapiMalloc(L7_OSAPI_COMPONENT_ID, num_symbols*OSAPI_ADDRESS_LOOKUP_NAME_SIZE);
  if (symbols == NULL) {
    printf("symbols malloc failed\n");
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    num_symbols = 0;
    return(1);
  }
  addresses = osapiMalloc(L7_OSAPI_COMPONENT_ID, num_symbols*4);
  if (addresses == NULL) {
    printf("addresses malloc failed\n");
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    osapiFree(L7_OSAPI_COMPONENT_ID, symbols);
    num_symbols = 0;
    return(1);
  }
  f = popen("zcat /usr/local/ptin/sbin/devshell_symbols.gz 2>/dev/null", "r");        /* PTin modified: paths */
  if (f == NULL) {
    perror("popen failed");
    osapiFree(L7_OSAPI_COMPONENT_ID, buf);
    osapiFree(L7_OSAPI_COMPONENT_ID, symbols);
    osapiFree(L7_OSAPI_COMPONENT_ID, addresses);
    num_symbols = 0;
    return(1);
  }
  i = 0;
  while (!feof(f)) {
    if(0 > fscanf(f, OSAPI_ADDRESS_LOOKUP_FMT_STRING, &cur_addr, buf)){}
    buf[OSAPI_ADDRESS_LOOKUP_NAME_SIZE-1] = 0;    
    cur_sym = symbols + OSAPI_ADDRESS_LOOKUP_NAME_SIZE*i;
    strcpy(cur_sym, buf);
    addresses[i] = cur_addr;
    i++;
  }
  pclose(f);
  printf("Done.\n");
  osapiFree(L7_OSAPI_COMPONENT_ID, buf);
  return(0);
}

int osapiDevshellSymbolsDump(void) {
  if (num_symbols == 0) {
    printf("Devshell symbols not loaded.\n");
    return(1);
  }
  num_symbols = 0;
  osapiFree(L7_OSAPI_COMPONENT_ID, symbols);
  osapiFree(L7_OSAPI_COMPONENT_ID, addresses);
  printf("Symbol memory freed.\n");
  return(0);
}

