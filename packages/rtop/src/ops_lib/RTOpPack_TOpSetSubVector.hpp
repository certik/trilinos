// @HEADER
// ***********************************************************************
// 
// RTOp: Interfaces and Support Software for Vector Reduction Transformation
//       Operations
//                Copyright (2006) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Roscoe A. Bartlett (rabartl@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#ifndef RTOPPACK_TOP_SET_SUB_VECTOR_HPP
#define RTOPPACK_TOP_SET_SUB_VECTOR_HPP

#include "RTOpPack_RTOpTHelpers.hpp"
#include "RTOpPack_SparseSubVectorT.hpp"


namespace RTOpPack {


/** \brief Advanced transformation operator that assigns elements from a
 * sparse explicit vector.
 *
 * ToDo: Finish documentation!
 */
template<class Scalar>
class TOpSetSubVector : public RTOpT<Scalar> {
public:

  using RTOpT<Scalar>::apply_op;
  
  /** \brief . */
  typedef typename RTOpT<Scalar>::primitive_value_type primitive_value_type;

  /** \brief . */
  TOpSetSubVector();

  /** \brief . */
  TOpSetSubVector( const SparseSubVectorT<Scalar> &sub_vec );

  /** \brief . */
  void set_sub_vec( const SparseSubVectorT<Scalar> &sub_vec );

  /** @name Overridden from RTOpT */
  //@{

  /** \brief . */
  void get_op_type_num_entries(
    int* num_values
    ,int* num_indexes
    ,int* num_chars
    ) const;
  /** \brief . */
  void extract_op_state(
    int num_values
    ,primitive_value_type value_data[]
    ,int num_indexes
    ,index_type index_data[]
    ,int num_chars
    ,char_type char_data[]
    ) const;
  /** \brief . */
  void load_op_state(
    int num_values
    ,const primitive_value_type value_data[]
    ,int num_indexes
    ,const index_type index_data[]
    ,int num_chars
    ,const char_type char_data[]
    );
  /** \brief . */
  bool coord_invariant() const;

  //@}

  /** \name Deprecated. */
  //@{


  /** \brief Deprecated. */
  virtual void apply_op(
    const int num_vecs, const ConstSubVectorView<Scalar> sub_vecs[],
    const int num_targ_vecs, const SubVectorView<Scalar> targ_sub_vecs[],
    ReductTarget *reduct_obj
    ) const
    {
      apply_op(
        Teuchos::arrayView(sub_vecs, num_vecs),
        Teuchos::arrayView(targ_sub_vecs, num_targ_vecs),
        Teuchos::ptr(reduct_obj)
        );
    }

  //@}

protected:

  /** \name Overridden protected functions from RTOpT. */
  //@{

  /** \brief . */
  void apply_op_impl(
    const ArrayView<const ConstSubVectorView<Scalar> > &sub_vecs,
    const ArrayView<const SubVectorView<Scalar> > &targ_sub_vecs,
    const Ptr<ReductTarget> &reduct_obj
    ) const;

  //@}

private:

  enum { num_sub_vec_members = 6 };

  SparseSubVectorT<Scalar> sub_vec_;

}; // class TOpSetSubVector


} // namespace RTOpPack


#ifndef HAVE_TEUCHOS_EXPLICIT_INSTANIATION
#  include "RTOpPack_TOpSetSubVector_def.hpp"
#endif


#endif // RTOPPACK_TOP_SET_SUB_VECTOR_HPP
