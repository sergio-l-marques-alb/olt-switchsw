#include "addrmap.h"

/**************************************************************/
/* AddrAlloc (MAP)                                            */
/**************************************************************/
void *AddrAlloc(TAddrMap *AddrMap, int AddrStart, long AddrLen){
  char *fName="/dev/mem";

  if ((AddrMap->fd = open(fName, O_RDWR)) < 0){
    #ifdef _AddrMapDebug
    fprintf(stderr,"Open file %s: %s\n", fName, strerror(errno));
    #endif
    return(MAP_FAILED);
  }

  AddrMap->address = mmap(NULL, AddrLen, PROT_READ|PROT_WRITE, MAP_SHARED, AddrMap->fd, AddrStart);
  AddrMap->length  = AddrLen;

  #ifdef _AddrMapDebug
  if (AddrMap->address == MAP_FAILED) {
    switch (errno) {
      case EBADF: 
        fprintf(stderr,"EBADF:\n"\
                       "  fd is not a valid file descriptor (and MAP_ANONYMOUS\n"\
                       "  was not set).\n");
        break;
  
      case EACCES: 
        fprintf(stderr,"EACCES:\n"\
                       "  MAP_PRIVATE was asked, but fd is not open for read­\n"\
                       "  ing. Or MAP_SHARED was asked and PROT_WRITE is\n"\
                       "  set, fd is not open in read/write (O_RDWR) mode.\n");
        break;

      case EINVAL: 
        fprintf(stderr,"EINVAL:\n"\
                       "  We don't like start or length or offset. (E.g.,they\n"\
                       "  are too large, or not aligned on a PAGESIZE boundary.)\n");
        break;

      case ETXTBSY: 
        fprintf(stderr,"ETXTBSY:\n"\
                       "  MAP_DENYWRITE was set but the object specified  by\n"\
                       "  fd is open for writing.\n");
        break;

      case EAGAIN:
        fprintf(stderr,"EAGAIN:\n"\
                       "  The file has been locked, or too much memory has been locked.\n");
        break;

      case ENOMEM:
        fprintf(stderr,"ENOMEM:\n"\
                       "  No memory is available.\n");
        break;
      /*
      case SIGSEGV:
        fprintf(stderr,"SIGSEGV:\n"\
                       "  Attempted write into a region specified to mmap  as read-only.\n");
        break;
        
      case SIGBUS:
        fprintf(stderr,"SIGBUS:\n"\
                       "  Attempted  access  to  a portion of the buffer that\n"
                       "  does not  correspond  to  the  file  (for  example,\n"
                       "  beyond  the  end  of  the  file, including the case\n"
                       "  where another process has truncated the file).\n");
        break;
      */
    }
  }
  #endif
  return(AddrMap->address);
}

void *AddrAlloc64(TAddrMap *AddrMap, long long AddrStart, long AddrLen){
  char *fName="/dev/mem";

  if ((AddrMap->fd = open(fName, O_RDWR)) < 0){
    #ifdef _AddrMapDebug
    fprintf(stderr,"Open file %s: %s\n", fName, strerror(errno));
    #endif
    return(MAP_FAILED);
  }

  AddrMap->address = mmap64(NULL, AddrLen, PROT_READ|PROT_WRITE, MAP_SHARED, AddrMap->fd, AddrStart);
  AddrMap->length  = AddrLen;

  #ifdef _AddrMapDebug
  if (AddrMap->address == MAP_FAILED) {
    switch (errno) {
      case EBADF: 
        fprintf(stderr,"EBADF:\n"\
                       "  fd is not a valid file descriptor (and MAP_ANONYMOUS\n"\
                       "  was not set).\n");
        break;
  
      case EACCES: 
        fprintf(stderr,"EACCES:\n"\
                       "  MAP_PRIVATE was asked, but fd is not open for read­\n"\
                       "  ing. Or MAP_SHARED was asked and PROT_WRITE is\n"\
                       "  set, fd is not open in read/write (O_RDWR) mode.\n");
        break;

      case EINVAL: 
        fprintf(stderr,"EINVAL:\n"\
                       "  We don't like start or length or offset. (E.g.,they\n"\
                       "  are too large, or not aligned on a PAGESIZE boundary.)\n");
        break;

      case ETXTBSY: 
        fprintf(stderr,"ETXTBSY:\n"\
                       "  MAP_DENYWRITE was set but the object specified  by\n"\
                       "  fd is open for writing.\n");
        break;

      case EAGAIN:
        fprintf(stderr,"EAGAIN:\n"\
                       "  The file has been locked, or too much memory has been locked.\n");
        break;

      case ENOMEM:
        fprintf(stderr,"ENOMEM:\n"\
                       "  No memory is available.\n");
        break;
      /*
      case SIGSEGV:
        fprintf(stderr,"SIGSEGV:\n"\
                       "  Attempted write into a region specified to mmap  as read-only.\n");
        break;
        
      case SIGBUS:
        fprintf(stderr,"SIGBUS:\n"\
                       "  Attempted  access  to  a portion of the buffer that\n"
                       "  does not  correspond  to  the  file  (for  example,\n"
                       "  beyond  the  end  of  the  file, including the case\n"
                       "  where another process has truncated the file).\n");
        break;
      */
    }
  }
  #endif
  return(AddrMap->address);
}

/**************************************************************/
/* AddrFree (UNMAP)                                           */
/**************************************************************/

int AddrFree(TAddrMap *AddrMap)
{
  int RetValue;

  RetValue=munmap(AddrMap->address, AddrMap->length); 
  close(AddrMap->fd);
  return(RetValue);
}

/**************************************************************/

