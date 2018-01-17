/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename       bitset.c
 *
 * @purpose        Bitset object implementation
 *
 * @component      Routing Utils Component
 *
 * @comments
 *
 * @create         12/25/2000
 *
 * @author         slipov
 *
 * @end
 *
 * ********************************************************************/

#include "std.h"
#include "xx.ext"
#include <string.h>
#include "bitset.ext"
#include "bitset.h"

#define BS_STAND_ALONE_DEBUG 0 /* Use 1 only for stand alone debugging !!! */
#if BS_STAND_ALONE_DEBUG
#include <malloc.h>
#include <stdio.h>
#define _XX_Free(p, file, line) free(p)
#define _XX_MallocExt(heap, size, t1, t2) malloc(size)
#define __assertfail(msg, cond, file, line) printf("Assert %s", msg)
#endif


static t_BitSet *allocBitSet(ulng size);        /* Allocates bit set objects and array of given size */
static void     deleteBitSet(t_BitSet *p);      /* Deletes bit set objects and internal array */
static void     copyBitSet(t_BitSet *p_src, t_BitSet *p_dest); /* Copies bit set */
static e_Err    getAfter( t_BitSet  *p_BS, ulng startIndex, byte value, ulng *p_index);

/* Lookup tables for find and count operations */
byte BS_Bit_count[] = {
  0, /*   0 */ 1, /*   1 */ 1, /*   2 */ 2, /*   3 */ 1, /*   4 */
  2, /*   5 */ 2, /*   6 */ 3, /*   7 */ 1, /*   8 */ 2, /*   9 */
  2, /*  10 */ 3, /*  11 */ 2, /*  12 */ 3, /*  13 */ 3, /*  14 */
  4, /*  15 */ 1, /*  16 */ 2, /*  17 */ 2, /*  18 */ 3, /*  19 */
  2, /*  20 */ 3, /*  21 */ 3, /*  22 */ 4, /*  23 */ 2, /*  24 */
  3, /*  25 */ 3, /*  26 */ 4, /*  27 */ 3, /*  28 */ 4, /*  29 */
  4, /*  30 */ 5, /*  31 */ 1, /*  32 */ 2, /*  33 */ 2, /*  34 */
  3, /*  35 */ 2, /*  36 */ 3, /*  37 */ 3, /*  38 */ 4, /*  39 */
  2, /*  40 */ 3, /*  41 */ 3, /*  42 */ 4, /*  43 */ 3, /*  44 */
  4, /*  45 */ 4, /*  46 */ 5, /*  47 */ 2, /*  48 */ 3, /*  49 */
  3, /*  50 */ 4, /*  51 */ 3, /*  52 */ 4, /*  53 */ 4, /*  54 */
  5, /*  55 */ 3, /*  56 */ 4, /*  57 */ 4, /*  58 */ 5, /*  59 */
  4, /*  60 */ 5, /*  61 */ 5, /*  62 */ 6, /*  63 */ 1, /*  64 */
  2, /*  65 */ 2, /*  66 */ 3, /*  67 */ 2, /*  68 */ 3, /*  69 */
  3, /*  70 */ 4, /*  71 */ 2, /*  72 */ 3, /*  73 */ 3, /*  74 */
  4, /*  75 */ 3, /*  76 */ 4, /*  77 */ 4, /*  78 */ 5, /*  79 */
  2, /*  80 */ 3, /*  81 */ 3, /*  82 */ 4, /*  83 */ 3, /*  84 */
  4, /*  85 */ 4, /*  86 */ 5, /*  87 */ 3, /*  88 */ 4, /*  89 */
  4, /*  90 */ 5, /*  91 */ 4, /*  92 */ 5, /*  93 */ 5, /*  94 */
  6, /*  95 */ 2, /*  96 */ 3, /*  97 */ 3, /*  98 */ 4, /*  99 */
  3, /* 100 */ 4, /* 101 */ 4, /* 102 */ 5, /* 103 */ 3, /* 104 */
  4, /* 105 */ 4, /* 106 */ 5, /* 107 */ 4, /* 108 */ 5, /* 109 */
  5, /* 110 */ 6, /* 111 */ 3, /* 112 */ 4, /* 113 */ 4, /* 114 */
  5, /* 115 */ 4, /* 116 */ 5, /* 117 */ 5, /* 118 */ 6, /* 119 */
  4, /* 120 */ 5, /* 121 */ 5, /* 122 */ 6, /* 123 */ 5, /* 124 */
  6, /* 125 */ 6, /* 126 */ 7, /* 127 */ 1, /* 128 */ 2, /* 129 */
  2, /* 130 */ 3, /* 131 */ 2, /* 132 */ 3, /* 133 */ 3, /* 134 */
  4, /* 135 */ 2, /* 136 */ 3, /* 137 */ 3, /* 138 */ 4, /* 139 */
  3, /* 140 */ 4, /* 141 */ 4, /* 142 */ 5, /* 143 */ 2, /* 144 */
  3, /* 145 */ 3, /* 146 */ 4, /* 147 */ 3, /* 148 */ 4, /* 149 */
  4, /* 150 */ 5, /* 151 */ 3, /* 152 */ 4, /* 153 */ 4, /* 154 */
  5, /* 155 */ 4, /* 156 */ 5, /* 157 */ 5, /* 158 */ 6, /* 159 */
  2, /* 160 */ 3, /* 161 */ 3, /* 162 */ 4, /* 163 */ 3, /* 164 */
  4, /* 165 */ 4, /* 166 */ 5, /* 167 */ 3, /* 168 */ 4, /* 169 */
  4, /* 170 */ 5, /* 171 */ 4, /* 172 */ 5, /* 173 */ 5, /* 174 */
  6, /* 175 */ 3, /* 176 */ 4, /* 177 */ 4, /* 178 */ 5, /* 179 */
  4, /* 180 */ 5, /* 181 */ 5, /* 182 */ 6, /* 183 */ 4, /* 184 */
  5, /* 185 */ 5, /* 186 */ 6, /* 187 */ 5, /* 188 */ 6, /* 189 */
  6, /* 190 */ 7, /* 191 */ 2, /* 192 */ 3, /* 193 */ 3, /* 194 */
  4, /* 195 */ 3, /* 196 */ 4, /* 197 */ 4, /* 198 */ 5, /* 199 */
  3, /* 200 */ 4, /* 201 */ 4, /* 202 */ 5, /* 203 */ 4, /* 204 */
  5, /* 205 */ 5, /* 206 */ 6, /* 207 */ 3, /* 208 */ 4, /* 209 */
  4, /* 210 */ 5, /* 211 */ 4, /* 212 */ 5, /* 213 */ 5, /* 214 */
  6, /* 215 */ 4, /* 216 */ 5, /* 217 */ 5, /* 218 */ 6, /* 219 */
  5, /* 220 */ 6, /* 221 */ 6, /* 222 */ 7, /* 223 */ 3, /* 224 */
  4, /* 225 */ 4, /* 226 */ 5, /* 227 */ 4, /* 228 */ 5, /* 229 */
  5, /* 230 */ 6, /* 231 */ 4, /* 232 */ 5, /* 233 */ 5, /* 234 */
  6, /* 235 */ 5, /* 236 */ 6, /* 237 */ 6, /* 238 */ 7, /* 239 */
  4, /* 240 */ 5, /* 241 */ 5, /* 242 */ 6, /* 243 */ 5, /* 244 */
  6, /* 245 */ 6, /* 246 */ 7, /* 247 */ 5, /* 248 */ 6, /* 249 */
  6, /* 250 */ 7, /* 251 */ 6, /* 252 */ 7, /* 253 */ 7, /* 254 */
  8  /* 255 */
}; /* end BS_Bit_count */

