
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//                                                                      //
//              ||||||\   ||||||||   ||||||   ||    ||                  //
//              ||   ||      ||        ||     ||||  ||                  //
//              |||||||      ||        ||     || || ||                  //
//              ||           ||        ||     ||  ||||                  //
//              ||           ||        ||     ||   |||                  //
//              ||           ||      ||||||   ||    ||                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FileName:                                                            //
//            qualidade_RFC2819.h                                       //
//                                                                      //
//----------------------------------------------------------------------//
// Projecto:                                                            //
//            PR710_SDH_RA                                              //
//                                                                      //
//----------------------------------------------------------------------//
// Descrição: Este implementa as funcoes para processamento da qualidade//
//                                                                      //
//----------------------------------------------------------------------//
//                                                                      //
// Histórico:                                                           //
//            13 Novembro 2003 - primeira versão                        //
//                                                                      //
//----------------------------------------------------------------------//
// Autor: Celso Lemos                                                   //
//////////////////////////////////////////////////////////////////////////
#ifndef __QUALIDADE_RFC2819_H__
#define __QUALIDADE_RFC2819_H__

#include <time.h>
#include "datatypes.h"

#define RFC2819_ACTUAL     0
#define RFC2819_PER24HORAS 1

#define RFC2819_PROBE_ENABLE   1 
#define RFC2819_PROBE_DISABLE  0

#define CONCAT_QUAL_PATH(dir, type, porto) ( (((dir)<<31) & 0x80000000) | (((type)<<28) & 0x70000000) | (((porto)<<14) & 0x0FFFC000) )

//Estrutura que suporta a analise qualidade (performance)                                        
typedef struct {
  struct {
    L7_int     estado;   //0-stop, 1-running
    L7_uint8   slot;
    L7_ulong32 path;     //Circuito sobre o qual se esta a medir performance
    L7_ulong32 periodo;  //periodo de integracao (1-1min 2-15min, 4-24H, 8-60min)
  } conf;

  L7_uint64 Octets;
  L7_uint64 Pkts;
  L7_uint64 Broadcast;
  L7_uint64 Multicast;
  L7_uint64 CRCAlignErrors;
  L7_uint64 UndersizePkts;
  L7_uint64 OversizePkts;
  L7_uint64 Fragments;
  L7_uint64 Jabbers;
  L7_uint64 Collisions;
  L7_uint64 Utilization;
  L7_uint64 Pkts64Octets;
  L7_uint64 Pkts65to127Octets;
  L7_uint64 Pkts128to255Octets;
  L7_uint64 Pkts256to511Octets;
  L7_uint64 Pkts512to1023Octets;
  L7_uint64 Pkts1024to1518Octets;

  struct {
    int  cTempo;   //Tempo efectivamente analisado
    L7_uint64 Octets;
    L7_uint64 Pkts;
    L7_uint64 Broadcast;
    L7_uint64 Multicast;
    L7_uint64 CRCAlignErrors;
    L7_uint64 UndersizePkts;
    L7_uint64 OversizePkts;
    L7_uint64 Fragments;
    L7_uint64 Jabbers;
    L7_uint64 Collisions;
    L7_uint64 Utilization;
    L7_uint64 Pkts64Octets;
    L7_uint64 Pkts65to127Octets;
    L7_uint64 Pkts128to255Octets;
    L7_uint64 Pkts256to511Octets;
    L7_uint64 Pkts512to1023Octets;
    L7_uint64 Pkts1024to1518Octets;
  } status[2];

  struct {
    L7_int dia;
    L7_int mes;
    L7_int ano;
    L7_int hora;
    L7_int min;
  } reg_data;

} __attribute__ ((packed)) T_QUALIDADE_RFC2819;


extern L7_RC_t ptin_rfc2819_init(void);

extern void ptin_rfc2819_clear(T_QUALIDADE_RFC2819 *param);

extern void ptin_rfc2819_enable(T_QUALIDADE_RFC2819 *param);

extern void ptin_rfc2819_disable(T_QUALIDADE_RFC2819 *param);

extern L7_int ptin_rfc2819_get_admin(T_QUALIDADE_RFC2819 *param);

extern void ptin_rfc2819_proc(L7_uint8 buffer_15min, L7_uint8 buffer_24horas, T_QUALIDADE_RFC2819 *param, struct tm *data_hora, L7_int tgl_time);

extern void ptin_rfc2819_init_probe(T_QUALIDADE_RFC2819 *param, L7_uint8 slot, L7_ulong32 path);

extern void ptin_rfc2819_init_all_probes(void);

extern void ptin_rfc2819_init_buffers(void);

extern void ptin_rfc2819_clear_buffers(void);

extern void ptin_rfc2819_regista_15min(L7_uint8 slot, T_QUALIDADE_RFC2819 *qual);

extern void ptin_rfc2819_regista_24horas(L7_uint8 slot, T_QUALIDADE_RFC2819 *qual);

extern void ptin_rfc2819_init_probes(void);

extern L7_RC_t ptin_rfc2819_config_probe(L7_int Port, L7_uint8 Admin);

extern L7_int ptin_rfc2819_refresh_counters(L7_int Port);

extern L7_RC_t ptin_rfc2819_get_config_probe(L7_int Port, L7_uint8 *Admin);

#endif //__QUALIDADE_H__

