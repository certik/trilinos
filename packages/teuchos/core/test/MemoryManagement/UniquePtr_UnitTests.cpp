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

TEUCHOS_UNIT_TEST( UniquePtr, assign1 )
{
  UniquePtr<A> a_uptr;
  UniquePtr<A> b_uptr;
  TEST_ASSERT(is_null(a_uptr));
  TEST_ASSERT(is_null(b_uptr));
  b_uptr = std::move(a_uptr);
  TEST_ASSERT(is_null(a_uptr));
  TEST_ASSERT(is_null(b_uptr));
}

TEUCHOS_UNIT_TEST( UniquePtr, assign2 )
{
  UniquePtr<A> a_uptr(new A);
  UniquePtr<A> b_uptr;
  TEST_ASSERT(nonnull(a_uptr));
  TEST_ASSERT(is_null(b_uptr));
  TEST_EQUALITY( a_uptr->A_f(), A_f_return );
  b_uptr = std::move(a_uptr);
  TEST_ASSERT(nonnull(b_uptr));
  TEST_ASSERT(is_null(a_uptr));
  TEST_EQUALITY( b_uptr->A_f(), A_f_return );
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *a_uptr, DanglingReferenceError );
  TEST_THROW( a_uptr->A_f(), DanglingReferenceError );
#endif
}

TEUCHOS_UNIT_TEST( UniquePtr, assign3 )
{
  UniquePtr<A> a_uptr(new A);
  TEST_ASSERT(nonnull(a_uptr));
  TEST_EQUALITY( a_uptr->A_f(), A_f_return );
  UniquePtr<A> b_uptr = std::move(a_uptr);
  TEST_ASSERT(nonnull(b_uptr));
  TEST_ASSERT(is_null(a_uptr));
  TEST_EQUALITY( b_uptr->A_f(), A_f_return );
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *a_uptr, DanglingReferenceError );
  TEST_THROW( a_uptr->A_f(), DanglingReferenceError );
#endif
}

