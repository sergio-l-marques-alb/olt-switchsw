/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib_validate.c
*
* @purpose Utility functions for validations
*
* @component  XLIB
*
* @comments
*
* @create 04/17/2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

/*! \file */

#include <string.h>
#include<stdio.h>
#include<stdlib.h>
#include "xlib_regex.h"
#include "xlib_private.h"
/*#define RE_BACKSLASH_ESCAPE_IN_LISTS (1) */
#define RE_CHAR_CLASSES (RE_BK_PLUS_QM << 1)

xLibVal_t stk;

/*! \fn     xLibRC_t xLibCheckIntRange (void *arg, xLibU32_t min, xLibU32_t max)
 *  \brief  Validates buffer context for integer min and max
 *  \param  min  minimum value
 *  \param  max  maximum value
*/
xLibRC_t xLibCheckIntRange (void *arg, xLibU32_t min, xLibU32_t max)
{
  return XLIBRC_SUCCESS;
}

void xLibValInfoSet (xLibVal_t * val, xLibS8_t * output, xLibU32_t level, xLibU32_t type,
                     xLibU32_t parentType, xLibU32_t error)
{
  val->output = output;
  val->level = level;
  val->type = type;
  val->parentType = parentType;
  val->error = error;
}

xLibRC_t xLibValidateTravList (xLibVal_t val[], xLibU32_t size, xLibS8_t * value)
{
  regex_t preg;
  xLibS8_t *regex;
  xLibU32_t result;
  xLibU32_t count = 0, index = 0, stkSize = 0;
  xLibU32_t opCount = 0;
  xLibU32_t i, k, j;
  xLibVal_t *stk = NULL;
  xLibVal_t *temp = NULL;
  xLibVal_t *opStk = NULL; 
  if(size !=0)
  {
     opStk = (xLibVal_t *) xLibMalloc(sizeof(xLibVal_t) * size);
     stk = (xLibVal_t *) xLibMalloc(sizeof(xLibVal_t) * size);
     temp = (xLibVal_t *) xLibMalloc(sizeof(xLibVal_t) * size);
  }
  else
  {
     return XLIBRC_FAILURE;
  }
 

  memset (&preg, 0, sizeof (regex_t));

  for (i = 0; i < size; i++)
  {
    if (val[i].type == XLIB_AND)
    {
      memcpy (&stk[stkSize], &val[i], sizeof (xLibVal_t));
      stkSize++;
      continue;
    }
    /* <or> */
    else if (val[i].type == XLIB_OR)
    {
      memcpy (&stk[stkSize], &val[i], sizeof (xLibVal_t));
      stkSize++;
      continue;
    }
    /* <regexpand >  */
    else if (val[i].type == XLIB_REG_AND)
    {
      regex = (xLibS8_t *) xLibMalloc (strlen (val[i].output) * sizeof (xLibS8_t));
      strcpy (regex, val[i].output);
      regcomp (&preg, regex, REG_EXTENDED);
      result = regexec (&preg, value, 0, 0, 0);
      l7_regfree(&preg);
      memset (&preg, 0, sizeof (regex_t));
      xLibFree(regex);
			
      if (result)
      {
        /* for regexpand find the parent <and> and set the error there */
        /* error shall already be set */
        val[i].rc = XLIBRC_FAILURE;
        memcpy (&stk[stkSize], &val[i], sizeof (xLibVal_t));
        stkSize++;
      }
      else
      {
        val[i].rc = XLIBRC_SUCCESS;
        memcpy (&stk[stkSize], &val[i], sizeof (xLibVal_t));
        stkSize++;
      }
      
    }
    /* <regexpor> */
    else if (val[i].type == XLIB_REG_OR)
    {
      regex = (xLibS8_t *) xLibMalloc (strlen (val[i].output) * sizeof (xLibS8_t));
      strcpy (regex, val[i].output);
      regcomp (&preg, regex, REG_EXTENDED);
      result = regexec (&preg, value, 0, 0, 0);
      l7_regfree(&preg);
      memset (&preg, 0, sizeof (regex_t));
      xLibFree(regex);
      /*check the result for all the regexpOr in block and proceed */
      if (result)
      {
        /* error shall be set at corresponding OR block */
        val[i].rc = XLIBRC_FAILURE;
        memcpy (&stk[stkSize], &val[i], sizeof (xLibVal_t));
        stkSize++;
      }
      else
      {
        val[i].rc = XLIBRC_SUCCESS;
        memcpy (&stk[stkSize], &val[i], sizeof (xLibVal_t));
        stkSize++;
      }
      
    }
    count = 0;
    k = 0;
    if ((val[i].type == XLIB_CLOSE_OR) || (val[i].type == XLIB_CLOSE_AND))
    {
      index = 0;
      for (j = stkSize - 1; j >= 0; j--)
      {
        memcpy (&temp[index], &stk[j], sizeof (xLibVal_t));
        if ((temp[index].type == XLIB_AND) || (temp[index].type == XLIB_OR))
        {
          stkSize = j;
          count = index;
          break;
        }
        index++;
      }
      if (temp[count].type == XLIB_AND)
      {
        /* start thr process for regular expressions (avoid checking the and node) */        
        for (k = count-1; k >=0; k--)
        {
          /* checking elements in <and> structure */
          if (temp[k].rc == XLIBRC_FAILURE)
          {
            temp[count].rc = XLIBRC_FAILURE;
            temp[count].error = temp[k].error;
            break;
          }
          temp[count].rc = XLIBRC_SUCCESS;
          if(k ==0)
          break;
        }
        memcpy (&opStk[opCount], &temp[count], sizeof (xLibVal_t));
      }
      else if (temp[count].type == XLIB_OR)
      {
        /* for <or> check rc of all the regular expression and then pick up from one( avoid checking or group ) */
        for (k = 0; k < count; k++)
        {
          /* checking each <or> structure */
          if (temp[k].rc == XLIBRC_SUCCESS)
          {
            /* remove the error from or block */
            temp[count].rc = XLIBRC_SUCCESS;
            break;
          }
          /* Or Block has error set already */
          temp[count].rc = XLIBRC_FAILURE;
        }
        memcpy (&opStk[opCount], &temp[count], sizeof (xLibVal_t));
      }
      opCount++;
    }
  }
  xLibFree(stk);
  xLibFree(temp);
  return xLibFinalValidate (opStk, opCount);
}

