/*
// @HEADER
// ***********************************************************************
//
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ***********************************************************************
// @HEADER
*/

#include "TeuchosCore_ConfigDefs.hpp"
#include "Teuchos_UnitTestHarness.hpp"
#include "Teuchos_Ptr.hpp"
#include "Teuchos_UniquePtr.hpp"
#include "Teuchos_getConst.hpp"
#include "TestClasses.hpp"


namespace {


using Teuchos::as;
using Teuchos::null;
using Teuchos::Ptr;
using Teuchos::UniquePtr;
using Teuchos::uniqueptr;
using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcpFromRef;
using Teuchos::rcpFromUndefRef;
using Teuchos::outArg;
using Teuchos::rcpWithEmbeddedObj;
using Teuchos::getEmbeddedObj;
using Teuchos::getOptionalEmbeddedObj;
using Teuchos::getOptionalNonconstEmbeddedObj;
using Teuchos::set_extra_data;
using Teuchos::get_optional_nonconst_extra_data;
using Teuchos::getConst;
using Teuchos::NullReferenceError;
using Teuchos::DanglingReferenceError;
using Teuchos::DuplicateOwningRCPError;
using Teuchos::RCP_STRONG;
using Teuchos::RCP_WEAK;
using Teuchos::RCPNodeTracer;

#ifdef HAVE_TEUCHOSCORE_CXX11

TEUCHOS_UNIT_TEST( UniquePtr, assignSelf_null )
{
  UniquePtr<A> a_uptr;
  a_uptr = std::move(a_uptr);
  TEST_ASSERT(is_null(a_uptr));
}

TEUCHOS_UNIT_TEST( UniquePtr, assignSelf_nonnull )
{
  UniquePtr<A> a_uptr(new A);
  A *a_raw_ptr = a_uptr.getRawPtr();
  a_uptr = std::move(a_uptr);
  TEST_ASSERT(nonnull(a_uptr));
  TEST_EQUALITY(a_uptr.getRawPtr(), a_raw_ptr);
}

TEUCHOS_UNIT_TEST( UniquePtr, getConst )
{
  UniquePtr<A> a_uptr(new A);
  Ptr<const A> ca_ptr = a_uptr.getConst();
  TEST_EQUALITY(a_uptr.getRawPtr(), ca_ptr.getRawPtr());
}

TEUCHOS_UNIT_TEST( UniquePtr, explicit_null )
{
  UniquePtr<A> a_uptr(0);
  TEST_ASSERT(is_null(a_uptr));
}

TEUCHOS_UNIT_TEST( UniquePtr, danglingPtr1 )
{
  ECHO(UniquePtr<A> a_uptr = uniqueptr(new A));
  ECHO(Ptr<A> a_ptr = a_uptr.ptr());
  ECHO(A *badPtr = a_uptr.getRawPtr());
  ECHO(a_uptr = null);
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *a_ptr, DanglingReferenceError );
  (void)badPtr;
#else
  TEST_EQUALITY( a_ptr.getRawPtr(), badPtr );
#endif
}

TEUCHOS_UNIT_TEST( UniquePtr, danglingPtr2 )
{
  ECHO(Ptr<A> a_ptr);
  ECHO(A *badPtr = 0);
  {
    ECHO(UniquePtr<A> a_uptr = uniqueptr(new A));
    ECHO(badPtr = a_uptr.getRawPtr());
    ECHO(a_ptr = a_uptr.ptr());
    TEST_EQUALITY( a_ptr.getRawPtr(), badPtr );
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *a_ptr, DanglingReferenceError );
  (void)badPtr;
#else
  TEST_EQUALITY( a_ptr.getRawPtr(), badPtr );
#endif
}

TEUCHOS_UNIT_TEST( UniquePtr, danglingPtr3 )
{
  ECHO(Ptr<A> a_ptr);
  ECHO(A *badPtr = 0);
  {
    ECHO(UniquePtr<A> a_uptr = uniqueptr(new A));
    ECHO(badPtr = a_uptr.getRawPtr());
    ECHO(Ptr<A> a_ptr2(a_uptr.ptr()));
    ECHO(Ptr<A> a_ptr3(a_ptr2));
    ECHO(a_ptr = a_ptr3);
    TEST_EQUALITY( a_ptr.getRawPtr(), badPtr );
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *a_ptr, DanglingReferenceError );
  (void)badPtr;
#else
  TEST_EQUALITY( a_ptr.getRawPtr(), badPtr );
#endif
}

#endif // HAVE_TEUCHOSCORE_CXX11



} // namespace
