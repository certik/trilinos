/* @HEADER@ */
/* ***********************************************************************
// 
//           TSFExtended: Trilinos Solver Framework Extended
//                 Copyright (2004) Sandia Corporation
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// **********************************************************************/
/* @HEADER@ */

#ifndef TSFSCALEDOPERATOR_HPP
#define TSFSCALEDOPERATOR_HPP

#include "TSFConfigDefs.hpp"
#include "TSFCoreLinearOp.hpp"
#include "TSFCoreVectorStdOps.hpp"
#include "TSFCoreVectorSpace.hpp"
#include "TSFOpDescribableByTypeID.hpp"
#include "Teuchos_RefCountPtr.hpp"

namespace TSFExtended
{
  /** 
   * ScaledOperator is a LinearOperator formed by multiplying
   * another LinearOperator by a constant scaling factor.
   */
  template <class Scalar> 
  class ScaledOperator : public OpDecribableByTypeID<Scalar>
  {
  public:
    /**
     * Construct the LinearOperator and the constant scaling factor.
     */
    ScaledOperator(const LinearOperator& op, const Scalar& scale)
      : op_(op.ptr()), scale_(scale) {;}

    /** Virtual dtor */
    virtual ~ScaledOperator(){;}

    /** 
     * ScaledOperator is a linear operator scaled by a scalar, which maps any 
     * vector in the domain space to the appropriate vector in the range space.
     */
    virtual void apply(
                       const ETransp            M_trans
                       ,const TSFCore::Vector<Scalar>    &x
                       ,TSFCore::Vector<Scalar>          *y
                       ,const Scalar            alpha = 1.0
                       ,const Scalar            beta  = 0.0
                       ) const 
    {
      op_->apply(M_trans, x, y, alpha, beta);
      Vt_S(y, scale_);
    }

    /** Return the domain of the operator. */
    virtual RefCountPtr< const TSFCore::VectorSpace<Scalar> > domain() const {return op_->domain();}
    }

    /** Return the range of the operator. */
    virtual RefCountPtr< const TSFCore::VectorSpace<Scalar> > range() const {return op_->range();}

  private:

    RefCountPtr<const LinearOperator> op_;
    Scalar scale_;
  };
}

#endif
