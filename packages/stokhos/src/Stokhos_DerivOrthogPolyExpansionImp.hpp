// $Id$ 
// $Source$ 
// @HEADER
// ***********************************************************************
// 
//                           Stokhos Package
//                 Copyright (2008) Sandia Corporation
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
// Questions? Contact Eric T. Phipps (etphipp@sandia.gov).
// 
// ***********************************************************************
// @HEADER

#include "Teuchos_TestForException.hpp"
#include "Stokhos_DynamicArrayTraits.hpp"

template <typename ordinal_type, typename value_type> 
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
DerivOrthogPolyExpansion(
     const Teuchos::RCP<const Stokhos::OrthogPolyBasis<ordinal_type, value_type> >& basis_) :
  basis(basis_),
  Bij(basis->getDerivDoubleProductTensor()),
  Cijk(basis->getTripleProductTensor()),
  Dijk(basis->getDerivTripleProductTensor()),
  sz(basis->size()),
  A(2*sz,2*sz),
  B(2*sz,2),
  piv(2*sz),
  lapack()
{
}

extern "C" {
  double dlange_(char*, int*, int*, double*, int*, double*);
}

template <typename ordinal_type, typename value_type>
ordinal_type
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
solve(ordinal_type s, ordinal_type nrhs)
{
  if (s == 0 || nrhs == 0)
    return 0;

  ordinal_type info;
//   lapack.GESV(s, nrhs, A.values(), A.numRows(), &(piv[0]), b.values(), 
// 	      b.numRows(), &info);
  lapack.GETRF(s, s, A.values(), A.numRows(), &(piv[0]), &info);
  value_type norm, rcond;
  Teuchos::Array<ordinal_type> iwork(4*s);
  Teuchos::Array<value_type> work(4*s);
  norm = 1.0;
  ordinal_type n = A.numRows();
  char t = '1';
  norm = dlange_(&t, &s, &s, A.values(), &n, &work[0]);
  lapack.GECON('1', s, A.values(), A.numRows(), norm, &rcond, &work[0], 
	       &iwork[0], &info);
  //std::cout << "condition number = " << 1.0/rcond << std::endl;
  lapack.GETRS('N', s, nrhs, A.values(), A.numRows(), &(piv[0]), B.values(), 
	       B.numRows(), &info);
  return info;
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
unaryMinus(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
	   const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  value_type* cc = c.coeff();
  const value_type* ca = a.coeff();
  ordinal_type pa = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pa, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::unaryMinus()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pa << ", c.size() = " << c.size()
		     << ".");
#endif

  for (ordinal_type i=0; i<pa; i++)
    cc[i] = -ca[i];
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
plusEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, const value_type& val)
{
  c[0] += val;
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
minusEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, const value_type& val)
{
  c[0] -= val;
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
timesEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, const value_type& val)
{
  ordinal_type pc = c.size();
  value_type* cc = c.coeff();
  for (ordinal_type i=0; i<pc; i++)
    cc[i] *= val;
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
divideEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, const value_type& val)
{
  ordinal_type pc = c.size();
  value_type* cc = c.coeff();
  for (ordinal_type i=0; i<pc; i++)
    cc[i] /= val;
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
plusEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
	  const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& x)
{
  ordinal_type p = c.size();
  ordinal_type xp = x.size();
#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(p < xp, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::plusEqual()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "x.size() = " << xp << ", c.size() = " << p << ".");
#endif

  value_type* cc = c.coeff();
  const value_type* xc = x.coeff();
  for (ordinal_type i=0; i<xp; i++)
    cc[i] += xc[i];
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
minusEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
	   const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& x)
{
  ordinal_type p = c.size();
  ordinal_type xp = x.size();
#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(p < xp, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::minusEqual()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "x.size() = " << xp << ", c.size() = " << p << ".");
#endif

  value_type* cc = c.coeff();
  const value_type* xc = x.coeff();
  for (ordinal_type i=0; i<xp; i++)
    cc[i] -= xc[i];
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
timesEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
	   const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& x)
{
  ordinal_type p = c.size();
  ordinal_type xp = x.size();
#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(p < xp, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::timesEqual()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "x.size() = " << xp << ", c.size() = " << p << ".");
#endif

  value_type* cc = c.coeff();
  const value_type* xc = x.coeff();
  
  if (p > 1 && xp > 1) {
#ifdef STOKHOS_DEBUG
    TEST_FOR_EXCEPTION(p != xp, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::timesEqual()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "x.size() = " << xp << ", c.size() = " << p << ".");
    TEST_FOR_EXCEPTION(size() < p, std::logic_error,
		       "Stokhos::DerivOrthogPolyExpansion::timesEqual()" 
		       << ":  Expansion size (" << size() 
		       << ") is too small for computation (" << p 
		       << " needed).");
#endif

    // Copy c coefficients into temporary array
    value_type* tc = Stokhos::ds_array<value_type>::get_and_fill(cc,p);
    value_type tmp, cijk;
    ordinal_type i,j;
    for (ordinal_type k=0; k<p; k++) {
      tmp = value_type(0.0);
      ordinal_type n = Cijk->num_values(k);
      for (ordinal_type l=0; l<n; l++) {
	Cijk->value(k,l,i,j,cijk);
	tmp += cijk*tc[i]*xc[j];
      }
      cc[k] = tmp / basis->norm_squared(k);
    }
  }
  else {
    for (ordinal_type i=0; i<p; i++)
      cc[i] *= xc[0];
  }
}

