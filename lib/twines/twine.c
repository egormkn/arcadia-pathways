/*
 Twines is a fast and simple method for drawing a curve through control points
 Copyright (C) 1998  James Marsh
               2009  Alice Villeger, University of Manchester
                     <alice.villeger@manchester.ac.uk>
 
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

/*** twine.c
 * 
 *  Twine Library Version 1.0
 *  James Marsh
 *
 *  History
 *  29 December 1997       First version started.
 *  05 May 1998            DLL version, pack_twine added.
 *  17 December 1998       Duplicate point bug fixed. (Added point exists
 *                         error message).
 *
 *  Modifications by Alice Villeger
 *  11 June 2009           Replaced rb.c and rb.h by newer versions (2000)
 *                         In twine.c, replaced rb_free_gtree by rb_free_tree
 *                         In twine.c, added explicit license information
 * 
 *****************************************************************************/

#include <stdlib.h>
#include <math.h>
#include "twine.h"
#include "rb.h"

/***  struct TW_HEADER
 *
 *  Purpose:
 *    Stores details of the twine structure
 *
 *  Contains:
 *    unsigned int controls - number of control points
 *    TW_TYPE type          - behaviour of twine
 *    TW_INTERVAL start     - lowest interval control point
 *    TW_VALUE start_value  - lowest interval control point value
 *    TW_INTERVAL end       - highest interval control point
 *    TW_VALUE end_value    - highest interval control point value
 *    Rb_node tree          - root of tree of control points
 *
 *****************************************************************************/

struct TW_HEADER
{
  unsigned int controls;
  TW_TYPE type;
  TW_INTERVAL start;
  TW_VALUE start_value;
  TW_INTERVAL end;
  TW_VALUE end_value;
  Rb_node tree;
};

/***
 *
 *  prototypes of static functions
 *
 *****************************************************************************/

static void get_start_coefficients (TW_VALUE *a, TW_VALUE *b, TW_VALUE *c,
				    TW_VALUE *d, TW_INTERVAL x2, TW_VALUE y2, 
				    TW_INTERVAL x3, TW_VALUE y3,
				    TW_INTERVAL x4, TW_VALUE y4);

static void get_middle_coefficients (TW_VALUE *a, TW_VALUE *b, TW_VALUE *c,
				     TW_VALUE *d, TW_INTERVAL x1, TW_VALUE y1,
				     TW_INTERVAL x2, TW_VALUE y2, 
				     TW_INTERVAL x3, TW_VALUE y3,
				     TW_INTERVAL x4, TW_VALUE y4);

static void get_end_coefficients (TW_VALUE *a, TW_VALUE *b, TW_VALUE *c,
				  TW_VALUE *d, TW_INTERVAL x2, TW_VALUE y2, 
				  TW_INTERVAL x3, TW_VALUE y3,
				  TW_INTERVAL x4, TW_VALUE y4);

static TW_ERROR evaluate_Limit (TW_VALUE * value, TW_TWINE twine, 
				TW_INTERVAL interval);

static TW_ERROR evaluate_Repeat (TW_VALUE * value, TW_TWINE twine, 
				 TW_INTERVAL interval);

static TW_ERROR evaluate_ContinueSmooth (TW_VALUE * value, TW_TWINE twine, 
					 TW_INTERVAL interval);

static TW_ERROR evaluate_ContinueLinear (TW_VALUE * value, TW_TWINE twine,
					 TW_INTERVAL interval);

static TW_ERROR evaluate_ContinueConstant (TW_VALUE * value, TW_TWINE twine,
					   TW_INTERVAL interval);

static int compare_intervals (void *i, void *j);

/***
 *
 *  definitions of functions
 *
 *****************************************************************************/

/***  static int compare_intervals(void *i, void *j)
 *
 *  Purpose:
 *    Compare two variables of type TW_INTERVAL and return whether they are
 *    equal, greater or smaller.
 *
 *
 *  Entry:
 *    void * i - pointer to first variable
 *    void * j - pointer to second variable
 *
 *    Parameters are passed as void* pointers rather than TW_INTERVAL* 
 *    pointers due to the interface to tree functions used. i and j
 *    must be both TW_INTERVAL values.
 *
 *  Exit:
 *	  RESULT=
 *      i equals j       :  0
 *      j smaller than i :  1
 *      j larger than i  : -1
 *
 *****************************************************************************/

