// /////////////////////////////////////////////////////////////////////////////////////
// IterQuantityAccess.h
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

#ifndef ITER_QUANTITY_ACCESS_H
#define ITER_QUANTITY_ACCESS_H

#include "IterQuantity.h"

namespace GeneralIterationPack {

///
/** Iterface to Iteration Quanities.
  *
  * Quanities are updated, read and queried about using the
  * offset to the current iteration k.  For example to set
  * a quanity for the k+1 iteration you would call set_k(+1).
  * The functions ending with prefix_k(offset) are ment to suggest
  * prefix k offset.  For example:\\
  * has_storage_k(+1) => has storage k+1\\
  * get_k(-1) => get k-1\\
  *
  * Subclasses can implement this interface in a varity of ways.  But
  * they must follow a few simple rules: <ul>\\
  * <li>	Only forward transitions are allowed.  This effects the behavior of
  *			has_storage_k() and set_k().
  * </ul>
  *
  * The client should not have to worry about how much memory is advalible.  Instead
  * it is for the object that configures the client to provide the approprite
  * subclass to meet the needs of the client.
  */
template<class T_info>
class IterQuantityAccess : public IterQuantity {
public:

	///
	typedef	IterQuantity::NoStorageAvailable	NoStorageAvailable;
	///
	typedef IterQuantity::QuanityNotSet			QuanityNotSet;

	///
	/** Return a reference for the k #offset# iteration quanity.
	  *
	  * Clients call this member function to access a quantity for a
	  * given iteration or modify the quantity if has already been
	  * set for that iteration.
	  *
	  * Preconditions:<ul>
	  * <li> #this->updated_k(offset) == true# (throw QuanityNotSet)
	  * </ul>
	  */
	virtual T_info& get_k(int offset) = 0;

	///
	/** Return a const reference for the k #offset# iteration quanity.
	  *
	  * Clients call this member function to access a const quantity for a
	  * given iteration.
	  *
	  * Preconditions:<ul>
	  * <li> #this->updated_k(offset) == true# (throw QuanityNotSet)
	  * </ul>
	  */
	virtual const T_info& get_k(int offset) const = 0;

	///
	/** Return a reference to the storage location for the k #offset# iteration quanity.
	  *
	  * This function will return a reference to the storage for the k #offset# iteration
	  * quanity.  Calling this function may cause the loss of memory for a back iteration.
	  * If #will_loose_mem(back_offset,offset)# returns true then #updated_k(back_offset)#
	  * will return false after this function returns without throwing an exception.
	  *
	  * Precondtions:<ul>
	  * <li> #this->has_storage_k(offset) == true# (throw #NoStorageAvailable#)
	  * </ul>
	  *
 	  * Postcondtions:<ul>
	  * <li> #this->updated_k(offset) == true#
	  * <li> #this->updated_k(i) == false# for i in the set of { i : #this->will_loose_mem(i,offset) == true# }
	  * </ul> 
	  */
	virtual T_info& set_k(int offset) = 0;

};	// end class IterQuantityAccess 

}	// end namespace GeneralIterationPack

#endif	// ITER_QUANTITY_ACCESS_H
