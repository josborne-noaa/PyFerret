/*
*
*  This software was developed by the Thermal Modeling and Analysis
*  Project(TMAP) of the National Oceanographic and Atmospheric
*  Administration's (NOAA) Pacific Marine Environmental Lab(PMEL),
*  hereafter referred to as NOAA/PMEL/TMAP.
*
*  Access and use of this software shall impose the following
*  obligations and understandings on the user. The user is granted the
*  right, without any fee or cost, to use, copy, modify, alter, enhance
*  and distribute this software, and any derivative works thereof, and
*  its supporting documentation for any purpose whatsoever, provided
*  that this entire notice appears in all copies of the software,
*  derivative works and supporting documentation.  Further, the user
*  agrees to credit NOAA/PMEL/TMAP in any publications that result from
*  the use of this software or in any product that includes this
*  software. The names TMAP, NOAA and/or PMEL, however, may not be used
*  in any advertising or publicity to endorse or promote any products
*  or commercial entity unless specific written permission is obtained
*  from NOAA/PMEL/TMAP. The user also understands that NOAA/PMEL/TMAP
*  is not obligated to provide the user with any support, consulting,
*  training or assistance of any kind with regard to the use, operation
*  and performance of this software nor to provide the user with any
*  updates, revisions, new versions or "bug fixes".
*
*  THIS SOFTWARE IS PROVIDED BY NOAA/PMEL/TMAP "AS IS" AND ANY EXPRESS
*  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL NOAA/PMEL/TMAP BE LIABLE FOR ANY SPECIAL
,
*  INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
*  RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
*  CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
*  CONNECTION WITH THE ACCESS, USE OR PERFORMANCE OF THIS SOFTWARE.  
*
*/


/*
  Code to perform tab, comma, etc delimited reads from Ferret
  Top level Routines are
  anal_file_ - determines the number of fields and types of each field
  decode_file_ - reads an entire file based upon analysis suppied

  Support routines are
  analRec  - performs analysis of a single record
            ... possibly with missing fields
  decodeRec - reads a single record of the input file
  nexstrtok - breaks input into successive fields
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ez_delimited_read.h"


/*
 *
 decode_file - skip header & read an entire file based upon analysis suppied
 decode_file_ arguments
 fname - input filename (null terminated)
 recptr - buffer to hold file records
 reclen - size of record buffer
 delims - string containing delimiter in file (note that blank is
          considered a special delimiter representing any number of blanks)
 skip - number of heading record in file to be skipped
 maxrec - maximum number of records to read (beyond skipped header)
 nrec - out number of records actually read
 nfields - number of fields to decode on each record
 field_types - type of each field (input)
 mrlist - list of integers pointing to Ferret memory blocks
 memptr - pointer to base of Ferret "heap" storage
 mr_blk1 - memory chunk numbers indexed by mr_list
 mblk_size - chunk size within Ferret heap
 mr_bad_flags - missing value flags indexed by mr_list
 *
 */
void FORTRAN(decode_file_jacket)
		( char* fname, char *recptr, char *delims, int *skip,
		  int* maxrec, int* reclen, int* nfields,
		  int field_type[], int* nrec,
		  int mrlist[], float *memptr, int mr_blk1[], int* mblk_size,
		  float mr_bad_flags[], char ***mr_c_ptr)
{
  float **numeric_fields  = (float **) malloc(sizeof(float*) * (*nfields));
  char ***text_fields     = (char ***) malloc(sizeof(char**) * (*nfields));
  float *bad_flags        = (float *)  malloc(sizeof(float) * (*nfields));
  int i, mr;
  int pinc = 8/sizeof(char*);  /* pointers spaced 8 bytes apart */

  for (i=0; i<(*nfields); i++)
    {
      mr = mrlist[i] - 1;  /* -1 for C indexing */
      /* 
	 compute separate pointer arrays for numeric and text fields
      */
      numeric_fields[i] = (float *) NULL;
      text_fields[i] = (char**) NULL;
      
      if (field_type[i] == FTYP_CHARACTER )
	{
	  text_fields[i] = (char**) memptr + (mr_blk1[mr]-1)*(*mblk_size);
	  mr_c_ptr[mr*pinc] = text_fields[i];
	}
      else if (field_type[i] != FTYP_MISSING )
	{
	  numeric_fields[i] = memptr + (mr_blk1[mr]-1)*(*mblk_size);
	  mr_c_ptr[mr*pinc] = (char**) NULL;
	}
      /*
	isolate the bad data flags that correspond to the numeric fields
      */
      if ( (field_type[i]!=FTYP_MISSING) && (field_type[i]!=FTYP_CHARACTER) )
	bad_flags[i] = mr_bad_flags[mr];
      else
	bad_flags[i] = 0.0;
    }

  /*
    at last we actually read the file
  */
  decode_file (fname, recptr, delims, skip, 
	       maxrec, reclen, nfields,
	       field_type, nrec,
	       numeric_fields, text_fields, bad_flags);

  free(numeric_fields);
  free(text_fields);
  free(bad_flags);

  return;
}


