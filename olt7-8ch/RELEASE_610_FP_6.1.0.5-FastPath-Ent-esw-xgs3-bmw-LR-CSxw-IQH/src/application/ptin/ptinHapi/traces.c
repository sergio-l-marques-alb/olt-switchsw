#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "ptin/globaldefs.h"
#include "traces.h"

//static st_DebugParams   l_DebugSettings = {0, 0, 0, 0, 0, "/dev/ttyS0"};
static st_DebugParams   l_DebugSettings = {0, DEBUG_MODE_TIME|DEBUG_MODE_PID|DEBUG_MODE_ID|DEBUG_MODE_SEVERITY, 0xFFFF, 0xFFFF0000, 0x2, "/dev/ttyS0"};
static BYTE             l_ActivateMemDump = 0;
static int              l_previousTTY = -1;

// -------------------------------------------------------------
// Public Methods
// -------------------------------------------------------------

   void GetDebugSettings (st_DebugParams *data)
   {
      if (data!=NULL)
      {
         data->debugFormat    = l_DebugSettings.debugFormat;
         data->debugLayers    = l_DebugSettings.debugLayers;
         data->debugModules   = l_DebugSettings.debugModules;
         data->debugOutput    = l_DebugSettings.debugOutput;
         strcpy (data->debugDevice, l_DebugSettings.debugDevice);
      }
   } // GetDebugSettings (V1.5.0.040816)

   void SetDebugFormat (int format)
   {
      l_DebugSettings.debugFormat  = format;
   } // SetDebugFormat (V1.5.0.040816)

   void SetDebugFilters (UINT layers, UINT modules)
   {
      l_DebugSettings.debugLayers  = layers;
      l_DebugSettings.debugModules = modules;
   } // SetDebugFilters (V1.5.0.040816)

   void SetDebugOutput (int output)
   {
      l_DebugSettings.debugOutput = output;
   } // SetDebugOutput (V1.5.0.040816)

   void SetDebugDevice (char *device)
   {
      int fd;

      if ((fd = open(device, O_RDWR | O_APPEND)) != -1)
      {
         close (1);
         if (l_previousTTY!=-1)
            close (l_previousTTY);
         dup2  (fd, 1);
         l_previousTTY = fd;
         strncpy (l_DebugSettings.debugDevice, device, 50);
      }
   } // SetDebugDevice (V2.1.0.050621)

   void DebugTrace (int mask, int sev, const char *fmt, ...)
   {
      va_list  ap;
      char     buffer[DEBUG_LENGTH_MAX];
      UINT     mask_modules;
      UINT     mask_layers;

      mask_modules = mask & 0xFFFF0000;
      mask_layers  = mask & 0x0000FFFF;
	   if ((mask_modules & l_DebugSettings.debugModules) && (mask_layers & l_DebugSettings.debugLayers))
	   {
		   va_start(ap, fmt);
         vsprintf (buffer, fmt, ap);
		   if ((DEBUG_OUTPUT_FILE & l_DebugSettings.debugOutput) || (sev < TRACE_SEVERITY_ERROR))
		   {
			   FILE *fp;

			   if ((fp=fopen (DEBUG_FILENAME, "at"))!=NULL)
			   {
               DebugTrailer (fp, mask, sev);
				   fprintf  (fp, "%s\n\r", buffer);
				   fclose   (fp);
			   }
		   }
		   if (DEBUG_OUTPUT_CONSOLE & l_DebugSettings.debugOutput)
		   {
            DebugTrailer (stdout, mask, sev);
			   printf   ("%s\n\r", buffer);
			   fflush   (NULL);
		   }
		   va_end(ap);
	   } // filter mask
   } // DebugTrace (V2.0.0.050511)

   void DumpMemActivation (BOOLEAN a)
   {
      l_ActivateMemDump = a;
   } //DumpMemActivation

   void DumpMem (void(*dumpEntity)(void*), void *entity, size_t size, int num, int flags)
   {
      int   i;

      if ( 0 < l_ActivateMemDump )
      {
         if ( flags & 0x01 )
         {
            dumpEntity(NULL);
         }
         if ( DEBUG_OUTPUT_CONSOLE & l_DebugSettings.debugOutput )
         {
            for ( i = 0; num > i; i++ )
            {
               dumpEntity((void*)((char *)entity + i*size));
            }
         }
      } // filter mask
   } //DumpMem

   void LogTrace (int sev, const char *fmt, ...)
   {
   } // LogTrace (V1.5.0.040816)


