/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
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
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#ifndef SUNDANCE_EXPRFIELDWRAPPER_H
#define SUNDANCE_EXPRFIELDWRAPPER_H


#ifndef DOXYGEN_DEVELOPER_ONLY


#include "SundanceDefs.hpp"
#include "TSFHandleable.hpp"
#include "SundanceFieldBase.hpp"
#include "SundanceDiscreteFunction.hpp"
#include "SundanceDiscreteSpace.hpp"
#include "SundanceExpr.hpp"

namespace SundanceStdFwk
{
  namespace Internal
  {
    using namespace SundanceCore;
    using namespace SundanceCore;
    using namespace SundanceStdMesh;
    using namespace SundanceStdMesh::Internal;
    using namespace TSFExtended;
    /**
     *
     */
    class ExprFieldWrapper : public FieldBase
    {
    public:
      /** */
      ExprFieldWrapper(const Expr& expr) ;

      /** virtual dtor */
      virtual ~ExprFieldWrapper(){;}

      /** */
      virtual double getData(int cellDim, int cellID, int elem) const ;

      /** */
      virtual bool isDefined(int cellDim, int cellID, int elem) const ;

      /** */
      virtual bool isPointData() const {return isPointData_;}

      /* */
      GET_RCP(FieldBase);
      /**
       * Return the cell filter on which this field is defined 
       */
      virtual const CellFilter& domain() const 
        {
          return discreteSpace_.cellFilters(indices_[0]);
        }

    public:
      Expr expr_;

      const DiscreteFunctionData* df_;

      DiscreteSpace discreteSpace_;

      RefCountPtr<DOFMapBase> map_;
      
      Array<int> indices_;

      bool isPointData_;

    };
  }
}


#endif /* DOXYGEN_DEVELOPER_ONLY */

#endif
