/*
 * BaseDados.h
 * Declaração das estruturas e funções que suportam as configurações do sistema
*/
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdarg.h>
#include "globals.h"

#define MAXLINELEN 255

#define DEBUG_START_UP     0x000001
#define DEBUG_INS_MODULES  0x000002
#define DEBUG_PROCESSES    0x000008
#define DEBUG_BYTES_TRMI   0x000020
#define DEBUG_SINCRONISMO  0x000010
#define DEBUG_SINC_CONFIG  0x000004
#define DEBUG_TRAP         0x000040
#define DEBUG_SNCP         0x000080
#define DEBUG_SNCP_SD      0x000100
#define DEBUG_CRUZAMENTOS  0x000200
#define DEBUG_PROTBOARD    0x000400
#define DEBUG_XILINX       0x000800
#define DEBUG_RELES        0x001000
#define DEBUG_I2C          0x002000
#define DEBUG_SM_PBOARD    0x004000
#define DEBUG_SDEGRADE     0x008000
#define DEBUG_ALS          0x010000
#define DEBUG_ALARME_EXTERNO 0x020000
#define DEBUG_CWDM         0x040000
#define DEBUG_TRMI         0x080000
#define DEBUG_ATM          0x100000
#define DEBUG_ETH          0x200000
#define DEBUG_UNICOM       0x400000

typedef struct {
    int start_up_debug;    //0-inactivo, 1-activo
    int ins_modules_debug; //0-inactivo, 1-activo
    int processes_debug;   //0-inactivo, 1-activo
    int trmi_bytes_debug;  //0-inactivo, 1-activo
    int sincronismo_debug; //0-inactivo, 1-activo
    int sinc_config;       //0-inactivo, 1-activo
    int trap_debug;        //0-inactivo, 1-activo
    int sncp_debug;        //0-inactivo, 1-activo
    int sncp_debug_sd;     //0-inactivo, 1-activo
    int cruzamentos;       //0-inactivo, 1-activo
    int protboard;         //0-inactivo, 1-activo
    int xilinx;            //0-inactivo, 1-activo
    int reles;             //0-inactivo, 1-activo
    int i2c;               //0-inactivo, 1-activo
    int sm_pboard;         //0-inactivo, 1-activo
    int proc_sdegrade;     //0-inactivo, 1-activo
    int als;               //0-inactivo, 1-activo
    int alarme_externo;    //0-inactivo, 1-activo
    int cwdm;              //0-inactivo, 1-activo
    int trmi_debug;        //0-inactivo, 1-activo
    int atm_debug;         //0-inactivo, 1-activo
    int eth_debug;         //0-inactivo, 1-activo
    int unicom_debug;      //0-inactivo, 1-activo
} T_DEBUG;


extern T_DEBUG bd_debug;

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int debug_init(void);


#ifdef __cplusplus
extern "C"
#else
extern
#endif
int printf_debug(int level, const char *fmt,...);

//#define printf_debug(...)

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int debug_setlevel(int level);

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int debug_scr(int level, const char *fmt,...);

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int debug_activo(int level);

EXTERN_C void debug_api(void);

EXTERN_C int debug_print_levels(void);

//extern int debug_level;

#endif //_DEBUG_H_
