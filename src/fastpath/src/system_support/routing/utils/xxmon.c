/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      xxmon.c
 *
 * @purpose       A simple monitor
 *
 * @component     Routing Utils Component
 *
 * @comments
 *
 * @create        10/01/1998 
 * 
 * @author        Igor Ternovsky
 *
 * @end                                                                        
 *
 * ********************************************************************/        
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\xxmon.c 1.1.3.1 2002/02/13 23:07:03EST ssaraswatula Development  $";
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "std.h"

#if L7_MONITOR

#include "xx.ext"
#include "xxmon.h"

static t_MonEntry *p_mon_root;
static t_MonEntry *p_cur_dir;
static char       *p_inbuf;
static volatile Bool stopXXMonitor;

#define INBUF_LENGTH          160
#define OUTBUF_LENGTH         80
#define PRINT_LEN_PER_PARM    40
#define ROOT_NAME             "/"

/* Internal functions */
static void        _xxmon_allocRoot( void );
static void        _xxmon_displayDir( t_MonEntry *p_dir );
static t_TokenType _xxmon_getWord( char *name, word len );
static t_TokenType _xxmon_analizeToken( char *name );
static Bool        _xxmon_parseParms( t_MonEntry *p_token, word *pnParms );
static t_MonEntry *_xxmon_searchToken( t_MonEntry *p_dir, char *name );
static void        _xxmon_dirHelp( void );
static void        _xxmon_displayHelp( t_MonEntry *p_token );
static void        _xxmon_chooseAlias( t_MonEntry *p_dir, t_MonEntry *p_new_token );
static char       *_xxmon_strlwr( char *s );
static int         _xxmon_stricmp( char *s1, char *s2, word len );



/*********************************************************************
 * @purpose         Add subdirectory
 *
 *
 * @param p_root    @b{(input)}  root directory handle (might be NULL)
 * @param name      @b{(input)}  directory name
 *
 * @returns         the new created directory handle in case of success
 * @returns         =NULLP - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle XX_MonAddDir( t_Handle   hRoot,
                       const char *name )
{
   t_MonEntry *p_root = (t_MonEntry *)hRoot;
   t_MonEntry *p_dir;
   t_MonEntry **p_e;

   if (!p_mon_root)
      _xxmon_allocRoot( );

   if (!p_root)
      p_root = p_mon_root;

   ASSERT( p_root && (p_root->sel == Dir) );

   if ( (p_dir=XX_Malloc( sizeof( t_MonEntry )) ) == NULLP)
      return NULLP;

   memset( p_dir, 0, sizeof( t_MonEntry ) );
   strncpy( p_dir->name, name, MAX_NAME_LEN );
   p_dir->sel = Dir;
   _xxmon_chooseAlias( p_root, p_dir );

   /* Add new directory to the root's list */
   p_dir->u.dir.upper = p_root;
   p_e = &(p_root->u.dir.first);
   while (*p_e)
      p_e = &((*p_e)->next);
   *p_e = p_dir;

   return p_dir;
}

/* Scan directory tree and look for directory with name starts from
   root directory with name root_name
*/
t_Handle XX_MonGetDirEntry( const char  *root_name, 
                            const char  *name )
{
   root_name = root_name;
   name = name;
   return NULLP;
}