byte BS_First_one[] = {
  0, /*   0 */ 0, /*   1 */ 1, /*   2 */ 0, /*   3 */ 2, /*   4 */
  0, /*   5 */ 1, /*   6 */ 0, /*   7 */ 3, /*   8 */ 0, /*   9 */
  1, /*  10 */ 0, /*  11 */ 2, /*  12 */ 0, /*  13 */ 1, /*  14 */
  0, /*  15 */ 4, /*  16 */ 0, /*  17 */ 1, /*  18 */ 0, /*  19 */
  2, /*  20 */ 0, /*  21 */ 1, /*  22 */ 0, /*  23 */ 3, /*  24 */
  0, /*  25 */ 1, /*  26 */ 0, /*  27 */ 2, /*  28 */ 0, /*  29 */
  1, /*  30 */ 0, /*  31 */ 5, /*  32 */ 0, /*  33 */ 1, /*  34 */
  0, /*  35 */ 2, /*  36 */ 0, /*  37 */ 1, /*  38 */ 0, /*  39 */
  3, /*  40 */ 0, /*  41 */ 1, /*  42 */ 0, /*  43 */ 2, /*  44 */
  0, /*  45 */ 1, /*  46 */ 0, /*  47 */ 4, /*  48 */ 0, /*  49 */
  1, /*  50 */ 0, /*  51 */ 2, /*  52 */ 0, /*  53 */ 1, /*  54 */
  0, /*  55 */ 3, /*  56 */ 0, /*  57 */ 1, /*  58 */ 0, /*  59 */
  2, /*  60 */ 0, /*  61 */ 1, /*  62 */ 0, /*  63 */ 6, /*  64 */
  0, /*  65 */ 1, /*  66 */ 0, /*  67 */ 2, /*  68 */ 0, /*  69 */
  1, /*  70 */ 0, /*  71 */ 3, /*  72 */ 0, /*  73 */ 1, /*  74 */
  0, /*  75 */ 2, /*  76 */ 0, /*  77 */ 1, /*  78 */ 0, /*  79 */
  4, /*  80 */ 0, /*  81 */ 1, /*  82 */ 0, /*  83 */ 2, /*  84 */
  0, /*  85 */ 1, /*  86 */ 0, /*  87 */ 3, /*  88 */ 0, /*  89 */
  1, /*  90 */ 0, /*  91 */ 2, /*  92 */ 0, /*  93 */ 1, /*  94 */
  0, /*  95 */ 5, /*  96 */ 0, /*  97 */ 1, /*  98 */ 0, /*  99 */
  2, /* 100 */ 0, /* 101 */ 1, /* 102 */ 0, /* 103 */ 3, /* 104 */
  0, /* 105 */ 1, /* 106 */ 0, /* 107 */ 2, /* 108 */ 0, /* 109 */
  1, /* 110 */ 0, /* 111 */ 4, /* 112 */ 0, /* 113 */ 1, /* 114 */
  0, /* 115 */ 2, /* 116 */ 0, /* 117 */ 1, /* 118 */ 0, /* 119 */
  3, /* 120 */ 0, /* 121 */ 1, /* 122 */ 0, /* 123 */ 2, /* 124 */
  0, /* 125 */ 1, /* 126 */ 0, /* 127 */ 7, /* 128 */ 0, /* 129 */
  1, /* 130 */ 0, /* 131 */ 2, /* 132 */ 0, /* 133 */ 1, /* 134 */
  0, /* 135 */ 3, /* 136 */ 0, /* 137 */ 1, /* 138 */ 0, /* 139 */
  2, /* 140 */ 0, /* 141 */ 1, /* 142 */ 0, /* 143 */ 4, /* 144 */
  0, /* 145 */ 1, /* 146 */ 0, /* 147 */ 2, /* 148 */ 0, /* 149 */
  1, /* 150 */ 0, /* 151 */ 3, /* 152 */ 0, /* 153 */ 1, /* 154 */
  0, /* 155 */ 2, /* 156 */ 0, /* 157 */ 1, /* 158 */ 0, /* 159 */
  5, /* 160 */ 0, /* 161 */ 1, /* 162 */ 0, /* 163 */ 2, /* 164 */
  0, /* 165 */ 1, /* 166 */ 0, /* 167 */ 3, /* 168 */ 0, /* 169 */
  1, /* 170 */ 0, /* 171 */ 2, /* 172 */ 0, /* 173 */ 1, /* 174 */
  0, /* 175 */ 4, /* 176 */ 0, /* 177 */ 1, /* 178 */ 0, /* 179 */
  2, /* 180 */ 0, /* 181 */ 1, /* 182 */ 0, /* 183 */ 3, /* 184 */
  0, /* 185 */ 1, /* 186 */ 0, /* 187 */ 2, /* 188 */ 0, /* 189 */
  1, /* 190 */ 0, /* 191 */ 6, /* 192 */ 0, /* 193 */ 1, /* 194 */
  0, /* 195 */ 2, /* 196 */ 0, /* 197 */ 1, /* 198 */ 0, /* 199 */
  3, /* 200 */ 0, /* 201 */ 1, /* 202 */ 0, /* 203 */ 2, /* 204 */
  0, /* 205 */ 1, /* 206 */ 0, /* 207 */ 4, /* 208 */ 0, /* 209 */
  1, /* 210 */ 0, /* 211 */ 2, /* 212 */ 0, /* 213 */ 1, /* 214 */
  0, /* 215 */ 3, /* 216 */ 0, /* 217 */ 1, /* 218 */ 0, /* 219 */
  2, /* 220 */ 0, /* 221 */ 1, /* 222 */ 0, /* 223 */ 5, /* 224 */
  0, /* 225 */ 1, /* 226 */ 0, /* 227 */ 2, /* 228 */ 0, /* 229 */
  1, /* 230 */ 0, /* 231 */ 3, /* 232 */ 0, /* 233 */ 1, /* 234 */
  0, /* 235 */ 2, /* 236 */ 0, /* 237 */ 1, /* 238 */ 0, /* 239 */
  4, /* 240 */ 0, /* 241 */ 1, /* 242 */ 0, /* 243 */ 2, /* 244 */
  0, /* 245 */ 1, /* 246 */ 0, /* 247 */ 3, /* 248 */ 0, /* 249 */
  1, /* 250 */ 0, /* 251 */ 2, /* 252 */ 0, /* 253 */ 1, /* 254 */
  0, /* 255 */
}; /* end BS_First_one */