static int 
compare_intervals (void *i, void *j)
{
  TW_INTERVAL I, J;

  I = *(TW_INTERVAL *) i;
  J = *(TW_INTERVAL *) j;

  if (I == J)
    return 0;
  if (I > J)
    return 1;
  else
    return -1;
}

/***  char *TW_GetErrorMessage (TW_ERROR error)
 *
 *  Purpose:
 *    Return a pointer to a string containing a description of the error
 *
 *  Entry:
 *    TW_ERROR error - error (as returned from most library functions)
 *
 *  Exit:
 *    RESULT is pointer to a string containing an error description.
 *    (This string must not be written to)
 *
 *****************************************************************************/

char *
TW_GetErrorMessage (TW_ERROR error)
{
  static char *messages[] =
  {"OK.", "Unspecified error.", "Out of memory.", "Point out of range.", "Point already exists."};

  if (error > 4 || error < 0)
    return "Error unknown - There was an invalid parameter to "
      "TW_GetErrorMessage.";
  else
    return messages[error];
}

/***  TW_ERROR TW_InitialiseTwine (TW_TWINE * twine, TW_TYPE type)
 *
 *  Purpose:
 *    Initialises a newly declared (or deleted) twine
 *
 *  Entry:
 *    TW_TWINE *twine - pointer to twine to initialise 
 *    TW_TYPE type    - behaviour of twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *twine - if successful this is updated to be a valid twine
 *
 *****************************************************************************/

TW_ERROR 
TW_InitialiseTwine (TW_TWINE * twine, TW_TYPE type)
{
  *twine = (TW_TWINE) malloc (sizeof (struct TW_HEADER));
  if (NULL == *twine)
    return TW_ERROR_OutOfMemory;

  (*twine)->controls = 0;
  (*twine)->type = type;
  (*twine)->start = 0;
  (*twine)->end = 0;
  (*twine)->tree = make_rb ();

  return TW_ERROR_OK;
}

/***  TW_ERROR TW_DeleteTwine (TW_TWINE twine)
 *
 *  Purpose:
 *    Delete an existing twine and free all memory associated with it
 *
 *  Entry:
 *    TW_TWINE twine - twine to delete
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *
 *****************************************************************************/

TW_ERROR
TW_DeleteTwine (TW_TWINE twine)
{
  // Modified by Alice Villeger [2009/06/11]
  //  rb_free_gtree(twine->tree); // old version
  rb_free_tree(twine->tree);
  // End of modification [2009/06/11]
	
  free (twine);
  return TW_ERROR_OK;
}

/***  TW_ERROR TW_SetTwineType (TW_TWINE twine, TW_TYPE newtype)
 *
 *  Purpose:
 *    Change the behaviour of an already initialised Twine
 *
 *  Entry:
 *    TW_TWINE twine  - twine to modify
 *    TW_TYPE newtype - new behaviour of twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *
 *****************************************************************************/

TW_ERROR 
TW_SetTwineType (TW_TWINE twine, TW_TYPE newtype)
{
  twine->type = newtype;
  return TW_ERROR_OK;
}

/***  TW_ERROR TW_GetTwineType (TW_TYPE * type, TW_TWINE twine)
 *
 *  Purpose:
 *    Read the behaviour of a Twine
 *
 *  Entry:
 *    TW_TYPE* type  - pointer to variable to hold the behaviour of twine
 *    TW_TWINE twine - twine to examine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *
 *****************************************************************************/

TW_ERROR 
TW_GetTwineType (TW_TYPE * type, TW_TWINE twine)
{
  *type = twine->type;
  return TW_ERROR_OK;
}

/***  TW_ERROR TW_AddControl (TW_TWINE twine, TW_INTERVAL interval, 
 *                            TW_VALUE value)
 *
 *  Purpose:
 *    Add a control point to a Twine
 *
 *  Entry:
 *    TW_TWINE twine       - twine to modify
 *    TW_INTERVAL interval - interval of control point
 *    TW_VALUE value       - value of control point
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *
 *****************************************************************************/

