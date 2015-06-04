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


#include "Teuchos_PtrDecl.hpp"
#include "Teuchos_UniquePtr.hpp"


namespace Teuchos {

#ifdef HAVE_TEUCHOSCORE_CXX11

template<class T>
class Viewable {
public:
  template <class... Args>
  Viewable(Args&&... args) :
#ifdef TEUCHOS_DEBUG
      uptr_(new T(std::forward<Args>(args)...))
#else
      m_(std::forward<Args>(args)...)
#endif
    {}

  Ptr<const T> cptr() {
#ifdef TEUCHOS_DEBUG
    return uptr_.ptr();
#else
    return Teuchos::ptrFromRef(m_);
#endif
  }

  Ptr<T> ptr() const {
#ifdef TEUCHOS_DEBUG
    return uptr_.ptr();
#else
    // FIXME: this is a hack, do it properly:
    T *p = const_cast<T*>(&m_);
    return Ptr<T>(p);
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

private:
#ifdef TEUCHOS_DEBUG
  UniquePtr<T> uptr_;
#else
  T m_;
#endif
};


#endif // HAVE_TEUCHOSCORE_CXX11


} // namespace Teuchos

#endif // TEUCHOS_VIEWABLE_HPP