/*********************************************************************
 * @purpose     Add token
 *
 *
 * @param hDir       @b{(input)}  directory handle (might be NULL=root)
 * @param *name      @b{(input)}  token name
 * @param *help      @b{(input)}  help string
 * @param p_func     @b{(input)}  token handler
 * @param p_parms[]  @b{(input)}  array of parameter descriptions.
 *                           Must be terminated with {0}.
 *                           Each parm descriptor is a struct
 *                           { "myname",         - name
 *                              "%d",             - format
 *                              10,               - low value
 *                              20,               - high value
 *                              0 }               - default value =-1 no default
 *
 *
 * @returns                     E_OK - OK
 * @returns                     !=0 - error
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err XX_MonAddToken( t_Handle      hDir,
                      const char    *name,
                      const char    *help,
                      t_TokenFunc   p_func,
                      t_MonParm     p_parms[] )
{
   t_MonEntry *p_dir = (t_MonEntry *)hDir;
   t_MonEntry *p_token;
   t_MonEntry **p_e;
   word       i;

   if (!p_mon_root)
      _xxmon_allocRoot( );

   if (!p_dir)
      p_dir = p_mon_root;

   ASSERT( p_dir && (p_dir->sel == Dir) );

   if ((p_token=XX_Malloc( sizeof( t_MonEntry ) )) == NULLP)
      return E_NOMEMORY;

   /* Initialize token structure */
   memset( p_token, 0, sizeof( t_MonEntry ) );
   /* Copy name */
   strncpy( p_token->name, name, MAX_NAME_LEN );
   strncpy( p_token->help, help, MAX_HELP_LEN );
   p_token->sel = Token;
   p_token->u.token.f_tokenFunc = p_func;
   /* Convert name to lower case and choose alias */
   _xxmon_chooseAlias( p_dir, p_token );

   /* Copy parameters */
   i = 0;
   if ( p_parms )
   {
      while( p_parms->name && 
             p_parms->name[0] && 
             ( i < MAX_NUM_OF_PARMS) )
      {
         t_MonParm *p_token_parm = &p_token->u.token.parm[i];

         /* String parameter must have an address */
         if ((p_parms->flags & XXMON_PARM_STRING) &&
              ((p_parms->value == 0)    || 
               (p_parms->low_val == 0)  ||
               (p_parms->flags & XXMON_PARM_RANGE)
              ) )   
         {
            XX_Free( p_token );
            return E_BADPARM;
         }
         /* Pointer parameter must have an address */
         if ((p_parms->flags & XXMON_PARM_USERIO) &&
             ((p_parms->value == 0)    || 
              (p_parms->low_val == 0)  ||
              (p_parms->hi_val == 0)   ||
              (p_parms->flags & XXMON_PARM_RANGE)
             ) )
         {
            XX_Free( p_token );
            return E_BADPARM;
         }

         /* Copy parameter */
         *p_token_parm = *p_parms;
         p_token_parm->name = (const char *)p_token->u.token.name[i];
         p_token_parm->format = (const char *)p_token->u.token.format[i];
         strncpy( p_token->u.token.name[i], p_parms->name, MAX_NAME_LEN );
         if ( p_parms->format )
            strncpy( p_token->u.token.format[i], p_parms->format, MAX_FORMAT_LEN );

         ++p_parms;
         ++i;
      }
      if ((i == MAX_NUM_OF_PARMS) && p_parms->name[0])
      {
         XX_Free( p_token );
         return E_TOOMANY;
      }
   }

   p_token->u.token.nParms = i;
   
   /* Add token to the directory */
   p_e = &(p_dir->u.dir.first);
   while (*p_e)
      p_e = &((*p_e)->next);
   *p_e = p_token;

   return E_OK;
}


/* Monitor driver */
void XX_MonDriver( void )
{
   char inbuf[INBUF_LENGTH];

   if (!p_mon_root)
      return;

   stopXXMonitor = FALSE;
   _xxmon_displayDir( p_cur_dir );

   while(!stopXXMonitor)
   {
      if ( !XX_Gets( (byte *)inbuf, (word)sizeof( inbuf) ) )
         return;

      XX_MonParseString( inbuf );
   }
}