TW_ERROR 
TW_AddControl (TW_TWINE twine, TW_INTERVAL interval, TW_VALUE value)
{
  
  TW_INTERVAL *key;
  TW_VALUE *val;
  Rb_node node;

  
  /* Create a copy of the variables ready for storage in tree */
  key = (TW_INTERVAL *) malloc (sizeof (TW_INTERVAL));
  val = (TW_VALUE *) malloc (sizeof (TW_VALUE));
  if (NULL == key || NULL == val)
    return TW_ERROR_OutOfMemory;

  *key = interval;
  *val = value;

  /* Check if point exists */
  node = rb_find_gkey (twine->tree, &interval, compare_intervals);

  if ((node != twine->tree) && (*((TW_INTERVAL *) node->k.key) == interval))
      return TW_ERROR_PointExists;
  
  /* insert into tree */
  rb_insertg (twine->tree, key, val, compare_intervals);

  /* update twine control values as necessary */
  if (interval <= twine->start || 0 == twine->controls)
    {
      twine->start = interval;
      twine->start_value = value;
    }
  if (interval >= twine->end || 0 == twine->controls)
    {
      twine->end = interval;
      twine->end_value = value;
    }

  twine->controls++;
  return TW_ERROR_OK;
}

/***  TW_ERROR TW_EvaluateTwine (TW_VALUE * value, TW_TWINE twine, 
 *                               TW_INTERVAL interval)
 *
 *  Purpose:
 *    Evaluate the value of the Twine at a given interval
 *
 *  Entry:
 *    TW_VALUE *value      - pointer to variable to hold the value
 *    TW_TWINE twine       - twine to examine
 *    TW_INTERVAL interval - interval at which to evaluate the twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *value - contains the relevant value for the given interval (0 if error).
 *
 *****************************************************************************/

TW_ERROR 
TW_EvaluateTwine (TW_VALUE * value, TW_TWINE twine, TW_INTERVAL interval)
{
  
  static TW_ERROR (*evaluation_function[]) 
    (TW_VALUE *, TW_TWINE, TW_INTERVAL) =
  {
    evaluate_Limit,
    evaluate_Repeat,
    evaluate_ContinueSmooth,
    evaluate_ContinueLinear,
    evaluate_ContinueConstant
  };
  
  return (*evaluation_function[twine->type]) (value, twine, interval);
}

TW_ERROR TW_DeleteTo (TW_TWINE twine, TW_INTERVAL interval);
/* Delete all control points up before interval. Create a new control point at
   interval if necessary */

TW_ERROR TW_DeleteFrom (TW_TWINE twine, TW_INTERVAL interval);
/* Delete all control points after interval. Create a new control point at
   interval if necessary */

/***  static void get_start_coefficients (TW_VALUE *a, TW_VALUE *b,
 *                                        TW_VALUE *c, TW_VALUE *d,
 *                                        TW_INTERVAL x2, TW_VALUE y2,
 *                                        TW_INTERVAL x3, TW_VALUE y3,
 *                                        TW_INTERVAL x4, TW_VALUE y4)
 *
 *  Purpose:
 *    Return the cubic coefficients of the start segment of a twine. This will
 *    in fact be a quadratic curve.
 *
 *  Entry:
 *    TW_TYPE *a, *b, *c, *d - pointers to variables to hold coefficients
 *    TW_INTERVAL x2, x3, x4 - values of 3 relevant control points
 *    TW_VALUE y2, y3, y4    - intervals of 3 relevant control points
 *
 *  Exit:
 *    *a, *b, *c, *d - contain the coefficients of an equation of the form
 *                     y = ax^3 + bx^2 + c^x + d      
 *
 *****************************************************************************/

static void 
get_start_coefficients (TW_VALUE *a, TW_VALUE *b, TW_VALUE *c, TW_VALUE *d,
			TW_INTERVAL x2, TW_VALUE y2,
			TW_INTERVAL x3, TW_VALUE y3,
			TW_INTERVAL x4, TW_VALUE y4)
{
  *a = 0;
  *b = (y3 - y2 + ((y4 - y2) * (x2 - x3)) / (x4 - x2)) /
    ((x3 - x2) * (x2 - x3));
  *c = (((y4 - y2) / (x4 - x2)) - (TW_VALUE) 2 * (*b) * x3);
  *d = y3 - (*b) * (x3 * x3) - (*c) * x3;
}

