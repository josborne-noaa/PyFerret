/* tm_unix_versions.c */
/* code to assist with the creation of file version numbers on Unix systems */
/*
* programmer - steve hankin
* NOAA/PMEL, Seattle, WA - Tropical Modeling and Analysis Program
* originally written for DECstation under Ultrix operating system
* V1.0 5/6/91
* v1.1 10/17/91  -- <kob>
*  now pass a path name into high_ver_name to enable a search into another
*    directory besides the current one.
*/
/* based on readdir man  pages */
/* routines:
   char *tm_c_ver_name_(name, next_name,path)
       ... return the file name of the next version to create
   int high_ver_name(name, path)
       ... return the (integer) value of the highest existing version
   int tilda_strcmp( rootnam, testnam )
       ... compare to see if "test" name is a version of "root" name
*/

/* had to add ifdef check for trailing underscore in routine name
   for aix port *kob* 10/94 */



#define NULL 0

#include <sys/types.h>
#include <dirent.h>


int tilda_strcmp( rootnam, testnam )

   char rootnam[], testnam[];

/* compare root name with test to see if they match apart from ".~nnn~"
   If they do, return nnn (0 for identical match).
   Else return -1  */

{
   int rlen, tlen, i, tilda;
   char tbuff[4];

   rlen = strlen(rootnam);
   tlen = strlen(testnam);

/* quick screen 1:
   test name may be exact same length, or at least 4 chars (".~n~") longer */
   if ( tlen != rlen && tlen < rlen+4 ) return (-1);
    
/* quick screen 2:
   if test name is a version it must end in tilda */
   if ( tlen != rlen && testnam[tlen-1] != '~' ) return (-1);

/* root name must match test name for its full length */
   for (i=0 ; (rootnam[i] != '\0') && (rootnam[i]==testnam[i]) ; i++) ;
   if ( i != rlen ) return (-1);
     
/* identically the same name ? */
   if ( rlen == tlen ) return 0;

/* next two characters must be ".~" and last must be "~" */
   if ( testnam[rlen]   != '.' ||  testnam[rlen+1] != '~' ) return (-1);

/* intervening characters must be digits */
   for (i=rlen+2; i<tlen-1; i++) {
     if ( !isdigit(testnam[i]) ) return (-1);
   }

/* we have found a numbered version of the root file */
/* decode the ~nnn~ value */
   strncpy( tbuff, testnam+rlen+2, tlen-rlen-2 );
   tbuff[tlen-rlen-3] = '\0';
   sscanf(tbuff, "%d", &tilda );
   return tilda;
}


int high_ver_name(name,path)
  char name[], path[];
/* find the highest numbered version of file "name" in the given directory */
/* if no directory is given then the current directory is used.         */
/* Also, if given path does not exist, then the procedure is exited  */
/* modified 10/91 to do this        <kob> */


{
  int next, tilda=(-1);
  struct dirent *dp;
  DIR *dirp;
  int closedir();

/*  dirp = opendir("."); */

  if (path[0] == '.' || path[0] == ' ')
    dirp = opendir(".");
  else
    dirp = opendir(path);
  if (dirp != NULL)  {
    for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)){
      next = tilda_strcmp( name, dp->d_name);
      if ( next > tilda ) tilda = next;
    }
	     
    closedir(dirp);
  }

  return tilda; 
	    
}

#ifdef NO_ENTRY_NAME_UNDERSCORES
char *tm_c_ver_name(name, next_name,path)
#else
char *tm_c_ver_name_(name, next_name,path)
#endif
  char name[], next_name[], path[];
/* generate the name for the next version of a file in this directory */

{
  int high, len;

/* get value of current highest version */
  high = high_ver_name(name,path);

  if ( high == (-1) )
/* ... no versions currently exist */
    next_name[0] = '\0';

  else {
    strcpy ( next_name, name );
    len = strlen(name);
    next_name[len] = '.';
    next_name[len+1] = '~';
    sprintf( next_name+len+2, "%d", high+1 );
    len = strlen(next_name);
    next_name[len] = '~';
    next_name[len+1] = '\0';
  }

  return next_name;
}