/*********************************************************************
 * @purpose           Creates a bit set object of a given size.
 *
 *
 * @param  size       @b{(input)}  bitset size in bits
 * @param  flags      @b{(input)}  Bitset flags
 * @param  p_bitset   @b{(input)}  pointer to the created object is returned
 *                                 here
 *
 * @returns           e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_Init( IN   ulng      size,
               IN   ulng      flags,
               OUT  t_Handle  *p_bitset)
{
   ASSERT(p_bitset);
   if(size < 1)
      return E_BADPARM;

   *p_bitset = allocBitSet(size);
   if(*p_bitset == NULLP)
      return E_NOMEMORY;
   ((t_BitSet*)(*p_bitset))->flags = flags;
   return E_OK;
}



/*********************************************************************
 * @purpose           Initializes a bit set object from an array of a
 *                    given size.
 *
 *
 * @param bsHandle     @b{(input)}  bitset handle
 * @param p_array      @b{(input)}  pointer to array of long
 * @param arSize       @b{(input)}  size of array in longs
 *
 * @returns            e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_From_Array( IN   t_Handle  bsHandle,
                     IN   ulng      *p_array,
                     IN   ulng      arSize)
{
   ulng longs, len, i, j;
   t_BitSet *p = (t_BitSet*)bsHandle;
   ASSERT(p && p->status == XX_STAT_Valid);
   ASSERT(p_array);
   /* Choose the shortest length */
   longs = BS_GET_LONG_INDEX(p->size);
   len = (longs > arSize) ? arSize : longs;
   memcpy(p->p_array, p_array, len<<2);  /* Copy array to the bitset */

   if(p->flags & BS_IS_ALLOC)
   {
      /* This bitset used as an allocator. Update the variables */
      p->flags &= ~BS_INDEX_0_VALID;        /* Clear the cache */
      p->num_0 = 0;                         /* Recount num_0 */
      for(i = 0; i < longs; i++)
      {
         byte *p_byte = (byte*)&p->p_array[i];
         for(j = 0; j < BYTES_IN_LONG; j++)
         {
            p->num_0 += BITS_IN_BYTE - BS_Bit_count[p_byte[j]];
         }
      }
   }
   return E_OK;
}



