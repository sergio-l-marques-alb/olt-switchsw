/*
 *
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * Example application routines for EmWeb POSIX reference port
 *
 * Usage:       ews hostname[:port]
 */

#include <stdio.h>
#if !defined(WIN32) && !defined(_WIN32_WCE)
#include <unistd.h>
#endif

#include "posix_config.h"
#include "ew_config.h"
#include "ews_api.h"
#include "ew_db.h"

#include "ewnet.h"


#ifdef EMWEB_OSE
# include "content_proto.h"
#else
# include "ew_proto.h"
#endif

#ifdef EW_CONFIG_OPTION_UPLOAD_ARCHIVE
#include "uploadproto.h"
#include "uploadcode.c"
#include "uploaddata.c"
#endif

#define _EWMAIN_C_
#include "ewa_routines.h"


#ifdef EMWEB_POSIX_EXAMPLE
extern void init_toaster(void);
extern void init_device(void);
#endif /* EMWEB_POSIX_EXAMPLE */

#if defined(_WIN32_WCE) || defined(WIN32)
#ifdef _WIN32_WCE
/* Leave room for name containing dotted-quad IP address, colon, port number */
  char  local_host_name[3+1+3+1+3+1+3+1+6+1];
  char  local_host_just_name[3+1+3+1+3+1+3+1];
  static HKEY hRoot, hKey, hKey2;
  static TCHAR temp[256];
  static TCHAR szValue[256];
  static DWORD dwType;
  static DWORD dwSize;
  static BOOL bFound=FALSE;
#else
  char *      local_host_name;
  char *      local_host_just_name;
  NCB         ncb;
  struct ASTAT
    {
      ADAPTER_STATUS adapt;
      NAME_BUFFER NameBuff[30];
    } Adapter;
#endif
int ewaSetLocalHostName(int port);
#define EMWEB_CE_PORT   8080
#else
  char *      local_host_name;
  char *      local_host_just_name;
/* # endif */
#endif

static char   realm_qualifier[64];

#define malloc_safe(n)       Malloc_Safe ( n, __FILE__, __LINE__ )

static void *Malloc_Safe(size_t s, const char *file, uint32 line);


/*
 * Set up to handle either a compiled-in archive or one read from a file:
 *   ew_archive_data_p is a pointer to the archive's data.
 *   ew_archive_data links to the compiled-in archive (if there is one).
 *   archive_file is the name of the file (if there is one).
 *   EMWEB_USAGE is the usage message to give the user.
 */

#ifdef EMWEB_USE_FILE_ARCHIVE

/* Set up for archive in a file. */

#define EMWEB_USAGE "Usage: %s hostname[:port] archive-data-file\n"
uint8 *ew_archive_data_p;
char  *archive_file;

#else /* ! EMWEB_USE_FILE_ARCHIVE */

/* Set up for archive compiled into the executable. */

#define EMWEB_USAGE "Usage: %s hostname[:port]\n"

/* Use a cast to eliminate the 'const' if we are using C++. */
uint8 *ew_archive_data_p = (uint8 *) ew_archive_data;

#endif /* EMWEB_USE_FILE_ARCHIVE */

#ifdef EW_CONFIG_OPTION_UPNP
/* The system identification string used to make device UUIDs. */
char ewaUPnPSystemIdString[50];

#ifdef POSIX_MAC_IOCTL
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#endif /* POSIX_MAC_IOCTL */

#endif /* EW_CONFIG_OPTION_UPNP */



# if defined(WIN32) && ! defined(EMWEB_OSE)
  WSADATA              wsaData;
# define               WINSOCK_MAJ_VER  1
# define               WINSOCK_MIN_VER  1
#endif /* WIN32 */

/* ################################################################ */
/*                          M A I N                                 */
/* ################################################################ */

