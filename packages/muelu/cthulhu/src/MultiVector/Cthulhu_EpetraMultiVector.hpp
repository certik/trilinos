#ifndef CTHULHU_EPETRAMULTIVECTOR_HPP
#define CTHULHU_EPETRAMULTIVECTOR_HPP

/* this file is automatically generated - do not edit (see script/epetra.py) */

#include "Cthulhu_EpetraConfigDefs.hpp"

#include "Cthulhu_MultiVector.hpp"
#include "Cthulhu_Vector.hpp"

#include "Cthulhu_EpetraMap.hpp" 
#include "Cthulhu_EpetraExport.hpp"
#include "Cthulhu_Utils.hpp"
#include "Cthulhu_EpetraUtils.hpp"

#include <Epetra_MultiVector.h>

namespace Cthulhu {

  // TODO: move that elsewhere
  const Epetra_MultiVector & toEpetra(const MultiVector<double,int,int> &);

  Epetra_MultiVector & toEpetra(MultiVector<double, int,int> &);
  //

  // #ifndef DOXYGEN_SHOULD_SKIP_THIS
  //   // forward declaration of EpetraVector, needed to prevent circular inclusions
  //   template<class S, class LO, class GO, class N> class EpetraVector;
  // #endif

  class EpetraMultiVector
    : public virtual MultiVector<double, int, int>
  {

    typedef double Scalar;
    typedef int LocalOrdinal;
    typedef int GlobalOrdinal;
    typedef Kokkos::DefaultNode::DefaultNodeType Node;

  public:

    //! @name Constructor/Destructor Methods
    //@{

    //! Basic MultiVector constuctor.
    EpetraMultiVector(const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &map, size_t NumVectors, bool zeroOut=true)
      : vec_(Teuchos::rcp(new Epetra_MultiVector(toEpetra(map), NumVectors, zeroOut))) { }

    //! MultiVector copy constructor.
    EpetraMultiVector(const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &source)
      : vec_(Teuchos::rcp(new Epetra_MultiVector(toEpetra(source)))) { }

    //! MultiVector destructor.
    virtual ~EpetraMultiVector() { }

    //@}

    //! @name Post-construction modification routines
    //@{

    //! Initialize all values in a multi-vector with specified value.
    void putScalar(const Scalar &value) { vec_->PutScalar(value); }

    //! Set multi-vector values to random numbers.
    void randomize() { vec_->Random(); }

    //@}

    //! @name Data Copy and View get methods
    //@{

    //! 
    Teuchos::ArrayRCP< const Scalar > getData(size_t j) const;

    //! 
    Teuchos::ArrayRCP< Scalar > getDataNonConst(size_t j);

    //@}

    //! @name Mathematical methods
    //@{

    //! Computes dot product of each corresponding pair of vectors, dots[i] = this[i].dot(A[i]).
    void dot(const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &A, const Teuchos::ArrayView< Scalar > &dots) const;

    //! Puts element-wise absolute values of input Multi-vector in target: A = abs(this).
    void abs(const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &A) { vec_->Abs(toEpetra(A)); }

    //! Puts element-wise reciprocal values of input Multi-vector in target, this(i,j) = 1/A(i,j).
    void reciprocal(const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &A) { vec_->Reciprocal(toEpetra(A)); }

    //! Scale the current values of a multi-vector, this = alpha*this.
    void scale(const Scalar &alpha) { vec_->Scale(alpha); }

    //! Update multi-vector values with scaled values of A, this = beta*this + alpha*A.
    void update(const Scalar &alpha, const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &A, const Scalar &beta) { vec_->Update(alpha, toEpetra(A), beta); }

    //! Update multi-vector with scaled values of A and B, this = gamma*this + alpha*A + beta*B.
    void update(const Scalar &alpha, const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &A, const Scalar &beta, const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &B, const Scalar &gamma) { vec_->Update(alpha, toEpetra(A), beta, toEpetra(B), gamma); }

    //! Compute 1-norm of each vector in multi-vector.
    void norm1(const Teuchos::ArrayView< Teuchos::ScalarTraits< Scalar >::magnitudeType > &norms) const;

    //! Compute 2-norm of each vector in multi-vector.
    void norm2(const Teuchos::ArrayView< Teuchos::ScalarTraits< Scalar >::magnitudeType > &norms) const;

    //! Compute Inf-norm of each vector in multi-vector.
    void normInf(const Teuchos::ArrayView< Teuchos::ScalarTraits< Scalar >::magnitudeType > &norms) const;

    //! Compute Weighted 2-norm (RMS Norm) of each vector in multi-vector.
    void normWeighted(const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &weights, const Teuchos::ArrayView< Teuchos::ScalarTraits< Scalar >::magnitudeType > &norms) const;

    //! Compute mean (average) value of each vector in multi-vector.
    void meanValue(const Teuchos::ArrayView< Scalar > &means) const;

    //! Matrix-Matrix multiplication, this = beta*this + alpha*op(A)*op(B).
    void multiply(Teuchos::ETransp transA, Teuchos::ETransp transB, const Scalar &alpha, const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &A, const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &B, const Scalar &beta) { vec_->Multiply(toEpetra(transA), toEpetra(transB), alpha, toEpetra(A), toEpetra(B), beta); }

    //! Element-wise multiply of a Vector A with a MultiVector B.
    void elementWiseMultiply(Scalar scalarAB, const Vector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &A, const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &B, Scalar scalarThis) { vec_->Multiply(scalarAB, toEpetra(A), toEpetra(B), scalarThis); }

    //@}

    //! @name Attribute access functions
    //@{

    //! Returns the number of vectors in the multi-vector.
    size_t getNumVectors() const { return vec_->NumVectors(); }

    //! Returns the local vector length on the calling processor of vectors in the multi-vector.
    size_t getLocalLength() const { return vec_->MyLength(); }

    //! Returns the global vector length of vectors in the multi-vector.
    global_size_t getGlobalLength() const { return vec_->GlobalLength(); }

    //@}

    //! @name Overridden from Teuchos::Describable
    //@{

    //! Return a simple one-line description of this object.
    std::string description() const;

    //! Print the object with some verbosity level to an FancyOStream object.
    void describe(Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel=Teuchos::Describable::verbLevel_default) const;

    //@}

    //! Implements DistObject interface
    //{@

    //! Access function for the Tpetra::Map this DistObject was constructed with.
    const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > getMap() const { return toCthulhu(vec_->Map()); }

    //! Import.
    void doImport(const DistObject<Scalar, LocalOrdinal, GlobalOrdinal, Node> &source, const Import< LocalOrdinal, GlobalOrdinal, Node > &importer, CombineMode CM);

    //! Export.
    void doExport(const DistObject<Scalar, LocalOrdinal, GlobalOrdinal, Node> &dest, const Import< LocalOrdinal, GlobalOrdinal, Node >& importer, CombineMode CM);

    //! Import (using an Exporter).
    void doImport(const DistObject<Scalar, LocalOrdinal, GlobalOrdinal, Node> &source, const Export< LocalOrdinal, GlobalOrdinal, Node >& exporter, CombineMode CM);

    //! Export (using an Importer).
    void doExport(const DistObject<Scalar, LocalOrdinal, GlobalOrdinal, Node> &dest, const Export< LocalOrdinal, GlobalOrdinal, Node >& exporter, CombineMode CM);

    //@}

    //! @name Cthulhu specific
    //@{

    //! EpetraMultiVector constructor to wrap a Epetra_MultiVector object
    EpetraMultiVector(const RCP<Epetra_MultiVector> &vec) : vec_(vec) { } //TODO removed const

    //! Get the underlying Epetra multivector
    RCP<Epetra_MultiVector> getEpetra_MultiVector() const { return vec_; }

    //! Set seed for Random function.
    void setSeed(unsigned int seed) { vec_->SetSeed(seed); }
 
    //@}
    
  private:

    RCP< Epetra_MultiVector > vec_;
    
  }; // EpetraMultiVector class

} // Cthulhu namespace

#endif // CTHULHU_EPETRAMULTIVECTOR_HPP