/* Parse the given input string and exit.
   All commands in the input string are executed one by one. 
*/
void XX_MonParseString( const char *input_string )
{
   t_MonEntry  *p_token;
   char        name[MAX_NAME_LEN];
   t_TokenType tType;
   word        nParms;

   if (!p_mon_root)
      return;

   p_inbuf = (char *)input_string;
   stopXXMonitor = FALSE;

   /* Interpret empty string as "display directory" */
   if ( p_inbuf && !*p_inbuf )
      _xxmon_displayDir( p_cur_dir );

   while(!stopXXMonitor && p_inbuf && *p_inbuf)
   {
      tType = _xxmon_getWord( name, MAX_NAME_LEN );
      switch( tType )
      {

      case NameToken: 
         p_token = _xxmon_searchToken( p_cur_dir, name );
         if (p_token == NULLP)
         {
            XX_Print( "**Error**");
            p_inbuf = NULLP;
         }
         else if (p_token->sel == Dir)
         {
            p_cur_dir = p_token;
            _xxmon_displayDir( p_cur_dir );
         }
         else 
         {  /* Function token */
            if (!_xxmon_parseParms( p_token, &nParms ))
               _xxmon_displayHelp( p_token );
            else
               p_token->u.token.f_tokenFunc( p_token->u.token.parm, nParms ); 
         }
         break;

      case UpToken: /* Go to upper directory */
         if (p_cur_dir->u.dir.upper)
            p_cur_dir = p_cur_dir->u.dir.upper;
         _xxmon_displayDir( p_cur_dir );
         break;

      case RootToken: /* Go to the root directory */
         if (p_cur_dir->u.dir.upper)
            p_cur_dir = p_mon_root;
         _xxmon_displayDir( p_cur_dir );
         break;

      case HelpToken: /* Display help */
         if (( _xxmon_getWord( name, MAX_NAME_LEN ) == NameToken ) && 
             ((p_token = _xxmon_searchToken( p_cur_dir, name )) != NULLP ) &&
             (p_token->sel == Token) )
            _xxmon_displayHelp( p_token );
         else
            _xxmon_dirHelp( );
         break;

      case DirHelpToken:
         _xxmon_displayDir( p_cur_dir );
         XX_Print( "Type ? <name> for command help, \"/\"-root, \"..\"-upper" );
         break;

      case BreakToken: /* Clear buffer */
         p_inbuf = NULLP;
         break;

      case EmptyToken:
         break;

      }
   }
}


/* Stop monitor driver */
void XX_MonStop( void )
{
   stopXXMonitor = TRUE;
}


/*********************************************************/
/* Internal functions                                    */
/*********************************************************/

/* Allocate root directory */
void _xxmon_allocRoot( void )
{
   /* The very first call. Allocate root structure */
   if ((p_mon_root=XX_Malloc( sizeof( t_MonEntry ) ) ) == NULLP)
   {
      ASSERT( p_mon_root );
      return;
   }
   memset( p_mon_root, 0, sizeof( t_MonEntry ) );
   strcpy( p_mon_root->name, ROOT_NAME );
   p_mon_root->sel = Dir;
   p_cur_dir = p_mon_root;
}

/* Display directory */
void _xxmon_displayDir( t_MonEntry *p_dir )
{
   char out_buf[MAX_BUF_LENGTH];
   t_MonEntry *p_token;

   sprintf( out_buf, "%s%s> ", (p_dir==p_mon_root)?"":".../", p_dir->name );
   p_token = p_dir->u.dir.first;
   while( p_token )
   {
      strcat( out_buf, p_token->name );
      if ( p_token->sel == Dir )
         strcat( out_buf, "/" );
      p_token = p_token->next;
      if (p_token)
         strcat( out_buf, ", " );
   }

   XX_Print( out_buf );
}


/* Cut the first word from <p_inbuf>.
   Return the word in <name> and updated <p_inbuf>
*/
t_TokenType _xxmon_getWord( char *name, word len )
{
   word        i=0;
   t_TokenType tType;

   /* Skip leading blanks */
   while( *p_inbuf && *p_inbuf==' ')
      ++p_inbuf;

   while( *p_inbuf && *p_inbuf!=' ' && i<len )
      name[i++] = *(p_inbuf++);

   if (i<len)
      name[i] = 0;

   tType   = _xxmon_analizeToken( name );

   return tType;
}


/* Make a preliminary analizis of <name> token.
   Returns a token type (Empty, Up, Root, Break, Name)
*/ 
t_TokenType _xxmon_analizeToken( char *name )
{
   if (!name[0])
      return EmptyToken;

   if (!strcmp( name, TOKEN_UP ) )
      return UpToken;

   if (!strcmp( name, TOKEN_ROOT ) )
      return RootToken;

   if (!strcmp( name, TOKEN_BREAK ) )
      return BreakToken;

   if (!strcmp( name, TOKEN_HELP ) )
      return HelpToken;

   if (!strcmp( name, TOKEN_DIRHELP ) )
      return DirHelpToken;

   return NameToken;

}


