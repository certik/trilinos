// ///////////////////////////////////////////////////////////////////////////
// MatrixVectorTemplateOp.h

#ifndef MATRIX_VECTOR_TEMPLATE_OP_H
#define MATRIX_VECTOR_TEMPLATE_OP_H

#include <stdexcept>
#include "SparseLinAlgPackTypes.h"

namespace SparseLinAlgPack {

/** @name {\bf Templated Matrix-Vector Operations}.
  *
  * These are the declarations (MatrixVectorTemplateOp.h) for template functions for
  * performing selected matrix-vector and matrix-matrix operations.  The templated
  * matrix type must have the following interface:
  * \begin{itemize}
  * \item #T_Matrix::size_type# - Member type for the type returned by #rows# and #cols#
  * \item #T_Matrix::row_type# - Member type for the type returned by #row(i)#
  * \item #T_Matrix::col_type# - Member type for the type returned by #col(j)#
  * \item #T_Matrix::size_type T_Matrix::rows() const#
  * \item #T_Matrix::size_type T_Matrix::cols() const#
  * \item #const T_Matrix::row_type& T_Matrix::row(size_type i) const# \
  *		- Returns a reference to the ith row (1,2,...,#T_Matrix::rows()#).
  * \item #const T_Matrix::col_type& T_Matrix::col(size_type j) const# \ 
  *		- Returns a reference to the ith row (1,2,...,#T_Rect_Matrix::rows()#).
  * \end{itemize}
  *
  * In addition, the following operations (UML notation) must be accessible for the types
  * #T_Matrix::row_type# and #T_Matrix::col_type# when these template functions
  * are instantiated:
  *
  * assign(v_lhs:Vector&,v_rhs:T_Matrix::const row_type&)
  * assign(vs_lhs:VectorSlice&,v_rhs:const T_Matrix::row_type&)
  * dot(vs_rhs1:const VectorSlice&, vs_rhs2: const T_Matrix::row_type&):value_type 
  * dot(vs_rhs1:const VectorSlice&, vs_rhs2: const T_Matrix::col_type&):value_type 
  */
//@{

// ////////////////////////
// assign to a dense matrix

/// gm_lhs = T_M (templated matrix type T_M)
template<class T_Matrix>
void assign(GenMatrix& gm_lhs, const T_Matrix& gm_rhs, BLAS_Cpp::Transp trans_rhs);

/// gms_lhs = T_M (templated matrix type T_M)
template<class T_Matrix>
void assign(GenMatrixSlice& gms_lhs, const T_Matrix& gm_rhs, BLAS_Cpp::Transp trans_rhs);

// /////////////////////////////
// Matrix-Vector multiplication

/// v_lhs = T_M * vs_lhs (templated matrix type T_M)
template<class T_Matrix>
void V_MtV(Vector& v_lhs, const T_Matrix& gm_rhs1, BLAS_Cpp::Transp trans_rhs1
	, const VectorSlice& vs_rhs2);

/// vs_lhs = T_M * vs_lhs (templated matrix type T_M)
template<class T_Matrix>
void V_MtV(VectorSlice& v_lhs, const T_Matrix& gm_rhs1, BLAS_Cpp::Transp trans_rhs1
	, const VectorSlice& vs_rhs2);

//@}

}

#endif // MATRIX_VECTOR_TEMPLATE_OP_H