/*
 *
 decode_file - skip header & read an entire file based upon analysis suppied
 decode_file_ arguments
 fname - input filena0me
 recptr - buffer to hold file records
 reclen - size of record buffer
 delims - string containing delimiter in file (note that blank is
          considered a special delimiter representing any number of blanks)
 skip - number of heading record in file to be skipped
 maxrec - maximum number of records to read (beyond skipped header)
 nrec - out number of records actually read
 nfields - number of fields to decode on each record
 field_types - type of each field (input)
 numeric_fields - array to be filled with numeric field values
                (2D - # of vareiables by # of records to read)
 bad_flags - missing value flags (used for numeric fields, only) 
 text_fields - array to be filled with string pointers
 (2D - # of vareiables by # of records to read)
 *
 */

int decode_file (char* fname, char *recptr, char *delims, int *skip, 
			  int* maxrec, int* reclen, int* nfields,
			  int field_type[], int* nrec, float** numeric_fields,
			  char*** text_fields, float bad_flags[])
{

  FILE *fp;
  int slen, i;
  int pinc = 8/sizeof(char*);  /* pointers spaced 8 bytes apart */

  *nrec = 0;

  fp = fopen(fname,"r");

  /* skip initial records */
  for (i=0; i<*skip; i++)
    {  
      if (feof(fp)) break;
      fgets(recptr,*reclen,fp);
    }

  while (!feof(fp) && (*nrec)<(*maxrec))
    {

      if ( fgets(recptr,*reclen,fp) )
	{
      /* overwrite the newline record terminator with a NULL */
	  if ((slen = strlen(recptr)) > 0)
	    if (recptr[slen-1] == '\n')
	      recptr[slen-1] = NULL;
	  
	  decodeRec(recptr, delims, nfields, field_type, *nrec,
		    numeric_fields, text_fields, bad_flags);

#ifdef diagnostic_output	  /* ************* */
	  for (i=0; i<(*nfields); i++)
	    if (field_type[i] == FTYP_CHARACTER )
	      printf( "%d character: %s\n",i, (*(text_fields+i))[(*nrec)*pinc] );
	    else if (field_type[i] != FTYP_MISSING)
	      printf( "%d numeric: %f\n",i,(*(numeric_fields+i))[(*nrec)] );
#endif                            /* ************* */

	  (*nrec)++;
	}
    }

  fclose(fp);

  return 0; /* always OK ? */

}
/*
 *
 anal_file_ - determine the number of fields and types of each field
          - if an input record has missing fields (e.g. ",,") then keep looking
	  - if subsequent records produce differing analyses of a field then
	    flag it as a CHARACTER fields
 anal_file_ arguments
 fname - input filena0me
 recptr - buffer to hold file records
 reclen - size of record buffer
 delims - string containing delimiter in file (note that blank is
          considered a special delimiter representing any number of blanks)
 skip - number of heading record in file to be skipped
 maxrec - maximum number of records to read (beyond skipped header)
 nfields - (output) number of fields determined to be in file
 field_types - (output)type of each field
 max_fields - (input) maximum number of fields to consider
 *
 */