int
main(int argc, char **argv)
{
  int             exit_status;
  char*           cp;
  int             sz;
  uint16          port;
  struct hostent* hostent;
  uint32          listenAddr;
#if defined(WIN32) || defined(_WIN32_WCE)
  int             win32_arguments;
#ifdef _WIN32_WCE
  int             key_count;
#endif
#endif

  /*
   * Handle arguments
   */

/* #ifdef EW_CONFIG_OPTION_SSL_RSA */
#if !defined(_WIN32_WCE)
  local_host_just_name = NULL;
#ifdef WIN32
  /* Remember the number of arguments for Win32 and force it through
     the argument checking even if no arguments were given. Win32 will
     then acquire the host address automatically later on.
   */
  win32_arguments = argc;
  if (argc<2)
    {
      argc = 2;
    }
#endif
/* #endif */
#else
  /* since WinCE has no command line, force it to go through the argument
     checking anyway and get the host address afterwards through a separate
     function call.
   */
  argc = 2;
  win32_arguments = 0;
#endif /* _WIN32_WCE */
  /* Increment argument pointers past program name. */
  argc--;
  argv++;

  /* Require host:port argument. */
  if ( argc >= 1 )
    {
      /* Skip the command line processing for Win CE */
#if !defined(_WIN32_WCE)
#ifdef WIN32
      if (win32_arguments<2)
        {
          /* There weren't any command line arguments for Win32 so
             allocate just enough space to hold an IP address and port
             and fill the buffer so the following code works correctly
           */
          local_host_name = malloc_safe(3+1+3+1+3+1+3+1+6+1);
          strcpy(local_host_name, "xxx.xxx.xxx.xxx:pppp");
        }
      else
        {
          local_host_name = malloc_safe(strlen(argv[0]) + 1);
          strcpy(local_host_name, argv[0]);
        }
#else
      local_host_name = malloc_safe(strlen(argv[0]) + 1);
      strcpy(local_host_name, argv[0]);
#endif
      /* Split host and port components. */
      cp = strchr(local_host_name, ':');
      if (cp != NULL)
        {
          sz = cp - local_host_name;
          sscanf(cp+1, "%hd", &port);
        }
      else
        {
          sz = strlen(local_host_name);
          port = EMWEB_LISTEN_PORT;
        }
      local_host_just_name = malloc_safe(sz + 1);
      strncpy(local_host_just_name, local_host_name, sz);
      local_host_just_name[sz] = '\0';

#endif /* !_WIN32_WCE */
      /* Increment argument pointers past host:port argument. */
      argc--;
      argv++;
#     ifdef EMWEB_USE_FILE_ARCHIVE
      /* Required archive name was supplied. */
      if ( argc >= 1 )
        {
          uintf namelen;

          namelen = strlen( argv[0] );
          /* Require name argument has nonzero length. */
          if ( namelen == 0 )
            {
              EMWEB_ERROR(( "ews: must specify an archive data file name.\n"));
#             ifdef EW_CONFIG_OPTION_SSL_RSA
              if (local_host_just_name != NULL)
                {
                  ewaFree(local_host_just_name);
                  local_host_just_name = NULL;
                }
#             endif
              ERROR_EXIT;
            }

          archive_file = malloc_safe( namelen );
          strcpy( archive_file, argv[0] );
        }
      else
        {
          EMWEB_ERROR(( "ews: must specify an archive data file name.\n"));
#         ifdef EW_CONFIG_OPTION_SSL_RSA
          if (local_host_just_name != NULL)
            {
              ewaFree(local_host_just_name);
              local_host_just_name = NULL;
            }
#         endif
          ERROR_EXIT;
        }

      /* Increment argument pointers past archive file argument. */
      argc--;
      argv++;

#     endif /* EMWEB_USE_FILE_ARCHIVE */

# if (defined(WIN32) || defined(_WIN32_WCE)) && ! defined(EMWEB_OSE)
  if (WSAStartup( MAKEWORD( WINSOCK_MAJ_VER, WINSOCK_MIN_VER )
                 ,&wsaData ) != 0)
    {
      EMWEB_ERROR( ("Unable to find a suitable WINSOCK.DLL\n") );
#     ifdef EW_CONFIG_OPTION_SSL_RSA
      if (local_host_just_name != NULL)
        {
          ewaFree(local_host_just_name);
          local_host_just_name = NULL;
        }
#     endif
      ERROR_EXIT;
    }
# endif

      /* Assemble realm_qualifier based on the host name. */
      strcpy(realm_qualifier, "@");
      strncat(realm_qualifier, local_host_name, sizeof(realm_qualifier)-1);

#     if defined(_WIN32_WCE) || defined(WIN32)
      /* gethostbyname fails on windows systems if an ip address is specified
       * instead of the name.  Unix accepts both.  If windows, check if ip
       * address is specified, and if so, don't do gethostbyname(), but just
       * convert the address to an integer...
       * For Win32 test if there were any command line arguments - if not
       * look for host address the same way WinCE does ..
       */
      if (win32_arguments<2)
        {
          port = EMWEB_CE_PORT;
          ewaSetLocalHostName(port);
          cp = strchr(local_host_name, ':');
          if (cp!=NULL)
            {
              *cp = 0;
            }
        }

      if ((listenAddr = inet_addr( local_host_name )) != INADDR_NONE)
        {
          /* host specified as ip address in dot notation */
        }
      else
#     endif  /* WIN32 */
        {
          hostent = gethostbyname(local_host_just_name);
          if (hostent != NULL)
            {
              memcpy(&listenAddr, hostent->h_addr, 4);
            }
          else
            {
              exit_status = 1;
#             ifndef _WIN32_WCE
              EMWEB_ERROR(( "gethostbyname(%s): %s\n"
                            ,local_host_name
                            ,strerror(errno)));
#             endif

#             if (defined(WIN32) || defined(_WIN32_WCE)) && ! defined(EMWEB_OSE)
              WSACleanup();     /* unload WINSOCK.DLL */
#             endif
#             ifdef EW_CONFIG_OPTION_SSL_RSA
              if (local_host_just_name != NULL)
                {
                  ewaFree(local_host_just_name);
                }
#             endif
              return exit_status;
            }
        }

      listenAddr = osapiNtohl(listenAddr);
      if (cp != NULL)
        {
          *cp = ':';
        }


      /* Require that there be no more arguments. */
      if (argc >= 1)
        {
          EMWEB_ERROR(( "ews: extra arguments ignored.\n"));
        }

#     ifdef EW_CONFIG_OPTION_UPNP

      /*
       * Calculate the system ID string for UPnP.
       *
       * We do this by concatenating the MAC address of interface
       * EMWEB_INTERFACE_NAME with the HTTP listening port.  This combination
       * will not change if the IP address of the interface changes, and it
       * also gives distinct values if multiple copies of the software are
       * run on one computer.
       */
      {
#     if defined(WIN32) && !defined(_WIN32_WCE)

        /* Use a constant value for Windows, until we know how to get the
         * real value. */
        strcpy(ewaUPnPSystemIdString, "Windows");

        memset(&ncb, 0, sizeof(ncb));
        ncb.ncb_command = NCBRESET;
        ncb.ncb_lana_num = 0;
        if (NRC_GOODRET == Netbios(&ncb))
          {
            memset(&ncb, 0, sizeof(ncb));
            ncb.ncb_command = NCBASTAT;
            ncb.ncb_lana_num = 0;
            strcpy((char *)ncb.ncb_callname, "*");

            memset(&Adapter, 0, sizeof(Adapter));
            ncb.ncb_buffer = (unsigned char *)&Adapter;
            ncb.ncb_length = sizeof(Adapter);

            if (0 == Netbios(&ncb))
              {
                /* Convert the binary MAC address into characters. */
                sprintf(ewaUPnPSystemIdString,
                        "%02X:%02X:%02X:%02X:%02X:%02X",
                        Adapter.adapt.adapter_address[0],
                        Adapter.adapt.adapter_address[1],
                        Adapter.adapt.adapter_address[2],
                        Adapter.adapt.adapter_address[3],
                        Adapter.adapt.adapter_address[4],
                        Adapter.adapt.adapter_address[5]
                        );
              }
          }


#     elif defined(_WIN32_WCE)
        strcpy(ewaUPnPSystemIdString, "WinCE");

        /* The code to retrieve the MAC address for WinCE systems requires
           the cooperation of the network card device driver to put that
           address inside the registry as a string. Unfortunately not all
           drivers behave that way and the following code will come up empty.
         */

        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Drivers\\Active"), 0, KEY_READ, &hRoot)==ERROR_SUCCESS)
          {
            for (key_count=0; key_count<30; ++key_count)
              {
                _stprintf(szValue, _T("%02d"), key_count);
                if (RegOpenKeyEx(hRoot, szValue, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
                  {
                    dwSize = 256;
                    if (RegQueryValueEx(hKey, _T("Name"), 0, &dwType, (LPBYTE)szValue, &dwSize)==ERROR_SUCCESS)
                      {
                        if (_tcsicmp(szValue, _T("NDS1:"))==0)
                          {
                            dwSize = 256;
                            if (RegQueryValueEx(hKey, _T("Key"), 0, &dwType, (LPBYTE)szValue, &dwSize)==ERROR_SUCCESS)
                              {
                                bFound = TRUE;
                                break;
                              }
                          }
                      }
                    RegCloseKey(hKey);
                  }
              }
            RegCloseKey(hRoot);
            if (bFound)
              {
                bFound = FALSE;
                if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szValue, 0, KEY_READ, &hRoot)==ERROR_SUCCESS)
                  {
                    dwSize = 256;
                    if (RegQueryValueEx(hRoot, _T("Miniport"), 0, &dwType, (LPBYTE)temp, &dwSize)==ERROR_SUCCESS)
                      {
                        _stprintf(szValue, _T("Comm\\%s\\Linkage"), temp);
                        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szValue, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
                          {
                            dwSize = 256;
                            if (RegQueryValueEx(hKey, _T("Route"), 0, &dwType, (LPBYTE)temp, &dwSize)==ERROR_SUCCESS)
                              {
                                _stprintf(szValue, _T("Comm\\%s\\Parms"), temp);
                                if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szValue, 0, KEY_READ, &hKey2)==ERROR_SUCCESS)
                                  {
                                    dwSize = 256;
                                    if (RegQueryValueEx(hKey, _T("NetworkAddress"), 0, &dwType, (LPBYTE)szValue, &dwSize)==ERROR_SUCCESS)
                                      {
#                                     ifdef _UNICODE
                                        WideCharToMultiByte(CP_ACP, WC_SEPCHARS, szValue, -1, ewaUPnPSystemIdString, 49, NULL, NULL);
#                                     else
                                        strcpy(ewaUPnPSystemIdString, szValue);
#                                     endif
                                      }
                                    RegCloseKey(hKey2);
                                  }
                              }
                            RegCloseKey(hKey);
                          }
                      }
                    RegCloseKey(hRoot);
                  }
              }
          }

#     elif defined(POSIX_MAC_IFCONFIG)

        FILE *f;

        /* Call some Unix utilities to get the MAC address. */
        /* This should be replaced with the ioctl() that ifconfig uses to
         * fetch this information. */
        f = popen("/sbin/ifconfig " EMWEB_INTERFACE_NAME " | grep 'HWaddr' | "
                  "sed -e 's/^.*HWaddr *//'",
                  "r");
        if (NULL == f)
          {
            EMWEB_ERROR(("ews: error getting MAC address.\n"));
            ERROR_EXIT;
          }
        fgets(ewaUPnPSystemIdString, sizeof (ewaUPnPSystemIdString) - 1,
              f);
        pclose(f);
        /* Trim off any trailing junk. */
        ewaUPnPSystemIdString[strspn(ewaUPnPSystemIdString,
                                     "0123456789ABCDEFabcdef:")] ='\0';
#     elif defined(POSIX_MAC_IOCTL)

        struct ifreq ifr;
        int net_socket;

        /* Open a socket in the INET address space. */
        net_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (net_socket < 0)
          {
            EMWEB_ERROR(("ews: error opening socket to get MAC address.\n"));
            ERROR_EXIT;
          }

        /* Insert the interface name into the I/O block for the ioctl(). */
        strncpy(ifr.ifr_name, EMWEB_INTERFACE_NAME, IFNAMSIZ);

        /* Execute the ioctl to get information about the interface. */
        if (ioctl(net_socket, SIOCGIFHWADDR, &ifr) < 0)
          {
            EMWEB_ERROR(("ews: error executing ioctl(SIOCGIFHWADDR) to get "
                         "MAC address.\n"));
            ERROR_EXIT;
          }

        /* Convert the binary MAC address into characters. */
        sprintf(ewaUPnPSystemIdString,
                "%02X:%02X:%02X:%02X:%02X:%02X",
                ifr.ifr_hwaddr.sa_data[0] & 0xFF,
                ifr.ifr_hwaddr.sa_data[1] & 0xFF,
                ifr.ifr_hwaddr.sa_data[2] & 0xFF,
                ifr.ifr_hwaddr.sa_data[3] & 0xFF,
                ifr.ifr_hwaddr.sa_data[4] & 0xFF,
                ifr.ifr_hwaddr.sa_data[5] & 0xFF);

#     elif

#       error "No method for finding the MAC address specified."

#     endif /* Options to specify how to determine MAC address. */

        /* Append the HTTP listening port number. */
        sprintf(ewaUPnPSystemIdString + strlen(ewaUPnPSystemIdString),
                ":%d",
                port);

        EMWEB_TRACE(("ews: Using ewaUPnPSystemId = '%s'\n",
                     ewaUPnPSystemIdString));
      }

