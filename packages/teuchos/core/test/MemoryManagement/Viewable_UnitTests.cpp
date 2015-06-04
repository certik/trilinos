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
#include "Teuchos_Viewable.hpp"

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
using Teuchos::NullReferenceError;
using Teuchos::DanglingReferenceError;
using Teuchos::DuplicateOwningRCPError;
using Teuchos::RCP_STRONG;
using Teuchos::RCP_WEAK;
using Teuchos::RCPNodeTracer;
using Teuchos::Viewable;

#ifdef HAVE_TEUCHOSCORE_CXX11

// Raw pointers example with an unsafe dangling pointer
TEUCHOS_UNIT_TEST( Viewable, Dangling1 )
{
  int *ip;
  {
    int i = 1;
    i += 10;
    ip = &i;
    TEST_EQUALITY(*ip, 11);
  }
  // *ip is now dangling
}

// Viewable example with a safe dangling pointer
TEUCHOS_UNIT_TEST( Viewable, Dangling2 )
{
  Ptr<int> ip;
  {
    Viewable<int> i = 1;
    *i += 10;
    ip = i.ptr();
    TEST_EQUALITY(*ip, 11);
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *ip, DanglingReferenceError );
#endif
}

class A {
private:
  Viewable<std::map<int, int>> m_;
public:
  A(std::map<int, int> &&m) : m_(std::move(m)) {}

  Ptr<std::map<int, int>> get_access() {
    return m_.ptr();
  }

  Ptr<const std::map<int, int>> get_access() const {
    return m_.cptr();
  }

  void someFunc1(std::map<int, int> &d) {
  }
  void someFunc2(const std::map<int, int> &d) {
  }

  void someOtherFunc()
  {
    someFunc1(*m_);  // T&
//    ...
    someFunc2(*m_);  // const T&
//    ...
  }
};

TEUCHOS_UNIT_TEST( Viewable, TestA )
{
  std::map<int, int> d;
  d[5] = 6;
  d[7] = 8;
  Ptr<const std::map<int, int>> pd;
  {
    A a(std::move(d));
    pd = a.get_access();
    TEST_EQUALITY(pd->at(5), 6);
    TEST_EQUALITY(pd->at(7), 8);
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *pd, DanglingReferenceError );
#endif
}

TEUCHOS_UNIT_TEST( Viewable, Copy )
{
  {
    int i = 5;
    int j = i;
    TEST_EQUALITY(j, 5);
  }
  {
    Viewable<int> i = 5;
    Viewable<int> j = i;
    TEST_EQUALITY(*j, 5);
  }
  {
    Viewable<int> i = 5;
    Viewable<int> j = std::move(i);
    TEST_EQUALITY(*j, 5);
  }
  {
    Viewable<int> i = 5;
    Viewable<int> j;
    j = i;
    TEST_EQUALITY(*j, 5);
  }
  {
    Viewable<int> i = 5;
    Viewable<int> j;
    j = std::move(i);
    TEST_EQUALITY(*j, 5);
  }
}

TEUCHOS_UNIT_TEST( Viewable, Containers )
{
  {
    std::vector<Viewable<int>> v;
    v.push_back(5);
    TEST_EQUALITY(*(v[0]), 5);
  }

  {
    std::vector<int> v1, v2;
    v1.push_back(5);
    TEST_EQUALITY(v1[0], 5);
    v2 = v1;
    TEST_EQUALITY(v1[0], 5);
    TEST_EQUALITY(v2[0], 5);
  }

  {
    std::vector<Viewable<int>> v1, v2;
    v1.push_back(5);
    TEST_EQUALITY(*(v1[0]), 5);
    v2 = v1;
    TEST_EQUALITY(*(v1[0]), 5);
    TEST_EQUALITY(*(v2[0]), 5);
  }

  {
    std::vector<Viewable<int>> v1 = {1, 2, 3};
    TEST_EQUALITY(*(v1[0]), 1);
    TEST_EQUALITY(*(v1[1]), 2);
    TEST_EQUALITY(*(v1[2]), 3);
  }
  Ptr<int> pi1, pi2;
  {
    std::vector<Viewable<int>> v1 = {1, 2};
    pi1 = v1[0].ptr();
    pi2 = v1[1].ptr();
    TEST_EQUALITY(*pi1, 1);
    TEST_EQUALITY(*pi2, 2);
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *pi1, DanglingReferenceError );
  TEST_THROW( *pi2, DanglingReferenceError );
#endif

  {
    std::map<int, Viewable<int>> v1 = {{1, 2}, {3, 4}};
    pi1 = v1[1].ptr();
    pi2 = v1[3].ptr();
    TEST_EQUALITY(*pi1, 2);
    TEST_EQUALITY(*pi2, 4);
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *pi1, DanglingReferenceError );
  TEST_THROW( *pi2, DanglingReferenceError );
#endif
}

#endif // HAVE_TEUCHOSCORE_CXX11



} // namespace
