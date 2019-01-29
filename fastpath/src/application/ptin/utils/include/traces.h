/* ============================================================================
Projecto:   OMG_SIRAC
Modulo:     ********
Copyright:  SIR1@PT Inovacao
Descricao:  Biblioteca de registo de actividades.

Autor:      Ana Cláudia Dias (ACD), Augusta Manuela (AM), Vitor Mirones (VM)

Historico:  2004.08.16  - Criacao da biblioteca, separação do log e do debug
=============================================================================== */
#ifndef SIR_TRACES_H
#define SIR_TRACES_H

//#include <PacketPad.h>
#include "globaldefs.h"
// --------------------------------------------------------------------
// Public Definitions
// --------------------------------------------------------------------

   #ifndef  APPLICATION_NAME
      #define APPLICATION_NAME            ""
   #endif

   #ifdef DEBUGTRACE
      #undef DEBUGTRACE
   #endif
   #define  DEBUGTRACE                    DebugTrace
   #define  LOGTRACE                      LogTrace

   #define  DEBUG_LENGTH_MAX               1280

   // Níveis de severidade
   #define  TRACE_SEVERITY_EMERGENCY      0x00           // system is unusable
   #define  TRACE_SEVERITY_ALERT          0x01           // action must be taken immediately
   #define  TRACE_SEVERITY_CRITICAL       0x02           // critical conditions
   #define  TRACE_SEVERITY_ERROR          0x03           // error conditions
   #define  TRACE_SEVERITY_WARNING        0x04           // warning conditions
   #define  TRACE_SEVERITY_NOTICE         0x05           // normal but significant condition
   #define  TRACE_SEVERITY_INFORMATIONAL  0x06           // informational messages
   #define  TRACE_SEVERITY_DEBUG          0x07           // debug-level messages

   // Files
   #define  LOG_FILENAME                  "Config.log"
   #define  DEBUG_FILENAME                "Debug.log"

   // Debug output
   #define  DEBUG_OUTPUT_NOLOG            0x0000
   #define  DEBUG_OUTPUT_FILE             0x0001
   #define  DEBUG_OUTPUT_CONSOLE          0x0002

   // Debug Format
   #define  DEBUG_MODE_TIME               0x0001
   #define  DEBUG_MODE_PID                0x0002
   #define  DEBUG_MODE_ID                 0x0004
   #define  DEBUG_MODE_SEVERITY           0x0008

   // Debug Modules (EmiloX16)   
   #define  TRACE_MODULE_SYSTEM           0x00010000
   #define  TRACE_MODULE_SYNC             0x00020000   
   #define  TRACE_MODULE_SYNCHRONISM      0x00020000   
   #define  TRACE_MODULE_BOARDS           0x00040000
   #define  TRACE_MODULE_CARD             0x00040000
   #define  TRACE_MODULE_INTERFACE        0x00080000
   #define  TRACE_MODULE_INTERFACES       0x00080000 //deprecated
   #define  TRACE_MODULE_SDH              0x00100000
   #define  TRACE_MODULE_STMx             0x00100000 //deprecated
   #define  TRACE_MODULE_Ex               0x00200000
   #define  TRACE_MODULE_ETH              0x00800000 //deprecated
   #define  TRACE_MODULE_ETHERNET         0x00800000
   #define  TRACE_MODULE_ETHERNET_MEF     0x00800000
   #define  TRACE_MODULE_PDH              0x01000000
   #define  TRACE_MODULE_OTN              0x00100000

   #define  TRACE_MODULE_IP               0x02000000
   #define  TRACE_MODULE_NETWORK          0x02000000
   #define  TRACE_MODULE_LINKS            0x04000000
   #define  TRACE_MODULE_MPLSLINK         0x04000000
   #define  TRACE_MODULE_SNCP             0x08000000
   #define  TRACE_MODULE_MSP              0x10000000
   #define  TRACE_MODULE_ATM              0x20000000
   #define  TRACE_MODULE_TRAPS            0x40000000
   #define  TRACE_MODULE_MAINTENANCE      0x80000000
   #define  TRACE_MODULE_ALL              0xFFFF0000

   // Debug Layers
   #define  TRACE_LAYER_APP               0x0001
   #define  TRACE_LAYER_CAPI              0x0002
   #define  TRACE_LAYER_SAPI              0x0004
   #define  TRACE_LAYER_RAPI              0x0008
   #define  TRACE_LAYER_HAPI              0x0010
   #define  TRACE_LAYER_IPC               0x0020
   #define  TRACE_LAYER_DATA              0x0040
   #define  TRACE_LAYER_DB                0x0080
   #define  TRACE_LAYER_AUTH              0x0100
   #define  TRACE_LAYER_VALID             0x0200
   #define  TRACE_LAYER_CORE              0x0400
   #define  TRACE_LAYER_LOG               0x8000         // Configuration log flag
   #define  TRACE_LAYER_ALL               0xFFFF

   // Color codes (for console output)
   #define  DEBUG_ESCFIM                  "\x1B[00m"
   #define  DEBUG_ESCVERMELHO             "\x1B[00;31m"
   #define  DEBUG_ESCVERDE                "\x1B[00;32m"
   #define  DEBUG_ESCAMARELO              "\x1B[00;33m"
   #define  DEBUG_ESCAZUL                 "\x1B[00;34m"
   #define  DEBUG_ESCMAGENTA              "\x1B[00;35m"
   #define  DEBUG_ESCCIANO                "\x1B[00;36m"
   #define  DEBUG_ESCBRANCO               "\x1B[00;37m"
   #define  DEBUG_ESCVERMELHOVIVO         "\x1B[01;31m"
   #define  DEBUG_ESCVERDEVIVO            "\x1B[01;32m"
   #define  DEBUG_ESCAMARELOVIVO          "\x1B[01;33m"
   #define  DEBUG_ESCAZULVIVO             "\x1B[01;34m"
   #define  DEBUG_ESCMAGENTAVIVO          "\x1B[01;35m"
   #define  DEBUG_ESCCIANOVIVO            "\x1B[01;36m"
   #define  DEBUG_ESCBRANCOVIVO           "\x1B[01;37m"

   typedef struct _DebugParams
   {
      int   mask;
      int   debugFormat;                                 // Defines log string format  (How)
      UINT  debugLayers;                                 // Define software layer      (When)
      UINT  debugModules;                                // Define technology Module   (What)
      int   debugOutput;                                 // Define where to write      (Where)
      char  debugDevice[50];                             // Device for Console output
   } st_DebugParams;