#     endif /* EW_CONFIG_OPTION_UPNP */

      /*
       * Run the Server.
       */

      exit_status = ewmain( listenAddr, port );
              /* may never get here */

      /*
       *
       */

    }
  else
    {
      exit_status = 1;
      EMWEB_ERROR((EMWEB_USAGE, argv[0] ));
    }

# if (defined(WIN32) || defined(_WIN32_WCE)) && ! defined(EMWEB_OSE)
  WSACleanup();     /* unload WINSOCK.DLL */
# endif
# ifdef EW_CONFIG_OPTION_SSL_RSA
  if (local_host_just_name != NULL)
    {
      ewaFree(local_host_just_name);
    }
# endif
  return exit_status;
}

#if defined(WIN32) || defined(_WIN32_WCE)
int
ewaSetLocalHostName(int port)
{
  char hostname[256];
  struct hostent *pHostEnt;
  unsigned u1, u2, u3, u4;

  if (gethostname(hostname, sizeof(hostname)) < 0)
    {
      EMWEB_ERROR(("Failed in gethostname()"));
      return 1;
    }
  if ((pHostEnt = gethostbyname(hostname)) == NULL)
    {
      EMWEB_ERROR(("Failed in gethostbyname()"));
      return 2;
    }
  u1 = (unsigned char)pHostEnt->h_addr[0];
  u2 = (unsigned char)pHostEnt->h_addr[1];
  u3 = (unsigned char)pHostEnt->h_addr[2];
  u4 = (unsigned char)pHostEnt->h_addr[3];

  sprintf(local_host_name, "%u.%u.%u.%u:%d"
          ,u1
          ,u2
          ,u3
          ,u4
          ,(int) (unsigned short) port
          );

  sprintf(local_host_just_name, "%d.%d.%d.%d"
          ,u1
          ,u2
          ,u3
          ,u4
          );

  return 0;
}
#endif /* WIN32 || _WIN32_WCE */


