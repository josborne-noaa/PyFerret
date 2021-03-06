*  separate_str.F
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
*  ARE DISCLAIMED. IN NO EVENT SHALL NOAA/PMEL/TMAP BE LIABLE FOR ANY SPECIAL,
*  INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
*  RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
*  CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
*  CONNECTION WITH THE ACCESS, USE OR PERFORMANCE OF THIS SOFTWARE. 

* Ansley Manke June 2013
* Given a data variable and a ROWSIZE indicating trajectory length,
* Insert a bad-value between trajectories.
*
*
*  In this subroutine we provide information about
*  the function.  The user configurable information 
*  consists of the following:
* 
*  descr              Text description of the function
* 
*  num_args           Required number of arguments
* 
*  axis_inheritance   Type of axis for the result
*                        ( CUSTOM, IMPLIED_BY_ARGS, NORMAL, ABSTRACT )
*                        CUSTOM          - user defined axis
*                        IMPLIED_BY_ARGS - same axis as the incoming argument
*                        NORMAL          - the result is normal to this axis
*                        ABSTRACT        - an axis which only has index values
* 
*  piecemeal_ok       For memory optimization:
*                        axes where calculation may be performed piecemeal
*                        ( YES, NO )
* 
*
* For each argument we provide the following information:
*
* name               Text name for an argument
*
* unit               Text units for an argument
*
* desc               Text description of an argument
*
* axis_influence     Are this argument's axes the same as the result grid?
*                       ( YES, NO )
*
* axis_extend       How much does Ferret need to extend arg limits relative to result 
*


      SUBROUTINE separate_str_init(id)

      IMPLICIT NONE
      INCLUDE 'EF_Util.cmn'

      INTEGER id, arg

* **********************************************************************
*                                            USER CONFIGURABLE PORTION |
*                                                                      |
*                                                                      V

      CALL ef_set_desc(id,
     . 'Insert a bad-value between features, string variable' )

      CALL ef_set_num_args(id, 3)
      CALL ef_set_has_vari_args(id, NO)
      CALL ef_set_axis_inheritance_6d(id, ABSTRACT, NORMAL, NORMAL, 
     .                                             NORMAL, NORMAL, NORMAL)
      CALL ef_set_piecemeal_ok_6d(id, NO, NO, NO, NO, NO, NO)

      arg = 1
      CALL ef_set_arg_name(id, arg, 'A')
      CALL ef_set_arg_desc(id, arg, 'variable to sample on I axis')
      CALL ef_set_axis_influence_6d(id, arg, YES, NO, NO, NO, NO, NO)
      CALL ef_set_arg_type (id, arg, STRING_ARG)

      arg = 2
      CALL ef_set_arg_name(id, arg, 'Rowsize')
      CALL ef_set_arg_desc(id, arg,
     .          'array with per-feature row lengths')
      CALL ef_set_axis_influence_6d(id, arg, NO, NO, NO, NO, NO, NO)

      arg = 3
      CALL ef_set_arg_name(id, arg, 'FixLongitudes')
      CALL ef_set_arg_desc(id, arg,
     .          'ignored (included for parity with numeric fcn)')
      CALL ef_set_axis_influence_6d(id, arg, NO, NO, NO, NO, NO, NO)

      CALL ef_set_result_type (id, STRING_RETURN)

*                                                                      ^
*                                                                      |
*                                            USER CONFIGURABLE PORTION |
* **********************************************************************

      RETURN 
      END




      SUBROUTINE separate_str_result_limits(id)

      INCLUDE 'EF_Util.cmn'

      INTEGER id

* **********************************************************************
*                                            USER CONFIGURABLE PORTION |
*                                                                      |
*                                                                      V

*     Use utility functions  to get context information about the arguments.
*     indices.


      INTEGER arg_lo_ss(6,EF_MAX_ARGS), arg_hi_ss(6,EF_MAX_ARGS),
     .     arg_incr(6,EF_MAX_ARGS)

      INTEGER nx, nr, nee, lo_lim, hi_lim
      CHARACTER*100 errtxt

      CALL ef_get_arg_subscripts_6d(id, arg_lo_ss, arg_hi_ss, arg_incr)
      nx = arg_hi_ss(X_AXIS, ARG1) - arg_lo_ss(X_AXIS, ARG1) + 1