/***  static void get_middle_coefficients (TW_VALUE *a, TW_VALUE *b,
 *                                         TW_INTERVAL x1, TW_VALUE y1,
 *                                         TW_VALUE *c, TW_VALUE *d,
 *                                         TW_INTERVAL x2, TW_VALUE y2,
 *                                         TW_INTERVAL x3, TW_VALUE y3,
 *                                         TW_INTERVAL x4, TW_VALUE y4)
 *
 *  Purpose:
 *    Return the cubic coefficients of the start segment of a twine. This will
 *    in fact be a quadratic curve.
 *
 *  Entry:
 *    TW_TYPE *a, *b, *c, *d     - pointers to variables to hold coefficients
 *    TW_INTERVAL x1, x2, x3, x4 - values of 3 relevant control points
 *    TW_VALUE y1, y2, y3, y4    - intervals of 3 relevant control points
 *
 *  Exit:
 *    *a, *b, *c, *d - contain the coefficients of an equation of the form
 *                     y = ax^3 + bx^2 + c^x + d      
 *
 *****************************************************************************/

static void
get_middle_coefficients (TW_VALUE *a, TW_VALUE *b, TW_VALUE *c, TW_VALUE *d, 
			 TW_INTERVAL x1, TW_VALUE y1,
			 TW_INTERVAL x2, TW_VALUE y2,
			 TW_INTERVAL x3, TW_VALUE y3,
			 TW_INTERVAL x4, TW_VALUE y4)
{
  TW_VALUE g2, g3; /* gradient at points 2 and 3 respectively */

  g2 = (y3 - y1) / (x3 - x1);
  g3 = (y4 - y2) / (x4 - x2);

  *a = (g2 * (x3 - x2) + g3 * (x3 - x2) + (TW_VALUE) 2 * (y2 - y3)) / 
    ((x3 - x2) * (x3 - x2) * (x3 - x2));
  *b = ((TW_VALUE) 3 * (*a) * (x2 * x2 - x3 * x3) - g2 + g3) / 
    ((TW_VALUE) 2 * (x3 - x2));
  *c = (TW_VALUE) - 3 * (*a) * (x2 * x2) - (TW_VALUE) 2 * (*b) * x2 + g2;
  *d = -(*a) * x2 * x2 * x2 - (*b) * (x2 * x2) - (*c) * x2 + y2;
}

/***  static void get_end_coefficients (TW_VALUE *a, TW_VALUE *b,
 *                                      TW_VALUE *c, TW_VALUE *d,
 *                                      TW_INTERVAL x1, TW_VALUE y1,
 *                                      TW_INTERVAL x2, TW_VALUE y2,
 *                                      TW_INTERVAL x3, TW_VALUE y3)
 *
 *  Purpose:
 *    Return the cubic coefficients of the start segment of a twine. This will
 *    in fact be a quadratic curve.
 *
 *  Entry:
 *    TW_TYPE *a, *b, *c, *d - pointers to variables to hold coefficients
 *    TW_INTERVAL x1, x2, x3 - values of 3 relevant control points
 *    TW_VALUE y1, y2, y3    - intervals of 3 relevant control points
 *
 *  Exit:
 *    *a, *b, *c, *d - contain the coefficients of an equation of the form
 *                     y = ax^3 + bx^2 + c^x + d      
 *
 *****************************************************************************/

static void
get_end_coefficients (TW_VALUE *a, TW_VALUE *b, TW_VALUE *c, TW_VALUE *d, 
		      TW_INTERVAL x1, TW_VALUE y1,
		      TW_INTERVAL x2, TW_VALUE y2, 
		      TW_INTERVAL x3, TW_VALUE y3)
{
  TW_VALUE g2; /* gradient at point 2 */

  g2 = (y3 - y1) / (x3 - x1);

  *a = 0;
  *b = (g2 * (x2 - x3) - y2 + y3) / ((x3 - x2) * (x3 - x2));
  *c = g2 - (TW_VALUE) 2 * (*b) * x2;
  *d = -(*b) * x2 * x2 - (*c) * x2 + y2;
}

/***  static TW_ERROR get_coefficients_within_limits (TW_VALUE *a, TW_VALUE *b, 
 *                                                    TW_VALUE *c, TW_VALUE *d,
 *                                                    TW_TWINE twine,
 *                                                    TW_INTERVAL interval)
 *
 *  Purpose:
 *    Evaluate the twine within range defined by first and last control points
 *
 *  Entry:
 *    TW_VALUE *value      - pointer to variable to hold the value
 *    TW_TWINE twine       - twine to examine
 *    TW_INTERVAL interval - interval at which to evaluate the twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *value - contains the relevant value for the given interval.
 *
 *****************************************************************************/