// -------------------------------------------------------------
// Private Methods
// -------------------------------------------------------------

   void DebugTrailer (FILE *fp, int mask, int sev)
   {
      char tempo  [240];
      char pid    [10];
      char auxid  [12];
      char sevstr [50];

      // Date/Time
      if (l_DebugSettings.debugFormat & DEBUG_MODE_TIME)
         getTimeMarker (tempo);
      else
         tempo[0] = 0;
      // Process ID
      if (l_DebugSettings.debugFormat & DEBUG_MODE_PID)
         sprintf (pid, "%d-", getpid());
      else
         pid[0] = 0;
      // Trace mask
      if (l_DebugSettings.debugFormat & DEBUG_MODE_ID)
         sprintf (auxid, "%08X-", mask);
      else
         auxid[0] = 0;
      // Severity
      if (l_DebugSettings.debugFormat & DEBUG_MODE_SEVERITY)
         getSeverity (sev, DEBUG_OUTPUT_FILE, sevstr);
      else
         sevstr[0] = 0;

      fprintf (fp, "%s: %s%s%s%s ", APPLICATION_NAME, tempo, pid, auxid, sevstr);
   } // DebugTrailer (V1.5.0.040816)

   char* getTimeMarker (char* output)
   {
	   struct timeval	tv;
	   time_t			t;
	   struct tm		tempo;

	   if (output)
	   {
		   time (&t);
		   localtime_r (&t, &tempo);
		   gettimeofday (&tv, NULL);
		   sprintf (output, "%04d%02d%02d-%02d:%02d:%02d.%03d-", tempo.tm_year+1900, tempo.tm_mon+1, tempo.tm_mday, tempo.tm_hour, tempo.tm_min, tempo.tm_sec, (int)(tv.tv_usec/1000));
	   }
	   return output;
   } // getTimeMarker (V1.5.0.040816)

   char* getSeverity (int sev, int mode, char* output)
   {
	   if (output)
	   {
		   switch (sev)
         {
            case TRACE_SEVERITY_EMERGENCY:      if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sEMG%s-", DEBUG_ESCVERMELHO, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "EMG-");
                                                break;
            case TRACE_SEVERITY_ALERT:          if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sALR%s-", DEBUG_ESCVERDE, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "ALR");
                                                break;
            case TRACE_SEVERITY_CRITICAL:       if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sCRT%s-", DEBUG_ESCAMARELO, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "CRT-");
                                                break;
            case TRACE_SEVERITY_ERROR:          if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sERR%s-", DEBUG_ESCAZUL, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "ERR-");
                                                break;
            case TRACE_SEVERITY_WARNING:        if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sWAR%s-", DEBUG_ESCMAGENTA, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "WAR-");
                                                break;
            case TRACE_SEVERITY_NOTICE:         if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sNOT%s-", DEBUG_ESCCIANO, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "NOT-");
                                                break;
            case TRACE_SEVERITY_INFORMATIONAL:  if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sINF%s-", DEBUG_ESCBRANCOVIVO, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "INF-");
                                                break;
            case TRACE_SEVERITY_DEBUG:          if (mode & DEBUG_OUTPUT_CONSOLE)
                                                   sprintf(output, "%sDEB%s-", DEBUG_ESCBRANCO, DEBUG_ESCFIM);
                                                else
                                                   sprintf(output, "DEB-");
                                                break;
         }
	   }
	   return output;
   } // getSeverity (V1.5.0.040816)
