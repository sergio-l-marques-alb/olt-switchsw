/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename               atmtypes.h
 *
 * @purpose                Standard ATM typedefs
 *
 * @component              Routing Utils Component
 *
 * @comments
 *
 * @create                 
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/

#ifndef atmtypes_h
#define atmtypes_h


typedef  byte  t_ESI[6];         /* End System Identifier */

typedef  byte  t_Prefix[13];     /* Network Prefix        */

typedef  struct
{
   t_Prefix    prefix;   /* Network Prefix set by the switch */
   t_ESI       esi;      /* ESI or MAC address               */
   byte        sel;      /* Selector                         */
}  t_ATMAddr;            /* ATM address                      */

typedef struct
{
   t_ATMAddr atmAddress;  /* ATM address              */
   t_Handle  signObj;     /* signalling object (port) */
   t_Handle  atmObj;      /* ATM object (port)        */
} t_ATMEndPt;             /* ATM end point            */


typedef  struct
{
   word     vpi;          /* Virual Path Identifier     */
   word     vci;          /* Virtual Circuit Identifier */
} t_VCC;

typedef enum
{
   e_UBR   = 0,           /* unspecified bit rate        */
   e_CBR   = 1,           /* constant bit rate           */
   e_VBR   = 2,           /* variable bit rate           */
   e_VBRrt = 3,           /* real time variable bit rate */
   e_ABR   = 4            /* available bit rate          */
} e_ServiceCategory;

typedef struct tagt_ConnCR
{
   byte           Type;     /* service category (enum e_ServiceCategory) */
   ulng           _MCR;     /* minimum cells per second                  */
   ulng           _PCR;     /* peak cells per second                     */
   ulng           _ICR;     /* initial cells per second                  */
   ulng           MaxBurst; /* maximum burst rate for VBR connections    */
                            /* (cells sent at peak rate)                 */
                            /* ABR  parameters                           */
   ulng           _TBE;     /* transient buffer exposure                 */
   ulng           _FRTT;    /* fixed round trip time                     */
   byte           _RIF;     /* rate increase factor                      */
   byte           _RDF;     /* rate decrease factor                      */
                            /* ABR  optional parameters                  */
   byte           _NRM;
   byte           _TRM;
   byte           _CDF;
   word           _ADTF;
} t_ConnCR;

typedef  struct
{
   t_VCC       vcc;
   word        outmtu;
   t_ConnCR    outTrafParms;
} t_AAL5Info;

typedef  struct
{
   t_ATMEndPt  locAtmEndPt;      /* local ATM end point                     */
   t_ATMAddr   remAtmAddress;    /* ATM address of destination ATM endpoint */
   t_AAL5Info  aal5Info;
}  t_PVCInfo;                    /* PVC external structure                  */

typedef  e_Err   ErrCode;

#endif

/* --- end of file atmtypes_h --- */
