/*
 * ptinHapi_Structs.h
 *
 *  Created on: 2010/04/13
 *      Author: Andre Brizido
 */

#ifndef PTINHAPI_STRUCTS_H_
#define PTINHAPI_STRUCTS_H_

typedef struct _debug
{
   UINT     mask;
   UINT     traceoutput;
   UINT     tracemode;
   UINT     tracemask;
   char     tty[100];
} st_debug;


#endif /* PTINHAPI_STRUCTS_H_ */
