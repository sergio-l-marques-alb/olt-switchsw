#ifndef __QUAL_RFC2819_BUFFER_H__
#define __QUAL_RFC2819_BUFFER_H__

//*****************************************************************************

#include <stdio.h>
#include "datatypes.h"
#include "ptin_globaldefs.h"

//Ring buffer entry
typedef struct {
  L7_long32  index; 
  L7_long32  arg; 
  L7_long32  time;
  L7_long32  path; 
  L7_long32  cTempo;  

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

} __attribute__ ((packed)) TBufferRegQualRFC2819;





#define MAX_QUAL_RFC2819_REG_NUM     PTIN_SYSTEM_N_PORTS*96  // Number of entrys for each Ring buffer (96 registers per port)

#define RFC2819_BUFFER_15MIN         0
#define RFC2819_BUFFER_24HOURS       1
#define MAX_QUAL_RFC2819_BUFFERS     2                       // 0-15min, 1-24hours      

// buffer structure...
//----------------------------
typedef struct {
  L7_uint16    wrptr[MAX_QUAL_RFC2819_BUFFERS];      //wr pointer for each ring buffer
  L7_uint16    bufferfull[MAX_QUAL_RFC2819_BUFFERS]; //buffer full flag for each ring buffer

  TBufferRegQualRFC2819 reg[MAX_QUAL_RFC2819_BUFFERS][MAX_QUAL_RFC2819_REG_NUM];   //Ring Buffer
} __attribute__ ((packed)) TBufferQualRFC2819;


//*****************************************************************************
//* functions to export
//*****************************************************************************
extern L7_int ptin_rfc2819_buffer_init(L7_int32 flag);

extern L7_RC_t ptin_rfc2819_buffer_write(L7_int32 buffer_index, void* data, L7_int32 flag);

extern void ptin_rfc2819_buffer_print_ctrl(void);

extern L7_int ptin_rfc2819_buffer_get(L7_int32 buffer_index, L7_int32 reg_index, TBufferRegQualRFC2819 *reg);

extern L7_int ptin_rfc2819_buffer_get_inv(L7_int32 buffer_index, L7_int32 reg_index, TBufferRegQualRFC2819 *reg);

extern L7_RC_t ptin_rfc2819_buffer_clear(L7_int32 buffer_index);

extern L7_RC_t ptin_rfc2819_buffer_fill(L7_uint8 buffer, L7_int32 n_elementos);

extern L7_RC_t ptin_rfc2819_get_buffer_status(L7_int buffer_type, L7_uint16 *max_regs,  L7_uint16 *wrptr, L7_uint16 *bufferfull);

#endif //__QUAL_RFC2819_BUFFER_H__