/* Parse p_inbuf string based on parameter descriptions in <p_token>.
   Fill parameter values in <p_token>.
   Returns the number of parameters filled.
   To Do: add a option of one-by-one user input of missing parameters.
*/
Bool _xxmon_parseParms( t_MonEntry *p_token, word *pnParms )
{
   word nParms=0;
   char parm[INBUF_LENGTH];
   char out_buf[OUTBUF_LENGTH];
   word i;
   ulng val;

   /* Mark all parameters as don't having an explicit value */
   for( i=0; i<p_token->u.token.nParms; i++ )
      p_token->u.token.parm[i].flags |= XXMON_PARM_NOVAL;

   /* Build a format string */
   for( i=0; i<p_token->u.token.nParms; i++ )
   {

      if (p_token->u.token.parm[i].flags & XXMON_PARM_STRING)
      {
         /* For a string parameter value is the string address */
         /* and low_val is the string length                   */
         if (_xxmon_getWord(  parm,
                              (word)p_token->u.token.parm[i].low_val ) != NameToken)
            break;
         strncpy( (char *)p_token->u.token.parm[i].value, 
                  parm, 
                  p_token->u.token.parm[i].low_val );
      }
      else
      {

         if (_xxmon_getWord( parm, MAX_PARM_LEN ) != NameToken)
            break;
      
         /* Convert */
         if (p_token->u.token.parm[i].flags & XXMON_PARM_USERIO)
         {
            /* call user-defined scan function */
            if ( ((t_userscan_f)p_token->u.token.parm[i].low_val) 
                  (  parm, 
                     p_token->u.token.format[i],
                     &p_token->u.token.parm[i].value ) != 0)
               break;
         }
         else
         {
            if (sscanf( parm, p_token->u.token.format[i], &val ) != 1)
               break;

            /* Check value */
            if (p_token->u.token.parm[i].flags & XXMON_PARM_RANGE)
            {
               if ((val < p_token->u.token.parm[i].low_val) ||
                   (val > p_token->u.token.parm[i].hi_val) )
               {
                  sprintf( out_buf, "%s: %ld out of range (%ld, %ld)",
                           p_token->u.token.parm[i].name, 
                           val,
                           p_token->u.token.parm[i].low_val,
                           p_token->u.token.parm[i].hi_val );
                  XX_Print( out_buf );
                  return FALSE;
               }
            }

            p_token->u.token.parm[i].value = val;

         }


      }

      p_token->u.token.parm[i].flags &= ~XXMON_PARM_NOVAL;
      ++nParms;
   }

   /* Process default values */
   for( ; i<p_token->u.token.nParms; i++ )
   {
      if ((p_token->u.token.parm[i].flags & XXMON_PARM_DEFVAL) != 0)
      {
         p_token->u.token.parm[i].flags &= ~XXMON_PARM_NOVAL;
         ++nParms;
      }
      else if (!(p_token->u.token.parm[i].flags & XXMON_PARM_OPTIONAL) )
      {
         /* Mandatory parameter missing */
         return FALSE;
      }
   }

   *pnParms = nParms;

   return TRUE;
}

/* Serach a token by name in the current directory */
t_MonEntry *_xxmon_searchToken( t_MonEntry *p_dir, char *name )
{
   t_MonEntry *p_token;
   word        name_len = (word)strlen( name );

   /* Check name */
   p_token = p_dir->u.dir.first;
   while( p_token )
   {
      if (!_xxmon_stricmp( p_token->name, name, name_len ) )
         break;
      p_token = p_token->next;
   }
   if (p_token)
      return p_token;
   
   /* Check alias */
   p_token = p_dir->u.dir.first;
   while( p_token )
   {
      if (p_token->alias &&
          (name_len == p_token->alias_len) &&
          !_xxmon_stricmp( p_token->alias, name, p_token->alias_len) )
         break;
      p_token = p_token->next;
   }

   return p_token;
}


