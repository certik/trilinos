// @HEADER
// ***********************************************************************
//
//                 Anasazi: Block Eigensolvers Package
//                 Copyright (2004) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ***********************************************************************
// @HEADER
//
//  This test is for the LOBPCG solver
//
#include "AnasaziConfigDefs.hpp"
#include "AnasaziTypes.hpp"

#include "AnasaziEpetraAdapter.hpp"
#include "Epetra_CrsMatrix.h"
#include "Epetra_Vector.h"

#include "AnasaziLOBPCG.hpp"

#include "AnasaziBasicEigenproblem.hpp"
#include "AnasaziBasicOutputManager.hpp"
#include "AnasaziSVQBOrthoManager.hpp"
#include "AnasaziBasicSort.hpp"
#include "AnasaziStatusTestMaxIters.hpp"
#include "AnasaziModalSolverUtils.hpp"
#include "Teuchos_CommandLineProcessor.hpp"
#include "Teuchos_SerialDenseMatrix.hpp"

#ifdef HAVE_MPI
#include "Epetra_MpiComm.h"
#include <mpi.h>
#else
#include "Epetra_SerialComm.h"
#endif

#include "ModeLaplace1DQ1.h"

using namespace Teuchos;
using namespace Anasazi;

typedef double                              ScalarType;
typedef ScalarTraits<ScalarType>                   SCT;
typedef SCT::magnitudeType               MagnitudeType;
typedef Epetra_MultiVector                 MV;
typedef Epetra_Operator                    OP;
typedef MultiVecTraits<ScalarType,MV>     MVT;
typedef OperatorTraits<ScalarType,MV,OP>  OPT;

class get_out : public std::logic_error {
  public: get_out(const std::string &whatarg) : std::logic_error(whatarg) {}
};

void checks( RefCountPtr<LOBPCG<ScalarType,MV,OP> > solver, int blocksize, bool fullortho, 
             RefCountPtr<Eigenproblem<ScalarType,MV,OP> > problem,
             RefCountPtr<MatOrthoManager<ScalarType,MV,OP> > ortho,
             ModalSolverUtils<ScalarType,MV,OP> &msutils) {
  LOBPCGState<ScalarType,MV> state = solver->getState();
  
  TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.X)  != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for X");
  TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.KX) != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for KX");
  TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.R)  != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for R");
  TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.H)  != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for H");
  TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.KH) != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for KH");
  TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.P)  != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for P");
  TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.KP) != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for KP");
  if (solver->getProblem().getM() != null) {
    TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.MX) != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for MX");
    TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.MH) != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for MH");
    TEST_FOR_EXCEPTION(MVT::GetNumberVecs(*state.MP) != solver->getBlockSize(),get_out,"blockSize() does not match allocated size for MP");
  }
  else {
    TEST_FOR_EXCEPTION(state.MX != null,get_out,"MX should null; problem has no M matrix");
    TEST_FOR_EXCEPTION(state.MH != null,get_out,"MH should null; problem has no M matrix");
    TEST_FOR_EXCEPTION(state.MP != null,get_out,"MP should null; problem has no M matrix");
  }
  TEST_FOR_EXCEPTION(solver->getBlockSize() != blocksize, get_out,"Solver block size does not match specified block size.");  
  TEST_FOR_EXCEPTION(solver->getFullOrtho() != fullortho, get_out,"Solver full ortho does not match specified state.");
  TEST_FOR_EXCEPTION(&solver->getProblem() != problem.get(),get_out,"getProblem() did not return the submitted problem.");
  TEST_FOR_EXCEPTION(solver->getResidualVecs() != state.R,get_out,"getResidualVecs() not pointing to state.R");
  TEST_FOR_EXCEPTION(solver->getEvecs() != state.X,get_out,"getEvecs() not pointing to state.X");
  TEST_FOR_EXCEPTION(solver->getMaxSubspaceDim() != 3*blocksize,get_out,"LOBPCG::getMaxSubspaceDim() should always be 3*blocksize");

  if (solver->isInitialized()) 
  {
    // check residuals
    RefCountPtr<const MV> evecs = solver->getEvecs();
    RefCountPtr<MV> Kevecs, Mevecs;
    Kevecs = MVT::Clone(*evecs,blocksize);
    OPT::Apply(*problem->getOperator(),*evecs,*Kevecs);
    if (problem->getM() == null) {
      Mevecs = MVT::CloneCopy(*evecs);
    }
    else {
      Mevecs = MVT::Clone(*evecs,blocksize);
      OPT::Apply(*problem->getM(),*evecs,*Mevecs);
    }
    vector<MagnitudeType> theta = solver->getEigenvalues();
    TEST_FOR_EXCEPTION(theta.size() != blocksize,get_out,"getEigenvalues() has incorrect size.");
    SerialDenseMatrix<int,ScalarType> T(blocksize,blocksize);
    for (int i=0; i<blocksize; i++) T(i,i) = theta[i];
    // LOBPCG computes residuals like R = K*X - M*X*T 
    MVT::MvTimesMatAddMv(-1.0,*Mevecs,T,1.0,*Kevecs);
    MagnitudeType error = msutils.errorEquality(Kevecs.get(),solver->getResidualVecs().get());
    // residuals from LOBPCG should be exact; we will cut a little slack
    TEST_FOR_EXCEPTION(error > 1e-14,get_out,"Residuals from solver did not match eigenvectors.");

    // check eigenvalues
    // X should be ritz vectors; they should diagonalize K to produce the current eigenvalues
    MagnitudeType ninf = T.normInf();
    OPT::Apply(*problem->getOperator(),*evecs,*Kevecs);
    MVT::MvTransMv(1.0,*evecs,*Kevecs,T);
    for (int i=0; i<blocksize; i++) T(i,i) -= theta[i];
    error = T.normFrobenius() / ninf;
    TEST_FOR_EXCEPTION(error > 1e-14,get_out,"Ritz values don't match eigenvectors.");

    // check that eigenvectors match ritz vectors
    std::vector<MagnitudeType> rvals = solver->getRitzValues();
    TEST_FOR_EXCEPTION(rvals.size() != solver->getCurSubspaceDim(),get_out,"getRitzValues() has incorrect size.");
    error = 0.0;
    for (int i=0; i<blocksize; i++) error += SCT::magnitude(theta[i] - rvals[i]);
    // this should be exact; we will require it
    TEST_FOR_EXCEPTION(error != 0.0,get_out,"Ritz values don't match eigenvalues.");

    if (solver->hasP() && solver->getFullOrtho() == true) {
      // this should be small
      error = ortho->orthogError(*state.X,*state.P);
      TEST_FOR_EXCEPTION(error >= 1e-14,get_out,"FullOrtho enabled and P not orthogonal to X.");
    }
  }
  else {
    // not initialized
    TEST_FOR_EXCEPTION(solver->hasP() != false,get_out,"In unitialized state, hasP() should be false.");
    TEST_FOR_EXCEPTION(solver->getCurSubspaceDim() != 0,get_out,"In unitialized state, getCurSubspaceDim() should be 0.");
  }
}