template <typename ordinal_type, typename value_type> 
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
divideEqual(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
	    const OrthogPolyApprox<ordinal_type, value_type>& x)
{
  ordinal_type p = c.size();
  ordinal_type xp = x.size();
#ifdef STOKHOS_DEBUG
  const char *func = "Stokhos::DerivOrthogPolyExpansion::divideEqual()";
  TEST_FOR_EXCEPTION(p < xp, std::logic_error,
		     func
                     << ":  Arguments have incompatible sizes:  "
		     << "x.size() = " << xp << ", c.size() = " << p << ".");
#endif

  value_type* cc = c.coeff();
  const value_type* xc = x.coeff();
  
  if (xp > 1) {

#ifdef STOKHOS_DEBUG
    TEST_FOR_EXCEPTION(p != xp, std::logic_error,
                     func 
                     << ":  Arguments have incompatible sizes:  "
		     << "x.size() = " << xp << ", c.size() = " << p << ".");
    TEST_FOR_EXCEPTION(size() < xp, std::logic_error,
		       func
		       << ":  Expansion size (" << size() 
		       << ") is too small for computation (" << xp
		       << " needed).");
#endif
    
    // Fill A
    value_type cijk;
    ordinal_type i,j;
    for (ordinal_type i=0; i<xp; i++)
      for (ordinal_type j=0; j<xp; j++)
	A(i,j) = 0.0;
    for (ordinal_type k=0; k<xp; k++) {
      ordinal_type n = Cijk->num_values(k);
      for (ordinal_type l=0; l<n; l++) {
	Cijk->value(k,l,i,j,cijk);
	A(i,j) += cijk*xc[k]/basis->norm_squared(i);
      }
    }
    
    // Fill B
    for (ordinal_type i=0; i<xp; i++)
      B(i,0) = cc[i];

    // Solve system
    int info = solve(xp, 1);

    TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		       func << ":  Argument " << info 
		            << " for solve had illegal value");
    TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		       func << ":  Diagonal entry " << info 
		            << " in LU factorization is exactly zero");

    // Get coefficients
    for (ordinal_type i=0; i<xp; i++)
      cc[i] = B(i,0);
  }
  else {
    for (ordinal_type i=0; i<p; i++)
      cc[i] /= xc[0];
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
plus(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::plus()";
  TEST_FOR_EXCEPTION((a.size() != b.size()) && (a.size() != 1) && 
		     (b.size() != 1), 
		     std::logic_error,
		     func << ":  Arguments have incompatible sizes!");
#endif

  ordinal_type pa = a.size();
  ordinal_type pb = b.size();
  ordinal_type pc = pa > pb ? pa : pb;

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::plus()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pa << ", b.size() = " << pb
		     << ", c.size() = " << c.size() << ".");
#endif

  const value_type* ca = a.coeff();
  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

  if (pa > 1 && pb > 1) {
    for (ordinal_type i=0; i<pc; i++)
      cc[i] = ca[i] + cb[i];
  }
  else if (pa > 1) {
    cc[0] = ca[0] + cb[0];
    for (ordinal_type i=1; i<pc; i++)
      cc[i] = ca[i];
  }
  else if (pb >= 1) {
    cc[0] = ca[0] + cb[0];
    for (ordinal_type i=1; i<pc; i++)
      cc[i] = cb[i];
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
plus(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const value_type& a, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  ordinal_type pc = b.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::plus()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "b.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

  cc[0] = a + cb[0];
  for (ordinal_type i=1; i<pc; i++)
    cc[i] = cb[i];
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
plus(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
     const value_type& b)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::plus()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* ca = a.coeff();
  value_type* cc = c.coeff();

  cc[0] = ca[0] + b;
  for (ordinal_type i=1; i<pc; i++)
    cc[i] = ca[i];
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
minus(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::minus()";
  TEST_FOR_EXCEPTION((a.size() != b.size()) && (a.size() != 1) && 
		     (b.size() != 1), 
		     std::logic_error,
		     func << ":  Arguments have incompatible sizes!");
#endif

  ordinal_type pa = a.size();
  ordinal_type pb = b.size();
  ordinal_type pc = pa > pb ? pa : pb;

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::minus()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pa << ", b.size() = " << pb
		     << ", c.size() = " << c.size() << ".");
#endif

  const value_type* ca = a.coeff();
  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

  if (pa > 1 && pb > 1) {
    for (ordinal_type i=0; i<pc; i++)
      cc[i] = ca[i] - cb[i];
  }
  else if (pa > 1) {
    cc[0] = ca[0] - cb[0];
    for (ordinal_type i=1; i<pc; i++)
      cc[i] = ca[i];
  }
  else if (pb >= 1) {
    cc[0] = ca[0] - cb[0];
    for (ordinal_type i=1; i<pc; i++)
      cc[i] = -cb[i];
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
minus(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const value_type& a, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  ordinal_type pc = b.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::minus()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "b.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

  cc[0] = a - cb[0];
  for (ordinal_type i=1; i<pc; i++)
    cc[i] = -cb[i];
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
minus(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, const value_type& b)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::minus()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* ca = a.coeff();
  value_type* cc = c.coeff();

  cc[0] = ca[0] - b;
  for (ordinal_type i=1; i<pc; i++)
    cc[i] = ca[i];
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
times(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::times()";
  TEST_FOR_EXCEPTION((a.size() != b.size()) && (a.size() != 1) && 
		     (b.size() != 1), 
		     std::logic_error,
		     func << ":  Arguments have incompatible sizes!");
#endif

  ordinal_type pa = a.size();
  ordinal_type pb = b.size();
  ordinal_type pc = pa > pb ? pa : pb;

  const value_type* ca = a.coeff();
  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::times()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pa << ", b.size() = " << pb
		     << ", c.size() = " << c.size() << ".");
#endif

  if (pa > 1 && pb > 1) {
#ifdef STOKHOS_DEBUG
    TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		       func << ":  Expansion size (" << size()
		       << ") is too small for computation (" << pc
		       << " needed).");
#endif

    value_type tmp, cijk;
    ordinal_type i,j;
    for (ordinal_type k=0; k<pc; k++) {
      tmp = value_type(0.0);
      ordinal_type n = Cijk->num_values(k);
      for (ordinal_type l=0; l<n; l++) {
	Cijk->value(k,l,i,j,cijk);
	tmp += cijk*ca[i]*cb[j];
      }
      cc[k] = tmp / basis->norm_squared(k);
    }
  }
  else if (pa > 1) {
    for (ordinal_type i=0; i<pc; i++)
      cc[i] = ca[i]*cb[0];
  }
  else if (pb > 1) {
    for (ordinal_type i=0; i<pc; i++)
      cc[i] = ca[0]*cb[i];
  }
  else {
    cc[0] = ca[0]*cb[0];
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
times(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const value_type& a, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  ordinal_type pc = b.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::times()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "b.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

  for (ordinal_type i=0; i<pc; i++)
    cc[i] = a*cb[i];
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
times(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
      const value_type& b)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::times()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* ca = a.coeff();
  value_type* cc = c.coeff();

  for (ordinal_type i=0; i<pc; i++)
    cc[i] = ca[i]*b;
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
divide(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
       const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
       const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::divide()";
  TEST_FOR_EXCEPTION((a.size() != b.size()) && (a.size() != 1) && 
		     (b.size() != 1), 
		     std::logic_error,
		     func << ":  Arguments have incompatible sizes!");
#endif

  ordinal_type pa = a.size();
  ordinal_type pb = b.size();
  ordinal_type pc = pa > pb ? pa : pb;

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::divide()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pa << ", b.size() = " << pb
		     << ", c.size() = " << c.size() << ".");
#endif

  const value_type* ca = a.coeff();
  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

  if (pb > 1) {

#ifdef STOKHOS_DEBUG
    TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		       func << ":  Expansion size (" << size() 
		       << ") is too small for computation (" << pc
		       << " needed).");
#endif

    // Fill A
    value_type cijk;
    ordinal_type i,j;
    for (ordinal_type i=0; i<pc; i++)
      for (ordinal_type j=0; j<pc; j++)
	A(i,j) = 0.0;

    for (ordinal_type k=0; k<pb; k++) {
      ordinal_type n = Cijk->num_values(k);
      for (ordinal_type l=0; l<n; l++) {
	Cijk->value(k,l,i,j,cijk);
	A(i,j) += cijk*cb[k] / basis->norm_squared(i);
      }
    }
    
    // Fill B
    for (ordinal_type i=0; i<pa; i++)
      B(i,0) = ca[i];
    for (ordinal_type i=pa; i<pc; i++)
      B(i,0) = value_type(0.0);

    // Solve system
    int info = solve(pc, 1);

    TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		       func << ":  Argument " << info 
		            << " for solve had illegal value");
    TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		       func << ":  Diagonal entry " << info 
		            << " in LU factorization is exactly zero");

    // Get coefficients
    for (ordinal_type i=0; i<pc; i++)
      cc[i] = B(i,0);
  }
  else {
    for (ordinal_type i=0; i<pa; i++)
      cc[i] = ca[i]/cb[0];
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
divide(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
       const value_type& a, 
       const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  ordinal_type pc = b.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::divide()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "b.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

  if (pc > 1) {
#ifdef STOKHOS_DEBUG
    const char* func = "Stokhos::DerivOrthogPolyExpansion::divide()";
    TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		       func << ":  Expansion size (" << size()
		       << ") is too small for computation (" << pc
		       << " needed).");
#endif
   
    // Fill A
    value_type cijk;
    ordinal_type i,j;
    for (ordinal_type i=0; i<pc; i++)
      for (ordinal_type j=0; j<pc; j++)
	A(i,j) = 0.0;

    for (ordinal_type k=0; k<pc; k++) {
      ordinal_type n = Cijk->num_values(k);
      for (ordinal_type l=0; l<n; l++) {
	Cijk->value(k,l,i,j,cijk);
	A(i,j) += cijk*cb[k] / basis->norm_squared(i);
      }
    }

    // Fill B
    B(0,0) = a;
    for (ordinal_type i=1; i<pc; i++)
      B(i,0) = value_type(0.0);

    // Solve system
    int info = solve(pc, 1);

    TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		       func << ":  Argument " << info 
		            << " for solve had illegal value");
    TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		       func << ":  Diagonal entry " << info 
		            << " in LU factorization is exactly zero");

    // Get coefficients
    for (ordinal_type i=0; i<pc; i++)
      cc[i] = B(i,0);
  }
  else 
    cc[0] = a / cb[0];
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
divide(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
       const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
       const value_type& b)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::divide()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* ca = a.coeff();
  value_type* cc = c.coeff();

  for (ordinal_type i=0; i<pc; i++)
    cc[i] = ca[i]/b;
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
exp(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::exp()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::exp()";
  TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		     func << ":  Expansion size (" << size() 
		     << ") is too small for computation (" << pc
		     << " needed).");
#endif

  const value_type* ca = a.coeff();
  value_type* cc = c.coeff();
  value_type psi_0 = basis->evaluateZero(0);

  // Fill A and B
  for (ordinal_type i=1; i<pc; i++) {
    B(i-1,0) = 0.0;
    for (ordinal_type j=1; j<pc; j++) {
      A(i-1,j-1) = (*Bij)(i-1,j);
      for (ordinal_type k=1; k<pc; k++)
	A(i-1,j-1) -= ca[k]*(*Dijk)(i-1,j,k);
      B(i-1,0) += ca[j]*(*Bij)(i-1,j);
    }
    B(i-1,0) *= psi_0;
  }

  // Solve system
  int info = solve(pc-1, 1);

  TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		     func << ":  Argument " << info 
		     << " for solve had illegal value");
  TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		     func << ":  Diagonal entry " << info 
		     << " in LU factorization is exactly zero");

  // Compute order-0 coefficient
  value_type s = psi_0 * ca[0];
  value_type t = psi_0;
  for (ordinal_type i=1; i<pc; i++) {
    s += basis->evaluateZero(i) * ca[i];
    t += basis->evaluateZero(i) * B(i-1,0);
  }
  s = std::exp(s);
  cc[0] = (s/t);

  // Compute remaining coefficients
  for (ordinal_type i=1; i<pc; i++)
    cc[i] = B(i-1,0) * cc[0];
}

