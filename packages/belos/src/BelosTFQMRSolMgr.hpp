// @HEADER
// ***********************************************************************
//
//                 Belos: Block Linear Solvers Package
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

#ifndef BELOS_TFQMR_SOLMGR_HPP
#define BELOS_TFQMR_SOLMGR_HPP

/*! \file BelosTFQMRSolMgr.hpp
 *  \brief The Belos::TFQMRSolMgr provides a solver manager for the TFQMR linear solver.
*/

#include "BelosConfigDefs.hpp"
#include "BelosTypes.hpp"

#include "BelosLinearProblem.hpp"
#include "BelosSolverManager.hpp"

#include "BelosTFQMRIter.hpp"
#include "BelosStatusTestMaxIters.hpp"
#include "BelosStatusTestGenResNorm.hpp"
#include "BelosStatusTestCombo.hpp"
#include "BelosStatusTestOutput.hpp"
#include "BelosOutputManager.hpp"
#include "Teuchos_BLAS.hpp"
#include "Teuchos_LAPACK.hpp"
#include "Teuchos_TimeMonitor.hpp"

/** \example TFQMR/TFQMREpetraEx.cpp
    This is an example of how to use the Belos::TFQMRSolMgr solver manager.
*/

/*! \class Belos::TFQMRSolMgr
 *
 *  \brief The Belos::TFQMRSolMgr provides a powerful and fully-featured solver manager over the TFQMR linear solver.
 
 \ingroup belos_solver_framework
 
 \author Heidi Thornquist
*/

namespace Belos {
  
  //! @name TFQMRSolMgr Exceptions
  //@{
  
  /** \brief TFQMRSolMgrLinearProblemFailure is thrown when the linear problem is
   * not setup (i.e. setProblem() was not called) when solve() is called.
   *
   * This std::exception is thrown from the TFQMRSolMgr::solve() method.
   *
   */
  class TFQMRSolMgrLinearProblemFailure : public BelosError {public:
    TFQMRSolMgrLinearProblemFailure(const std::string& what_arg) : BelosError(what_arg)
    {}};
  
  /** \brief TFQMRSolMgrOrthoFailure is thrown when the orthogonalization manager is
   * unable to generate orthonormal columns from the initial basis vectors.
   *
   * This std::exception is thrown from the TFQMRSolMgr::solve() method.
   *
   */
  class TFQMRSolMgrOrthoFailure : public BelosError {public:
    TFQMRSolMgrOrthoFailure(const std::string& what_arg) : BelosError(what_arg)
    {}};
  
  template<class ScalarType, class MV, class OP>
  class TFQMRSolMgr : public SolverManager<ScalarType,MV,OP> {
    
  private:
    typedef MultiVecTraits<ScalarType,MV> MVT;
    typedef OperatorTraits<ScalarType,MV,OP> OPT;
    typedef Teuchos::ScalarTraits<ScalarType> SCT;
    typedef typename Teuchos::ScalarTraits<ScalarType>::magnitudeType MagnitudeType;
    typedef Teuchos::ScalarTraits<MagnitudeType> MT;
    
  public:
    
    //! @name Constructors/Destructor
    //@{ 

    /*! \brief Empty constructor for TFQMRSolMgr.
     * This constructor takes no arguments and sets the default values for the solver.
     * The linear problem must be passed in using setProblem() before solve() is called on this object.
     * The solver values can be changed using setParameters().
     */
     TFQMRSolMgr();

    /*! \brief Basic constructor for TFQMRSolMgr.
     *
     * This constructor accepts the LinearProblem to be solved in addition
     * to a parameter list of options for the solver manager. These options include the following:
     *   - "Maximum Iterations" - an \c int specifying the maximum number of iterations the 
     *                            underlying solver is allowed to perform. Default: 1000
     *   - "Convergence Tolerance" - a \c MagnitudeType specifying the level that residual norms 
     *                               must reach to decide convergence. Default: 1e-8.
     *   - "Verbosity" - a sum of MsgType specifying the verbosity. Default: Belos::Errors
     *   - "Output Stream" - a reference-counted pointer to the output stream where all
     *                       solver output is sent.  Default: Teuchos::rcp(&std::cout,false)
     *   - "Output Frequency" - an \c int specifying how often convergence information should be 
     *                          outputted.  Default: -1 (never)
     *   - "Timer Label" - a \c std::string to use as a prefix for the timer labels.  Default: "Belos"
     */
    TFQMRSolMgr( const Teuchos::RCP<LinearProblem<ScalarType,MV,OP> > &problem,
		 const Teuchos::RCP<Teuchos::ParameterList> &pl );
    