static TW_ERROR 
get_coefficients_within_limits (TW_VALUE *a, TW_VALUE *b, TW_VALUE *c, 
                                TW_VALUE *d, TW_TWINE twine,
                                TW_INTERVAL interval)
{
  Rb_node node1, node2, node3, node4;
  TW_VALUE y1, y2, y3, y4;
  TW_INTERVAL x1, x2, x3, x4;
  
  *a=0; *b=0; *c=0; *d=0;

  if (1 == twine->controls)	/* One control - constant value */
    {
      *d = twine->start_value;
      return TW_ERROR_OK;
    }
  else if (2 == twine->controls)	/* Two controls - straight line */
    {
      *c=(twine->end_value - twine->start_value) / (twine->end - twine->start);
      *d=twine->start_value - ((*c) * twine->start);

      return TW_ERROR_OK;
    }
  node3 = rb_find_gkey (twine->tree, &interval, compare_intervals);

  /* check if first point */
  if (twine->start == interval)
    {
      /* rb_find_gkey returns the point or the closest one greater than 
         the value looked upfirst point. We need to therefore get the
         second point in order to work out the curve that passes through
         the first point. */
      node3=rb_next (node3); 
    }

  x3 = *((TW_INTERVAL *) node3->k.key);
  y3 = *((TW_VALUE *) node3->v.val);

  /* must be at least one node preceding value to be evaluated as we
     are within the defined range of the curve */

  node2 = rb_prev (node3);

  x2 = *((TW_INTERVAL *) node2->k.key);
  y2 = *((TW_VALUE *) node2->v.val);

  node1 = rb_prev (node2);
  node4 = rb_next (node3);

  /* check if we are in first or last segments of the curve */

  if (twine->tree == node1)	/* start segment */
    {
      x4 = *((TW_INTERVAL *) node4->k.key);
      y4 = *((TW_VALUE *) node4->v.val);

      get_start_coefficients (a, b, c, d, x2, y2, x3, y3, x4, y4);
    }
  else if (twine->tree == node4)	/* end segment */
    {
      x1 = *((TW_INTERVAL *) node1->k.key);
      y1 = *((TW_VALUE *) node1->v.val);

      get_end_coefficients (a, b, c, d, x1, y1, x2, y2, x3, y3);
    }
  else
    {
      /* middle segment */
      x1 = *((TW_INTERVAL *) node1->k.key);
      y1 = *((TW_VALUE *) node1->v.val);
      
      x4 = *((TW_INTERVAL *) node4->k.key);
      y4 = *((TW_VALUE *) node4->v.val);
      
      get_middle_coefficients (a, b, c, d, x1, y1, x2, y2, x3, y3, x4, y4);
    }
  
  return TW_ERROR_OK;
}

/***  static TW_ERROR evaluate_Limit (TW_VALUE * value, TW_TWINE twine, 
 *                                    TW_INTERVAL interval)
 *
 *  Purpose:
 *    Evaluate a limited twine.
 *
 *  Entry:
 *    TW_VALUE *value      - pointer to variable to hold the value
 *    TW_TWINE twine       - twine to examine
 *    TW_INTERVAL interval - interval at which to evaluate the twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *value - contains the relevant value for the given interval.
 *
 *****************************************************************************/

static TW_ERROR 
evaluate_Limit (TW_VALUE * value, TW_TWINE twine, TW_INTERVAL interval)
{
  TW_VALUE a, b, c, d;
  TW_ERROR result;
  
  *value = 0;
  
  if (interval > twine->end || interval < twine->start || 0 == twine->controls)
      result = TW_ERROR_OutOfRange;
  else
      result = get_coefficients_within_limits (&a, &b, &c, &d, twine, interval);
  if (TW_ERROR_OK == result)  
       *value = a * (interval * interval * interval) + b * (interval * interval)
                + c * interval + d;
   
  return result;
 }

