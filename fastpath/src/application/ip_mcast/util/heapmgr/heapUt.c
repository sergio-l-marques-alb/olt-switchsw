/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename   heapUt.c
 *
 * @purpose    Unit Testing Stubs of Memory Heap Storage
 *
 * @component  util
 *
 * @comments   none
 *
 * @create     02/02/2007
 *
 * @author     PKB
 * @end
 *
 **********************************************************************/
#include <stdio.h>
#include <string.h>
#include "osapi.h"
#include "osapi_support.h"
#include "heap_api.h"

/* Test without any kind of debug information */
void heapTestWithoutDebug(void)
{
  L7_uchar8      *buffer[20];
  heapHandle_t   handle;
  heapBuffPool_t blist[] = {{17, 8,"17ByteBuffers", 1},   /* Pool #1 */
                            {121,2,"121ByteBuffers",1},   /* Pool #4 */
                            {64, 3,"64ByteBuffers", 0},   /* Pool #3 */
                            {35, 5,"35ByteBuffers", 1},   /* Pool #2 */
                            {0,  0,"",              0}};
  L7_uint32      i;

  handle = heapCreate(blist, HEAP_MALLOC_ENABLE, L7_LAST_COMPONENT_ID);
  heapDbgPrintSet(handle,1);

  /* Exhaust the lowest size pool */
  printf("Allocating from the lowest sized (17) pool:\n");
  for(i=0;i<4;i++)
  {
    buffer[i] = heapAlloc(handle,11,__FILE__,__LINE__);
    printf("   Buffer[%d] = 0x%x, Req.Size = 11\n", i, (L7_uint32)&(buffer[i]));
  }
  for(i=4;i<8;i++)
  {
    buffer[i] = heapAlloc(handle,19,__FILE__,__LINE__);
    printf("   Buffer[%d] = 0x%x, Req.Size = 19\n", i, (L7_uint32)&(buffer[i]));
  }
  /* Try overflowing */
  buffer[8] = heapAlloc(handle,15,__FILE__,__LINE__);
  printf("   Buffer[%d] = 0x%x, Req.Size = 15\n", i, (L7_uint32)&(buffer[i]));
  printf("Stage 1 Buffer Stats:\n");
  heapDbgStatsDisplay(handle);

  /* Exhaust all pools */
  for(i=9; i<13; i++)
  {
    buffer[i] = heapAlloc(handle,32,__FILE__,__LINE__);
    printf("   Buffer[%d] = 0x%x, Req.Size = 32\n", i, (L7_uint32)&(buffer[i]));
  }
  for(i=13; i<16; i++)
  {
    buffer[i] = heapAlloc(handle,60,__FILE__,__LINE__);
    printf("   Buffer[%d] = 0x%x, Req.Size = 60\n", i, (L7_uint32)&(buffer[i]));
  }
  for(i=16; i<18; i++)
  {
    buffer[i] = heapAlloc(handle,100,__FILE__,__LINE__);
    printf("   Buffer[%d] = 0x%x, Req.Size = 100\n", i, (L7_uint32)&(buffer[i]));
  }
  printf("Stage 2 Buffer Stats:\n");
  heapDbgStatsDisplay(handle);

  /* Try overflowing from pool 3 */
  buffer[18] = heapAlloc(handle,45,__FILE__,__LINE__);
  printf("   Buffer[%d] = 0x%x, Req.Size = 45\n", 18, (L7_uint32)&(buffer[18]));
  /* Try overflowing from pool 4 */
  buffer[19] = heapAlloc(handle,100,__FILE__,__LINE__);
  printf("   Buffer[%d] = 0x%x, Req.Size = 100\n", 19, (L7_uint32)&(buffer[19]));
  printf("Stage 3 Buffer Stats:\n");
  heapDbgStatsDisplay(handle);

  /* Try freeing-up from a pool and re-allocating */
  printf("Freeing-up 0x%x from buffer 15\n", (L7_uint32)buffer[15]);
  heapFree(handle, buffer[15]);
  buffer[15] = heapAlloc(handle,3,__FILE__,__LINE__);
  printf("   Buffer[%d] = 0x%x, Req.Size = 100\n", 15, (L7_uint32)&(buffer[15]));
  printf("Stage 4 Buffer Stats:\n");
  heapDbgStatsDisplay(handle);

  /* Clean-up the heap */
  for(i = 0; i<20; i++)
  {
    if(buffer[i] != L7_NULLPTR)
    {
      heapBuffValidate(handle,buffer[i]);
      heapFree(handle,buffer[i]);
    }
  }
  printf("Stage 5 Buffer Stats:\n");
  heapDbgStatsDisplay(handle);

  heapShow(handle);

  /* Destroy the heap */
  heapDestroy(handle, L7_FALSE);
}

