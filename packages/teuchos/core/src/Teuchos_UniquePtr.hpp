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


#ifndef TEUCHOS_UNIQUEPTR_HPP
#define TEUCHOS_UNIQUEPTR_HPP


#include "Teuchos_PtrDecl.hpp"
#include "Teuchos_RCP.hpp"


namespace Teuchos {

#ifdef HAVE_TEUCHOSCORE_CXX11

template<class T, class Deleter=std::default_delete<T>>
class UniquePtr {
public:
  inline UniquePtr( ENull null_in = null );

  inline explicit UniquePtr( T *ptr_in ) : ptr_(ptr_in) { }

  inline UniquePtr( T *ptr_in,
      typename std::conditional<std::is_reference<Deleter>::value,
        Deleter, const Deleter&>::type d)
      : ptr_(ptr_in
#ifdef TEUCHOS_DEBUG
          , deallocFunctorDelete<T, Deleter>(d), true
#endif
          ), d_(d) { }

  inline UniquePtr( T *ptr_in,
      typename std::remove_reference<Deleter>::type &&d)
      : ptr_(ptr_in
#ifdef TEUCHOS_DEBUG
          , deallocFunctorDelete<T, Deleter>(d), true
#endif
          ), d_(std::move(d)) { }

  ~UniquePtr();

  // Copy constructor and assignment are disabled
  inline UniquePtr(const UniquePtr<T>& ptr) = delete;
  template<class U, class E>
  inline UniquePtr(const UniquePtr<U, E>& ptr) = delete;
  UniquePtr<T>& operator=(const UniquePtr<T>& ptr) = delete;
  template<class U, class E>
  UniquePtr<T>& operator=(const UniquePtr<U, E>& ptr) = delete;

  // Move constructor and assignment
  inline UniquePtr(UniquePtr &&r_ptr) : ptr_(r_ptr.release()),
      d_(std::forward<Deleter>(r_ptr.get_deleter())) { }

  template<class U, class E>
  inline UniquePtr(UniquePtr<U, E> &&r_ptr) : ptr_(r_ptr.release()),
      d_(std::forward<E>(r_ptr.get_deleter())) { }

  UniquePtr& operator=(UniquePtr &&r_ptr) {
    reset(r_ptr.release());
    get_deleter() = std::forward<Deleter>(r_ptr.get_deleter());
    return *this;
  }

  template<class U, class E>
  UniquePtr& operator=(UniquePtr<U, E> &&r_ptr) {
    reset(r_ptr.release());
    get_deleter() = std::forward<E>(r_ptr.get_deleter());
    return *this;
  }

  inline T* operator->() const { return ptr_.operator->(); }
  inline T& operator*() const { return ptr_.operator*(); }

  /** \brief Get the raw C++ pointer to the underlying object. */
  inline T* get() const;

  /** \brief Get the raw C++ pointer to the underlying object. */
  inline T* getRawPtr() const;

  /** \brief Return a Ptr<T> version of *this. */
  inline const Ptr<T> ptr() const;

  /** \brief Return a Ptr<const T> version of *this. */
  inline Ptr<const T> getConst() const;

  inline T* release() {
    T *p = get();
#ifdef TEUCHOS_DEBUG
    ptr_.release(); // So that the next line does not delete the object
#endif
    ptr_ = null;
    return p;
  }

  /** \brief Reset to r_ptr (default: null). */
  inline void reset(T *r_ptr=nullptr);

  void swap(UniquePtr & other) noexcept {
    std::swap(ptr_, other.ptr_);
    std::swap(get_deleter(), other.get_deleter());
  }

  Deleter& get_deleter() noexcept {
    return d_;
  }

  const Deleter& get_deleter() const noexcept {
    return d_;
  }

private:
#ifdef TEUCHOS_DEBUG
  RCP<T> ptr_;
#else
  Ptr<T> ptr_;
#endif
  Deleter d_;
};

// -------------------------
// Non member functions

template<class T, class Deleter=std::default_delete<T>>
inline
UniquePtr<T, Deleter>
uniqueptr(T* p)
{
  return UniquePtr<T>(p);
}

template<class T2, class T1, class Deleter=std::default_delete<T1>>
Ptr<T2> uniqueptr_implicit_cast(const UniquePtr<T1, Deleter>& p1)
{
  return Ptr<T2>(p1.get()); // Will only compile if conversion is legal!
}

// --------------------------------
// Implementation of UniquePtr

template<class T, class Deleter>
UniquePtr<T, Deleter>::UniquePtr( ENull /*null_in*/ )
  : ptr_(0)
{}

template<class T, class Deleter>
UniquePtr<T, Deleter>::~UniquePtr()
{
#ifndef TEUCHOS_DEBUG
  d_(ptr_.get()); // Note: the pointer can be null
#endif
}

template<class T, class Deleter>
inline
const Ptr<T> UniquePtr<T, Deleter>::ptr() const {
  return ptr_.ptr();
}

template<class T, class Deleter>
inline
T* UniquePtr<T, Deleter>::get() const
{
  return ptr_.get();
}

template<class T, class Deleter>
inline
T* UniquePtr<T, Deleter>::getRawPtr() const
{
  return get();
}

template<class T, class Deleter>
inline
Ptr<const T> UniquePtr<T, Deleter>::getConst() const
{
  return uniqueptr_implicit_cast<const T>(*this);
}

template<class T, class Deleter>
inline
void UniquePtr<T, Deleter>::reset(T *r_ptr)
{
#ifdef TEUCHOS_DEBUG
  ptr_.reset();
  ptr_ = rcpWithDealloc(r_ptr, deallocFunctorDelete<T, Deleter>(d_));
#else
  d_(ptr_.get()); // Note: the pointer can be null
  ptr_ = Ptr<T>(r_ptr);
#endif
}

//------------------------------------------------------
// Non member functions


/** \brief Returns true if <tt>p.get()==NULL</tt>.
 *
 * \relates UniquePtr
 */
template<class T, class Deleter=std::default_delete<T>>
inline
bool is_null( const UniquePtr<T, Deleter> &p )
{
  return p.get() == 0;
}

/** \brief Returns true if <tt>p.get()!=NULL</tt>
 *
 * \relates UniquePtr
 */
template<class T, class Deleter=std::default_delete<T>>
inline
bool nonnull( const UniquePtr<T, Deleter> &p )
{
  return p.get() != 0;
}

#endif // HAVE_TEUCHOSCORE_CXX11


} // namespace Teuchos

#endif // TEUCHOS_PTR_HPP
