/*
Twines is a fast and simple method for drawing a curve through control points
Copyright (C) 1998  James Marsh

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*** twine.h
 * 
 *  Twine Library Version 1.0
 *  James Marsh
 *
 *  History
 *  29 December 1997       First version started.
 *  05 May 1998            DLL version, pack_twine added.
 *  17 December 1998       Duplicate point bug fixed. (Point exists error 
 *                         message added).
 * 
 *****************************************************************************/

#ifndef _INC_TWINE_H
#define _INC_TWINE_H 

#ifdef __cplusplus
extern "C" {
#endif             
  
/***** TYPES *****/

typedef double TW_VALUE, TW_INTERVAL;

typedef enum {TW_TYPE_Limit=0, TW_TYPE_Repeat=1, TW_TYPE_ContinueSmooth=2,
	      TW_TYPE_ContinueLinear=3, TW_TYPE_ContinueConstant=4} TW_TYPE;

typedef enum {TW_ERROR_OK=0, TW_ERROR_Failed=1,TW_ERROR_OutOfMemory=2, 
	      TW_ERROR_OutOfRange=3, TW_ERROR_PointExists=4} TW_ERROR;

typedef struct TW_HEADER * TW_TWINE;

/****** FUNCTIONS ******/

char * TW_GetErrorMessage(TW_ERROR error);
TW_ERROR TW_InitialiseTwine (TW_TWINE * twine, TW_TYPE type);
TW_ERROR TW_DeleteTwine (TW_TWINE twine);
TW_ERROR TW_SetTwineType (TW_TWINE twine, TW_TYPE newtype);
TW_ERROR TW_GetTwineType (TW_TYPE * type, TW_TWINE twine);
TW_ERROR TW_AddControl (TW_TWINE twine , TW_INTERVAL interval, TW_VALUE value);
  
  /* 
     BEGIN NOT IMPLEMENTED

TW_ERROR TW_DeleteTo (TW_TWINE twine, TW_INTERVAL interval);
TW_ERROR TW_DeleteFrom (TW_TWINE twine, TW_INTERVAL interval);

     END NOT IMPLEMENTED
  */

TW_ERROR TW_EvaluateTwine (TW_VALUE *value, TW_TWINE twine, 
			   TW_INTERVAL interval);
TW_ERROR TW_PackTwine (double **controlarray, int * entries, TW_TWINE twine);

#ifdef __cplusplus
}
#endif  
#endif
