/* /////////////////////////////////////////
// RTOp_obj_value_value_vtbl.h
//
// Copyright (C) 2001 Roscoe Ainsworth Bartlett
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the "Artistic License" (see the web site
//   http://www.opensource.org/licenses/artistic-license.html).
// This license is spelled out in the file COPYING.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// above mentioned "Artistic License" for more details.
*/

#ifndef RTOP_OBJ_VALUE_VALUE_VTBL_H
#define RTOP_OBJ_VALUE_VALUE_VTBL_H

#include "RTOp.h"

typedef double crap;

#ifdef __cplusplus
extern "C" {
#endif

/** Virtual function table for a simple object of a <tt>{RTOp_value_type,RTOp_value_type}</tt> pair.
  *
  * The functions do the following:
  * <ul>
  *	<li> <tt>get_obj_type_num_entries(...)</tt>
  *		<ul>
  *		<li> <tt>vtbl</tt>          [in] Ignored
  *		<li> <tt>instance_data</tt> [in] Ignored
  *		<li> <tt>num_values</tt>    [out] Returns 2
  *		<li> <tt>num_indexes</tt>   [out] Returns 0
  *		<li> <tt>num_chars</tt>     [out] Returns 0
  *		</ul>
  *	<li> <tt>obj_create(...)</tt>
  *		<ul>
  *		<li> <tt>vtbl</tt>          [in] Ignored
  *		<li> <tt>instance_data</tt> [in] Ignored
  *		<li> <tt>obj</tt>           [out] Points an allocated to a <tt>RTOp_value_value_type</tt>
  *                                 object initialized to <tt>{0.0,0.0}</tt>
  *		</ul>
  *	<li> <tt>obj_reinit(...)</tt>
  *		<ul>
  *		<li> <tt>vtbl</tt>          [in] Ignored
  *		<li> <tt>instance_data</tt> [in] Ignored
  *		<li> <tt>obj</tt>           [in/out] <tt>RTOp_value_value_type</tt> object reinitialized to <tt>{0.0,0.0}</tt>
  *		</ul>
  *	<li> <tt>obj_free(...)</tt>
  *		<ul>
  *		<li> <tt>vtbl</tt>          [in] Ignored
  *		<li> <tt>instance_data</tt> [in] Ignored
  *		<li> <tt>obj</tt>           [in/out] allocated object is freed and <tt>obj</tt>
  *                                 is set to NULL.
  *		</ul>
  *	<li> <tt>extract_state(...)</tt>
  *		<ul>
  *		<li> <tt>vtbl</tt>          [in] Ignored
  *		<li> <tt>instance_data</tt> [in] Ignored
  *     <li> <tt>obj</tt>           [in] Allocated <tt>RTOp_value_value_type</tt> object.
  *		<li> <tt>num_values</tt>    [in] Must be 2
  *     <li> <tt>value_data</tt>    [out] <ul><li><tt>value_data[0] = ((RTOp_value_value_type*)obj->value1)</tt></li>
  *                                       <li><tt>value_data[1] = ((RTOp_value_value_type*)obj->value2)</tt></li></ul>
  *		<li> <tt>num_indexes</tt>   [in] Must be 0
  *     <li> <tt>index_data</tt>    [out] Must be NULL
  *		<li> <tt>num_chars</tt>     [in] Must be 0
  *     <li> <tt>char_data</tt >    [out] Must be NULL
  *		</ul>
  *	<li> <tt>load_state(...)</tt>
  *		<ul>
  *		<li> <tt>vtbl</tt>          [in] Ignored
  *		<li> <tt>instance_data</tt> [in] Ignored
  *		<li> <tt>num_values</tt>    [in] Must be 2
  *     <li> <tt>value_data</tt>    [in] Array (size 2)
  *		<li> <tt>num_indexes</tt>   [in] Must be 0
  *     <li> <tt>index_data</tt>    [in] Must be NULL
  *		<li> <tt>num_chars</tt>     [in] Must be 0
  *     <li> <tt>char_data</tt >    [in] Must be NULL
  *     <li> <tt>obj</tt>           [in/out] If <tt>*obj == NULL</tt> then
  *                                 <tt>*obj = malloc(sizeof(RTOp_value_value_type))</tt> and
  *                                 <tt>*(RTOp_value_value_type)*obj = {value_data[0],value_data[1]}</tt>
  *		</ul>
  * </ul>
  */
/*@{*/

/*!< Virtual function table */
extern const struct RTOp_obj_type_vtbl_t   RTOp_obj_value_value_vtbl;

/*!< Object type structure for a value, index pair. */
struct RTOp_value_value_type {
	/*!<*/
	RTOp_value_type  value1;
	/*!<*/
	RTOp_value_type  value2;
};

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* RTOP_OBJ_VALUE_VALUE_VTBL_H */
