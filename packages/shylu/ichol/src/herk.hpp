#pragma once
#ifndef __HERK_HPP__
#define __HERK_HPP__

/// \file herk.hpp
/// \brief Sparse hermitian rank one update on given sparse patterns.
/// \author Kyungjoo Kim (kyukim@sandia.gov)

namespace Example {

  using namespace std;

  template<int ArgUplo, int ArgTrans, int ArgAlgo>
  struct Herk {

    // data-parallel interface
    // =======================
    template<typename ParallelForType,
             typename ScalarType,
             typename CrsExecViewType>
    KOKKOS_INLINE_FUNCTION
    static int invoke(const typename CrsExecViewType::policy_type::member_type &member,
                      const ScalarType alpha,
                      const CrsExecViewType &A,
                      const ScalarType beta,
                      const CrsExecViewType &C);

    // task-data parallel interface
    // ============================
    template<typename ParallelForType,
             typename ScalarType,
             typename CrsExecViewType>
    class TaskFunctor {
    private:
      ScalarType _alpha, _beta;
      CrsExecViewType _A, _C;

    public:
      typedef typename CrsExecViewType::policy_type policy_type;
      typedef typename policy_type::member_type member_type;
      typedef int value_type;

      TaskFunctor(const ScalarType alpha,
                  const CrsExecViewType A,
                  const ScalarType beta,
                  const CrsExecViewType C)
        : _alpha(alpha),
          _beta(beta),
          _A(A),
          _C(C)
      { }

      string Label() const { return "Herk"; }

      // task execution
      void apply(value_type &r_val) {
        r_val = Herk::invoke<ParallelForType,ScalarType,CrsExecViewType>(policy_type::member_null(),
                                                                         _alpha, _A, _beta, _C);
      }

      // task-data execution
      void apply(const member_type &member, value_type &r_val) const {
        r_val = Herk::invoke<ParallelForType,ScalarType,CrsExecViewType>(member,
                                                                         _alpha, _A, _beta, _C);
      }

    };

  };

}

#include "herk_u_t.hpp"

#endif