/*********************************************************************
 * @purpose         Creates a bit set object cloned from another bit set
 *                  object
 *
 *
 * @param src       @b{(input)}  original bit set to be cloned
 * @param p_bitset  @b{(input)}  pointer to the created object is returned here
 *
 * @returns         e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_Clone(IN   t_Handle  src,
               OUT  t_Handle  *p_bitset)
{
   t_BitSet *p = (t_BitSet*)src;
   ASSERT(p && p->status == XX_STAT_Valid);
   ASSERT(p_bitset);
   *p_bitset = allocBitSet(p->size);
   if(*p_bitset == NULLP)
      return E_NOMEMORY;
   copyBitSet(p, (t_BitSet*)(*p_bitset));
   return E_OK;
}




/*********************************************************************
 * @purpose          Copies a bit set object from another bit set object
 *
 *
 * @param src        @b{(input)}  original bit set to be copied
 * @param dest       @b{(input)}  destination bit set
 *
 * @returns          e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_Copy( IN  t_Handle  src,
               IN  t_Handle  dest)
{
   t_BitSet *p_src  = (t_BitSet*)src;
   t_BitSet *p_dest = (t_BitSet*)dest;
   ASSERT(p_src && (p_src->status == XX_STAT_Valid));
   ASSERT(p_dest && (p_dest->status == XX_STAT_Valid));
   if(p_src->size != p_dest->size)
      return E_BADPARM;
   copyBitSet(p_src, p_dest);
   return E_OK;
}


/*********************************************************************
 * @purpose             Destroys a bit set object.
 *
 *
 * @param  bitset       @b{(input)}  bitset to be destoyed
 *
 * @returns             e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_Destroy( IN t_Handle  bitset)
{
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   deleteBitSet(p);
   return E_OK;
}


/*********************************************************************
 * @purpose            Returns the value of the bit at index location.
 *
 *
 * @param bitset       @b{(input)}  bitset handle
 * @param index        @b{(input)}  tested bit index
 * @param p_value      @b{(input)}  bit value is returned here (least signi
 *                                  ficant bit of the byte)
 *
 * @returns             e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_GetBit( IN  t_Handle  bitset,
                 IN  ulng      index,
                 OUT byte      *p_value)
{
   ulng tmp;
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   ASSERT(p_value);
   if(index >= p->size)
      return E_BADPARM;
   tmp = p->p_array[BS_GET_LONG_INDEX(index)];
   *p_value = ((byte)(tmp >> BS_GET_LONG_REM(index))) & 0x01;
   return E_OK;
}


/*********************************************************************
 * @purpose           Sets the value of the bit at index location
 *
 *
 * @param  bitset      @b{(input)}  bitset handle
 * @param  index       @b{(input)}  tested bit index
 * @param  value       @b{(input)}  bit value to set (least significant
 *                                               bit of the byte)
 *
 * @returns           e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_SetBit( IN  t_Handle  bitset,
                 IN  ulng      index,
                 IN  byte      value)
{
   ulng tmp;
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   if(index >= p->size)
      return E_BADPARM;
   tmp = 1 << BS_GET_LONG_REM(index);
   if(value & 0x01)
   {
      if((p->p_array[BS_GET_LONG_INDEX(index)] & tmp) == 0)
         p->num_0--;
      p->p_array[BS_GET_LONG_INDEX(index)] |= tmp;
      if(p->index_0 == index)
         p->flags &= ~BS_INDEX_0_VALID; /* Clear the cache */
   }
   else
   {
      if((p->p_array[BS_GET_LONG_INDEX(index)] & tmp) != 0)
         p->num_0++;
      p->p_array[BS_GET_LONG_INDEX(index)] &= ~tmp;
      /* Add to the cache */
      p->index_0 = index;
      p->flags |= BS_INDEX_0_VALID;
   }
   return E_OK;
}



/*********************************************************************
 * @purpose            Sets the entire bit set to the given value.
 *
 *
 * @param bitset       @b{(input)}  bitset handle
 * @param value        @b{(input)}  bit value to set (least significant
 *                                                       bit of the byte)
 *
 * @returns            e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_SetAll( IN  t_Handle  bitset,
                 IN  byte      value)
{
   byte tmp;
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   if(value & 0x01)
   {
      tmp = ALL_ONES_IN_BYTE;
      p->num_0 = 0;
      p->flags &= ~BS_INDEX_0_VALID; /* Clear the cache */
   }
   else
   {
      tmp = 0;
      p->num_0 = p->size;
      /* Init the cache */
      p->index_0 = 0;
      p->flags |= BS_INDEX_0_VALID;
   }
   memset(p->p_array, tmp, BS_GET_BYTE_SIZE(p->size));
   return E_OK;
}



