#include<iostream>
#include"OrthogonalPolynomials.hpp"
#include"Lagrange.hpp"
#include"LinearAlgebra.hpp"

#ifndef __NODAL_BASIS__
#define __NODAL_BASIS__


template<class Real>
class NodalBasis{

    private:
        const Teuchos::LAPACK<int,Real> * const lapack_;
 
        //! \param ni_ Number of interpolation points
        const int ni_;

        //! \param nq_ Number of quadrature points
        const int nq_;

    public:
        NodalBasis(const Teuchos::LAPACK<int,Real> * const lapack, const int ni, const int nq);    
        ~NodalBasis();

         //! \param xi_ Vector of interpolation points
        std::vector<Real> xi_;

        //! \param xq_ Vector of quadrature points
        std::vector<Real> xq_;

        //! \param wq_ Vector of quadrature weights
        std::vector<Real> wq_;

        //! \param L_ Column stacked vector of vectors of interpolating functions  
        std::vector<Real> L_;
        std::vector<Real> Lp_;

      
};

/** \brief Set up quantities we will need repeatedly
*/
template<class Real>
NodalBasis<Real>::NodalBasis(const Teuchos::LAPACK<int,Real> * const lapack, const int ni, const int nq): 
    lapack_(lapack), ni_(ni), nq_(nq), xi_(ni_,0), xq_(nq_,0), wq_(nq_,0), L_(ni_*nq_,0), Lp_(ni_*nq_,0) {

    // Generate Legendre-Gauss-Lobatto nodes and weights (unused)
    std::vector<Real> ai(ni_,0);
    std::vector<Real> bi(ni_,0);
    std::vector<Real> wi(ni_,0);
    rec_jacobi(this->lapack_,0,0,ai,bi); 
    rec_lobatto(this->lapack_,-1.0,1.0,ai,bi);
    gauss(this->lapack_,ai,bi,xi_,wi);

    // Generate Legendre-Gauss nodes and weights
    std::vector<Real> aq(nq_,0);
    std::vector<Real> bq(nq_,0);
    rec_jacobi(this->lapack_,0,0,aq,bq); 
    gauss(this->lapack_,aq,bq,xq_,wq_);

    std::vector<Real> e(ni_,0);
    std::vector<Real> ell(nq,0);

    Lagrange<Real> lagrange(xi_,xq_);

    // Loop over canonical vectors
    for(int i=0;i<ni_;++i) {

        lagrange.interpolant(i,ell);

        std::copy(ell.begin(),ell.end(),L_.begin()+i*nq_);

        lagrange.derivative(i,ell);
        std::copy(ell.begin(),ell.end(),Lp_.begin()+i*nq_);

//        for(int j=0;j<nq_;++j){
//            std::cout << L_[i*nq_+j] << ' ';
//        }
//        std::cout << std::endl;
        // Rezero the vector 
        std::fill(e.begin(),e.end(),0);
    }     



}

template<class Real>
NodalBasis<Real>::~NodalBasis(){
}

#endif
