/*@HEADER
// ***********************************************************************
//
//       Ifpack2: Tempated Object-Oriented Algebraic Preconditioner Package
//                 Copyright (2009) Sandia Corporation
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
// ***********************************************************************
//@HEADER
*/

#include "Ifpack2_BlockRelaxation_decl.hpp"

#ifdef HAVE_IFPACK2_EXPLICIT_INSTANTIATION

#include "Ifpack2_BlockRelaxation_def.hpp"
#include "Ifpack2_SparseContainer_decl.hpp"
#include "Ifpack2_SparseContainer_def.hpp"
#include "Ifpack2_LinearPartitioner_decl.hpp"
#include "Ifpack2_LinearPartitioner_def.hpp"
#include "Ifpack2_OverlappingPartitioner_def.hpp"

#include "Ifpack2_ILUT_decl.hpp"
#include "Ifpack2_ILUT_def.hpp"
#include "Ifpack2_ExplicitInstantiationHelpers.hpp"


// Note: Add similar explicit instantiation for SparseContainer<ILU> and DenseContainer when those get implemented

#define IFPACK2_INST_SPARSE_ILUT(CLASSNAME,S,LO,GO) \
  template class CLASSNAME<Tpetra::CrsMatrix<S,LO,GO,Kokkos::DefaultNode::DefaultNodeType,Kokkos::DefaultKernels<S,LO,Kokkos::DefaultNode::DefaultNodeType>::SparseOps>, \
			   Ifpack2::SparseContainer<Tpetra::CrsMatrix<S,LO,GO,Kokkos::DefaultNode::DefaultNodeType,Kokkos::DefaultKernels<S,LO,Kokkos::DefaultNode::DefaultNodeType>::SparseOps>, \
						    Ifpack2::ILUT<Tpetra::CrsMatrix<S,LO,LO,Kokkos::DefaultNode::DefaultNodeType,Kokkos::DefaultKernels<S,LO,Kokkos::DefaultNode::DefaultNodeType>::SparseOps> > > >



namespace Ifpack2 {
#ifdef HAVE_TPETRA_INST_FLOAT
IFPACK2_INST_SPARSE_ILUT(BlockRelaxation,float,int,int);
#endif
#ifdef HAVE_TPETRA_INST_DOUBLE
IFPACK2_INST_SPARSE_ILUT(BlockRelaxation,double,int,int);
#endif
#ifdef HAVE_TPETRA_INST_COMPLEX_FLOAT
IFPACK2_INST_SPARSE_ILUT(BlockRelaxation,std::complex<float>,int,int);
#endif
#ifdef HAVE_TPETRA_INST_COMPLEX_DOUBLE
IFPACK2_INST_SPARSE_ILUT(BlockRelaxation,std::complex<double>,int,int);
#endif
}

#endif