/*
 * ewaInstallArchives
 *   Called at init time - it should make any calls
 *   to ewsDocumentInstallArchive that are needed.
 */
EwaStatus ewaInstallArchives( void )
{
  EwsStatus status;
  EwaStatus app_status;

# ifdef EW_CONFIG_OPTION_AUTH
  EwsAuthorization      auth;           /* authorization */
  EwsAuthHandle         auth_handle;    /* returned handle */
# endif

#ifdef EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE
  char md5_secret1[50];  /* use for example registration */
  char md5_secret2[50];  /* use for example registration */
  char md5_secret3[50];  /* use for example registration */
  char *md5_secretp = md5_secret1;
  EwsAuthHandle auth_handle1;
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE */

#ifdef EMWEB_USE_FILE_ARCHIVE
  FILE*     archive_file_p;
  STATBUF   statbuf;
  size_t    archive_size;

  if ( STAT( archive_file, &statbuf ) < 0 )
    {
      EMWEB_ERROR(( "ews (ewaInstallArchives): Error %s (errno=%d) from stat('%s')\n",
                    strerror(errno), errno, archive_file));
      exit(1);
    }

  /* Get the size of the archive and allocate memory for it */
  archive_size = statbuf.st_size;
  ew_archive_data_p = (uint8 *) malloc_safe( archive_size );

  /*
   * Read a data portion of a compiler-generated archive.  We link this with
   * the example archive object created in the example library by the
   * EmWeb/Compiler.
   */
  archive_file_p = fopen( archive_file, "rb");
  if ( archive_file_p == NULL )
    {
      EMWEB_ERROR(( "ews (ewaInstallArchives): Error %s (errno=%d) while attemping to open file '%s' for reading\n",
                    strerror(errno), errno, archive_file));
      exit(1);
    }

  if (  fread( ew_archive_data_p, 1, archive_size, archive_file_p )
      < archive_size
      )
    {
      EMWEB_ERROR(( "ews (ewaInstallArchives): Error %s (errno=%d) while attemping to read file '%s'\n",
                    strerror(errno), errno, archive_file));
      exit(1);
    }

  fclose( archive_file_p );
#endif /* EMWEB_USE_FILE_ARCHIVE */


  /*
   * Install archive in EmWeb/Server database.
   */
  status = ewsDocumentInstallArchive( ew_archive
                                     ,ew_archive_data_p
                                     );
  if (status == EWS_STATUS_OK)
    {
      app_status = EWA_STATUS_OK;

      /* ########################### */
      /* Initialize any content here */
      /* ########################### */
#     ifdef EW_CONFIG_OPTION_UPLOAD_ARCHIVE
      uploadInit();
#     endif
#     ifdef EMWEB_POSIX_EXAMPLE
      init_toaster();
      init_device();
#     endif /* EMWEB_POSIX_EXAMPLE */

      /* ########################### */
      /* Initialize Authentication   */
      /* ########################### */

#     ifdef EW_CONFIG_OPTION_AUTH_BASIC
      /*
       * Register Aladdin:open sesame for realm "EmWeb Demonstration"
       */
      auth.scheme = ewsAuthSchemeBasic;
      auth.params.basic.userName = "Aladdin";
      auth.params.basic.passWord = "open sesame";
      auth.handle = (void *) 0; /* not allowed to use SSP */
      auth_handle = ewsAuthRegister("EmWeb Demonstration", &auth);
      if (auth_handle == EWS_AUTH_HANDLE_NULL)
        {
          EMWEB_ERROR(( "ewsAuthRegister failed\n" ));
          app_status = EWA_STATUS_ERROR;
        }

#     endif /* EW_CONFIG_OPTION_AUTH_BASIC */

#     ifdef EW_CONFIG_OPTION_AUTH_DIGEST
#ifndef EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE
      /*
       * Register digest:digest for realm digest (testing digest auth)
       */
      auth.scheme = ewsAuthSchemeDigest;
      auth.params.digest.userName = "digest";
      auth.params.digest.passWord = "digest";

      auth_handle = ewsAuthRegister("digest", &auth);
      if (auth_handle == EWS_AUTH_HANDLE_NULL)
        {
          EMWEB_ERROR(( "ewsAuthRegister failed\n" ));
          app_status = EWA_STATUS_ERROR;
        }
#else
      /* DIGEST_CHANGE OPTION IS TURNED ON */
      /*
       * Register digest:digest for realm digest (testing digest auth)
       */
      auth.scheme = ewsAuthSchemeDigest;
      /*
       * Register digest: (secret) for realm 'digest'
       */
      auth.scheme = ewsAuthSchemeDigest;
      md5_secretp=md5_secret1;

      /* Get the hash to register calling ewsAuthHashMD5.
       * user=digest, realm=digest, password = digest
       */
      ewsAuthHashMD5("digest","digest", "digest", (char **)&md5_secretp);
      auth.scheme = ewsAuthSchemeDigest;
      auth.params.digest_change.userName = "digest";
      auth.params.digest_change.md5_secret=md5_secretp;

      auth.handle = (void *) 0; /* not allowed to use SSP */
      auth_handle1 = ewsAuthRegister("digest", &auth);
      auth_handle=auth_handle1;
      if (auth_handle == EWS_AUTH_HANDLE_NULL)
        {
          EMWEB_ERROR(( "ewsAuthRegister failed\n" ));
          app_status = EWA_STATUS_ERROR;
        }

      /*
       * Register admin: (secret) for realm 'realm_config'
       */
      auth.scheme = ewsAuthSchemeDigest;
      md5_secretp=md5_secret2;
      /* Get the hash to register calling ewsAuthHashMD5.
       * user=admin, realm=realm_config, password = pw1
       */
      ewsAuthHashMD5("admin","realm_config", "pw1", (char **)&md5_secretp);
      auth.scheme = ewsAuthSchemeDigest;
      auth.params.digest_change.userName = "admin";
      auth.params.digest_change.md5_secret=md5_secretp;

      auth.handle = (void *) 0; /* not allowed to use SSP */
      auth_handle1 = ewsAuthRegister("realm_config", &auth);
      auth_handle=auth_handle1;
      if (auth_handle == EWS_AUTH_HANDLE_NULL)
        {
          EMWEB_ERROR(( "ewsAuthRegister failed\n" ));
          app_status = EWA_STATUS_ERROR;
        }

      /*
       * Register user1: (secret) for realm 'realm_config'
       */
      auth.scheme = ewsAuthSchemeDigest;
      md5_secretp=md5_secret3;
      /* Get the hash to register calling ewsAuthHashMD5.
       * user=user1, realm=realm_config, password = user1
       */
      ewsAuthHashMD5("user1","realm_config", "user1", (char **)&md5_secretp);
      auth.scheme = ewsAuthSchemeDigest;
      auth.params.digest_change.userName = "user1";
      auth.params.digest_change.md5_secret=md5_secretp;

      auth.handle = (void *) 0; /* not allowed to use SSP */
      auth_handle = ewsAuthRegister("realm_config", &auth);
      if (auth_handle == EWS_AUTH_HANDLE_NULL)
        {
          EMWEB_ERROR(( "ewsAuthRegister failed\n" ));
          app_status = EWA_STATUS_ERROR;
        }

#endif /* IFNDEF EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE */
#     endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
    }
  else
    {
      EMWEB_ERROR(( "ewsDocumentInstallArchive failed %d\n", status ));
      app_status = EWA_STATUS_ERROR;
    }

  return app_status;
}