int FORTRAN(anal_file) (char* fname, char *recptr, char *delims, int* skip,
			int* maxrec, int* reclen, int* nfields,
			int field_type[], int *max_fields)     
{

  FILE *fp;
  int slen, i, rec;

  fp = fopen(fname,"r");

  /* skip initial records */
  for (rec=0; rec<*skip; rec++)
    {  
      if (feof(fp)) break;
      fgets(recptr,*reclen,fp);
    }

  /* initially set all field types to missing (no information) */
  for (i=0; i<(*max_fields); i++)
    field_type[i] = FTYP_MISSING;
  *nfields = 0;

  rec = 0;
  while ( !feof(fp) && rec<(*maxrec) )
    {
      if ( fgets(recptr,*reclen,fp) )
	{
	  rec++;

	  /* overwrite the newline record terminator with a NULL */
	  if ((slen = strlen(recptr)) > 0)
	    recptr[slen-1] = NULL;
	  
	  analRec(recptr, delims, nfields, field_type, *max_fields);

	  /* check for unknown field types */
	  i = 0;
	  while ( i<*nfields && (field_type[i] != FTYP_MISSING) )
	    i++;

	  /* success */
	  if (i == *nfields)
	    {
	      fclose(fp);
	      return 0;
	    }
	}
    }

  /*
    all records have been analyzed and missing fields still remain
  */
  fclose(fp);

  /* only an incomplete analysis of the file was possible */
  return FANAL_HAS_MISSING;

}

/*
 *
 decodeRec - parse and return values from a record based upon analysis suppied
 decodeRec arguments
 recptr - buffer holding file record
 delims - string containing delimiter in file (note that blank is
          considered a special delimiter representing any number of blanks)
 nfields - number of fields to decode on each record
 field_types - type of each field (input)
 rec - (input) current record number (points to location in output arrays)
 numeric_fields - array to be filled with numeric field values
                (2D - # of vareiables by # of records to read)
 bad_flags - missing value flags (used for numeric fields, only) 
 text_fields - array to be filled with string pointers
 (2D - # of vareiables by # of records to read)
 *
 */


int decodeRec(char *recptr, char *delims, int* nfields, int field_type[],
	      int rec,
	      float** numeric_fields, char*** text_fields, float bad_flags[])
{

  char *p, *pnext, str1[2], errstr[2];
  float dummy;
  int idummy1, idummy2, idummy3, i;
  char blankstr[] = " ";
  double days_1900 = 59958230400.0 / (60.*60.*24.);
  int pinc = 8/sizeof(char*);  /* pointers spacd 8 bytes apart */

  p = recptr;

  for (i=0; i<*nfields; i++) {
    pnext = nexstrtok(p, delims);
    if ( field_type[i] == FTYP_MISSING ) {
      /* do nothing -- a skipped field */

    } else if (p==NULL || *p == NULL) {
      /* missing data field */
      if ( field_type[i] == FTYP_CHARACTER ) {
	(*(text_fields+i))[rec*pinc] = (char *) malloc(sizeof(char)*2);
	strcpy( (*(text_fields+i))[rec*pinc], blankstr );
      }
      else {
	(*(numeric_fields+i))[rec] = bad_flags[i];
      }

    } else {
      switch (field_type[i]) {
	
	/* latitude */
      case FTYP_LAT:
	if (sscanf(p,"%f%1[Nn]%1s",&dummy,str1,errstr) == 2)
	  (*(numeric_fields+i))[rec] = dummy;
	else if (sscanf(p,"%f%1[Ss]",&dummy,str1) == 2)
	  (*(numeric_fields+i))[rec] = -1 * dummy;
	else
	  (*(numeric_fields+i))[rec] = bad_flags[i];
	break;
	
	/* longitude */
      case FTYP_LON:
	if (sscanf(p,"%f%[Ee]%1s",&dummy,str1,errstr) == 2)
	  (*(numeric_fields+i))[rec] = dummy;
	else if (sscanf(p,"%f%1[Ww]",&dummy,str1) == 2)
	  (*(numeric_fields+i))[rec] = -1 * dummy;
	else
	  (*(numeric_fields+i))[rec] = bad_flags[i];
	break;
	
	/* date */
      case FTYP_DATE:
	if (sscanf(p,"%d/%d/%d%1s",&idummy1,&idummy2,&idummy3,errstr) == 3) {
	  if (idummy3 < 20)   /* will break after 2019 or before 1920 */
	    idummy3 += 2000;
	  else
	    idummy3 += 1900;
	  (*(numeric_fields+i))[rec] =
	    days_from_day0_(&days_1900,&idummy3,&idummy1,&idummy2);
	} else if (sscanf(p,"%d-%d-%d%1s",
			  &idummy1,&idummy2,&idummy3,errstr) == 3)
	  (*(numeric_fields+i))[rec] =
	    days_from_day0_(&days_1900,&idummy1,&idummy2,&idummy3);
	else
	  (*(numeric_fields+i))[rec] = bad_flags[i];
	break;
	
	/* date */
      case FTYP_EURODATE:
	if (sscanf(p,"%d/%d/%d%1s",&idummy1,&idummy2,&idummy3,errstr) == 3) {
	  if (idummy3 < 20)   /* will break after 2019 or before 1920 */
	    idummy3 += 2000;
	  else
	    idummy3 += 1900;
	  (*(numeric_fields+i))[rec] =
	    days_from_day0_(&days_1900,&idummy3,&idummy1,&idummy2);
	} else if (sscanf(p,"%d-%d-%d%1s",
			  &idummy1,&idummy2,&idummy3,errstr) == 3)
	  (*(numeric_fields+i))[rec] =
	    days_from_day0_(&days_1900,&idummy1,&idummy2,&idummy3);
	else
	  (*(numeric_fields+i))[rec] = bad_flags[i];
	break;
	
	/* time */
      case FTYP_TIME:
	if (sscanf(p,"%d:%d:%f%1s",&idummy1,&idummy2,&dummy,errstr) == 3)
	  (*(numeric_fields+i))[rec] = idummy1 + idummy2/60. + dummy/3600.;
	else if (sscanf(p,"%d:%d%1s",&idummy1,&idummy2,errstr) == 2)
	  (*(numeric_fields+i))[rec] = idummy1 + idummy2/60.;
	else
	  (*(numeric_fields+i))[rec] = bad_flags[i];
	break;
	
	/* generic numeric field */
      case FTYP_NUMERIC:
	if ( sscanf(p,"%f%1s",&((*(numeric_fields+i))[rec]),errstr ) != 1)
	  (*(numeric_fields+i))[rec] = bad_flags[i];
	break;
	
	/* character field */
      case FTYP_CHARACTER:
	{
	  (*(text_fields+i))[rec*pinc] = (char *) malloc(sizeof(char)*strlen(p));
	  strcpy( (*(text_fields+i))[rec*pinc], p );
	}
	break;
	
      default:
	printf("internal error: unknown field type");
      }
    }

    p = pnext;
    }
}