// template <typename ordinal_type, typename value_type>
// void
// Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
// exp(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
//     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
// {
//   const char* func = "Stokhos::DerivOrthogPolyExpansion::exp()";

//   ordinal_type pc = a.size();

// #ifdef STOKHOS_DEBUG
//   TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
// 		     func << ":  Expansion size (" << size() 
// 		     << ") is too small for computation (" << pc
// 		     << " needed).");
// #endif

//   const value_type* ca = a.coeff();
//   value_type* cc = c.coeff();
//   value_type psi_0 = basis->evaluateZero(0);

//   // Fill A and B
//   for (ordinal_type i=1; i<pc; i++) {
//     B(i-1,0) = 0.0;
//     for (ordinal_type j=1; j<pc; j++) {
//       A(i-1,j-1) = 0.0;
//       for (ordinal_type k=0; k<pc; k++) {
// 	A(i-1,j-1) -= ca[k]*Cijk.triple_integral(i,j,k);
// 	std::cout << "E[" << i << "," << j << "," << k << "] = " 
// 		  << Cijk.triple_integral(i,j,k) << std::endl;
//       }
//       B(i-1,0) += ca[j]*Cijk.triple_integral(i,0,j);
//       if (i == j)
// 	A(i-1,j-1) += 1.0;
//     }
//     B(i-1,0) *= psi_0;
//   }