    //! Destructor.
    virtual ~TFQMRSolMgr() {};
    //@}
    
    //! @name Accessor methods
    //@{ 
    
    const LinearProblem<ScalarType,MV,OP>& getProblem() const {
      return *problem_;
    }

    /*! \brief Get a parameter list containing the valid parameters for this object.
     */
    Teuchos::RCP<const Teuchos::ParameterList> getValidParameters() const;
    
    /*! \brief Get a parameter list containing the current parameters for this object.
     */
    Teuchos::RCP<const Teuchos::ParameterList> getCurrentParameters() const { return params_; }
    
    /*! \brief Return the timers for this object. 
     *
     * The timers are ordered as follows:
     *   - time spent in solve() routine
     */
    Teuchos::Array<Teuchos::RCP<Teuchos::Time> > getTimers() const {
      return tuple(timerSolve_);
    }
    
    //! Get the iteration count for the most recent call to \c solve().
    int getNumIters() const {
      return numIters_;
    }
    
    /*! \brief Return whether a loss of accuracy was detected by this solver during the most current solve.
     */
    bool isLOADetected() const { return false; }
    
    //@}
    
    //! @name Set methods
    //@{
    
    //! Set the linear problem that needs to be solved. 
    void setProblem( const Teuchos::RCP<LinearProblem<ScalarType,MV,OP> > &problem ) { problem_ = problem; }
    
    //! Set the parameters the solver manager should use to solve the linear problem. 
    void setParameters( const Teuchos::RCP<Teuchos::ParameterList> &params );
    
    //@}
    
    //! @name Reset methods
    //@{
    /*! \brief Performs a reset of the solver manager specified by the \c ResetType.  This informs the
     *  solver manager that the solver should prepare for the next call to solve by resetting certain elements
     *  of the iterative solver strategy.
     */
    void reset( const ResetType type ) { if ((type & Belos::Problem) && !Teuchos::is_null(problem_)) problem_->setProblem(); }
    //@}
    
    //! @name Solver application methods
    //@{ 
    
    /*! \brief This method performs possibly repeated calls to the underlying linear solver's 
     *         iterate() routine until the problem has been solved (as decided by the solver manager) 
     *         or the solver manager decides to quit.
     *
     * This method calls TFQMRIter::iterate(), which will return either because a 
     * specially constructed status test evaluates to ::Passed or an std::exception is thrown.
     *
     * A return from TFQMRIter::iterate() signifies one of the following scenarios:
     *    - the maximum number of iterations has been exceeded. In this scenario, the current solutions 
     *      to the linear system will be placed in the linear problem and return ::Unconverged.
     *    - global convergence has been met. In this case, the current solutions to the linear system 
     *      will be placed in the linear problem and the solver manager will return ::Converged
     *
     * \returns ::ReturnType specifying:
     *     - ::Converged: the linear problem was solved to the specification required by the solver manager.
     *     - ::Unconverged: the linear problem was not solved to the specification desired by the solver manager.
     */
    ReturnType solve();
    
    //@}
    
    /** \name Overridden from Teuchos::Describable */
    //@{
    
    /** \brief Method to return description of the TFQMR solver manager */
    std::string description() const;
    
    //@}
    
  private:

    // Linear problem.
    Teuchos::RCP<LinearProblem<ScalarType,MV,OP> > problem_;
    
    // Output manager.
    Teuchos::RCP<OutputManager<ScalarType> > printer_;
    Teuchos::RCP<std::ostream> outputStream_;
    
    // Status test.
    Teuchos::RCP<StatusTest<ScalarType,MV,OP> > sTest_;
    Teuchos::RCP<StatusTestMaxIters<ScalarType,MV,OP> > maxIterTest_;
    Teuchos::RCP<StatusTestGenResNorm<ScalarType,MV,OP> > convTest_;
    Teuchos::RCP<StatusTestOutput<ScalarType,MV,OP> > outputTest_;
    
    // Current parameter list.
    Teuchos::RCP<ParameterList> params_;
    
    // Default solver values.
    static const MagnitudeType convtol_default_;
    static const int maxIters_default_;
    static const int verbosity_default_;
    static const int outputFreq_default_;
    static const std::string label_default_;
    static const Teuchos::RCP<std::ostream> outputStream_default_;