/*********************************************************************
 * @purpose            Flips the value of the bit at index location.
 *
 *
 * @param bitset       @b{(input)}  bitset handle
 * @param index        @b{(input)}  tested bit index
 *
 * @returns            e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_FlipBit( IN  t_Handle  bitset,
                  IN  ulng      index)
{
   ulng tmp;
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   if(index >= p->size)
      return E_BADPARM;
   tmp = 1 << BS_GET_LONG_REM(index);
   if(p->p_array[BS_GET_LONG_INDEX(index)] & tmp)
   {
      p->num_0++;
      p->p_array[BS_GET_LONG_INDEX(index)] &= ~tmp;
   }
   else
   {
      p->num_0--;
      p->p_array[BS_GET_LONG_INDEX(index)] |= tmp;
   }
   if(p->index_0 == index)
      p->flags &= ~BS_INDEX_0_VALID; /* Clear the cache */
   return E_OK;
}


/*********************************************************************
 * @purpose             Flips the entire bit set.
 *
 *
 * @param  bitset       @b{(input)}  bitset handle
 *
 * @returns             e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_FlipAll( IN  t_Handle  bitset)
{
   ulng size, i;
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   p->num_0 = p->size - p->num_0;
   size = BS_GET_LONG_INDEX(p->size - 1) + 1;
   for(i=0;i < size;i++)
      p->p_array[i] ^= ALL_ONES_IN_LONG;
   p->flags &= ~BS_INDEX_0_VALID; /* Clear the cache */
   return E_OK;
}



/*********************************************************************
 * @purpose            AND operation over the entire bit set, i.e.
 *                     bitset L&= bitsetR.
 *
 *
 * @param  bitsetL     @b{(input)}  left hand bitset handle, source and
 *                                  result.
 * @param  bitsetR     @b{(input)}   rigth hand bitset
 *
 * @returns            e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_AndAll( IN OUT  t_Handle  bitsetL,
                 IN      t_Handle  bitsetR)
{
   ulng i, j, full_longs, long_rem, tmp, num_0 = 0;
   t_BitSet *p_L = (t_BitSet*)bitsetL;
   t_BitSet *p_R = (t_BitSet*)bitsetR;
   ASSERT(p_L && (p_L->status == XX_STAT_Valid));
   ASSERT(p_R && (p_R->status == XX_STAT_Valid));
   if(p_L == p_R)
      return E_OK;
   if(p_L->size != p_R->size)
      return E_BADPARM;
   if(p_L->flags & BS_IS_ALLOC)
   {
      full_longs = BS_GET_LONG_INDEX(p_L->size);
      for(i=0;i < full_longs; i++)
      {
         /* Handle all the full longs  */
         byte *p = (byte*)&p_L->p_array[i];
         p_L->p_array[i] &= p_R->p_array[i];
         for(j = 0; j < BYTES_IN_LONG; j++)
         {
            num_0 += BITS_IN_BYTE - BS_Bit_count[p[j]];
         }
      }
      /* Handle the partial last long */
      long_rem = BS_GET_LONG_REM(p_L->size);
      if(long_rem)
      {
         p_L->p_array[i] &= p_R->p_array[i];
         tmp = p_L->p_array[i];
         for(; long_rem; long_rem--)
         {
            if(!(tmp & 1))
               num_0++;
            tmp >>= 1;
         }
      }
      p_L->num_0 = num_0;
   }
   else
   {
      full_longs = BS_GET_LONG_INDEX(p_L->size - 1) + 1;
      for(i=0;i < full_longs; i++)
      {
         p_L->p_array[i] &= p_R->p_array[i];
      }
   }
   return E_OK;
}



/*********************************************************************
 * @purpose           OR operation over the entire bit set, i.e. bitsetL
 *                    |= bitsetR
 *
 *
 * @param  bitsetL    @b{(input)}  left hand bitset handle, source and
 *                                 result.
 * @param  bitsetR    @b{(input)}  rigth hand bitset
 *
 * @returns           e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_OrAll( IN OUT  t_Handle  bitsetL,
                IN      t_Handle  bitsetR)
{
   ulng i, j, full_longs, long_rem, tmp, num_0 = 0;
   t_BitSet *p_L = (t_BitSet*)bitsetL;
   t_BitSet *p_R = (t_BitSet*)bitsetR;
   ASSERT(p_L && (p_L->status == XX_STAT_Valid));
   ASSERT(p_R && (p_R->status == XX_STAT_Valid));
   if(p_L == p_R)
      return E_OK;
   if(p_L->size != p_R->size)
      return E_BADPARM;
   if(p_L->flags & BS_IS_ALLOC)
   {
      full_longs = BS_GET_LONG_INDEX(p_L->size);
      for(i=0;i < full_longs; i++)
      {
         /* Handle all the full longs  */
         byte *p = (byte*)&p_L->p_array[i];
         p_L->p_array[i] |= p_R->p_array[i];
         for(j = 0; j < BYTES_IN_LONG; j++)
         {
            num_0 += BITS_IN_BYTE - BS_Bit_count[p[j]];
         }
      }
      /* Handle the partial last long */
      long_rem = BS_GET_LONG_REM(p_L->size);
      if(long_rem)
      {
         p_L->p_array[i] |= p_R->p_array[i];
         tmp = p_L->p_array[i];
         for(; long_rem; long_rem--)
         {
            if(!(tmp & 1))
               num_0++;
            tmp >>= 1;
         }
      }
      p_L->num_0 = num_0;
      p_L->flags &= ~BS_INDEX_0_VALID; /* Clear the cache */
   }
   else
   {
      full_longs = BS_GET_LONG_INDEX(p_L->size - 1) + 1;
      for(i=0;i < full_longs; i++)
      {
         p_L->p_array[i] |= p_R->p_array[i];
      }
   }
   return E_OK;
}