//   std::cout << "A = " << std::endl;
//   A.print(std::cout);

//   std::cout << "B = " << std::endl;
//   B.print(std::cout);

//   // Solve system
//   int info = solve(pc-1, 1);

//   std::cout << "X = " << std::endl;
//   B.print(std::cout);

//   TEST_FOR_EXCEPTION(info < 0, std::logic_error,
// 		     func << ":  Argument " << info 
// 		     << " for solve had illegal value");
//   TEST_FOR_EXCEPTION(info > 0, std::logic_error,
// 		     func << ":  Diagonal entry " << info 
// 		     << " in LU factorization is exactly zero");

//   // Compute order-0 coefficient
//   value_type s = psi_0 * ca[0];
//   value_type t = psi_0;
//   for (ordinal_type i=1; i<pc; i++) {
//     s += basis->evaluateZero(i) * ca[i];
//     t += basis->evaluateZero(i) * B(i-1,0);
//   }
//   s = std::exp(s);
//   cc[0] = (s/t);

//   // Compute remaining coefficients
//   for (ordinal_type i=1; i<pc; i++)
//     cc[i] = B(i-1,0) * cc[0];
// }

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
log(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::log()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::log()";
  TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		     func << ":  Expansion size (" << size() 
		     << ") is too small for computation (" << pc
		     << " needed).");