void testsolver( RefCountPtr<BasicEigenproblem<ScalarType,MV,OP> > problem,
                 RefCountPtr< OutputManager<ScalarType> > printer,
                 RefCountPtr< MatOrthoManager<ScalarType,MV,OP> > ortho,
                 RefCountPtr< SortManager<ScalarType,MV,OP> > sorter,
                 ParameterList &pls)
{
  // create a status tester
  RefCountPtr< StatusTest<ScalarType,MV,OP> > tester = rcp( new StatusTestMaxIters<ScalarType,MV,OP>(1) );
  // create the solver
  RefCountPtr< LOBPCG<ScalarType,MV,OP> > solver = rcp( new LOBPCG<ScalarType,MV,OP>(problem,sorter,printer,tester,ortho,pls) );

  const int  blocksize = pls.get<int>("Block Size");
  const bool fullortho = pls.get<bool>("Full Ortho");

  ModalSolverUtils<ScalarType,MV,OP> msutils(printer);

  // solver should be uninitialized
  TEST_FOR_EXCEPTION(solver->isInitialized() != false,get_out,"Solver should be un-initialized after instantiation.");  
  TEST_FOR_EXCEPTION(solver->getNumIters() != 0,get_out,"Number of iterations after initialization should be zero after init.")
  TEST_FOR_EXCEPTION(solver->hasP() != false,get_out,"Uninitialized solver should not have valid search directions.");
  TEST_FOR_EXCEPTION(solver->getAuxVecs().size() != 0,get_out,"getAuxVecs() should return empty.");
  checks(solver,blocksize,fullortho,problem,ortho,msutils);

  // initialize solver and perform checks
  solver->initialize();
  std::vector<MagnitudeType> vals1 = solver->getEigenvalues();
  MagnitudeType sum1 = 0.0;
  for (int i=0; i<blocksize; i++) sum1 += vals1[i];
  TEST_FOR_EXCEPTION(solver->isInitialized() != true,get_out,"Solver should be initialized after call to initialize().");  
  TEST_FOR_EXCEPTION(solver->getNumIters() != 0,get_out,"Number of iterations should be zero.")
  TEST_FOR_EXCEPTION(solver->hasP() != false,get_out,"Solver should not have valid P.");
  TEST_FOR_EXCEPTION(solver->getAuxVecs().size() != 0,get_out,"getAuxVecs() should return empty.");
  TEST_FOR_EXCEPTION(solver->getCurSubspaceDim() != blocksize,get_out,"after init, getCurSubspaceDim() should be blocksize.");
  checks(solver,blocksize,fullortho,problem,ortho,msutils);

  // call iterate(); solver should perform exactly one iteration and return; status test should be passed
  solver->iterate();
  std::vector<MagnitudeType> vals2 = solver->getEigenvalues();
  MagnitudeType sum2 = 0.0;
  for (int i=0; i<blocksize; i++) sum2 += vals2[i];
  TEST_FOR_EXCEPTION(tester->getStatus() != Passed,get_out,"Solver returned from iterate() but getStatus() not Passed.");
  TEST_FOR_EXCEPTION(sum2 < sum1,get_out,"LOBPCG set to ascent method; sum of eigenvalues should increase.");
  TEST_FOR_EXCEPTION(solver->isInitialized() != true,get_out,"Solver should be initialized after call to initialize().");  
  TEST_FOR_EXCEPTION(solver->getNumIters() != 1,get_out,"Number of iterations should be zero.")
  TEST_FOR_EXCEPTION(solver->hasP() != true,get_out,"Solver should have valid P.");
  TEST_FOR_EXCEPTION(solver->getAuxVecs().size() != 0,get_out,"getAuxVecs() should return empty.");
  TEST_FOR_EXCEPTION(solver->getCurSubspaceDim() != 2*blocksize,get_out,"after one step, getCurSubspaceDim() should be 2*blocksize.");
  checks(solver,blocksize,fullortho,problem,ortho,msutils);

  // reset numiters, call iterate(); solver should perform exactly one iteration and return; status test should be passed
  solver->resetNumIters();
  TEST_FOR_EXCEPTION(solver->getNumIters() != 0,get_out,"Number of iterations should be zero after resetNumIters().")
  solver->iterate();
  std::vector<MagnitudeType> vals3 = solver->getEigenvalues();
  MagnitudeType sum3 = 0.0;
  for (int i=0; i<blocksize; i++) sum3 += vals3[i];
  TEST_FOR_EXCEPTION(tester->getStatus() != Passed,get_out,"Solver returned from iterate() but getStatus() not Passed.");
  TEST_FOR_EXCEPTION(sum3 < sum2,get_out,"LOBPCG set to ascent method; sum of eigenvalues should increase.");
  TEST_FOR_EXCEPTION(solver->isInitialized() != true,get_out,"Solver should be initialized after call to initialize().");  
  TEST_FOR_EXCEPTION(solver->getNumIters() != 1,get_out,"Number of iterations should be zero.")
  TEST_FOR_EXCEPTION(solver->hasP() != true,get_out,"Solver should have valid P.");
  TEST_FOR_EXCEPTION(solver->getAuxVecs().size() != 0,get_out,"getAuxVecs() should return empty.");
  TEST_FOR_EXCEPTION(solver->getCurSubspaceDim() != 3*blocksize,get_out,"after two steps, getCurSubspaceDim() should be 3*blocksize.");
  checks(solver,blocksize,fullortho,problem,ortho,msutils);
}