/*********************************************************************
 * @purpose         XOR operation over the entire bit set, i.e. bitsetL
 *                  ^= bitsetR.
 *
 *
 * @param bitsetL        @b{(input)}  left hand bitset handle, source
 *                                    and result.
 * @param bitsetR        @b{(input)}  rigth hand bitset
 *
 * @returns              e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_XorAll( IN OUT  t_Handle  bitsetL,
                 IN      t_Handle  bitsetR)
{
   ulng i, j, full_longs, long_rem, tmp, num_0 = 0;
   t_BitSet *p_L = (t_BitSet*)bitsetL;
   t_BitSet *p_R = (t_BitSet*)bitsetR;
   ASSERT(p_L && (p_L->status == XX_STAT_Valid));
   ASSERT(p_R && (p_R->status == XX_STAT_Valid));
   if(p_L == p_R)
   {
      BS_FlipAll(bitsetL);
      return E_OK;
   }
   if(p_L->size != p_R->size)
      return E_BADPARM;
   if(p_L->flags & BS_IS_ALLOC)
   {
      full_longs = BS_GET_LONG_INDEX(p_L->size);
      for(i=0;i < full_longs; i++)
      {
         /* Handle all the full longs  */
         byte *p = (byte*)&p_L->p_array[i];
         p_L->p_array[i] ^= p_R->p_array[i];
         for(j = 0; j < BYTES_IN_LONG; j++)
         {
            num_0 += BITS_IN_BYTE - BS_Bit_count[p[j]];
         }
      }
      /* Handle the partial last long */
      long_rem = BS_GET_LONG_REM(p_L->size);
      if(long_rem)
      {
         p_L->p_array[i] ^= p_R->p_array[i];
         tmp = p_L->p_array[i];
         for(; long_rem; long_rem--)
         {
            if(!(tmp & 1))
               num_0++;
            tmp >>= 1;
         }
      }
      p_L->num_0 = num_0;
      p_L->flags &= ~BS_INDEX_0_VALID; /* Clear the cache */
   }
   else
   {
      full_longs = BS_GET_LONG_INDEX(p_L->size - 1) + 1;
      for(i=0;i < full_longs; i++)
      {
         p_L->p_array[i] ^= p_R->p_array[i];
      }
   }
   return E_OK;
}


/*********************************************************************
 * @purpose             Returns bit set object size.
 *
 *
 * @param  bitset       @b{(input)}  bitset handle
 * @param  p_size       @b{(input)}  bitmap size in bits is returned here
 *
 * @returns             e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_GetSize( IN  t_Handle  bitset,
                  OUT ulng      *p_size)
{
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   ASSERT(p_size);
   *p_size = p->size;
   return E_OK;
}



/*********************************************************************
 * @purpose           Returns index of the first element set to a given
 *                    value (0 or 1).
 *
 *
 * @param bitset       @b{(input)}  bitset handle
 * @param value        @b{(input)}  bit value to find (least significant
 *                                                       cut of the byte)
 * @param p_index      @b{(input)}  index of the first set element is
 *                                    returned here
 *
 * @returns             e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_GetFirst( IN  t_Handle  bitset,
                   IN  byte      value,
                   OUT ulng      *p_index)
{
   t_BitSet *p_BS = (t_BitSet*)bitset;
   ASSERT(p_BS && (p_BS->status == XX_STAT_Valid));
   ASSERT(p_index);
   if(!(p_BS->flags & BS_IS_ALLOC))
      return E_FAILED;
   if(value & 0x01)
   {
      if(p_BS->num_0 == p_BS->size)
         return E_NOT_FOUND;
   }
   else
   {
      if(p_BS->num_0 == 0)
         return E_NOT_FOUND;
   }
   return getAfter( p_BS, 0, value, p_index);
}



/*********************************************************************
 * @purpose           Returns index of the first element set to a given
 *                    value (0 or 1) after the given previous element.
 *
 *
 * @param  bitset     @b{(input)}  bitset handle
 * @param  prevIndex  @b{(input)}  index of the previous found element
 * @param  value      @b{(input)}  bit value to find (least significant bit
 *                                                         of the byte)
 * @param  p_index    @b{(input)}  index of the next set element is returned
 *                                 here
 *
 * @returns           e_Err
 * @returns
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_GetNext(  IN  t_Handle  bitset,
                   IN  ulng      prevIndex,
                   IN  byte      value,
                   OUT ulng      *p_index)
{
   t_BitSet *p_BS = (t_BitSet*)bitset;
   ASSERT(p_BS && (p_BS->status == XX_STAT_Valid));
   ASSERT(p_index);
   if(!(p_BS->flags & BS_IS_ALLOC))
      return E_FAILED;
   prevIndex++; /* The prevIndex element should not be checked */
   if(prevIndex >= p_BS->size)
      return E_BADPARM;
   if(value & 0x01)
   {
      if(p_BS->num_0 == p_BS->size)
         return E_NOT_FOUND;
   }
   else
   {
      if(p_BS->num_0 == 0)
         return E_NOT_FOUND;
   }
   return getAfter( p_BS, prevIndex, value, p_index);
}