/***  static TW_ERROR evaluate_ContinueSmooth (TW_VALUE * value, 
 *                                             TW_TWINE twine, 
 *                                             TW_INTERVAL interval)
 *
 *  Purpose:
 *    Evaluate a smoothly continuing twine.
 *
 *  Entry:
 *    TW_VALUE *value      - pointer to variable to hold the value
 *    TW_TWINE twine       - twine to examine
 *    TW_INTERVAL interval - interval at which to evaluate the twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *value - contains the relevant value for the given interval.
 *
 *****************************************************************************/

static TW_ERROR 
evaluate_ContinueSmooth (TW_VALUE * value, TW_TWINE twine, 
                         TW_INTERVAL interval)
{
  TW_VALUE a, b, c, d;
  TW_ERROR result;

  *value = 0;
  a=0; b=0; c=0; d=0;

  if (0 == twine->controls)
	{
      result = TW_ERROR_OK;
	}
  else if (interval >= twine->end)
	{
      /* continue quadratic end curve */
      result=get_coefficients_within_limits (&a, &b, &c, &d, twine, 
                                             twine->end);      
 	}
  else if ( interval <= twine->start)
	{
      /* continue quadratic start curve */
      result=get_coefficients_within_limits (&a, &b, &c, &d, twine, 
                                             twine->start);
	}  
  else
  {
      result = get_coefficients_within_limits (&a, &b, &c, &d, twine, interval);
  }   
  if (TW_ERROR_OK == result)  
    *value = a * (interval * interval * interval) + b * (interval * interval)
             + c * interval + d;

  return result;
}

/***  static TW_ERROR evaluate_ContinueLinear (TW_VALUE * value, 
 *                                             TW_TWINE twine, 
 *                                             TW_INTERVAL interval)
 *
 *  Purpose:
 *    Evaluate a linearly continuing twine.
 *
 *  Entry:
 *    TW_VALUE *value      - pointer to variable to hold the value
 *    TW_TWINE twine       - twine to examine
 *    TW_INTERVAL interval - interval at which to evaluate the twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *value - contains the relevant value for the given interval.
 *
 *****************************************************************************/

static TW_ERROR 
evaluate_ContinueLinear (TW_VALUE * value, TW_TWINE twine, 
                         TW_INTERVAL interval)
{
  TW_VALUE a, b, c, d, gradient;
  TW_ERROR result;
  
  *value = 0;
  a=0; b=0; c=0; d=0;
  
  if (0 == twine->controls)
    {
      result = TW_ERROR_OK;
    }
  else if (interval >= twine->end)
    {
      
      result=get_coefficients_within_limits (&a, &b, &c, &d, twine, 
                                             twine->end);

      /* a will be 0 as quadratic curve at start */
      gradient = 2 * b * twine->end + c;
      d = twine->end_value - gradient * twine->end;
      c = gradient;
      b = 0;
    }
  else if ( interval <= twine->start)
    {
      result=get_coefficients_within_limits (&a, &b, &c, &d, twine, 
                                             twine->start);
      /* a will be 0 as quadratic curve at start */
      gradient = 2 * b * twine->start + c;
      d = twine->start_value - gradient * twine->start;
      c = gradient;
      b = 0;
    }  
  else
    {
      result = get_coefficients_within_limits (&a, &b, &c, &d, twine, 
					       interval);
    }   
  if (TW_ERROR_OK == result)  
    *value = a * (interval * interval * interval) + b * (interval * interval)
      + c * interval + d;
  
  return result;
}

/***  static TW_ERROR evaluate_ContinueConstant (TW_VALUE * value, 
 *                                              TW_TWINE twine, 
 *                                              TW_INTERVAL interval)
 *
 *  Purpose:
 *    Evaluate a constantly continuing twine.
 *
 *  Entry:
 *    TW_VALUE *value      - pointer to variable to hold the value
 *    TW_TWINE twine       - twine to examine
 *    TW_INTERVAL interval - interval at which to evaluate the twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *value - contains the relevant value for the given interval.
 *
 *****************************************************************************/