void heapTestWithFileInfo(void)
{
  L7_uchar8*   buffer[10];
  heapHandle_t  handle;
  heapBuffPool_t blist[] = {{128,8,"128 buffers",0},{100,5,"100 Buffers",0},{0,0,0,0}};
  L7_uint32 i;

  handle = heapCreate(blist,HEAP_DEBUG_FILE_INFO | HEAP_MALLOC_ENABLE ,0);
  heapDbgPrintSet(handle,1);

  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,128,__FILE__,__LINE__);

  /* validate memory location */
  printf("validate - buffer[0] %d \n",heapBuffValidate(handle,buffer[0]));
  /* validate junk memory location */
  printf("validate - junk %d \n",heapBuffValidate(handle,(void*)(buffer[0]+1)));

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);


  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,93+i,__FILE__,__LINE__);

  for(i=0;i<8;i++)
    heapDbgBuffInfo(handle,buffer[i]);

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);

  heapDbgStatsDisplay(handle);

  heapDestroy(handle,L7_FALSE);
  return;
}

void heapTestWithMemGuard(void)
{
  L7_uchar8*   buffer[10];
  heapHandle_t  handle;
  heapBuffPool_t blist[] = {{128,8,"128 buffers",0},{100,5,"100 Buffers",0},{0,0,0,0}};
  L7_uint32 i;

  handle = heapCreate(blist,HEAP_DEBUG_MEMORY_GUARD,0);
  heapDbgPrintSet(handle,1);

  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,128,__FILE__,__LINE__);

  /* validate memory location */
  printf("validate - buffer[0] %d \n",heapBuffValidate(handle,buffer[0]));
  /* validate junk memory location */
  printf("validate - junk %d \n",heapBuffValidate(handle,(void*)(buffer[0]+1)));

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);


  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,93+i,__FILE__,__LINE__);

  heapDbgStatsDisplay(handle);

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);

  heapDbgStatsDisplay(handle);

  heapDestroy(handle,L7_FALSE);
  return;
}

void heapTestWithMemGuardAndFileInfo(void)
{
  L7_uchar8*   buffer[10];
  heapHandle_t  handle;
  heapBuffPool_t blist[] = {{125,8,"128 buffers",0},{100,5,"100 Buffers",0},{0,0,0,0}};
  L7_uint32 i;

  handle = heapCreate(blist,HEAP_DEBUG_MEMORY_GUARD|HEAP_DEBUG_FILE_INFO,0);
  heapDbgPrintSet(handle,1);

  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,128,__FILE__,__LINE__);

  /* validate memory location */
  printf("validate - buffer[0] %d \n",heapBuffValidate(handle,buffer[0]));
  /* validate junk memory location */
  printf("validate - junk %d \n",heapBuffValidate(handle,(void*)(buffer[0]+1)));

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);


  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,93+i,__FILE__,__LINE__);

  heapDbgStatsDisplay(handle);

  for(i=0;i<8;i++)
    heapDbgBuffInfo(handle,buffer[i]);

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);

  heapDbgStatsDisplay(handle);

  heapDestroy(handle,L7_FALSE);
  return;
}


void heapTestWithNegativeInputs(void)
{
  L7_uchar8*   buffer[10];
  heapHandle_t  handle=0;
  heapBuffPool_t blist[] = {{125,8,"125 buffers",0},{128,5,"150 Buffers",0},{0,0,0,0}};
  L7_uint32 i;

  if(handle == 0)
    handle = heapCreate(blist,100,0);

  if(handle == 0)
    handle = heapCreate(blist,HEAP_DEBUG_MEMORY_GUARD|HEAP_DEBUG_FILE_INFO,0);

  heapDbgPrintSet(handle,1);

  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,128,__FILE__,__LINE__);

  /* validate memory location */
  printf("validate - buffer[0] %d \n",heapBuffValidate(handle,buffer[0]));
  /* validate junk memory location */
  printf("validate - junk %d \n",heapBuffValidate(handle,(void*)(buffer[0]+1)));

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);


  for(i=0;i<8;i++)
    buffer[i] = heapAlloc(handle,93+i,__FILE__,__LINE__);

  heapDbgStatsDisplay(handle);

  for(i=0;i<8;i++)
    heapDbgBuffInfo(handle,buffer[i]);

  for(i=0;i<8;i++)
    heapFree(handle,buffer[i]);

  heapDbgStatsDisplay(handle);

  heapDestroy(handle,L7_FALSE);
  return;
}




/* Include this file from anywhere during the system startup. This will ensure
   that the UT as well as the heap files do get linked into the binary */
void heapUtInit(void)
{
  printf("\n\n HEAP UT module being inialized.\n");
  heapTestWithoutDebug();
  heapTestWithFileInfo();
  heapTestWithMemGuard();
  heapTestWithMemGuardAndFileInfo();
  heapTestWithNegativeInputs();
}

int main()
{
  heapUtInit();
  return 0;
}