#endif

  const value_type* ca = a.coeff();
  value_type* cc = c.coeff();
  value_type psi_0 = basis->evaluateZero(0);

  // Fill A and B
  for (ordinal_type i=1; i<pc; i++) {
    B(i-1,0) = 0.0;
    for (ordinal_type j=1; j<pc; j++) {
      A(i-1,j-1) = 0.0;
      for (ordinal_type k=0; k<pc; k++)
	A(i-1,j-1) += ca[k]*(*Dijk)(i-1,k,j);
      B(i-1,0) += ca[j]*(*Bij)(i-1,j); 
    }
  }

  // Solve system
  int info = solve(pc-1, 1);

  TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		     func << ":  Argument " << info 
		     << " for solve had illegal value");
  TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		     func << ":  Diagonal entry " << info 
		     << " in LU factorization is exactly zero");

  // Compute order-0 coefficient
  value_type s = psi_0 * ca[0];
  value_type t = value_type(0.0);
  for (ordinal_type i=1; i<pc; i++) {
    s += basis->evaluateZero(i) * ca[i];
    t += basis->evaluateZero(i) * B(i-1,0);
  }
  cc[0] = (std::log(s) - t) / psi_0;

  // Compute remaining coefficients
  for (ordinal_type i=1; i<pc; i++)
    cc[i] = B(i-1,0);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
