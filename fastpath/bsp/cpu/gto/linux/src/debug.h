/*
 * BaseDados.h
 * Declaração das estruturas e funções que suportam as configurações do sistema
*/
#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef __cplusplus
#define _EXTERN_ extern "C"
#else
#define _EXTERN_ extern
#endif

//******************************************************************************

typedef struct {
  int    level;
  char   str[64];
} t_debug_help_tab;

//******************************************************************************
// Tipos de output:
//******************************************************************************

#define DEBUG_HELP_TAB_NUM      17

#define PRINT_VERBOSE           0x00000001
#define DEBUG_START_UP          0x00000002
#define DEBUG_INS_MODULES       0x00000004
#define DEBUG_PROCESSES         0x00000008
#define DEBUG_TRAP              0x00000010
#define DEBUG_GPON_HW           0x00000020
#define DEBUG_ETHER_HW          0x00000040
#define DEBUG_TRMI_MSG          0x00000080
#define DEBUG_XILINX            0x00000100
#define DEBUG_ERRORS            0x00000200
#define DEBUG_GPON_OMCI         0x00000400
#define DEBUG_SM_OMCI           0x00000800
#define DEBUG_GPON_CALLLBACK    0x00001000
#define DEBUG_SFP_XFP           0x00002000
#define DEBUG_LOG               0x00004000
#define DEBUG_OLT7_APP          0x00008000
#define DEBUG_APPLICATION       0x10000000

#ifdef _DEBUG_C_
const t_debug_help_tab debug_help_tab[DEBUG_HELP_TAB_NUM] = {
  { PRINT_VERBOSE         , "VERBOSE        "},
  { DEBUG_START_UP        , "START_UP       "},
  { DEBUG_INS_MODULES     , "INS_MODULES    "},
  { DEBUG_PROCESSES       , "PROCESSES      "},
  { DEBUG_TRAP            , "TRAP           "},
  { DEBUG_GPON_HW         , "GPON_HW        "},
  { DEBUG_ETHER_HW        , "ETHER_HW       "},
  { DEBUG_TRMI_MSG        , "TRMI_MSG       "},
  { DEBUG_XILINX          , "XILINX         "},
  { DEBUG_ERRORS          , "ERRORS         "},
  { DEBUG_GPON_OMCI       , "GPON_OMCI      "},
  { DEBUG_SM_OMCI         , "SM_OMCI        "},
  { DEBUG_GPON_CALLLBACK  , "GPON_CALLLBACK "},
  { DEBUG_SFP_XFP         , "SFP_XFP        "},
  { DEBUG_LOG             , "LOG            "},
  { DEBUG_OLT7_APP        , "OLT7_APP       "},
  { DEBUG_APPLICATION     , "APPLICATION    "}
};
#endif

//******************************************************************************

_EXTERN_ char *debug_print_help( char *str , int ident );
_EXTERN_ int  debug_setlevel(int level);
_EXTERN_ int  debug_getlevel( void );
_EXTERN_ int  debug_true(int level);
_EXTERN_ int  printf_debug( int level , const char *fmt,... );
_EXTERN_ int  debug_setoutput( char *path );
_EXTERN_ int  debug_print_dump( unsigned int level , char *ident , int col , char *buf , int n );

//******************************************************************************

#undef _EXTERN_

//******************************************************************************

#endif //_DEBUG_H_