// --------------------------------------------------------------------
// Public Methods
// --------------------------------------------------------------------

   #define  TRACE_SEVERITY(error)      ((error==0)?TRACE_SEVERITY_INFORMATIONAL:((error>>16)&0xFF))

   EXTERN_C void     GetDebugSettings  (st_DebugParams *data);
   EXTERN_C void     SetDebugFormat    (int format);
   EXTERN_C void     SetDebugFilters   (UINT layers, UINT modules);
   EXTERN_C void     SetDebugOutput    (int output);
   EXTERN_C void     SetDebugDevice    (char *device);

   EXTERN_C void     DebugTrace        (int mask, int sev, const char *fmt, ...);
   EXTERN_C void     LogTrace          (int sev, const char *fmt, ...);

// -------------------------------------------------------------
// Private Methods
// -------------------------------------------------------------
   
   void  DebugTrailer    (FILE *fp, int mask, int sev);

   char* getTimeMarker  (char* output);
   char* getSeverity    (int sev, int mode, char* output);

   void DumpMemActivation (BOOLEAN a);
   void DumpMem (void(*dumpEntity)(void*), void *entity, size_t size, int num, int flags);

   #define  DUMPMEM(dumper, entity, size, num, bit_header1)   DumpMem((void*)(dumper), (void*)(entity), (size_t)(size),(int)(num), (bit_header1))

#endif // SIR_TRACES_H