/*********************************************************************
 * @purpose           Returns index of any element set to a given value
 *                    (0 or 1).
 *
 *
 * @param bitset       @b{(input)}  bitset handle
 * @param value        @b{(input)}  bit value to find (least significant
 *                                                       bit of the byte)
 * @param p_index      @b{(input)}  index of the first set element is
 *                                    returned here
 *
 * @returns            e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_GetAny( IN  t_Handle  bitset,
                 IN  byte      value,
                 OUT ulng      *p_index)
{
   t_BitSet *p_BS = (t_BitSet*)bitset;
   ASSERT(p_BS && (p_BS->status == XX_STAT_Valid));
   ASSERT(p_index);
   if(!(p_BS->flags & BS_IS_ALLOC))
      return E_FAILED;
   if(((value & 0x01) == 0) && (p_BS->flags & BS_INDEX_0_VALID))
   {
      /* Return from the cache */
      *p_index = p_BS->index_0;
      return E_OK;
   }
   else
   {
      if(value & 0x01)
      {
         if(p_BS->num_0 == p_BS->size)
            return E_NOT_FOUND;
      }
      else
      {
         if(p_BS->num_0 == 0)
            return E_NOT_FOUND;
      }
      return getAfter( p_BS, 0, value, p_index);
   }
}



/*********************************************************************
 * @purpose          Returns index of the first element in the sequence
 *                   of elements set to a given value (0 or 1).
 *
 *
 * @param bitset     @b{(input)}  bitset handle
 * @param value      @b{(input)}  bit value to find (least significant bit
 *                                                         of the byte)
 * @param length     @b{(input)}  length of the sequence
 * @param p_index    @b{(input)}  index of the first set element is returned
 *                                here
 *
 * @returns          e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_GetSeq( IN  t_Handle  bitset,
                 IN  byte      value,
                 IN  ulng      length,
                 OUT ulng      *p_index)
{
   ulng startIndex = 0, prevIndex, foundIndex = 0;
   t_BitSet *p_BS = (t_BitSet*)bitset;
   ASSERT(p_BS && (p_BS->status == XX_STAT_Valid));
   ASSERT(p_index);
   if(!(p_BS->flags & BS_IS_ALLOC))
      return E_FAILED;
   if(length == 0)
      return E_BADPARM;
   if(value & 0x01)
   {
      if((p_BS->size - p_BS->num_0) < length)
         return E_NOT_FOUND;
   }
   else
   {
      if(p_BS->num_0 < length)
         return E_NOT_FOUND;
   }
   if(length == 1)
      return getAfter( p_BS, 0, value, p_index);

   /* length is greater than 1 if we are here */
   if(getAfter( p_BS, 0, value, &startIndex) != E_OK)
      return E_NOT_FOUND;
   
   for(prevIndex = startIndex + 1; prevIndex < p_BS->size; prevIndex = foundIndex + 1)
   {
      if(getAfter( p_BS, prevIndex, value, &foundIndex) == E_OK)
      {
         if(foundIndex != prevIndex)              
            startIndex = foundIndex;
         else
         {
            if((foundIndex - startIndex + 1) >= length)
            {
               /* The sequence is found! */
               *p_index = startIndex;
               return E_OK;
            }
         }
      }
      else
         return E_NOT_FOUND;

   }
   /* The end of the bitset is reached */
   return E_NOT_FOUND;
}



/*********************************************************************
 * @purpose            Returns number of elements having a given value.
 *
 *
 * @param bitset       @b{(input)}  bitset handle
 * @param value        @b{(input)}  0 or 1
 * @param p_num        @b{(input)}  number of 1s is returned here
 *
 * @returns            e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_NumSet(IN  t_Handle  bitset,
                IN  byte      value,
                OUT ulng      *p_num)
{
   t_BitSet *p = (t_BitSet*)bitset;
   ASSERT(p && (p->status == XX_STAT_Valid));
   ASSERT(p_num);
   if(!(p->flags & BS_IS_ALLOC))
      return E_FAILED;
   if(value & 0x01)
      *p_num = p->size - p->num_0;
   else
      *p_num = p->num_0;
   return E_OK;
}


/*********************************************************************
 * @purpose              Copies the bitset contents to the given array
 *                       of longs.
 *
 *
 * @param bsHandle      @b{(input)}  bitset handle
 * @param p_array       @b{(input)}  pointer to array of longs to copy
 *                                   the bitset
 * @param arSize        @b{(input)}  size of array in longs to fill
 *
 * @returns             e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BS_To_Array( IN   t_Handle  bsHandle,
                   IN   ulng      *p_array,
                   IN   ulng      arSize)
{
   ulng longs, len;
   t_BitSet *p = (t_BitSet*)bsHandle;
   ASSERT(p && p->status == XX_STAT_Valid);
   ASSERT(p_array);
   /* Choose the shortest length */
   longs = BS_GET_LONG_INDEX(p->size - 1) + 1;
   len = (longs > arSize) ? arSize : longs;
   memcpy(p_array, p->p_array, len<<2);  /* Copy bitset to the array */
   return E_OK;
}

/************************** STATIC FUNCTIONS **************************/