int main(int argc, char *argv[]) 
{

#ifdef HAVE_MPI
  // Initialize MPI
  MPI_Init(&argc,&argv);
  Epetra_MpiComm Comm(MPI_COMM_WORLD);
#else
  Epetra_SerialComm Comm;
#endif

  bool testFailed;
  bool verbose = false;

  CommandLineProcessor cmdp(false,true);
  cmdp.setOption("verbose","quiet",&verbose,"Print messages and results.");
  if (cmdp.parse(argc,argv) != CommandLineProcessor::PARSE_SUCCESSFUL) {
#ifdef HAVE_MPI
    MPI_Finalize();
#endif
    return -1;
  }

  // create the output manager
  RefCountPtr< OutputManager<ScalarType> > printer = rcp( new BasicOutputManager<ScalarType>() );

  if (verbose) {
    printer->stream(Errors) << Anasazi_Version() << endl << endl;
  }

  //  Problem information
  int space_dim = 1;
  std::vector<double> brick_dim( space_dim );
  brick_dim[0] = 1.0;
  std::vector<int> elements( space_dim );
  elements[0] = 100;

  // Create problem
  RefCountPtr<ModalProblem> testCase = rcp( new ModeLaplace1DQ1(Comm, brick_dim[0], elements[0]) );
  //
  // Get the stiffness and mass matrices
  RefCountPtr<Epetra_CrsMatrix> K = rcp( const_cast<Epetra_CrsMatrix *>(testCase->getStiffness()), false );
  RefCountPtr<Epetra_CrsMatrix> M = rcp( const_cast<Epetra_CrsMatrix *>(testCase->getMass()), false );
  //
  // Create the initial vectors
  const int nev = 4;
  RefCountPtr<Epetra_MultiVector> ivec = rcp( new Epetra_MultiVector(K->OperatorDomainMap(), nev) );
  ivec->Random();
  //
  // Create eigenproblem: one standard and one generalized
  RefCountPtr<BasicEigenproblem<ScalarType,MV,OP> > probstd = rcp( new BasicEigenproblem<ScalarType, MV, OP>(K, ivec) );
  RefCountPtr<BasicEigenproblem<ScalarType,MV,OP> > probgen = rcp( new BasicEigenproblem<ScalarType, MV, OP>(K, M, ivec) );
  //
  // Inform the eigenproblem that the operator A is symmetric
  probstd->setHermitian(true);
  probgen->setHermitian(true);
  //
  // Set the number of eigenvalues requested
  probstd->setNEV( nev );
  probgen->setNEV( nev );
  //
  // Inform the eigenproblem that you are finishing passing it information
  if ( probstd->setProblem() != true || probgen->setProblem() != true ) {
    if (verbose) {
      printer->stream(Errors) << "Anasazi::BasicEigenproblem::SetProblem() returned with error." << endl
                              << "End Result: TEST FAILED" << endl;	
    }
#ifdef HAVE_MPI
    MPI_Finalize() ;
#endif
    return -1;
  }

  // create the orthogonalization managers: one standard and one M-based
  RefCountPtr< MatOrthoManager<ScalarType,MV,OP> > orthostd = rcp( new SVQBOrthoManager<ScalarType,MV,OP>() );
  RefCountPtr< MatOrthoManager<ScalarType,MV,OP> > orthogen = rcp( new SVQBOrthoManager<ScalarType,MV,OP>(M) );
  // create the sort manager
  RefCountPtr< SortManager<ScalarType,MV,OP> > sorter = rcp( new BasicSort<ScalarType,MV,OP>("LR") );
  // create the parameter list specifying blocksize > nev and full orthogonalization
  ParameterList pls;

  // begin testing 
  testFailed = false;

  try 
  {

    pls.set<int>("Block Size",nev);
    pls.set<bool>("Full Ortho",false);
    if (verbose) {
      printer->stream(Errors) << "Testing solver(nev,false) with standard eigenproblem..." << endl;
    }
    testsolver(probstd,printer,orthostd,sorter,pls);
    pls.set<bool>("Full Ortho",true);
    if (verbose) {
      printer->stream(Errors) << "Testing solver(nev,true) with generalized eigenproblem..." << endl;
    }
    testsolver(probgen,printer,orthogen,sorter,pls);

    pls.set<int>("Block Size",2*nev);
    pls.set<bool>("Full Ortho",false);
    if (verbose) {
      printer->stream(Errors) << "Testing solver(2*nev,false) with standard eigenproblem..." << endl;
    }
    testsolver(probstd,printer,orthostd,sorter,pls);
    pls.set<bool>("Full Ortho",true);
    if (verbose) {
      printer->stream(Errors) << "Testing solver(2*nev,true) with generalized eigenproblem..." << endl;
    }
    testsolver(probgen,printer,orthogen,sorter,pls);

    pls.set<int>("Block Size",nev/2);
    pls.set<bool>("Full Ortho",false);
    if (verbose) {
      printer->stream(Errors) << "Testing solver(nev/2,false) with standard eigenproblem..." << endl;
    }
    testsolver(probstd,printer,orthostd,sorter,pls);
    pls.set<bool>("Full Ortho",true);
    if (verbose) {
      printer->stream(Errors) << "Testing solver(nev/2,true) with generalized eigenproblem..." << endl;
    }
    testsolver(probgen,printer,orthogen,sorter,pls);

  }
  catch (get_out go) {
    printer->stream(Errors) << "Test failed: " << go.what() << endl;
    testFailed = true;
  }
  catch (std::exception e) {
    printer->stream(Errors) << "Caught unexpected exception: " << e.what() << endl;
    testFailed = true;
  }

  
#ifdef HAVE_MPI
  MPI_Finalize() ;
#endif

  if (testFailed) {
    if (verbose) {
      printer->stream(Errors) << endl << "End Result: TEST FAILED" << endl;	
    }
    return -1;
  }
  //
  // Default return value
  //
  if (verbose) {
    printer->stream(Errors) << endl << "End Result: TEST PASSED" << endl;
  }
  return 0;

}	
