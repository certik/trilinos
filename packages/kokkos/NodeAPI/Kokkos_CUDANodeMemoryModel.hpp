#ifndef KOKKOS_CUDA_NODE_MEMORY_MODEL_HPP_
#define KOKKOS_CUDA_NODE_MEMORY_MODEL_HPP_

#include "Kokkos_NodeAPIConfigDefs.hpp"
#include <Teuchos_RCP.hpp>

// forward declarations of Teuchos classes
namespace std {
  template <typename CharT> class char_traits;
}
namespace Teuchos {
  class ParameterList;
  template <typename T> class ArrayRCP;
  template <typename T> class ArrayView;
  template <typename T> class RCP;
  template <typename CharT, typename Traits> class basic_FancyOStream;
  typedef basic_FancyOStream<char, std::char_traits<char> > FancyOStream;
}
namespace Kokkos {

  using Teuchos::ArrayRCP;
  using Teuchos::ArrayView;
  using Teuchos::RCP;

  /** \brief A default implementation of the Node memory architecture for Node with a distinct device memory space allocated by the CUDA runtime.
      \ingroup kokkos_node_api
   */
  class CUDANodeMemoryModel {
    public:
      //! Indicates that parallel buffers allocated by this node are not available for use on the host thread.
      static const bool isHostNode = false;

      //@{ Default Constructor 

      CUDANodeMemoryModel();

      //@}

      //@{ Memory management

      /*! \brief Allocate a parallel buffer, returning it as a pointer ecnapsulated in an ArrayRCP.

          Dereferencing the returned ArrayRCP or its underlying pointer in general results in undefined 
          behavior outside of parallel computations.

          The buffer will be automatically freed by the Node when no more references remain.

          @tparam T The data type of the allocate buffer. This is used to perform alignment and determine the number of bytes to allocate.
          @param[in] size The size requested for the parallel buffer, greater than zero.

          \post The method will return an ArrayRCP encapsulating a pointer. The underlying pointer may be used in parallel computation routines, 
                and is guaranteed to have size large enough to reference \c size number of entries of type \c T.
      */
      template <class T> inline
      ArrayRCP<T> allocBuffer(size_t size);

      /*! \brief Copy data to host memory from a parallel buffer.

          @param[in] size       The number of entries to copy from \c buffSrc to \c hostDest.
          @param[in] buffSrc    The parallel buffer from which to copy.
          @param[out] hostDest  The location in host memory where the data from \c buffSrc is copied to.

          \pre  \c size is non-negative.
          \pre  \c buffSrc has length at least <tt>size</tt>.
          \pre  \c hostDest has length equal to \c size.
          \post On return, entries in the range <tt>[0 , size)</tt> of \c buffSrc have been copied to \c hostDest entries in the range <tt>[0 , size)</tt>.
      */
      template <class T> inline
      void copyFromBuffer(size_t size, const ArrayRCP<const T> &buffSrc, const ArrayView<T> &hostDest);

      /*! \brief Copy data to host memory from a parallel buffer.

          @param[in]  size        The number of entries to copy from \c hostSrc to \c buffDest.
          @param[in]  hostSrc     The location in host memory from where the data is copied.
          @param[out] buffDest    The parallel buffer to which the data is copied.

          \pre  \c size is non-negative.
          \pre  \c hostSrc has length equal to \c size.
          \pre  \c buffSrc has length at least <tt>size</tt>.
          \post On return, entries in the range <tt>[0 , size)</tt> of \c hostSrc are allowed to be written to. The data is guaranteed to be present in \c buffDest before it is used in a parallel computation.
      */
      template <class T> inline
      void copyToBuffer(size_t size, const ArrayView<const T> &hostSrc, const ArrayRCP<T> &buffDest);

      /*! \brief Copy data between buffers.
          
        @param[in]     size     The size of the copy, greater than zero.
        @param[in]     buffSrc  The source buffer, with length at least as large as \c size.
        @param[in,out] buffDest The destination buffer, with length at least as large as \c size.

        \post The data is guaranteed to have been copied before any other usage of buffSrc or buffDest occurs.
      */
      template <class T> inline
      void copyBuffers(size_t size, const ArrayRCP<const T> &buffSrc, const ArrayRCP<T> &buffDest);

      template <class T> inline
      ArrayRCP<const T> viewBuffer(size_t size, ArrayRCP<const T> buff);

      template <class T> inline
      ArrayRCP<T> viewBufferNonConst(ReadWriteOption rw, size_t size, const ArrayRCP<T> &buff);

      inline void readyBuffers(ArrayView<ArrayRCP<const char> > buffers, ArrayView<ArrayRCP<char> > ncBuffers);

      //@}

      //@{ Book-keeping information

      void printStatistics(const RCP< Teuchos::FancyOStream > &os) const;

      //@}

    public:
      size_t allocSize_;
      size_t numCopiesD2H_, numCopiesH2D_, numCopiesD2D_;
      size_t bytesCopiedD2H_, bytesCopiedH2D_, bytesCopiedD2D_;
  };

} // end of namespace Kokkos

#ifndef KOKKOS_NO_INCLUDE_INSTANTIATIONS
#include "Kokkos_CUDANodeMemoryModelImpl.hpp"
#endif

#endif