    // Current solver values.
    MagnitudeType convtol_;
    int maxIters_, numIters_;
    int verbosity_, outputFreq_;
    int blockSize_;
    
    // Timers.
    std::string label_;
    Teuchos::RCP<Teuchos::Time> timerSolve_;

    // Internal state variables.
    bool isSet_;
  };


// Default solver values.
template<class ScalarType, class MV, class OP>
const typename Teuchos::ScalarTraits<ScalarType>::magnitudeType TFQMRSolMgr<ScalarType,MV,OP>::convtol_default_ = 1e-8;

template<class ScalarType, class MV, class OP>
const int TFQMRSolMgr<ScalarType,MV,OP>::maxIters_default_ = 1000;

template<class ScalarType, class MV, class OP>
const int TFQMRSolMgr<ScalarType,MV,OP>::verbosity_default_ = Belos::Errors;

template<class ScalarType, class MV, class OP>
const int TFQMRSolMgr<ScalarType,MV,OP>::outputFreq_default_ = -1;

template<class ScalarType, class MV, class OP>
const std::string TFQMRSolMgr<ScalarType,MV,OP>::label_default_ = "Belos";

template<class ScalarType, class MV, class OP>
const Teuchos::RCP<std::ostream> TFQMRSolMgr<ScalarType,MV,OP>::outputStream_default_ = Teuchos::rcp(&std::cout,false);


// Empty Constructor
template<class ScalarType, class MV, class OP>
TFQMRSolMgr<ScalarType,MV,OP>::TFQMRSolMgr() :
  outputStream_(outputStream_default_),
  convtol_(convtol_default_),
  maxIters_(maxIters_default_),
  verbosity_(verbosity_default_),
  outputFreq_(outputFreq_default_),
  blockSize_(1),
  label_(label_default_),
  isSet_(false)
{}


// Basic Constructor
template<class ScalarType, class MV, class OP>
TFQMRSolMgr<ScalarType,MV,OP>::TFQMRSolMgr( 
					     const Teuchos::RCP<LinearProblem<ScalarType,MV,OP> > &problem,
					     const Teuchos::RCP<Teuchos::ParameterList> &pl ) : 
  problem_(problem),
  outputStream_(outputStream_default_),
  convtol_(convtol_default_),
  maxIters_(maxIters_default_),
  verbosity_(verbosity_default_),
  outputFreq_(outputFreq_default_),
  blockSize_(1),
  label_(label_default_),
  isSet_(false)
{
  TEST_FOR_EXCEPTION(problem_ == Teuchos::null, std::invalid_argument, "Problem not given to solver manager.");
  
  // If the parameter list pointer is null, then set the current parameters to the default parameter list.
  if ( !is_null(pl) ) {
    setParameters( pl );  
  }
}
  
template<class ScalarType, class MV, class OP>
void TFQMRSolMgr<ScalarType,MV,OP>::setParameters( const Teuchos::RCP<Teuchos::ParameterList> &params )
{
  // Create the internal parameter list if ones doesn't already exist.
  if (params_ == Teuchos::null) {
    params_ = Teuchos::rcp( new Teuchos::ParameterList(*getValidParameters()) );
  }
  else {
    params->validateParameters(*getValidParameters());
  }

  // Check for maximum number of iterations
  if (params->isParameter("Maximum Iterations")) {
    maxIters_ = params->get("Maximum Iterations",maxIters_default_);

    // Update parameter in our list and in status test.
    params_->set("Maximum Iterations", maxIters_);
    if (maxIterTest_!=Teuchos::null)
      maxIterTest_->setMaxIters( maxIters_ );
  }

  // Check for blocksize
  if (params->isParameter("Block Size")) {
    blockSize_ = params->get("Block Size",1);    
    TEST_FOR_EXCEPTION(blockSize_ != 1, std::invalid_argument,
		       "Belos::TFQMRSolMgr: \"Block Size\" must be 1.");

    // Update parameter in our list.
    params_->set("Block Size", blockSize_);
  }

  // Check to see if the timer label changed.
  if (params->isParameter("Timer Label")) {
    std::string tempLabel = params->get("Timer Label", label_default_);

    // Update parameter in our list and solver timer
    if (tempLabel != label_) {
      label_ = tempLabel;
      params_->set("Timer Label", label_);
      std::string solveLabel = label_ + ": TFQMRSolMgr total solve time";
      timerSolve_ = Teuchos::TimeMonitor::getNewTimer(solveLabel);
    }
  }

  // Check for a change in verbosity level
  if (params->isParameter("Verbosity")) {
    if (Teuchos::isParameterType<int>(*params,"Verbosity")) {
      verbosity_ = params->get("Verbosity", verbosity_default_);
    } else {
      verbosity_ = (int)Teuchos::getParameter<Belos::MsgType>(*params,"Verbosity");
    }

    // Update parameter in our list.
    params_->set("Verbosity", verbosity_);
    if (printer_ != Teuchos::null)
      printer_->setVerbosity(verbosity_);
  }

  // output stream
  if (params->isParameter("Output Stream")) {
    outputStream_ = Teuchos::getParameter<Teuchos::RCP<std::ostream> >(*params,"Output Stream");

    // Update parameter in our list.
    params_->set("Output Stream", outputStream_);
    if (printer_ != Teuchos::null)
      printer_->setOStream( outputStream_ );
  }

  // frequency level
  if (verbosity_ & Belos::StatusTestDetails) {
    if (params->isParameter("Output Frequency")) {
      outputFreq_ = params->get("Output Frequency", outputFreq_default_);
    }

    // Update parameter in out list and output status test.
    params_->set("Output Frequency", outputFreq_);
    if (outputTest_ != Teuchos::null)
      outputTest_->setOutputFrequency( outputFreq_ );
  }

  // Create output manager if we need to.
  if (printer_ == Teuchos::null) {
    printer_ = Teuchos::rcp( new OutputManager<ScalarType>(verbosity_, outputStream_) );
  }  
  
  // Convergence
  typedef Belos::StatusTestCombo<ScalarType,MV,OP>  StatusTestCombo_t;
  typedef Belos::StatusTestGenResNorm<ScalarType,MV,OP>  StatusTestResNorm_t;

  // Check for convergence tolerance
  if (params->isParameter("Convergence Tolerance")) {
    convtol_ = params->get("Convergence Tolerance",convtol_default_);

    // Update parameter in our list and residual tests.
    params_->set("Convergence Tolerance", convtol_);
    if (convTest_ != Teuchos::null)
      convTest_->setTolerance( convtol_ );
  }
  
  // Create status tests if we need to.

  // Basic test checks maximum iterations and native residual.
  if (maxIterTest_ == Teuchos::null)
    maxIterTest_ = Teuchos::rcp( new StatusTestMaxIters<ScalarType,MV,OP>( maxIters_ ) );
  
  // Implicit residual test, using the native residual to determine if convergence was achieved.
  if (convTest_ == Teuchos::null)
    convTest_ = Teuchos::rcp( new StatusTestResNorm_t( convtol_, 1 ) );
  
  if (sTest_ == Teuchos::null)
    sTest_ = Teuchos::rcp( new StatusTestCombo_t( StatusTestCombo_t::OR, maxIterTest_, convTest_ ) );
  
  if (outputTest_ == Teuchos::null) {
    if (outputFreq_ > 0) {
      outputTest_ = Teuchos::rcp( new StatusTestOutput<ScalarType,MV,OP>( printer_, 
									  sTest_, 
									  outputFreq_, 
									  Passed+Failed+Undefined ) ); 
    }
    else {
      outputTest_ = Teuchos::rcp( new StatusTestOutput<ScalarType,MV,OP>( printer_, 
									  sTest_, 1 ) );
    }
  }

  // Create the timer if we need to.
  if (timerSolve_ == Teuchos::null) {
    std::string solveLabel = label_ + ": TFQMRSolMgr total solve time";
    timerSolve_ = Teuchos::TimeMonitor::getNewTimer(solveLabel);
  }

  // Inform the solver manager that the current parameters were set.
  isSet_ = true;
}

    
template<class ScalarType, class MV, class OP>
Teuchos::RCP<const Teuchos::ParameterList> 
TFQMRSolMgr<ScalarType,MV,OP>::getValidParameters() const
{
  static Teuchos::RCP<const Teuchos::ParameterList> validPL;
  
  // Set all the valid parameters and their default values.
  if(is_null(validPL)) {
    Teuchos::RCP<Teuchos::ParameterList> pl = Teuchos::parameterList();
    pl->set("Convergence Tolerance", convtol_default_,
      "The relative residual tolerance that needs to be achieved by the\n"
      "iterative solver in order for the linear system to be declared converged.");
    pl->set("Maximum Iterations", maxIters_default_,
      "The maximum number of block iterations allowed for each\n"
      "set of RHS solved.");
    pl->set("Verbosity", verbosity_default_,
      "What type(s) of solver information should be outputted\n"
      "to the output stream.");
    pl->set("Output Frequency", outputFreq_default_,
      "How often convergence information should be outputted\n"
      "to the output stream.");  
    pl->set("Output Stream", outputStream_default_,
      "A reference-counted pointer to the output stream where all\n"
      "solver output is sent.");
    pl->set("Timer Label", label_default_,
      "The string to use as a prefix for the timer labels.");
    //  pl->set("Restart Timers", restartTimers_);
    validPL = pl;
  }
  return validPL;
}

  
// solve()
template<class ScalarType, class MV, class OP>
ReturnType TFQMRSolMgr<ScalarType,MV,OP>::solve() {

  // Set the current parameters if they were not set before.
  // NOTE:  This may occur if the user generated the solver manager with the default constructor and 
  // then didn't set any parameters using setParameters().
  if (!isSet_) {
    setParameters(Teuchos::parameterList(*getValidParameters()));
  }

  Teuchos::BLAS<int,ScalarType> blas;
  Teuchos::LAPACK<int,ScalarType> lapack;
  
  TEST_FOR_EXCEPTION(!problem_->isProblemSet(),TFQMRSolMgrLinearProblemFailure,
                     "Belos::TFQMRSolMgr::solve(): Linear problem is not ready, setProblem() has not been called.");

  // Create indices for the linear systems to be solved.
  int startPtr = 0;
  int numRHS2Solve = MVT::GetNumberVecs( *(problem_->getRHS()) );
  int numCurrRHS = blockSize_;

  std::vector<int> currIdx, currIdx2;

  //  The index set is generated that informs the linear problem that some linear systems are augmented.
  currIdx.resize( blockSize_ );
  currIdx2.resize( blockSize_ );
  for (int i=0; i<numCurrRHS; ++i) 
    { currIdx[i] = startPtr+i; currIdx2[i]=i; }

  // Inform the linear problem of the current linear system to solve.
  problem_->setLSIndex( currIdx );

  //////////////////////////////////////////////////////////////////////////////////////
  // Parameter list
  Teuchos::ParameterList plist;
  plist.set("Block Size",blockSize_);
  
  // Reset the status test.  
  outputTest_->reset();

  // Assume convergence is achieved, then let any failed convergence set this to false.
  bool isConverged = true;	

  //////////////////////////////////////////////////////////////////////////////////////
  // TFQMR solver

  Teuchos::RCP<TFQMRIter<ScalarType,MV,OP> > tfqmr_iter = 
    Teuchos::rcp( new TFQMRIter<ScalarType,MV,OP>(problem_,printer_,outputTest_,plist) );

  // Enter solve() iterations
  {
    Teuchos::TimeMonitor slvtimer(*timerSolve_);

    while ( numRHS2Solve > 0 ) {
      //
      // Reset the active / converged vectors from this block
      std::vector<int> convRHSIdx;
      std::vector<int> currRHSIdx( currIdx );
      currRHSIdx.resize(numCurrRHS);

      // Reset the number of iterations.
      tfqmr_iter->resetNumIters();

      // Reset the number of calls that the status test output knows about.
      outputTest_->resetNumCalls();

      // Get the current residual for this block of linear systems.
      Teuchos::RCP<MV> R_0 = MVT::CloneView( *(Teuchos::rcp_const_cast<MV>(problem_->getInitPrecResVec())), currIdx );

      // Set the new state and initialize the solver.
      TFQMRIterState<ScalarType,MV> newstate;
      newstate.R = R_0;
      tfqmr_iter->initializeTFQMR(newstate);

      while(1) {
	
	// tell tfqmr_iter to iterate
	try {
	  tfqmr_iter->iterate();
	  
	  ////////////////////////////////////////////////////////////////////////////////////
	  //
	  // check convergence first
	  //
	  ////////////////////////////////////////////////////////////////////////////////////
	  if ( convTest_->getStatus() == Passed ) {
	    // We have convergence of at least one linear system.

	    // Figure out which linear systems converged.
	    std::vector<int> convIdx = Teuchos::rcp_dynamic_cast<StatusTestGenResNorm<ScalarType,MV,OP> >(convTest_)->convIndices();

	    // If the number of converged linear systems is equal to the
            // number of current linear systems, then we are done with this block.
	    if (convIdx.size() == currRHSIdx.size())
	      break;  // break from while(1){tfqmr_iter->iterate()}

	    // Inform the linear problem that we are finished with this current linear system.
	    problem_->setCurrLS();

	    // Reset currRHSIdx to have the right-hand sides that are left to converge for this block.
	    int have = 0;
            std::vector<int> unconvIdx(currRHSIdx.size());
	    for (unsigned int i=0; i<currRHSIdx.size(); ++i) {
	      bool found = false;
	      for (unsigned int j=0; j<convIdx.size(); ++j) {
		if (currRHSIdx[i] == convIdx[j]) {
		  found = true;
		  break;
		}
	      }
	      if (!found) {
                currIdx2[have] = currIdx2[i];
		currRHSIdx[have++] = currRHSIdx[i];
	      }
              else {
              } 
	    }
	    currRHSIdx.resize(have);
	    currIdx2.resize(have);

	    // Set the remaining indices after deflation.
	    problem_->setLSIndex( currRHSIdx );

	    // Get the current residual vector.
	    std::vector<MagnitudeType> norms;
            R_0 = MVT::CloneCopy( *(tfqmr_iter->getNativeResiduals(&norms)),currIdx2 );
	    for (int i=0; i<have; ++i) { currIdx2[i] = i; }

	    // Set the new blocksize for the solver.
	    tfqmr_iter->setBlockSize( have );

	    // Set the new state and initialize the solver.
	    TFQMRIterState<ScalarType,MV> defstate;
	    defstate.R = R_0;
	    tfqmr_iter->initializeTFQMR(defstate);
	  }
	  ////////////////////////////////////////////////////////////////////////////////////
	  //
	  // check for maximum iterations
	  //
	  ////////////////////////////////////////////////////////////////////////////////////
	  else if ( maxIterTest_->getStatus() == Passed ) {
	    // we don't have convergence
	    isConverged = false;
	    break;  // break from while(1){tfqmr_iter->iterate()}
	  }

	  ////////////////////////////////////////////////////////////////////////////////////
	  //
	  // we returned from iterate(), but none of our status tests Passed.
	  // something is wrong, and it is probably our fault.
	  //
	  ////////////////////////////////////////////////////////////////////////////////////

	  else {
	    TEST_FOR_EXCEPTION(true,std::logic_error,
			       "Belos::TFQMRSolMgr::solve(): Invalid return from TFQMRIter::iterate().");
	  }
	}
	catch (const std::exception &e) {
	  printer_->stream(Errors) << "Error! Caught std::exception in TFQMRIter::iterate() at iteration " 
				   << tfqmr_iter->getNumIters() << std::endl 
				   << e.what() << std::endl;
	  throw;
	}
      }
      
      // Inform the linear problem that we are finished with this block linear system.
      problem_->setCurrLS();
      
      // Update indices for the linear systems to be solved.
      startPtr += numCurrRHS;
      numRHS2Solve -= numCurrRHS;
      if ( numRHS2Solve > 0 ) {
	numCurrRHS = blockSize_;

	currIdx.resize( blockSize_ );
	currIdx2.resize( blockSize_ );
	for (int i=0; i<numCurrRHS; ++i) 
	  { currIdx[i] = startPtr+i; currIdx2[i] = i; }
	// Set the next indices.
	problem_->setLSIndex( currIdx );

	// Set the new blocksize for the solver.
	tfqmr_iter->setBlockSize( blockSize_ );	
      }
      else {
        currIdx.resize( numRHS2Solve );
      }
      
    }// while ( numRHS2Solve > 0 )
    
  }

  // print final summary
  sTest_->print( printer_->stream(FinalSummary) );
 
  // print timing information
  Teuchos::TimeMonitor::summarize( printer_->stream(TimingDetails) );
 
  // get iteration information for this solve
  numIters_ = maxIterTest_->getNumIters();
 
  if (!isConverged) {
    return Unconverged; // return from TFQMRSolMgr::solve() 
  }
  return Converged; // return from TFQMRSolMgr::solve() 
}

//  This method requires the solver manager to return a std::string that describes itself.
template<class ScalarType, class MV, class OP>
std::string TFQMRSolMgr<ScalarType,MV,OP>::description() const
{
  std::ostringstream oss;
  oss << "Belos::TFQMRSolMgr<...,"<<Teuchos::ScalarTraits<ScalarType>::name()<<">";
  oss << "{}";
  return oss.str();
}
  
} // end Belos namespace

#endif /* BELOS_TFQMR_SOLMGR_HPP */