/*
 *
 analRec - determine the number of fields and types of each field on one rec
	  - if subsequent records produce differing analyses of a field then
	    flag it as a CHARACTER fields
 analRec_ arguments
 recptr - buffer holding file record
 delims - string containing delimiter in file (note that blank is
          considered a special delimiter representing any number of blanks)
 nfields - (output) number of fields determined to be in file
 field_types - (output)type of each field
 max_fields - (input) maximum number of fields to consider
 *
 */

void analRec(char *recptr, char *delims, int* nfields, int field_type[],
	    int max_fields)
{

  char *p, *pnext, str1[2], latlon1[2];
  float dummy;
  int idummy1, idummy2, idummy3, i, nfields_in;

  p = recptr;
  nfields_in = *nfields;
  *nfields = 0;

  /* if information is available about a field for the first time, apply it
     if the analysis differs from a previous then call it a character field */
  while (p  != NULL) {
    pnext = nexstrtok(p, delims);
    if (*p == NULL)
      /* null field like 2 commas in a row */
      {
	/* retain previous information */
      } 
    else if (sscanf(p,"%d/%d/%d%1s",&idummy1,&idummy2,&idummy3,str1) == 3)
      /* date as mm/dd/yy */
      {
	if (field_type[(*nfields)] == FTYP_MISSING)
	  field_type[(*nfields)] = FTYP_DATE;
	else if (field_type[(*nfields)] != FTYP_DATE)
	  field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else if (sscanf(p,"%d-%d-%d%1s",&idummy1,&idummy2,&idummy3,str1) == 3)
      /* date as yyyy-mm-dd */
      {
	if (field_type[(*nfields)] == FTYP_MISSING)
	  field_type[(*nfields)] = FTYP_DATE;
	else if (field_type[(*nfields)] != FTYP_DATE)
	  field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else if (sscanf(p,"%d:%d:%f%1s",&idummy1,&idummy2,&dummy,str1) == 3)
      /* time as hh:mm:ss.s */
      {
	if (field_type[(*nfields)] == FTYP_MISSING)
	  field_type[(*nfields)] = FTYP_TIME;
	else if (field_type[(*nfields)] != FTYP_TIME)
	  field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else if (sscanf(p,"%d:%d%1s",&idummy1,&idummy2,str1) == 2)
      /* time as hh:mm */
      {
	if (field_type[(*nfields)] == FTYP_MISSING)
	  field_type[(*nfields)] = FTYP_TIME;
	else if (field_type[(*nfields)] != FTYP_TIME)
	  field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else if (sscanf(p,"%f%1[NnSs]%1s",&dummy,latlon1,str1) == 2)
      /* latitude */
      {
	if (field_type[(*nfields)] == FTYP_MISSING)
	  field_type[(*nfields)] = FTYP_LAT;
	else if (field_type[(*nfields)] != FTYP_LAT)
	  field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else if (sscanf(p,"%f%1[EeWw]%1s",&dummy,latlon1,str1) == 2)
      /* longitude */
      {
	if (field_type[(*nfields)] == FTYP_MISSING)
	  field_type[(*nfields)] = FTYP_LON;
	else if (field_type[(*nfields)] != FTYP_LON)
	  field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else if (sscanf(p,"%f%1s",&dummy,str1) == 2)
      /* digits followed by trash -- not a legal numeric field*/
      {
	field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else if (sscanf(p,"%f",&dummy) == 1)
      /* numeric field */
      {
	if (field_type[(*nfields)] == FTYP_MISSING)
	  field_type[(*nfields)] = FTYP_NUMERIC;
	else if (field_type[(*nfields)] != FTYP_NUMERIC)
	  field_type[(*nfields)] = FTYP_CHARACTER;
      }
    else
      /* any other text */
      field_type[(*nfields)] = FTYP_CHARACTER;
      
#ifdef diagnostic_output	  /* ************* */
    printf("%d %s:  %d\n", *nfields, p, field_type[(*nfields)]);
#endif                            /* ************* */

    if ( *nfields < max_fields )
      {    
	(*nfields)++;
	p = pnext;
      }
    else
      break;  /* cannot look at all of the fields */

    }

  /* if records have unequal length, return the longest found */
  *nfields = *nfields > nfields_in ? *nfields : nfields_in;

  return;
}

/*
 *
 *
 */

char *nexstrtok(char *s1, char *s2)

     /*
       like strtok but sensitive to multiple (non-white space) delimiters
       as significant. For example, 2 commas together indicate a missing field.
     */

     /* note - this routine will modify the s1 string */
{
  char *p1 = s1;
  char *p2;

  /* sanity check that we have a valid input record */
  if (p1 == NULL)
    return NULL;

  /* blanks are a special delimiter in that multiple blanks act as a single
     delimiter -- they do not indicate skipped fields
     Skip over leading blanks when blank is a delimiter character
  */
  if ( strpbrk(s2, " ") != NULL )
    while (*p1==' ')
      p1++;

  /* find the next delimiter */
  p2 = strpbrk( p1, s2 );

  if (p2 == NULL)
    return NULL;
  else
    {
      *p2 = 0;
      return p2+1;
    }
}

/*
 *
 save_delimited_info - allocate struct memoro and save special info needed
                       for delimited file reads
	nfields - number of fields (variables) in file
	field_type - field types for each variable
	delim - list of delimiters to use when reading the file
	ptr - returned pointer to structure
 *
 */
void FORTRAN(save_delimited_info) (int *nfields, int field_type[],
				   char *delim, DelimFileInfo **ptr)
{
  DelimFileInfo *fi = (DelimFileInfo *) calloc(1, sizeof(DelimFileInfo));
  int* _field_type  = (int *) malloc(sizeof(int) * (*nfields));
  char* _delim      = (char *) malloc(sizeof(char) * (int)strlen(delim));
  int i;

  for (i=0; i<*nfields; i++)
    _field_type[i] = field_type[i];

  strcpy(_delim, delim);

  fi->nfields = *nfields;
  fi->fieldType = _field_type;
  fi->delim = _delim;

  *ptr = fi;
  return;
}

void FORTRAN(get_delimited_info) (int *nfields, int field_type[],
				   char *delim, DelimFileInfo **ptr)
{

  int i,iout;
  DelimFileInfo *fi = *ptr;

  *nfields = fi->nfields;
  for (i=0; i<*nfields; i++)
    field_type[i] = (fi->fieldType)[i];
  strcpy(delim, fi->delim);
  return;
}

void FORTRAN(delete_delimited_info) (DelimFileInfo **ptr)
{
  DelimFileInfo *fi = *ptr;
  free(fi->fieldType);
  free(fi->delim);
  free(fi);
  return;
}