xLibRC_t xLibFinalValidate (xLibVal_t opStk[], xLibU32_t opCount)
{
  xLibU32_t travCount = opCount;
  xLibU32_t i = 0, k = 0, j = 0;
  xLibU32_t root = 0;
  xLibU32_t level;
  xLibU32_t peerCount = 0;
  xLibU32_t parCount = 0;
  xLibVal_t *peers = NULL;
  xLibVal_t *parents = NULL;
  xLibU32_t error =0;

  if(opCount != 0)
  {
    peers  = (xLibVal_t *) xLibMalloc(sizeof(xLibVal_t) * opCount);
    parents  = (xLibVal_t *) xLibMalloc(sizeof(xLibVal_t) * opCount);

  }
  else
  {
     return XLIBRC_FAILURE;
  }

  while (i < travCount)
  {
    level = opStk[i].level;
    k++;
    if (k < travCount)
    {
      /* Same level : Nodes are peers store the nodes sequentially in a peerStack */
      if (level == opStk[k].level)
      {
        memcpy (&peers[peerCount++], &opStk[i], sizeof (xLibVal_t));
        while (level == opStk[k].level)
        {
          memcpy (&peers[peerCount], &opStk[k], sizeof (xLibVal_t));
          k++;
          peerCount++;
        }
      }
      /* found the parent for the peer nodes. Get all nodes stored in peerStack and process */
      if (opStk[k].level < level)
      {
        /* parent at opStk[k] */
        if (opStk[k].type == XLIB_AND)
        {
          if (opStk[k].rc == XLIBRC_SUCCESS)
          {
            if(peerCount !=0)
            {
              for (j = 0; j < peerCount; j++)
              {
                if ((peers[j].rc == XLIBRC_SUCCESS) && (peers[j].parentType == XLIB_AND))
                {
                  opStk[k].rc = XLIBRC_SUCCESS;
                }
                else if (peers[j].parentType != XLIB_AND)
                {
                  /* Case not expected Debugging prupose */
                  XLIB_WARN ("\n Improper entires in peer stack ");
                }
                else
                {
                /* as failure due to child node  , Even if one child fails entire and block fails */
                /* no need to further process the peerStack hence make peerCount as 0 */
                peerCount = 0;
                opStk[k].error = peers[j].error;
                opStk[k].rc = XLIBRC_FAILURE;
                break;
                }
              }
            }
            /* Case when the camparision nodes are i and k and there are no peers added to peers list*/
            else
            {
                if((opStk[i].rc == XLIBRC_SUCCESS) && (opStk[i].parentType == XLIB_AND))
                {
                  opStk[k].rc = XLIBRC_SUCCESS;
                }
                else if (opStk[i].parentType != XLIB_AND)
                {
                  /* Case not expected Debugging prupose */
                  XLIB_WARN ("\n Improper entires in peer stack ");
                }
                else
                {
                /* as failure due to child node  ,if child node  fails entire and block fails */                
                opStk[k].error = opStk[i].error;
                opStk[k].rc = XLIBRC_FAILURE;                
                }
           } 
          }
          else 
          {
            /* else case need not be handled as it means failure of an element in parent <and> block */
            opStk[k].rc = XLIBRC_FAILURE;
          }
          peerCount = 0;
        }
        /* parent at opStk[k] */
        else if (opStk[k].type == XLIB_OR)
        {
          if (opStk[k].rc != XLIBRC_SUCCESS)
          {
            if(peerCount !=0)
            {
              for (j = 0; j < peerCount; j++)
              {
                if ((peers[j].rc == XLIBRC_SUCCESS) && (peers[j].parentType == XLIB_OR))
                {
                  /* no need to further process the peerStack hence make peerCount as 0 */
                  peerCount = 0;
                  /* return Success to parent block even if one child throws an Success */
                  opStk[k].rc = XLIBRC_SUCCESS;
                  break;
                }
                else if (peers[j].parentType != XLIB_OR)
                {
                  /* Case not expected Debugging prupose */
                  XLIB_WARN ("\n Improper entires in peer stack ");
                }
                 else
                {
                  /* need not overwrite <or> block error */
                  opStk[k].rc = XLIBRC_FAILURE;
                }

               }
            }
            /* Case when the camparision nodes are i and k and there are no peers added to peers list*/
            else
            {
               if((opStk[i].rc == XLIBRC_SUCCESS) && (opStk[i].parentType == XLIB_OR))
               {
                  opStk[k].rc = XLIBRC_SUCCESS;                  
               }
               else if(opStk[i].parentType != XLIB_OR)
               {
                  /* Case not expected Debugging prupose */
                  XLIB_WARN ("\n Improper entires in peer stack ");
               }
               else
               {                 
                  opStk[k].rc = XLIBRC_FAILURE;
               }
            }  
          }
          else
          {
            /* else case needed not be handled as it means success of the parent block */
            opStk[k].rc = XLIBRC_SUCCESS;
          }
          peerCount = 0;
        }
        /* puch the validated parent node to stack */
        memcpy (&parents[parCount], &opStk[k], sizeof (xLibVal_t));
        parCount++;
        i = k + 1;
        k = i;
        continue;
      }
      if (opStk[k].level > level)
      {
        /* copy the node to the parents stack and carry on with further process */
       /* copy all the nodes with  less level value and agin start processing from k */
        if(peerCount !=0)
        {
          for(j=0;j<peerCount;j++)
          {
             memcpy (&parents[parCount++], &peers[j],sizeof (xLibVal_t));             
          }
        }
        else
        {
           memcpy (&parents[parCount++], &opStk[i], sizeof (xLibVal_t));
        }
        peerCount = 0;
        i = k;
        continue;
      }
    }                           /*k < opCount */
    else
    {
      /* Copy node at level 0 to the parent stack */
      memcpy (&parents[parCount], &opStk[i], sizeof (xLibVal_t));
      if (parCount)
      {
        parCount++;
        travCount = parCount;
        /* copy the parents stack to opStk and iterate */
        for (j = 0; j < parCount; j++)
        {
          opStk[j] = parents[j];
        }
        /* set all the variables to zero before iterating */
        parCount = 0;
        i = 0;
        k = 0;
      }
      else
        break;
    }
  }

  /* Error propogated till the root node */
  if (parents[root].rc == XLIBRC_SUCCESS)
  {
    xLibFree(peers);
    xLibFree(parents);
    xLibFree(opStk);
    return XLIBRC_SUCCESS;
  }
  XLIB_WARN ("xLibFinalValidate: failed for rc = %s", xLibRcStrErr (parents[root].error));
  error = parents[root].error;
  xLibFree(peers);
  xLibFree(parents);
  xLibFree(opStk);
  return error;
}