TEUCHOS_UNIT_TEST( UniquePtr, assign4 )
{
  UniquePtr<int> orig(new int(5));
  bool t;
  t = (*orig == 5);
  TEST_ASSERT(t)
  auto stolen = std::move(orig);
#ifdef TEUCHOS_DEBUG
  TEST_THROW( t = (*orig == 5), DanglingReferenceError );
#endif
  TEST_EQUALITY(*stolen, 5);
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

TEUCHOS_UNIT_TEST( UniquePtr, reset_null )
{
  UniquePtr<A> a_uptr = uniqueptr(new A);
  a_uptr.reset();
  TEST_ASSERT(is_null(a_uptr));
}

TEUCHOS_UNIT_TEST( UniquePtr, reset_null2 )
{
  UniquePtr<A> a_uptr = uniqueptr(new A);
  int a_f_return = -1;
  UniquePtr<Get_A_f_return> af_uptr = uniqueptr(new Get_A_f_return(a_uptr.get(),
        &a_f_return));
  TEST_INEQUALITY( a_f_return, A_f_return );
  af_uptr.reset();
  TEST_EQUALITY( a_f_return, A_f_return );
  TEST_ASSERT(is_null(af_uptr));
}

TEUCHOS_UNIT_TEST( UniquePtr, deallocate )
{
  UniquePtr<A> a_uptr = uniqueptr(new A);
  int a_f_return = -1;
  {
    UniquePtr<Get_A_f_return> af_uptr = uniqueptr(
        new Get_A_f_return(a_uptr.get(), &a_f_return));
    TEST_INEQUALITY( a_f_return, A_f_return );
  }
  TEST_EQUALITY( a_f_return, A_f_return );
}

TEUCHOS_UNIT_TEST( UniquePtr, dereference )
{
  UniquePtr<A> a_uptr = uniqueptr(new A);
  TEST_EQUALITY( (*a_uptr).A_f(), A_f_return );
  TEST_EQUALITY( a_uptr->A_f(), A_f_return );
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

TEUCHOS_UNIT_TEST( UniquePtr, danglingPtr4 )
{
  ECHO(Ptr<A> a_ptr);
  ECHO(A *badPtr = 0);
  {
    ECHO(UniquePtr<C> c_uptr = uniqueptr(new C));
    ECHO(badPtr = c_uptr.getRawPtr());
    ECHO(Ptr<A> a_ptr2(c_uptr.ptr()));
    ECHO(a_ptr = a_ptr2);
    TEST_EQUALITY( a_ptr.getRawPtr(), badPtr );
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *a_ptr, DanglingReferenceError );
  (void)badPtr;
#else
  TEST_EQUALITY( a_ptr.getRawPtr(), badPtr );
#endif
}

struct Foo { // object to manage
  Foo() { } // Foo ctor  TODO: test that it is called
  Foo(const Foo&) { throw std::runtime_error("Foo copy ctor called"); }
  Foo(Foo&&) { throw std::runtime_error("Foo move ctor called"); }
  ~Foo() { } // ~Foo dtor TODO: test that it is called
};

struct D { // deleter
  D() {}; // TODO: test that it is called
  D(const D&) { } // D copy ctor TODO: test that it is called
  D(D&) { } // D non-const copy ctor TODO: test that it is called
  D(D&&) { } // D move ctor TODO: test that it is called
  void operator()(Foo* p) const {
    // D is deleting a Foo TODO: test that it is called
    delete p;
  };
};

template <template <typename T, typename Deleter=std::default_delete<T>> class UPtr>
bool test_unique_ptr_interface()
{
  // Test 1
  UPtr<A> p1(new A);
  {
    UPtr<A> p2(std::move(p1));
    p1 = std::move(p2);
  }

  // Test 2
  int x1 = 5;
  auto del1 = [](int * p) { std::cout << "Deleting x1, value is : " << *p << std::endl; };
  UPtr<int, decltype(del1)> px1(&x1, del1);

  // Test 3
  bool deleted = false;
  int x2 = 5;
  auto del2 = [&deleted](int * p) { deleted = true; };
  {
    if (deleted) return false;
    UPtr<int, decltype(del2)> px2(&x2, del2);
  }
  if (!deleted) return false;

  // Test 4
  UPtr<Foo> up1;
  if (up1.get() != nullptr) return false;
  UPtr<Foo> up1b(nullptr);
  if (up1b.get() != nullptr) return false;

  // Test 5
  {
    UPtr<Foo> up2(new Foo);
    if (up2.get() == nullptr) return false;
  }

  // Test 6
  D d;
  {
    UPtr<Foo, D> up3(new Foo, d); // deleter copied
    if (up3.get() == nullptr) return false;
  }
  {
    UPtr<Foo, D&> up3b(new Foo, d); // up3b holds a reference to d
    if (up3b.get() == nullptr) return false;
  }

  // Test 7
  {
    UPtr<Foo, D> up4(new Foo, D()); // deleter moved
    if (up4.get() == nullptr) return false;
  }

  // Test 8
  {
    UPtr<Foo> up5a(new Foo);
    if (up5a.get() == nullptr) return false;
    UPtr<Foo> up5b(std::move(up5a)); // ownership transfer
    if (up5a.get() != nullptr) return false;
    if (up5b.get() == nullptr) return false;
  }

  // Test 9
  {
    UPtr<Foo, D> up6a(new Foo, d); // D is copied
    UPtr<Foo, D> up6b(std::move(up6a)); // D is moved

    UPtr<Foo, D&> up6c(new Foo, d); // D is a reference
    UPtr<Foo, D> up6d(std::move(up6c)); // D is copied
  }

  // Test 10
  {
    UPtr<Foo> up(new Foo());
    Foo *fp = up.release();
    if (up.get() != nullptr) {
      delete fp;
      return false;
    }
    delete fp;
  }

  // Success
  return true;
}

TEUCHOS_UNIT_TEST( UniquePtr, std_unique_ptr_interface )
{
  TEST_ASSERT(test_unique_ptr_interface<std::unique_ptr>());
  TEST_ASSERT(test_unique_ptr_interface<UniquePtr>());
  A *a;
  std::tuple<A*, std::default_delete<A>> t;
  int x = 5;
  auto del = [](int * p) { std::cout << "Deleting x, value is : " << *p; };
  std::unique_ptr<int, decltype(del)> px(&x, del);
  std::cout << "SIZE: " << sizeof(std::default_delete<A>) << std::endl;
  std::cout << "SIZE: " << sizeof(std::unique_ptr<A>) << std::endl;
  std::cout << "SIZE: " << sizeof(a) << std::endl;
  std::cout << "SIZE: " << sizeof(t) << std::endl;
  std::cout << "SIZE: " << sizeof(px) << std::endl;
}

#endif // HAVE_TEUCHOSCORE_CXX11



} // namespace