/*
 * ewaTick is called once per tick by the server main loop (ewmain.c)
 */
void ewaTick( void )
{
}

/*
 * ewaConnectionInit
 *   Called when a new connection is allocated by the POSIX main loop.
 *   Should initialize any application-specific field in the net handle.
 */
void ewaConnectionInit( EwaNetHandle connection )
{
  EW_UNUSED(connection);
}

/*
 * ewaAuthRealmQualifier
 * Return qualifier to append to realms
 */
const char *
ewaAuthRealmQualifier ( void )
{
  return (realm_qualifier);
}

/*
 * ewaNetLocalHostName
 * Return configured name to EmWeb/Server
 */
const char *
ewaNetLocalHostName ( EwsContext context )
{
# ifdef EW_CONFIG_OPTION_SSL_RSA
  EwaNetHandle connection;

  connection = ewsContextNetHandle(context);
  return connection->ssl_connection_flg ?
      local_host_just_name :
      local_host_name;
# else
  EW_UNUSED(context);

  return (local_host_name);
#endif
}

/*
 * ewaArchiveDate
 *   Return date on which the archive was compiled
 */
const char* ewaArchiveDate( void )
{
  return ewsDocumentArchiveDate(ew_archive_data_p);
}

/*
 * Attempt to allocate memory and check that the allocation was successful.
 */
static void *
Malloc_Safe(size_t s, const char *file, uint32 line)
{
  void *p;


   p = ewaAlloc(s);


  if (NULL == p)
    {
      EMWEB_ERROR(("malloc_safe: attempted allocation of %d bytes failed\n",
                   s));
      ERROR_EXIT;
    }
  return p;
}
