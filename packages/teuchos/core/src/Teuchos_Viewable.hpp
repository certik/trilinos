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


#ifndef TEUCHOS_VIEWABLE_HPP
#define TEUCHOS_VIEWABLE_HPP

#include <type_traits>

#include "Teuchos_PtrDecl.hpp"
#include "Teuchos_UniquePtr.hpp"


namespace Teuchos {

#ifdef HAVE_TEUCHOSCORE_CXX11

template<class T>
class Viewable {
public:
  Viewable() = default;
  ~Viewable() = default;

#ifdef TEUCHOS_DEBUG
  Viewable(const Viewable<T> &v) : uptr_(new T(*v)) {}
#else
  Viewable(const Viewable<T> &v) = default;
#endif

#ifdef TEUCHOS_DEBUG
  Viewable<T>& operator=(const Viewable<T>& v) {
      uptr_.reset(new T(*v));
      return *this;
  }
#else
  Viewable<T>& operator=(const Viewable<T>& v) = default;
#endif

  Viewable(Viewable<T> &&v) = default;
  Viewable<T>& operator=(Viewable<T>&& v) = default;

  // The below variadic template makes the class not trivially copyable (it
  // seems it must catch some kind of Viewable arguments), so instead we
  // provide a specific Viewable(T&& arg) constructor below:
  /*
  template <class... Args>
  Viewable(Args&&... args) :
#ifdef TEUCHOS_DEBUG
      uptr_(new T(std::forward<Args>(args)...))
#else
      m_(std::forward<Args>(args)...)
#endif
    {}
    */

  Viewable(T&& arg) :
#ifdef TEUCHOS_DEBUG
      uptr_(new T(std::forward<T>(arg)))
#else
      m_(std::forward<T>(arg))
#endif
    {}

  Ptr<const T> cptr() const {
#ifdef TEUCHOS_DEBUG
    return uptr_.ptr();
#else
    return Teuchos::ptrFromRef(m_);
#endif
  }

  Ptr<T> ptr() {
#ifdef TEUCHOS_DEBUG
    return uptr_.ptr();
#else
    return Teuchos::ptrFromRef(m_);
#endif
  }

  T& nonconstRef() {
#ifdef TEUCHOS_DEBUG
    return *uptr_;
#else
    return m_;
#endif
  }

  const T& ref() const {
#ifdef TEUCHOS_DEBUG
    return *uptr_;
#else
    return m_;
#endif
  }

  T& operator*() { return nonconstRef(); }

  const T& operator*() const { return ref(); }

  T* operator->() { return ptr().get(); }

  const T* operator->() const { return cptr().get(); }

private:
#ifdef TEUCHOS_DEBUG
  UniquePtr<T> uptr_;
#else
  T m_;
#endif
};

#ifndef TEUCHOS_DEBUG
static_assert(std::is_pod<Viewable<int>>::value, "Viewable<int> is not a POD");
static_assert(sizeof(Viewable<int>) == sizeof(int), "Viewable<int> has incorrect size");
#endif


#endif // HAVE_TEUCHOSCORE_CXX11


} // namespace Teuchos

#endif // TEUCHOS_VIEWABLE_HPP