/* Display help for each entry in the current directory */
void  _xxmon_dirHelp( void )
{
   t_MonEntry *p_token;
   char        print_str[80];

   p_token = p_cur_dir->u.dir.first;

   while( p_token )
   {
      if (p_token->sel == Dir)
         sprintf( print_str, "%s: directory", p_token->name );
      else
         sprintf( print_str, "%s(%d parms): %s", 
                  p_token->name, p_token->u.token.nParms, p_token->help );
      XX_Print( print_str );
      p_token = p_token->next;
   }

   XX_Print( "Type ? <name> for command help, \"/\"-root, \"..\"-upper" );
}


/* Display help a token */
void  _xxmon_displayHelp( t_MonEntry *p_token )
{
   char tmp1[80], tmp2[80];
   char bra, ket;
   char *print_str;
   word i;

   print_str = XX_Malloc( (p_token->u.token.nParms+1) * PRINT_LEN_PER_PARM );
   if (print_str == NULLP)
      return;

   sprintf( print_str, "%s: %s ", p_token->help, p_token->name );
   for( i=0; i<p_token->u.token.nParms; i++ )
   {
      if (p_token->u.token.parm[i].flags & XXMON_PARM_OPTIONAL)
      {
         bra = '['; ket=']';
      }
      else
      {
         bra = '<'; ket='>';
      }
      sprintf( tmp1, "%c%s", bra, p_token->u.token.parm[i].name );
      if (p_token->u.token.parm[i].flags & XXMON_PARM_DEFVAL)
      {
         strcat( tmp1, "=" );
         if (p_token->u.token.parm[i].flags & XXMON_PARM_USERIO )
         {
            ((t_userprint_f)p_token->u.token.parm[i].hi_val) 
                  (  tmp2, 
                     p_token->u.token.format[i], 
                     p_token->u.token.parm[i].value );
            /* User-defined sprint function */
            strcat( tmp1, tmp2 );
         }
         else
         {
            sprintf( tmp2, p_token->u.token.format[i], 
                     p_token->u.token.parm[i].value );
            strcat( tmp1, tmp2 );
         }
      }
      if (p_token->u.token.parm[i].flags & XXMON_PARM_RANGE)
      {
         strcat( tmp1, " (" );
         sprintf( tmp2, "%ld..%ld)", 
                  p_token->u.token.parm[i].low_val, 
                  p_token->u.token.parm[i].hi_val );
         strcat( tmp1, tmp2 );
      }
      sprintf( tmp2, "%c ", ket );
      strcat( tmp1, tmp2 );
      
      strcat( print_str, tmp1 );
   }

   XX_Print( print_str );
   XX_Free( print_str );
}

/* Choose unique alias for <name> in <p_dir> */
/* Currently only single-character aliases are supported */
void _xxmon_chooseAlias( t_MonEntry *p_dir, t_MonEntry *p_new_token )
{
   t_MonEntry *p_token;
   int         i;
   char        c;

   _xxmon_strlwr( p_new_token->name );

   i = 0;
   while( p_new_token->name[i] )
   {
      c = p_new_token->name[i];
      p_token = p_dir->u.dir.first;

      while( p_token )
      {
         if (p_token->alias && 
             (tolower( *p_token->alias ) == c) )
            break;
         p_token = p_token->next;
      }
      if (p_token)
         ++i;
      else
      {
         p_new_token->name[i] = toupper( c );
         p_new_token->alias   = &p_new_token->name[i];
         p_new_token->alias_len = 1;
         break;
      }
   }
}


/* Convert string s to lower case. Return pointer to s */
char  * _xxmon_strlwr( char *s )
{
   char  *s0=s;

   while( *s )
   {
      *s = tolower( *s );
      ++s;
   }

   return s0;
}


/* Compare strings case incensitive */
int _xxmon_stricmp( char *s1, char *s2, word len )
{
   int  i;

   for( i=0; i<len && s1[i] && s2[i]; i++ )
   {
      if (tolower( s1[i])  != tolower( s2[i] ))
         break;
   }

   return ( (len - i) * (s1[i] - s2[i]) );
}


#endif /* #if L7_MONITOR */
