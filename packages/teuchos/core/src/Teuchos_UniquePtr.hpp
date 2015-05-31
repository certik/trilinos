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

  inline explicit UniquePtr( T *ptr_in );

  inline UniquePtr( T *ptr_in,
      typename std::conditional<std::is_reference<Deleter>::value,
        Deleter, const Deleter&>::type d);

  inline UniquePtr( T *ptr_in,
      typename std::remove_reference<Deleter>::type &&d);

  ~UniquePtr();

  // Copy constructor and assignment are disabled
  inline UniquePtr(const UniquePtr<T>& ptr) = delete;
  template<class U, class E>
  inline UniquePtr(const UniquePtr<U, E>& ptr) = delete;
  UniquePtr<T>& operator=(const UniquePtr<T>& ptr) = delete;
  template<class U, class E>
  UniquePtr<T>& operator=(const UniquePtr<U, E>& ptr) = delete;

  // Move constructor and assignment
  inline UniquePtr(UniquePtr &&r_ptr);

  template<class U, class E>
  inline UniquePtr(UniquePtr<U, E> &&r_ptr);

  inline UniquePtr& operator=(UniquePtr &&r_ptr);

  template<class U, class E>
  inline UniquePtr& operator=(UniquePtr<U, E> &&r_ptr);


  /*
   *                   Modifiers
   */

  inline T* release();

  /** \brief Reset to r_ptr (default: null). */
  inline void reset(T *r_ptr=nullptr);

  inline void swap(UniquePtr & other) noexcept;

  /*
   *                   Observers
   */

  /** \brief Get the raw C++ pointer to the underlying object. */
  inline T* get() const;

  inline Deleter& get_deleter() noexcept;

  inline const Deleter& get_deleter() const noexcept;

  inline explicit operator bool() const;

  /*
   *                   Dereferencing
   */

  inline T* operator->() const;
  inline T& operator*() const;

  /*
   *                   Observers Ptr
   */

  /** \brief Return a Ptr<T> version of *this. */
  inline const Ptr<T> ptr() const;

  /** \brief Return a Ptr<const T> version of *this. */
  inline Ptr<const T> getConst() const;

  /*
   *                   Teuchos compatibility methods
   */

  /** \brief Get the raw C++ pointer to the underlying object. */
  inline T* getRawPtr() const;


private:
  typedef
#ifdef TEUCHOS_DEBUG
    RCP<T>
#else
    Ptr<T>
#endif
      pointer;

  std::tuple<pointer, Deleter> t_;
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
inline UniquePtr<T, Deleter>::UniquePtr( ENull /*null_in*/ )
  : t_(null, Deleter())
{}

template<class T, class Deleter>
inline UniquePtr<T, Deleter>::UniquePtr( T *ptr_in )
  : t_(pointer(ptr_in), Deleter())
{}

template<class T, class Deleter>
inline UniquePtr<T, Deleter>::UniquePtr( T *ptr_in,
      typename std::conditional<std::is_reference<Deleter>::value,
        Deleter, const Deleter&>::type d)
  : t_(pointer(ptr_in
#ifdef TEUCHOS_DEBUG
        , deallocFunctorDelete<T, Deleter>(d), true
#endif
        ), d)
{}

template<class T, class Deleter>
inline UniquePtr<T, Deleter>::UniquePtr( T *ptr_in,
    typename std::remove_reference<Deleter>::type &&d)
  : t_(pointer(ptr_in
#ifdef TEUCHOS_DEBUG
        , deallocFunctorDelete<T, Deleter>(d), true
#endif
        ), std::move(d))
{}

template<class T, class Deleter>
UniquePtr<T, Deleter>::~UniquePtr()
{
#ifndef TEUCHOS_DEBUG
  get_deleter()(std::get<0>(t_).get()); // Note: the pointer can be null
#endif
}

template<class T, class Deleter>
inline UniquePtr<T, Deleter>::UniquePtr(UniquePtr &&r_ptr)
  : t_(pointer(r_ptr.release()), std::forward<Deleter>(r_ptr.get_deleter()))
{}

template<class T, class Deleter>
template<class U, class E>
inline UniquePtr<T, Deleter>::UniquePtr(UniquePtr<U, E> &&r_ptr)
  : t_(pointer(r_ptr.release()), std::forward<E>(r_ptr.get_deleter()))
{}

template<class T, class Deleter>
inline
const Ptr<T> UniquePtr<T, Deleter>::ptr() const {
  return std::get<0>(t_).ptr();
}

template<class T, class Deleter>
inline
UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr<T, Deleter> &&r_ptr) {
  reset(r_ptr.release());
  get_deleter() = std::forward<Deleter>(r_ptr.get_deleter());
  return *this;
}

template<class T, class Deleter>
template<class U, class E>
inline UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr<U, E> &&r_ptr) {
  reset(r_ptr.release());
  get_deleter() = std::forward<E>(r_ptr.get_deleter());
  return *this;
}

template<class T, class Deleter>
inline T* UniquePtr<T, Deleter>::release()
{
  T *p = get();
#ifdef TEUCHOS_DEBUG
  std::get<0>(t_).release(); // So that the next line does not delete the object
#endif
  std::get<0>(t_) = null;
  return p;
}

template<class T, class Deleter>
inline void UniquePtr<T, Deleter>::swap(UniquePtr & other) noexcept
{
  std::swap(std::get<0>(t_), std::get<0>(other.t_));
  std::swap(get_deleter(), other.get_deleter());
}

template<class T, class Deleter>
inline
T* UniquePtr<T, Deleter>::get() const
{
  return std::get<0>(t_).get();
}

template<class T, class Deleter>
inline
Deleter& UniquePtr<T, Deleter>::get_deleter() noexcept
{
  return std::get<1>(t_);
}

template<class T, class Deleter>
inline
const Deleter& UniquePtr<T, Deleter>::get_deleter() const noexcept
{
  return std::get<1>(t_);
}

template<class T, class Deleter>
inline
UniquePtr<T, Deleter>::operator bool() const
{
  return get() != nullptr;
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
T* UniquePtr<T, Deleter>::operator->() const
{
  return std::get<0>(t_).operator->();
}

template<class T, class Deleter>
inline
T& UniquePtr<T, Deleter>::operator*() const {
  return std::get<0>(t_).operator*();
}

template<class T, class Deleter>
inline
void UniquePtr<T, Deleter>::reset(T *r_ptr)
{
#ifdef TEUCHOS_DEBUG
  std::get<0>(t_).reset();
  std::get<0>(t_) = rcpWithDealloc(r_ptr, deallocFunctorDelete<T,
      Deleter>(get_deleter()));
#else
  get_deleter()(std::get<0>(t_).get()); // Note: the pointer can be null
  std::get<0>(t_) = Ptr<T>(r_ptr);
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
