/* ======================================================================
Projecto:   OMG_SIRAC
Modulo:     ********
Copyright:  SIR@PT Inovacao
Descricao:  Constantes e macros globais ao projecto.

Autores:    Ana Cláudia Dias (ACD), Álvaro Corga (AC),
            Augusta Manuela (AM), Vitor Mirones (VM)

Historico:  VM 2003.10.01  - Criacao do modulo V1.0.0.0
            VM 2004.08.18  - Adaptação para Biblioteca.

======================================================================= */
#ifndef GLNETBAND_GLOBAL_DEFS_H
#define GLNETBAND_GLOBAL_DEFS_H

#include <string.h>

#define IGMP_WHITELIST_FEATURE

// --------------------------------------------------------------------
// Public Definitions
// --------------------------------------------------------------------

   #ifdef __cplusplus
      #define EXTERN_C extern "C"
   #else
      #define EXTERN_C extern
   #endif

   // Data Types
   #ifndef SBYTE
      #define SBYTE                signed char
   #endif
   #ifndef BOOLEAN
      #define BOOLEAN              unsigned char
   #endif
   #ifndef UCHAR
      #define UCHAR                unsigned char
   #endif
   #ifndef BYTE
      #define BYTE                 unsigned char
   #endif
   #ifndef uint8
      #define uint8                unsigned char
   #endif

   #ifndef UWORD
      #define UWORD                unsigned short
   #endif
   #ifndef SWORD
      #define SWORD                short
   #endif
   #ifndef uint16
      #define uint16               unsigned short
   #endif

   #ifndef INT
      #define INT                   int
   #endif
   #ifndef SINT
      #define SINT                 int
   #endif
   #ifndef UINT
      #define UINT                 unsigned int
   #endif
   #ifndef uint32
      #define uint32               unsigned int
   #endif
   #ifndef HRES
      #define HRES                 unsigned int
   #endif

   #ifndef WORD32
      #define WORD32               unsigned long
   #endif
   #ifndef DWORD
      #define DWORD                unsigned long
   #endif
   #ifndef DLONG
      #define DLONG                unsigned long long
   #endif
   #ifndef uint64
      #define uint64               unsigned long long
   #endif
   #ifndef IFINDEX
      #define IFINDEX              unsigned int
   #endif
   #ifndef IFID
      #define IFID                 unsigned int
   #endif

   #ifndef FLOAT
      #define FLOAT                float
   #endif

   #ifndef SOCKADDR
      #define SOCKADDR             struct sockaddr_in
   #endif
   #ifndef ID_NULL
      #define ID_NULL                  ((UINT)(-1))
   #endif

   //Uma vez que no agente, existem includes para o boolean, fica aqui a definicao condicionada
   #ifndef TRUE
      #define TRUE                     1
      #define FALSE                    0
      #define BOOL                     unsigned char
   #endif

      typedef enum
      {
         READMODE_NULL    = 0,
         READMODE_PASS2HW = 1,
         READMODE_EMULHW  = 2,
      } READMODE;

      typedef enum
      {
         ROWSTATUS_FREE      = 0,      // resource free
         ROWSTATUS_RESERVED  = 1,      // resource reserved, cannot be read or configured, only deleted or inserted
         ROWSTATUS_USED      = 2,      // resource used, can be read, configured and deleted, cannot be inserted
         ROWSTATUS_ALLOCATED = 3,      // resource allocated, like used but can be inserted in DB
      } ROWSTATUS;

      typedef enum
      {
         ADMIN_NOTPRESENT  = 0,
         ADMIN_UP          = 1,
         ADMIN_DOWN        = 2,
         ADMIN_MAINTENANCE = 4,
      } ADMIN;

      #define PACKED                   __attribute__ ((packed))

      typedef struct
      {
         UINT     valA;
         UINT     valB;
         UINT     valC;
         UINT     valD;
         UINT     valE;
      } st_Nary;

   // Common return values
   #ifndef S_OK
      #define S_OK				      0
   #endif
   #ifndef S_RECEIVED
      #define S_RECEIVED		      1
   #endif
   #ifndef E_FAILED
      #define E_FAILED 		      -1
   #endif
   #ifndef E_INVALID
      #define E_INVALID		      -2
   #endif
   #ifndef E_EMPTY
      #define E_EMPTY  		      -3
   #endif

   #ifndef EMPTY_STRING
      #define EMPTY_STRING         ""
   #endif
   #ifndef QUERYSTR_LENGTH_MAX
      #define QUERYSTR_LENGTH_MAX  1024
   #endif
   #ifndef SQLQUERY_LENGTH
      #define SQLQUERY_LENGTH      1024
   #endif
   #ifndef LINE_LENGTH_MAX
      #define LINE_LENGTH_MAX      256
   #endif
   #ifndef XMLTAG_LENGTH
      #define XMLTAG_LENGTH         2048
   #endif

   // Common Macros
   #ifndef FREE_POINTER
      #define FREE_POINTER(p)            if(p){free(p);(p)=0;}
   #endif
   #ifndef STRNCPY
      #define STRNCPY(dest, src, len)    {strncpy((char*)(dest), (const char *)(src), (size_t)(len)); ((char*)(dest))[(size_t)(len)]=0;}
   #endif
   #ifndef STRNCMP
      #define STRNCMP(a, b, len)         strncmp((const char *)(a), (const char *)(b), (size_t)(len))
   #endif
   #ifndef MAX
      #define MAX(a, b)                   (((a) > (b))?(a):(b))
   #endif
   #ifndef MIN
      #define MIN(a, b)                   (((a) < (b))?(a):(b))
   #endif
   #ifndef GAMMA
      #define GAMMA(val, min, maj)        (((val) < (min))?(min):(((val) > (maj))?(maj):(val)))
   #endif

   // Values for Entity Layer
   // Neste momento existem 3 areas reservadas:
   //    Interfaces fisicas:  begin=0x00 ordem crescente (neste momento pode crescer ate 66 entidades nesta gama), dar primazia em por entidades de backplane para o fim desta gama
   //    Interfaces virtuais: begin=0x42 ordem crescente 
   //    Entidades virtuais:  begin=0xFF ordem decrescente (cresce em stack)
   typedef enum
   {
      // physical interfaces
      ENTITY_LAYER_STM_PHYSICAL      = 0x00,
      ENTITY_LAYER_ETHERNET_PHYSICAL = 0x01,
      ENTITY_LAYER_DS1_PHYSICAL      = 0x02,
      ENTITY_LAYER_OCH_PHYSICAL      = 0x03,
      ENTITY_LAYER_PON_PHYSICAL      = 0x04,
      ENTITY_LAYER_SUPPORT_PHYSICAL  = 0x40,    // interfaces físicas de suporte - não fornecem tráfego de cliente (eth0, rs232, ...)
      ENTITY_LAYER_MPLS_PHYSICAL     = 0x41,
      // virtual interfaces
      ENTITY_LAYER_VC4_VIRTUAL       = 0x42,
      ENTITY_LAYER_ETHERNET_VIRTUAL  = 0x84,
      ENTITY_LAYER_LOOPBACK_VIRTUAL  = 0x85,
      // virtual entities
   } ENTITY_LAYER;

   typedef union
   {
      UINT  raw;              // raw
      struct
      {
         BYTE        module;  // {0, ...}
         BYTE        layer;   // t_EntityLayer
         UWORD       id;      // {0, ...}, if possible starts in 1
      } st;                   // structered Id
   } st_EntityId;             // [4B]


/*
SHOW        0
SHOWNEXT    1
CREATE      2
REMOVE      3
CONFIG      4
STATUS      5
STATUSNEXT  6
COUNTERS    7
INFO        8
GETNEXT     9
CAPABILITY  10
GETFREEID   11

SPECIFIC1   14
SPECIFIC0   15
*/



#endif // GLNETBAND_GLOBAL_DEFS_H
