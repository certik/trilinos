// @HEADER
// ***********************************************************************
// 
//    Thyra: Interfaces and Support for Abstract Numerical Algorithms
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
// ***********************************************************************
// @HEADER

#ifndef THYRA_DEFAULT_CLUSTERED_MPI_PRODUCT_VECTOR_SPACE_HPP
#define THYRA_DEFAULT_CLUSTERED_MPI_PRODUCT_VECTOR_SPACE_HPP

#include "Thyra_DefaultClusteredMPIProductVectorSpaceDecl.hpp"
#include "Thyra_MPIVectorSpaceBase.hpp"
#include "Thyra_DefaultClusteredMPIProductVector.hpp"
#include "Thyra_VectorSpaceDefaultBase.hpp"
#include "Thyra_VectorStdOps.hpp"
#include "Thyra_MultiVectorStdOps.hpp"
#include "Thyra_MPIVectorSpaceUtilities.hpp"

namespace Thyra {

// Constructors/Intializers/Accessors

template<class Scalar>
DefaultClusteredMPIProductVectorSpace<Scalar>::DefaultClusteredMPIProductVectorSpace()
  :isEuclidean_(false),globalDim_(0),clusterSubDim_(-1),clusterOffset_(-1)
{}

template<class Scalar>
DefaultClusteredMPIProductVectorSpace<Scalar>::DefaultClusteredMPIProductVectorSpace(
  const Teuchos::RefCountPtr<Teuchos::OpaqueWrapper<MPI_Comm> >   &intraClusterComm
  ,const int                                                      clusterRootRank
  ,const Teuchos::RefCountPtr<Teuchos::OpaqueWrapper<MPI_Comm> >  &interClusterComm
  ,const int                                                      numBlocks
  ,const Teuchos::RefCountPtr<const VectorSpaceBase<Scalar> >     vecSpaces[]
  )
{
  initialize(intraClusterComm,clusterRootRank,interClusterComm,numBlocks,vecSpaces);
}

template<class Scalar>
void DefaultClusteredMPIProductVectorSpace<Scalar>::initialize(
  const Teuchos::RefCountPtr<Teuchos::OpaqueWrapper<MPI_Comm> >   &intraClusterComm
  ,const int                                                      clusterRootRank
  ,const Teuchos::RefCountPtr<Teuchos::OpaqueWrapper<MPI_Comm> >  &interClusterComm
  ,const int                                                      numBlocks
  ,const Teuchos::RefCountPtr<const VectorSpaceBase<Scalar> >     vecSpaces[]
  )
{
  // Set state
  intraClusterComm_ = intraClusterComm.assert_not_null();
  clusterRootRank_ = clusterRootRank;
  interClusterComm_ = interClusterComm.assert_not_null();
  vecSpaces_.resize(numBlocks);
  isEuclidean_ = true;
  int clusterSubDim = 0;
  for( int k = 0; k < numBlocks; ++k ) {
    clusterSubDim += vecSpaces[k]->dim();
    if(!vecSpaces[k]->isEuclidean())
      isEuclidean_ = false;
    vecSpaces_[k] = vecSpaces[k];
  }
  // We must compute the offsets between clusters and the global dimension by
  // only involving the root process in each cluster.
  if(*interClusterComm_!=MPI_COMM_NULL) {
    clusterOffset_ = MPIVectorSpaceUtilities::computeLocalOffset(
      *interClusterComm_,clusterSubDim
      );
    globalDim_ = MPIVectorSpaceUtilities::computeGlobalDim(
      *interClusterComm_,clusterSubDim
    );
  }
  // Here must then scatter the values to all processes within each cluster.
  MPIVectorSpaceUtilities::broadcast(*intraClusterComm_,clusterRootRank_,&clusterOffset_);
  MPIVectorSpaceUtilities::broadcast(*intraClusterComm_,clusterRootRank_,&globalDim_);
  // ToDo: Make the above a single broadcast to be more efficient!
  //
  clusterSubDim_ = clusterSubDim;
  // ToDo: Do a global communication across all clusters to see if all vector
  // spaces are all Euclidean.  It is unlikely to be the case where all of the
  // clusters do not have the same vector spaces so I do not think this will
  // even come up.  But just in case, we should keep this in mind!
}

// Overridden form Teuchos::Describable

template<class Scalar>
std::string DefaultClusteredMPIProductVectorSpace<Scalar>::description() const
{
  std::ostringstream oss;
  oss << "DefaultClusteredMPIProductVectorSpace{";
  oss << "numBlocks="<<vecSpaces_.size();
  oss << ",globalDim="<<globalDim_;
  oss << ",clusterOffset="<<clusterOffset_;
  oss << "}";
  return oss.str();
}

// Public overridden from VectorSpaceBase

template<class Scalar>
Index DefaultClusteredMPIProductVectorSpace<Scalar>::dim() const
{
  return globalDim_;
}

template<class Scalar>
bool DefaultClusteredMPIProductVectorSpace<Scalar>::isCompatible(
  const VectorSpaceBase<Scalar>& vecSpc
  ) const
{
  if( &vecSpc == this )
    return true;
  // For now, I will just do the dynamic cast but in the future, we could get
  // more sophisticated.
  TEST_FOR_EXCEPT(true);
  return false;
}

template<class Scalar>
Teuchos::RefCountPtr< const VectorSpaceFactoryBase<Scalar> >
DefaultClusteredMPIProductVectorSpace<Scalar>::smallVecSpcFcty() const
{
  if(!vecSpaces_.size())
    return Teuchos::null;
  return vecSpaces_[0]->smallVecSpcFcty();
}

template<class Scalar>
Scalar DefaultClusteredMPIProductVectorSpace<Scalar>::scalarProd(
  const VectorBase<Scalar>& x, const VectorBase<Scalar>& y
  ) const
{
  Scalar scalarProds[1];
  this->scalarProds(x,y,&scalarProds[0]);
  return scalarProds[0];
}

template<class Scalar>
void DefaultClusteredMPIProductVectorSpace<Scalar>::scalarProds(
  const MultiVectorBase<Scalar>& X, const MultiVectorBase<Scalar>& Y
  ,Scalar scalar_prods[]
  ) const
{
  TEST_FOR_EXCEPTION(
    !isEuclidean_, std::logic_error
    ,"Error, have not implemented support for none Euclidean scalar products yet!"
    );
  return dots(X,Y,scalar_prods);
  // ToDo:
  // * Create DefaultClusteredMPIProductMultiVector subclass
  // * Cast X and Y this type
  // * Accumulate the scalar products across all of the blocks in this cluster
  // * Accumulate the full scalar products across all of the clusters
  //   using *interClusterComm
  // * Broadcast the final scalar products to all of the processes in
  //   a cluster using *intraClusterComm
}

template<class Scalar>
bool DefaultClusteredMPIProductVectorSpace<Scalar>::isEuclidean() const
{
  return isEuclidean_;
}

template<class Scalar>
bool DefaultClusteredMPIProductVectorSpace<Scalar>::hasInCoreView(
  const Range1D& rng, const EViewType viewType, const EStrideType strideType
  ) const
{
  return false; // ToDo: Figure this out for real!
}

template<class Scalar>
Teuchos::RefCountPtr< const VectorSpaceBase<Scalar> >
DefaultClusteredMPIProductVectorSpace<Scalar>::clone() const
{
  return Teuchos::rcp(new DefaultClusteredMPIProductVectorSpace<Scalar>(*this));
}

// Protected overridden from ProductVectorSpaceBase

template<class Scalar>
int DefaultClusteredMPIProductVectorSpace<Scalar>::numBlocks() const
{
  return vecSpaces_.size();
}

template<class Scalar>
Teuchos::RefCountPtr<const VectorSpaceBase<Scalar> >
DefaultClusteredMPIProductVectorSpace<Scalar>::getBlock(const int k) const
{
  TEST_FOR_EXCEPT( !( 0 <= k && k < vecSpaces_.size() ) );
  return vecSpaces_[k];
} 

// Protected overridden from VectorSpaceBase

template<class Scalar>
Teuchos::RefCountPtr<VectorBase<Scalar> >
DefaultClusteredMPIProductVectorSpace<Scalar>::createMember() const
{
  using Teuchos::rcp;
  return rcp(new DefaultClusteredMPIProductVector<Scalar>(rcp(this,false),NULL));
}

template<class Scalar>
Teuchos::RefCountPtr<MultiVectorBase<Scalar> >
DefaultClusteredMPIProductVectorSpace<Scalar>::createMembers(int numMembers) const
{
  return VectorSpaceDefaultBase<Scalar>::createMembers(numMembers);
  // ToDo: Provide an optimized multi-vector implementation when needed!
}

} // end namespace Thyra

#endif // THYRA_DEFAULT_CLUSTERED_MPI_PRODUCT_VECTOR_SPACE_HPP