static TW_ERROR 
evaluate_ContinueConstant (TW_VALUE * value, TW_TWINE twine, 
                           TW_INTERVAL interval)
{
  TW_VALUE a, b, c, d;
  TW_ERROR result;
  TW_VALUE y1, y2, y3, y4;
  TW_INTERVAL x1, x2, x3, x4;
  Rb_node first_node, second_node, last_but_one_node, last_node;
  a=0; b=0; c=0; d=0;
  *value = 0;

  if (0 == twine->controls)
    {
      result = TW_ERROR_OK;      
    }
  else if (1 == twine->controls)	/* One control - constant value */
    {
      d=twine->start_value;
      result = TW_ERROR_OK;      
    }
  else if (interval >= twine->end)
    {
      d = twine->end_value;
      result = TW_ERROR_OK;
    }
  else if ( interval <= twine->start)
    {
      d = twine->start_value;
      result = TW_ERROR_OK;
    }  
  else if (2 == twine->controls)	/* Two controls - S shape */
    {
      /* set points 1 and 4 so as to start and end horizontal */
      x1=0;
      y1=0;
      x4=1;
      y4=0;
      x2=twine->start;
      y2=twine->start_value;
      x3=twine->end;
      y3=twine->end_value;
      get_middle_coefficients (&a, &b, &c, &d, 
			       x1, y1, x2, y2, x3, y3, x4, y4);

      result = TW_ERROR_OK;
    }  
  else
    {
      first_node=rb_first(twine->tree);
      second_node=rb_next(first_node);
      last_node=rb_last(twine->tree);
      last_but_one_node=rb_prev(last_node);

      if (interval > *((TW_INTERVAL *) last_but_one_node->k.key))
	{
	  x1=*((TW_INTERVAL *) rb_prev(last_but_one_node)->k.key);
	  y1=*((TW_VALUE *) rb_prev(last_but_one_node)->v.val);
	  x2=*((TW_INTERVAL *) last_but_one_node->k.key);
	  y2=*((TW_VALUE *) last_but_one_node->v.val);
	  x3=*((TW_INTERVAL *) last_node->k.key);
	  y3=*((TW_VALUE *) last_node->v.val);

	  /* end horizontal */
	  x4=x2+1;
	  y4=y2;
	  
	  get_middle_coefficients (&a, &b, &c, &d, 
				   x1, y1, x2, y2, x3, y3, x4, y4);
	  result=TW_ERROR_OK;
	}
      else if (interval < *((TW_INTERVAL *) second_node->k.key))
	{
	  x2=*((TW_INTERVAL *) first_node->k.key);
	  y2=*((TW_VALUE *) first_node->v.val);
	  x3=*((TW_INTERVAL *) second_node->k.key);
	  y3=*((TW_VALUE *) second_node->v.val);
	  x4=*((TW_INTERVAL *) (rb_next(second_node)->k.key));
	  y4=*((TW_VALUE *) (rb_next(second_node)->v.val));
	  
	  x1=x3-1;
	  y1=y3;
	  
	  get_middle_coefficients (&a, &b, &c, &d, 
				   x1, y1, x2, y2, x3, y3, x4, y4);
	  result=TW_ERROR_OK;
	}
      else
	{
	  result = get_coefficients_within_limits (&a, &b, &c, &d, twine, interval);
	}
    }
  if (TW_ERROR_OK == result)  
    *value = a * (interval * interval * interval) + b * (interval * interval)
      + c * interval + d;
  
  return result;
}

/***  static TW_ERROR evaluate_Repeat (TW_VALUE * value, TW_TWINE twine, 
 *                                     TW_INTERVAL interval)
 *
 *  Purpose:
 *    Evaluate a repeating twine.
 *
 *  Entry:
 *    TW_VALUE *value      - pointer to variable to hold the value
 *    TW_TWINE twine       - twine to examine
 *    TW_INTERVAL interval - interval at which to evaluate the twine
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *value - contains the relevant value for the given interval.
 *
 *****************************************************************************/