/* Allocates bit set objects and array of given size */
static t_BitSet *allocBitSet(ulng size)
{ 
   t_BitSet *p_bitset = XX_Malloc(sizeof(t_BitSet));
   if(p_bitset != NULLP)
   {
      memset(p_bitset, 0, sizeof(t_BitSet));
      /* Allocate space for possible reallignment */
      p_bitset->p_alloc = XX_Malloc(BS_GET_BYTE_SIZE(size) + BYTES_IN_LONG - 1);
      if(p_bitset->p_alloc == NULLP)
      {
         XX_Free(p_bitset);
         p_bitset = NULLP;
      }
      else
      {
         if(((ulng)(p_bitset->p_alloc)) % BYTES_IN_LONG)
         {
            p_bitset->p_array = (ulng*)((byte*)(p_bitset->p_alloc) + 
                                        BYTES_IN_LONG - ((ulng)(p_bitset->p_alloc)) % BYTES_IN_LONG);
         }
         else
         {
            p_bitset->p_array = (ulng*)(p_bitset->p_alloc);
         }
         memset(p_bitset->p_array, 0, BS_GET_BYTE_SIZE(size));
         p_bitset->size = size;
         p_bitset->num_0 = size;
#ifdef ERRCHK
         p_bitset->status = XX_STAT_Valid;
#endif

      }
   }
   return p_bitset;
}

/* Deletes bit set objects and internal array */
static void deleteBitSet(t_BitSet *p)
{
   if(p)
   {
#ifdef ERRCHK
      p->status = XX_STAT_Invalid;
#endif
      if(p->p_alloc)
         XX_Free(p->p_alloc);
      XX_Free(p);
   }
}

/* Copies bit set from p_src to p_dest */
static void copyBitSet(t_BitSet *p_src, t_BitSet *p_dest)
{
   /* Size should be the same */
   ASSERT(p_src->size == p_dest->size);
   if(p_src == p_dest)
      return;
   memcpy(p_dest->p_array, p_src->p_array, BS_GET_BYTE_SIZE(p_src->size));
   p_dest->num_0 = p_src->num_0;
   p_dest->index_0 = p_src->index_0;
   p_dest->flags = p_src->flags;
}


/*********************************************************************
 * @purpose            Returns index of the first element set to a given
 *                     value (0 or 1).
 *
 *
 * @param  p_BS         @b{(input)}  pointer to a bitset object
 * @param  startIndex   @b{(input)}  index to start the search from
 * @param  value        @b{(input)}  bit value to find (least significant
 *                                                       bit of the byte)
 * @param  p_index      @b{(input)}  index of the first set element is
 *                                   returned here
 *
 * @returns             e_Err
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err getAfter( IN  t_BitSet  *p_BS,
                       IN  ulng      startIndex,
                       IN  byte      value,
                       OUT ulng      *p_index)
{
   ulng i=0, j, full_longs, long_rem, tmp;

   /* Handle the partial first long */
   long_rem = BS_GET_LONG_REM(startIndex);
   if(long_rem)
   {
      ulng len;
      i = BS_GET_LONG_INDEX(startIndex);
      len = p_BS->size - i * BITS_IN_LONG;
      len = (len < BITS_IN_LONG) ? len : BITS_IN_LONG; 
      tmp = p_BS->p_array[i];
      tmp >>= long_rem;
      if(value & 0x01)
      {
         for(j=long_rem; j<len; j++)
         {
            if(tmp & 1)
            {
               *p_index = i * BITS_IN_LONG + j;
               return E_OK;
            }
            tmp >>= 1;
         }
      }
      else
      {
         for(j=long_rem; j<len; j++)
         {
            if(!(tmp & 1))
            {
               *p_index = i * BITS_IN_LONG + j;
               return E_OK;
            }
            tmp >>= 1;
         }
      }
      if(len < BITS_IN_LONG)
         return E_NOT_FOUND;
      i++;
   }
   else
   {
      i = BS_GET_LONG_INDEX(startIndex);
   }

   full_longs = BS_GET_LONG_INDEX(p_BS->size);
   if(value & 0x01)
   {
      for(;i < full_longs; i++)
      {
         /* Handle all the full longs  */
         byte *p = (byte*)&p_BS->p_array[i];
         if(p_BS->p_array[i] != 0)
         {
            for(j = 0; j < BYTES_IN_LONG; j++)
            {
               if(p[j] != 0)
               {
                  *p_index = i * BITS_IN_LONG + j * BITS_IN_BYTE + BS_First_one[p[j]];
                  return E_OK;
               }
            }
         }
      }
   }
   else
   {
      for(;i < full_longs; i++)
      {
         /* Handle all the full longs  */
         byte *p = (byte*)&p_BS->p_array[i];
         if(p_BS->p_array[i] != ALL_ONES_IN_LONG)
         {
            for(j = 0; j < BYTES_IN_LONG; j++)
            {
               if(p[j] != ALL_ONES_IN_BYTE)
               {
                  *p_index = i * BITS_IN_LONG + j * BITS_IN_BYTE + BS_First_one[p[j] + 1]; /* Lookup first 0 */
                  return E_OK;
               }
            }
         }
      }
   }

   /* Handle the partial last long */
   long_rem = BS_GET_LONG_REM(p_BS->size);
   if(long_rem)
   {
      tmp = p_BS->p_array[i];
      if(value & 0x01)
      {
         for(j=0; j<long_rem; j++)
         {
            if(tmp & 1)
            {
               *p_index = i * BITS_IN_LONG + j;
               return E_OK;
            }
            tmp >>= 1;
         }
      }
      else
      {
         for(j=0; j<long_rem; j++)
         {
            if(!(tmp & 1))
            {
               *p_index = i * BITS_IN_LONG + j;
               return E_OK;
            }
            tmp >>= 1;
         }
      }
   }
   return E_NOT_FOUND;
}
