#include<stdio.h>
#include<string.h>

#include "xlib_rc.h"
#include "fpobj_event_log_util.h"

int L7_event_log_string_get(char *msg,int type,char* delim,char* outstr)
{
  char* tmp[32];
  int skip;
  int count = 0;
  char* save_ptr = NULL;

  switch(type)
  {
    case EVENT:
      skip = 0;
      break;
    case FILENAME :
      skip = 1;
      break;
    case LINE_NUMBER:
      skip = 2;
      break;
    case TASKID:
      skip = 3;
      break;
    case CODE:
      skip = 4;
      break;
    case DAYS:
      skip = 5;
      break;
    case HOURS:
      skip = 6;
      break;
    case MINS:
      skip = 7;
      break;
    case SECS:
      skip = 8;
      break;
    default:
      skip = 0;
  }

  if(msg == NULL)
  {
    outstr = NULL;
    return XLIBRC_FAILURE;
  }
  tmp[count] = strtok_r(msg, delim,&save_ptr);
  while (tmp[count] != NULL && count < skip)
  {
    count++;
    tmp[count] = strtok_r(NULL, delim,&save_ptr);
  }
  memcpy(outstr,tmp[skip],strlen(tmp[skip]));
  return XLIBRC_SUCCESS;
}

