/* @HEADER@ */
/* @HEADER@ */

#ifndef TSFLINEAROPERATOR_HPP
#define TSFLINEAROPERATOR_HPP

#include "TSFConfigDefs.hpp"
#include "TSFHandle.hpp"
#include "TSFHandleable.hpp"
#include "TSFCoreLinearOp.hpp"
#include "TSFLoadableMatrix.hpp"
#include "Teuchos_TimeMonitor.hpp"
#include "TSFVector.hpp"

namespace TSFExtended
{
  using TSFCore::Index;
  using namespace Teuchos;

  /** 
   * User-level linear operator class
   */
  template <class Scalar>
  class LinearOperator : public Handle<TSFCore::LinearOp<Scalar> >
    {
    public:
      /** \name Constructors, Destructors, and Assignment Operators */
      //@{
      /** empty ctor. Constructs a null operator */
      LinearOperator();

      /** Construct with an existing smart pointer */
      LinearOperator(const RefCountPtr<TSFCore::LinearOp<Scalar> >& smartPtr);

      /** Construct with a handleable raw pointer */
      LinearOperator(Handleable<TSFCore::LinearOp<Scalar> >* rawPtr);
      
      //@}

      /** */
      RefCountPtr<const TSFCore::VectorSpace<Scalar> > domain() const 
      {return ptr()->domain();}

      /** */
      RefCountPtr<const TSFCore::VectorSpace<Scalar> > range() const 
      {return ptr()->range();}

      /** */
      void apply(const Vector<Scalar>& in,
                 Vector<Scalar>& out) const ;

      // /** */
//       void applyTranspose(const Vector<Scalar>& in,
//                           Vector<Scalar>& out) const ;

//       /** */
//       LinearOperator<Scalar> transpose() const ;

//       /** */
//       LinearOperator<Scalar> form() const ;
      
      
      /** Get a stopwtach for timing vector operations */
      static RefCountPtr<Time>& opTimer()
      {
        static RefCountPtr<Time> rtn 
          = TimeMonitor::getNewTimer("Low-level vector operations");
        return rtn;
      }

      RefCountPtr<LoadableMatrix<Scalar> > matrix()
      {
        RefCountPtr<LoadableMatrix<Scalar> > rtn 
          = rcp_dynamic_cast<LoadableMatrix<Scalar> >(ptr());
        return rtn;
      }
    private:
    };

  template <class Scalar> inline 
  LinearOperator<Scalar>::LinearOperator()
    : Handle<TSFCore::LinearOp<Scalar> >()
  {}


  template <class Scalar> inline 
  LinearOperator<Scalar>
  ::LinearOperator(const RefCountPtr<TSFCore::LinearOp<Scalar> >& smartPtr)
    : Handle<TSFCore::LinearOp<Scalar> >(smartPtr)
  {}

  template <class Scalar> inline 
  LinearOperator<Scalar>
  ::LinearOperator(Handleable<TSFCore::LinearOp<Scalar> >* ptr)
    : Handle<const TSFCore::LinearOp<Scalar> >(ptr)
  {}

  template <class Scalar> inline 
  void LinearOperator<Scalar>::apply(const Vector<Scalar>& in,
                                     Vector<Scalar>& out) const
  {
    ptr()->apply(TSFCore::NOTRANS, *(in.ptr().get()),
                 out.ptr().get());
  }
}


#endif