* will add a point for each row

      nr = arg_hi_ss(E_AXIS, ARG2) - arg_lo_ss( E_AXIS, ARG2) + 1
      nee = arg_hi_ss(X_AXIS, ARG2) - arg_lo_ss( X_AXIS, ARG2) + 1
      
      IF (nee .GT. nr) THEN
         IF (nr .NE. 1) THEN
	    errtxt = 'Error argument 2 must be 1D and on X or E axis'
	    GOTO 999
	 ENDIF
	 nr = nee
      ENDIF

      lo_lim = 1
      hi_lim = nx + nr - 1
      CALL ef_set_axis_limits(id, X_AXIS, lo_lim, hi_lim)

      RETURN
 999  CALL EF_BAIL_OUT(id, errtxt)

      END



*  In this subroutine we compute the result
* 
      SUBROUTINE separate_str_compute(id, arg_1, arg_2, arg_3, result)

      IMPLICIT NONE
      INCLUDE 'EF_Util.cmn'
      INCLUDE 'EF_mem_subsc.cmn'

      INTEGER id

      REAL arg_1(mem1lox:mem1hix, mem1loy:mem1hiy, mem1loz:mem1hiz, 
     .           mem1lot:mem1hit, mem1loe:mem1hie, mem1lof:mem1hif)
      REAL arg_2(mem2lox:mem2hix, mem2loy:mem2hiy, mem2loz:mem2hiz, 
     .           mem2lot:mem2hit, mem2loe:mem2hie, mem2lof:mem2hif)
      REAL arg_3(mem3lox:mem3hix, mem3loy:mem3hiy, mem3loz:mem3hiz, 
     .           mem3lot:mem3hit, mem3loe:mem3hie, mem3lof:mem3hif)

! We just know this is on the X axis and will be passed in and out correct.y.
      REAL result(memreslox:memreshix)

* After initialization, the 'res_' arrays contain indexing information 
* for the result axes.  The 'arg_' arrays will contain the indexing 
* information for each variable's axes. 

      INTEGER res_lo_ss(6), res_hi_ss(6), res_incr (6)
      INTEGER arg_lo_ss(6,EF_MAX_ARGS),
     .        arg_hi_ss(6,EF_MAX_ARGS),
     .        arg_incr (6,EF_MAX_ARGS)

      REAL bad_flag(EF_MAX_ARGS), bad_flag_result


* **********************************************************************
*                                            USER CONFIGURABLE PORTION |
*                                                                      |
*                                                                      V

      INTEGER i, i1, ir, k, m, slen
      INTEGER nx, msum, rend, nrr, nee, row_axis
      REAL fixlon, tol, nothing
      LOGICAL ifix
      CHARACTER*512 buff

      CALL ef_get_res_subscripts_6d(id, res_lo_ss, res_hi_ss, res_incr)
      CALL ef_get_arg_subscripts_6d(id, arg_lo_ss, arg_hi_ss, arg_incr)
      CALL ef_get_bad_flags(id, bad_flag, bad_flag_result)

      k = res_lo_ss(Z_AXIS)  ! index for unused dimensions

      nx = arg_hi_ss(X_AXIS, ARG1) - arg_lo_ss(X_AXIS, ARG1) + 1

      nee = arg_hi_ss(E_AXIS, ARG2) - arg_lo_ss( E_AXIS, ARG2) + 1
      nrr = arg_hi_ss(X_AXIS, ARG2) - arg_lo_ss( X_AXIS, ARG2) + 1

      row_axis = E_AXIS
      IF (nrr .GT. nee) row_axis = X_AXIS

      CALL ef_get_one_val(id, arg3, fixlon)
      ifix =  (fixlon .GT. 0.8)
      tol = 200.

      i1 = 1
      ir = 1
      msum = 0
      DO m = arg_lo_ss(row_axis,ARG2), arg_hi_ss(row_axis, ARG2)
         IF (row_axis .EQ. E_AXIS) THEN
	    rend = arg_2(k,k,k,k,m,k)
	 ELSE
	    rend = arg_2(m,k,k,k,k,k)
	 ENDIF

	 msum = msum + rend
	 IF (msum .GT. nx) msum = nx-1
	 DO i = i1, msum

	    CALL ef_get_string_arg_element_6d(id, ARG1, arg_1,
     .                       i,k,k,k,k,k, slen,buff)
	    CALL ef_put_string (buff, slen, result(ir) )
  
	    ir = ir + 1
	    IF (ir .GT. res_hi_ss(X_AXIS)) GOTO 9000
	 ENDDO
	 i1 = msum+1
	 buff = ' '
	 slen = 1
	 CALL ef_put_string (buff, slen, result(ir) )
	 ir = ir + 1
	 IF (ir .GT. res_hi_ss(X_AXIS)) GOTO 9000
      ENDDO

 9000 RETURN
c 999  CALL EF_BAIL_OUT(id, errtxt)
      RETURN

      END