log10(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  log(c,a);
  divide(c,c,std::log(10.0));
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
sqrt(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  log(c,a);
  timesEqual(c,value_type(0.5));
  exp(c,c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
pow(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a,
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  log(c,a);
  timesEqual(c,b);
  exp(c,c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
pow(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const value_type& a, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  times(c,std::log(a),b);
  exp(c,c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
pow(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
    const value_type& b)
{
  log(c,a);
  timesEqual(c,b);
  exp(c,c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
sincos(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& s, 
       Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
       const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::sincos()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
  TEST_FOR_EXCEPTION(s.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::sincos()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", s.size() = " << c.size()
		     << ".");
#endif

#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::sincos()";
  TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		     func << ":  Expansion size (" << size() 
		     << ") is too small for computation (" << pc 
		     << " needed).");
#endif

  const value_type* ca = a.coeff();
  value_type* cs = s.coeff();
  value_type* cc = c.coeff();
  value_type psi_0 = basis->evaluateZero(0);
  ordinal_type offset = pc-1;

  // Fill A and b
  B.putScalar(value_type(0.0));
  value_type tmp, tmp2;
  for (ordinal_type i=1; i<pc; i++) {
    tmp2 = value_type(0.0);
    for (ordinal_type j=1; j<pc; j++) {
      tmp = (*Bij)(i-1,j);
      A(i-1,j-1) = tmp;
      A(i-1+offset,j-1+offset) = tmp;
      tmp = value_type(0.0);
      for (ordinal_type k=1; k<pc; k++)
         tmp += ca[k]*(*Dijk)(i-1,j,k);
      A(i-1+offset,j-1) = tmp;
      A(i-1,j-1+offset) = -tmp;
      tmp2 += ca[j]*(*Bij)(i-1,j);
    }
    B(i-1,0) = tmp2*psi_0;
    B(i-1+offset,1) = tmp2*psi_0;
  }

  // Solve system
  int info = solve(2*pc-2, 2);

  TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		     func << ":  Argument " << info 
		     << " for solve had illegal value");
  TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		     func << ":  Diagonal entry " << info 
		     << " in LU factorization is exactly zero");

  // Compute order-0 coefficients
  value_type t = psi_0 * ca[0];
  value_type a00 = psi_0;
  value_type a01 = value_type(0.0);
  value_type a10 = value_type(0.0);
  value_type a11 = psi_0;
  value_type b0 = B(0,0);
  value_type b1 = B(1,0);
  for (ordinal_type i=1; i<pc; i++) {
    t += basis->evaluateZero(i) * ca[i];
    a00 -= basis->evaluateZero(i) * B(i-1,1);
    a01 += basis->evaluateZero(i) * B(i-1,0);
    a10 -= basis->evaluateZero(i) * B(i-1+offset,1);
    a11 += basis->evaluateZero(i) * B(i-1+offset,0);
  }
  A(0,0) = a00;
  A(0,1) = a01;
  A(1,0) = a10;
  A(1,1) = a11;
  B(0,0) = std::sin(t);
  B(1,0) = std::cos(t);

  info = solve(2, 1);

  TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		     func << ":  Argument " << info 
		     << " for (2x2) solve had illegal value");
  TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		     func << ":  Diagonal entry " << info 
		     << " in (2x2) LU factorization is exactly zero");
  cs[0] = B(0,0);
  cc[0] = B(1,0);

  // Compute remaining coefficients
  B(0,0) = b0;
  B(1,0) = b1;
  for (ordinal_type i=1; i<pc; i++) {
    cs[i] = cc[0]*B(i-1,0) - cs[0]*B(i-1,1);
    cc[i] = cc[0]*B(i-1+offset,0) - cs[0]*B(i-1+offset,1);
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
sin(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& s, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  OrthogPolyApprox<ordinal_type, value_type> c(s);
  sincos(s, c, a);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
cos(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  OrthogPolyApprox<ordinal_type, value_type> s(c);
  sincos(s, c, a);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
tan(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& t, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  OrthogPolyApprox<ordinal_type, value_type> c(t);
  
  sincos(t, c, a);
  divideEqual(t,c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
sinhcosh(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& s, 
	 Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
	 const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::sinhcosh()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
  TEST_FOR_EXCEPTION(s.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::sinhcosh()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", s.size() = " << c.size()
		     << ".");
#endif

#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::sinhcosh()";
  TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		     func << ":  Expansion size (" << size() 
		     << ") is too small for computation (" << pc
		     << " needed).");
#endif

  const value_type* ca = a.coeff();
  value_type* cs = s.coeff();
  value_type* cc = c.coeff();
  value_type psi_0 = basis->evaluateZero(0);
  ordinal_type offset = pc-1;

  // Fill A and b
  B.putScalar(value_type(0.0));
  value_type tmp, tmp2;
  for (ordinal_type i=1; i<pc; i++) {
    tmp2 = value_type(0.0);
    for (ordinal_type j=1; j<pc; j++) {
      tmp = (*Bij)(i-1,j);
      A(i-1,j-1) = tmp;
      A(i-1+offset,j-1+offset) = tmp;
      tmp = value_type(0.0);
      for (ordinal_type k=1; k<pc; k++)
         tmp += ca[k]*(*Dijk)(i-1,j,k);
      A(i-1+offset,j-1) = -tmp;
      A(i-1,j-1+offset) = -tmp;
      tmp2 += ca[j]*(*Bij)(i-1,j);
    }
    B(i-1,0) = tmp2*psi_0;
    B(i-1+offset,1) = tmp2*psi_0;
  }

  // Solve system
  int info = solve(2*pc-2, 2);

  TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		     func << ":  Argument " << info 
		     << " for solve had illegal value");
  TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		     func << ":  Diagonal entry " << info 
		     << " in LU factorization is exactly zero");

  // Compute order-0 coefficients
  value_type t = psi_0 * ca[0];
  value_type a00 = psi_0;
  value_type a01 = value_type(0.0);
  value_type a10 = value_type(0.0);
  value_type a11 = psi_0;
  value_type b0 = B(0,0);
  value_type b1 = B(1,0);
  for (ordinal_type i=1; i<pc; i++) {
    t += basis->evaluateZero(i) * ca[i];
    a00 += basis->evaluateZero(i) * B(i-1,1);
    a01 += basis->evaluateZero(i) * B(i-1,0);
    a10 += basis->evaluateZero(i) * B(i-1+offset,1);
    a11 += basis->evaluateZero(i) * B(i-1+offset,0);
  }
  A(0,0) = a00;
  A(0,1) = a01;
  A(1,0) = a10;
  A(1,1) = a11;
  B(0,0) = std::sinh(t);
  B(1,0) = std::cosh(t);
  info = solve(2, 1);
  TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		     func << ":  Argument " << info 
		     << " for (2x2) solve had illegal value");
  TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		     func << ":  Diagonal entry " << info 
		     << " in (2x2) LU factorization is exactly zero");
  cs[0] = B(0,0);
  cc[0] = B(1,0);

  // Compute remaining coefficients
  B(0,0) = b0;
  B(1,0) = b1;
  for (ordinal_type i=1; i<pc; i++) {
    cs[i] = cc[0]*B(i-1,0) + cs[0]*B(i-1,1);
    cc[i] = cc[0]*B(i-1+offset,0) + cs[0]*B(i-1+offset,1);
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
sinh(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& s, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  OrthogPolyApprox<ordinal_type, value_type> c(s);
  sinhcosh(s, c, a);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
cosh(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  OrthogPolyApprox<ordinal_type, value_type> s(c);
  sinhcosh(s, c, a);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
tanh(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& t, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  OrthogPolyApprox<ordinal_type, value_type> c(t);
  
  sinhcosh(t, c, a);
  divideEqual(t,c);
}

template <typename ordinal_type, typename value_type>
template <typename OpT>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
quad(const OpT& quad_func,
     Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a,
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  ordinal_type pc = a.size();

#ifdef STOKHOS_DEBUG
  TEST_FOR_EXCEPTION(c.size() != pc, std::logic_error,
                     "Stokhos::DerivOrthogPolyExpansion::quad()" 
                     << ":  Arguments have incompatible sizes:  "
		     << "a.size() = " << pc << ", c.size() = " << c.size()
		     << ".");
#endif

  const value_type* ca = a.coeff();
  const value_type* cb = b.coeff();
  value_type* cc = c.coeff();

#ifdef STOKHOS_DEBUG
  const char* func = "Stokhos::DerivOrthogPolyExpansion::quad()";
  TEST_FOR_EXCEPTION(size() < pc, std::logic_error,
		     func << ":  Expansion size (" << size()
		     << ") is too small for computation (" << pc
		     << " needed).");
#endif
  value_type psi_0 = basis->evaluateZero(0);
    
  // Fill A and B
  for (ordinal_type i=1; i<pc; i++) {
    B(i-1,0) = 0.0;
    for (ordinal_type j=1; j<pc; j++) {
      A(i-1,j-1) = 0.0;
      for (ordinal_type k=0; k<pc; k++)
	A(i-1,j-1) += cb[k]*(*Dijk)(i-1,k,j);
      B(i-1,0) += ca[j]*(*Bij)(i-1,j);
    }
  }
  
  // Solve system
  int info = solve(pc-1, 1);

  TEST_FOR_EXCEPTION(info < 0, std::logic_error,
		     func << ":  Argument " << info 
		     << " for solve had illegal value");
  TEST_FOR_EXCEPTION(info > 0, std::logic_error,
		     func << ":  Diagonal entry " << info 
		     << " in LU factorization is exactly zero");

  // Compute order-0 coefficient
  value_type s = psi_0 * ca[0];
  value_type t = value_type(0.0);
  for (ordinal_type i=1; i<pc; i++) {
    s += basis->evaluateZero(i) * ca[i];
    t += basis->evaluateZero(i) * B(i-1,0);
  }
  cc[0] = (quad_func(s) - t) / psi_0;

  // Get coefficients
  for (ordinal_type i=1; i<pc; i++)
    cc[i] = B(i-1,0);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
acos(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  times(c,a,a);
  minus(c,value_type(1.0),c);
  sqrt(c,c);
  timesEqual(c,value_type(-1.0));
  quad(acos_quad_func(), c, a, c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
asin(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  times(c,a,a);
  minus(c,value_type(1.0),c);
  sqrt(c,c);
  quad(asin_quad_func(), c, a, c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
atan(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  times(c,a,a);
  plusEqual(c,value_type(1.0));
  quad(atan_quad_func(), c, a, c);
}

// template <typename ordinal_type, typename value_type>
// Hermite<ordinal_type, value_type>
// atan2(const Hermite<ordinal_type, value_type>& a,
//       const Hermite<ordinal_type, value_type>& b)
// {
//   Hermite<ordinal_type, value_type> c = atan(a/b);
//   c.fastAccessCoeff(0) = atan2(a.coeff(0),b.coeff(0));
// }

// template <typename ordinal_type, typename value_type>
// Hermite<ordinal_type, value_type>
// atan2(const T& a,
//       const Hermite<ordinal_type, value_type>& b)
// {
//   Hermite<ordinal_type, value_type> c = atan(a/b);
//   c.fastAccessCoeff(0) = atan2(a,b.coeff(0));
// }

// template <typename ordinal_type, typename value_type>
// Hermite<ordinal_type, value_type>
// atan2(const Hermite<ordinal_type, value_type>& a,
//       const T& b)
// {
//   Hermite<ordinal_type, value_type> c = atan(a/b);
//   c.fastAccessCoeff(0) = atan2(a.coeff(0),b);
// }

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
acosh(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  times(c,a,a);
  minusEqual(c,value_type(1.0));
  sqrt(c,c);
  quad(acosh_quad_func(), c, a, c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
asinh(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  times(c,a,a);
  plusEqual(c,value_type(1.0));
  sqrt(c,c);
  quad(asinh_quad_func(), c, a, c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
atanh(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
      const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  times(c,a,a);
  minus(c,value_type(1.0),c);
  quad(atanh_quad_func(), c, a, c);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
fabs(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
     const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  if (a[0] >= 0)
    c = a;
  else
    unaryMinus(c,a);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
abs(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  if (a[0] >= 0)
    c = a;
  else
    unaryMinus(c,a);
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
max(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a,
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  if (a[0] >= b[0])
    c = a;
  else
    c = b;
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
max(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const value_type& a, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  if (a >= b[0]) {
    c = OrthogPolyApprox<ordinal_type, value_type>(basis);
    c[0] = a;
  }
  else
    c = b;
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
max(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
    const value_type& b)
{
  if (a[0] >= b)
    c = a;
  else {
    c = OrthogPolyApprox<ordinal_type, value_type>(basis);
    c[0] = b;
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
min(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a,
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  if (a[0] <= b[0])
    c = a;
  else
    c = b;
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
min(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const value_type& a, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& b)
{
  if (a <= b[0]) {
    c = OrthogPolyApprox<ordinal_type, value_type>(basis);
    c[0] = a;
  }
  else
    c = b;
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
min(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a, 
    const value_type& b)
{
  if (a[0] <= b)
    c = a;
  else {
    c = OrthogPolyApprox<ordinal_type, value_type>(basis);
    c[0] = b;
  }
}

template <typename ordinal_type, typename value_type>
void
Stokhos::DerivOrthogPolyExpansion<ordinal_type, value_type>::
derivative(Stokhos::OrthogPolyApprox<ordinal_type, value_type>& c, 
    const Stokhos::OrthogPolyApprox<ordinal_type, value_type>& a)
{
  ordinal_type pc = a.size();

  const value_type* ca = a.coeff();
  value_type* cc = c.coeff();

  for (ordinal_type i=0; i<pc; i++) {
    cc[i] = 0.0;
    for (ordinal_type j=0; j<pc; j++)
      cc[i] += ca[j]*(*Bij)(i,j);
    cc[i] /= basis->norm_squared(i);
  }
}