static TW_ERROR 
evaluate_Repeat (TW_VALUE * value, TW_TWINE twine, TW_INTERVAL interval)
{
  TW_VALUE a, b, c, d;
  TW_ERROR result;
  TW_VALUE multiplier, remainder;
  Rb_node first_node, second_node, last_but_one_node, last_node;
  TW_VALUE y1, y2, y3, y4;
  TW_INTERVAL x1, x2, x3, x4;

  a=0; b=0; c=0; d=0;

  if (0 == twine->controls)
    {
      result = TW_ERROR_OK;      
    }
  else if (1 == twine->controls)	/* One control - constant value */
    {
      d=twine->start_value;
      result = TW_ERROR_OK;      
    }
  else if (2 == twine->controls)	/* Two controls - straight line */
    {
      c=(twine->end_value - twine->start_value) / (twine->end - twine->start);
      d=twine->start_value - (c * twine->start);
      result = TW_ERROR_OK;
    }  
  else if (interval > twine->end || interval < twine->start)
    {
      if(interval > 0)
	multiplier = ceil(interval/(twine->end - twine->start));
      else
	multiplier = floor(interval/(twine->end - twine->start));
      
      remainder = interval - (multiplier * (twine->end - twine->start));
      result = evaluate_Repeat(value, twine, remainder);
      if (TW_ERROR_OK==result)
	*value=*value + multiplier * (twine->end_value-twine->start_value);
      return result;
    }
  else 
    {
      first_node=rb_first(twine->tree);
      second_node=rb_next(first_node);
      last_node=rb_last(twine->tree);
      last_but_one_node=rb_prev(last_node);

      if (interval > *((TW_INTERVAL *) last_but_one_node->k.key))
	{
	  x1=*((TW_INTERVAL *) rb_prev(last_but_one_node)->k.key);
	  y1=*((TW_VALUE *) rb_prev(last_but_one_node)->v.val);
	  x2=*((TW_INTERVAL *) last_but_one_node->k.key);
	  y2=*((TW_VALUE *) last_but_one_node->v.val);
	  x3=*((TW_INTERVAL *) last_node->k.key);
	  y3=*((TW_VALUE *) last_node->v.val);
	  x4=*((TW_INTERVAL *) second_node->k.key) -
	    *((TW_INTERVAL *) first_node->k.key)+x3;
	  y4=*((TW_VALUE *) second_node->v.val) + twine->end_value - 
	    twine->start_value;
	  
	  get_middle_coefficients (&a, &b, &c, &d, 
				   x1, y1, x2, y2, x3, y3, x4, y4);
	  result=TW_ERROR_OK;
	  
	}
      else if (interval < *((TW_INTERVAL *) second_node->k.key))
	{
	  x2=*((TW_INTERVAL *) first_node->k.key);
	  y2=*((TW_VALUE *) first_node->v.val);
	  x3=*((TW_INTERVAL *) second_node->k.key);
	  y3=*((TW_VALUE *) second_node->v.val);
	  x4=*((TW_INTERVAL *) (rb_next(second_node)->k.key));
	  y4=*((TW_VALUE *) (rb_next(second_node)->v.val));

	  x1=*((TW_VALUE *) last_but_one_node->k.key) -
	    *((TW_VALUE *) last_node->k.key) + x2;
	  y1=*((TW_INTERVAL *) last_but_one_node->v.val) - 
	    (twine->end_value - twine->start_value);
	  
	  get_middle_coefficients (&a, &b, &c, &d, 
				   x1, y1, x2, y2, x3, y3, x4, y4);
	  result=TW_ERROR_OK;
	}
      else
	{
	  result = get_coefficients_within_limits (&a, &b, &c, &d, twine, 
						   interval);
	}
    }
  
  if (TW_ERROR_OK == result)  
    *value = a * (interval * interval * interval) + b * 
      (interval * interval) + c * interval + d;
  
  return result;
}

/***  TW_ERROR TW_PackTwine (double *controlarray, int *entries) 
 *
 *  Purpose:
 *    Pack a twine and return an array containing *entries control
 *    point values in the form [interval, value, interval, value, ...]
 *
 *  Entry:
 *    double **controlarray - pointer to variable to hold the address of array
 *    int *entries          - pointer to variable to hold the number of points
 *
 *  Exit:
 *    RESULT contains an error value indicating success or failure
 *    *controlarray - contains the address of array
 *    *entries      - contains the number of control pairs
 *
 *****************************************************************************/

TW_ERROR 
TW_PackTwine (double **controlarray, int *entries, TW_TWINE twine) 
{
  Rb_node current_node;
  int node_count;

  *controlarray=calloc(sizeof(double), twine->controls * 2);
  if (NULL == *controlarray)
    return TW_ERROR_OutOfMemory;

  *entries = twine->controls;

  current_node = rb_first(twine->tree);
  for(node_count=0; node_count<*entries; node_count++)
    {
      (*controlarray)[node_count*2]  =*((double *) current_node->k.key);
      (*controlarray)[node_count*2+1]=*((double *) current_node->v.val);

      current_node = rb_next(current_node);
    }
  return TW_ERROR_OK;
} 




