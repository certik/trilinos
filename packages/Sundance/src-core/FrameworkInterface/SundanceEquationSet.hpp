/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
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
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#ifndef SUNDANCE_EQUATIONSET_H
#define SUNDANCE_EQUATIONSET_H

#include "SundanceDefs.hpp"
#include "SundanceExpr.hpp"
#include "SundanceDerivSet.hpp"
#include "SundanceComputationType.hpp"
#include "SundanceRegionQuadCombo.hpp"
#include "SundanceEvalContext.hpp"
#include "TSFObjectWithVerbosity.hpp"


namespace SundanceCore
{
using namespace SundanceUtils;
using namespace Teuchos;
using std::string;

class FunctionSupportResolver;
class CommonFuncDataStub;


/** 
 * Source: SundanceEquationSet.cpp
 *
 * Header: SundanceEquationSet.hpp
 *
 * EquationSet is an object in which the symbolic specification 
 * of a problem or functional, its BCs, its test 
 * and unknown functions, and the
 * point about which it is to be linearized are all gathered. 
 * With this information we can compile lists of which functions
 * are defined on which regions of the domain, which is what is 
 * required for the building of DOF maps. We can't build the
 * DOF map here because in the Sundance core we know nothing
 * of the mesh, so we provide accessors to the information collected
 * by the EquationSet.
 *
 * This is <em>NOT</em> normally a user-level object. However,
 * EquationSet is one of the most important classes for the
 * inner workings of Sundance, so it is critical for a developer
 * to understand it. It is used
 * internally in the operation of user-level classes such
 * as LinearProblem, NonlinearProblem, and Functional. 
 *
 * There are several modes in which one might construct an equation set.
 * The first is where one has written out a weak form in terms
 * of test functions. The second is where one is taking variations
 * of some functional. 
 *
 * Note that "EquationSet" is a bit of a misnomer. It was originally
 * written to deal with setting up forward problems, but it has since
 * been extended to encompass functionals and variations. The name
 * persists for historical reasons; there is no particular need to
 * change it.
 *
 * \section intSection Integrals (weak equations and functionals)
 *
 * \see Integral
 *
 * \subsection regionSection Specifying regions of integration
 *
 * Weak equations or functionals are written in terms of integrals;
 * the regions on which integration is done must be defined somehow.
 * Because the symbolic core knows nothing of how geometry is
 * represented in whatever frameworks it's interacting with, the
 * region of integration can be represented only with stub classes.
 *
 * \subsection quadSection Specifying quadrature
 *
 * \section varSection Specifying variables
 *
 * \subsection multipleVarSection Multiple variables: Lists and Blocks 
 *
 * In a multivariable problem it may be useful to group variables
 * into blocks; for example, in a segregated Navier-Stokes preconditioner
 * the linearized equations are set up as a block system with
 * the velocities and the pressure put into different blocks:
 * \f[
 \left[ \left(u_x, u_y, u_z\right), \left(p\right)\right]^T
 * \f]
 * We use the following convention for specifying block structure:
 * variables aggregated by Expr's listing operations are considered
 * to be within a single block. The Teuchos Array object is then
 * used to aggregate multiple blocks. 
 *
 * \subsection variationSection Specifying which variations are taken
 *
 * The EquationSet class can be used to define a functional, and
 * one can then take variations of that functional with respect to 
 * some subset of the unknown functions appearing in the functional.
 * We'll refer to these as the variational functions. For each
 * variational function it is necessary to specify an evaluation
 * point, which is simply the value about which variations are taken.
 *
 * This variational capability can be used to take gradients in
 * an optimization problem, or to derive state or adjoint equations. 
 *
 * \subsection fixedSection Specifying which fields are held fixed
 *
 * Some variables may remain fixed when variations are taken. For
 * example, in PDE-constrained optimization, the state equations
 * are derived by taking variations of the Lagrangian with respect
 * to the adjoint variables, holding the state and design variables
 * constant. 
 *
 * \subsection evalSection Specifying points at which functions are evaluated
 *
 * Every field variable given to an equation set must also be given
 * an evaluation point. The evaluation point is another expression,
 * which must be of one of two types: a discrete function (subtype
 * of DiscreteFunctionStub) or a zero expression. 
 *
 * \subsection updateSection Updating evaluation points
 *
 * It is important to understand how values of evaluation points
 * are updated. This is <em>NOT</em> done by rebuilding the
 * EquationSet object with new evaluation points. Rather, it is
 * done by resetting the functions' internal data; because the
 * EquationSet has stored shallow copies of the evaluation points,
 * the EquationSet is updated automatically to follow any external
 * changes.
 *
 * \section internalSection 
 *
 * \subsection funcIDSection Reduced and unreduced function IDs 
 *
 * Every symbolic (i.e., test or unknown) function and unknown parameter 
 * has a unique integer ID known as its function ID, or funcID for
 * short. This ID remains associated with the function, never
 * changing, throughout the life of the function. These IDs need
 * not be contiguous nor ordered (they are, however, guaranteed to
 * be unique). 
 * 
 * In building an EquationSet, we will also create other ID numbers
 * for each function based on the position of each function within
 * the lists of functions given as input arguments to the equation
 * set ctor. These IDs are contiguous and ordered, with the ordering
 * defined by position in the input argument list. We will call these
 * "reduced IDs." The ID intrinsic to a function is called here its
 * "unreduced ID." EquationSet provided methods for converting
 * between reduced and unreduced IDs. 
 *
 * Note that a function that appears in several equation sets
 * might have different reduced IDs in the different equation sets,
 * but its unreduced ID will always be the same.
 *
 */
class EquationSet : public TSFExtended::ParameterControlledObjectWithVerbosity<EquationSet>
{
public:
  /** \name Constructors */
  //@{
  /** Set up a functional to be integrated, where all 
   * field variables are fixed to specified values. This ctor should
   * be used when setting up a functional for evaluation without
   * differentiation.
   * 
   * @param eqns The expression defining which integrals are
   * to be done to evaluate the functional
   *
   * @param bcs The expression defining any BC-like terms that
   * strongly replace the ordinary equations on certain subdomains.
   * If no BC terms are appropriate for a problem, simply enter an
   * empty expression for this argument.
   *
   * @param params Any unknown parameters appearing in the functional.
   * Multiple parameters should be entered as a List expression.
   * If no parameters are present, enter an empty expression.
   *
   * @param paramValues Values of the parameters en
   *
   * @param fields The field variables (i.e., unknown functions)
   * appearing in the functional.
   *
   * @param fieldValues Evaluation points for
   * the variables entered in the fields
   * argument. 
   */
  EquationSet(const Expr& eqns, 
    const Expr& bcs,
    const Expr& params,
    const Expr& paramValues,
    const Array<Expr>& fields,
    const Array<Expr>& fieldValues,
    const ParameterList& verbParams = *defaultVerbParams());

  /** Set up equations written in weak form with test functions. This
   * ctor should be used when setting up an ordinary forward problem.
   * 
   * \param eqns The expression defining the weak equations. This
   * can be linear or nonlinear.
   *
   * \param bcs The expression defining any BC-like terms that
   * strongly replace the ordinary equations on certain subdomains.
   * If no BC terms are appropriate for a problem, simply enter an
   * empty expression for this argument.
   *
   * \param testFunctions The test functions used in defining the weak
   * problem. The evaluation points for these functions are zero, and
   * need not be given as arguments. These should be subtypes of
   * TestFunctionStub, or lists thereof.
   *
   * \param unks The unknown functions for which the weak equation
   * will be solved. These should be subtypes of
   * UnknownFunctionStub, or lists thereof.
   *
   * \param unkLinearizationPts The values of the unknown function
   * about which the equations are linearized.
   *
   * \param unkParams The unknown parameters for which the weak equation
   * will be solved. These should of type
   * UnknownParameter, or a list thereof.
   *
   * \param unkParamEvalPts The values of the unknown parameters
   * about which the equations are linearized.
   *
   * \param params Any parameters whose values are held fixed (i.e,
   * not solved for). These should be of type 
   * UnknownParameter, or a list thereof.
   *
   * \param paramValues Values of the parameters entered in the params
   * argument. 
   *
   * \param fixedFields  Any field variables whose values are held 
   * fixed. These should be subtypes of
   * UnknownFunctionStub, or lists thereof.
   *
   * \param fixedFieldValues Values of the fixed field variables.
   * argument. 
   * 
   * \todo If unknown parameters are present, sensitivity equations 
   * should be set up as well. This is partially implemented
   * but not finished.
   */
  EquationSet(const Expr& eqns, 
    const Expr& bcs,
    const Array<Expr>& testFunctions, 
    const Array<Expr>& unks,
    const Array<Expr>& unkLinearizationPts,
    const Expr& unkParams,
    const Expr& unkParamEvalPts, 
    const Expr& params,
    const Expr& paramValues,
    const Array<Expr>& fixedFields,
    const Array<Expr>& fixedFieldValues,
    const ParameterList& verbParams = *defaultVerbParams());


  /* Set up calculation of a functional and its first derivative wrt a 
   * specified set of functions, to evaluated at a specified
   * point. Other functions can be specified as fixed during the 
   * calculation of these derivatives. 
   * 
   * \param eqns The expression defining which integrals are
   * to be done to evaluate the functional.
   *
   * \param bcs The expression defining any BC-like terms that
   * strongly replace the ordinary equations on certain subdomains.
   * If no BC terms are appropriate for a problem, simply enter an
   * empty expression for this argument.
   *
   * \param vars The functions with which variations are
   * to be taken. These should be subtypes of
   * UnknownFunctionStub, or lists thereof.
   *
   * \param varLinearizationPts The values of the variational
   * functions at which variations are taken.
   *
   * \param params Any parameters whose values are held fixed (i.e,
   * not solved for). These should be of type 
   * UnknownParameter, or a list thereof.
   *
   * \param paramValues Values of the parameters entered in the params
   * argument. 
   *
   * \param fixedFields  Any field variables whose values are held 
   * fixed. These should be subtypes of
   * UnknownFunctionStub, or lists thereof.
   *
   * \param fixedFieldValues Values of the fixed field variables.
   * argument. 
   */
  EquationSet(const Expr& eqns, 
    const Expr& bcs,
    const Array<Expr>& vars,
    const Array<Expr>& varLinearizationPts, 
    const Expr& params,
    const Expr& paramValues,
    const Array<Expr>& fixedFields,
    const Array<Expr>& fixedFieldValues,
    const ParameterList& verbParams = *defaultVerbParams());

  /** Set up calculation of first and second variations of 
   * a functional. This ctor should be used when deriving the
   * linearized form of a variational problem. */
  EquationSet(const Expr& eqns, 
    const Expr& bcs,
    const Array<Expr>& vars, 
    const Array<Expr>& varLinearizationPts,
    const Array<Expr>& unks,
    const Array<Expr>& unkLinearizationPts, 
    const Expr& params,
    const Expr& paramValues,
    const Array<Expr>& fixedFields,
    const Array<Expr>& fixedFieldValues,
    const ParameterList& verbParams = *defaultVerbParams());

  /** */
  EquationSet(const RefCountPtr<FunctionSupportResolver>& fsr,
    const Array<Expr>& varLinearizationPts,
    const Array<Expr>& unkLinearizationPts, 
    const Expr& paramValues,
    const Array<Expr>& fixedFieldValues,
    const ParameterList& verbParams = *defaultVerbParams());

  //@}

  /** \name Finding integration regions for the equation set */
  //@{
  /** Returns the number of regions on which pieces of the equation
   * or BCs are defined. */
  unsigned int numRegions() const ;
      
  /** Returns the d-th region for this equation set */
  const RefCountPtr<CellFilterStub>& region(int d) const ;

  /** Returns the index of the given region */
  int indexForRegion(const OrderedHandle<CellFilterStub>& region) const ;

  /** Indicate whether the given region has an essential BC expression */
  bool isBCRegion(int d) const ;

  /** Return the set of regions on which the specified 
   * test func appears. */
  const Set<OrderedHandle<CellFilterStub> >& 
  regionsForTestFunc(int unreducedTestID) const ;
      
  /** Return the set of regions on which the specified 
   * unknown func appears */
  const Set<OrderedHandle<CellFilterStub> >& 
  regionsForUnkFunc(int unreducedUnkID) const ;

  /** Returns the list of distinct subregion-quadrature combinations
   * appearing in the equation set. */
  const Array<RegionQuadCombo>& regionQuadCombos() const 
    {return regionQuadCombos_;}

  /** Returns the list of distinct subregion-quadrature combinations
   * appearing in the boundary conditions */
  const Array<RegionQuadCombo>& bcRegionQuadCombos() const 
    {return bcRegionQuadCombos_;}
      
  /** Indicates whether any var-unk pairs appear in the given domain */
  bool hasVarUnkPairs(const OrderedHandle<CellFilterStub>& domain) const 
    {return varUnkPairsOnRegions_.containsKey(domain);}


  /** Indicates whether any BC var-unk pairs appear in the given domain */
  bool hasBCVarUnkPairs(const OrderedHandle<CellFilterStub>& domain) const 
    {return bcVarUnkPairsOnRegions_.containsKey(domain);}

  /** Returns the (var, unk) pairs appearing on the given domain.
   * This is required for determining the sparsity structure of the
   * matrix */
  const RefCountPtr<Set<OrderedPair<int, int> > >& varUnkPairs(const OrderedHandle<CellFilterStub>& domain) const 
    {return varUnkPairsOnRegions_.get(domain);}
      

  /** Returns the (var, unk) pairs appearing on the given domain.
   * This is required for determining the sparsity structure of the
   * matrix */
  const RefCountPtr<Set<OrderedPair<int, int> > >& bcVarUnkPairs(const OrderedHandle<CellFilterStub>& domain) const ;


  /** Returns the integrand on the rqc r. */
  const Expr& expr(const RegionQuadCombo& r) const 
    {return regionQuadComboExprs_.get(r);}

  /** Returns the BC integrand on rqc r. */
  const Expr& bcExpr(const RegionQuadCombo& r) const 
    {return bcRegionQuadComboExprs_.get(r);}

  /** Indicates whether any watch flags are active */
  bool hasActiveWatchFlag() const ;

  /** */
  const RefCountPtr<FunctionSupportResolver>& fsr() const {return fsr_;}

  //@}
      

  /** \name Creation of evaluation context objects */
  //@{
  /** Map RQC to the context for the derivs of the given compType */
  EvalContext rqcToContext(ComputationType compType, 
    const RegionQuadCombo& r) const ;


  /** Map BC RQC to the context for the derivs of the given compType */
  EvalContext bcRqcToContext(ComputationType compType, 
    const RegionQuadCombo& r) const ; 
  //@}

  /** \name Identification of RQCs to skip for given compType */
  //@{
  /** Map RQC to the context for the derivs of the given compType */
  bool skipRqc(ComputationType compType, 
    const RegionQuadCombo& r) const ;


  /** Map BC RQC to the context for the derivs of the given compType */
  bool skipBCRqc(ComputationType compType, 
    const RegionQuadCombo& r) const ;
  
  //@}


  /** \name Getting information about functions */
  //@{
  /** Returns the number of variational function blocks */
  unsigned int numVarBlocks() const ;

  /** Returns the number of unknown function blocks */
  unsigned int numUnkBlocks() const ;

  /** Returns the number of unknown parameters */
  unsigned int numUnkParams() const ;

  /** Returns the number of fixed parameters */
  unsigned int numFixedParams() const ;

  /** Returns the number of variational functions in this block */
  unsigned int numVars(int block) const ;

  /** Returns the number of unk functions in this block */
  unsigned int numUnks(int block) const ;

  /** Returns the number of variational function IDs in this block.
   * See the comment in FSR.hpp for an explanation of the difference 
   * between this and numVars(). */
  unsigned int numVarIDs(int block) const ;

  /** Returns the number of unk function IDs in this block.
   * See the comment in FSR.hpp for an explanation of the difference 
   * between this and numVars().  */
  unsigned int numUnkIDs(int block) const ;

  /** Returns the i-th variational function in block b */
  RCP<const CommonFuncDataStub> varFuncData(int b, int i) const ;

  /** Returns the i-th unknown function in block b */
  RCP<const CommonFuncDataStub> unkFuncData(int b, int i) const ;

  /** Returns the i-th unknown parameter */
  const Expr& unkParam(int i) const ;

  /** Returns the i-th unknown parameter */
  const Expr& fixedParam(int i) const ;

  /** Determine whether a given func ID is listed as a 
   * variational function in this equation set */
  bool hasVarID(int fid) const ;

  /** Determine whether a given func ID is listed as a unk function 
   * in this equation set */
  bool hasUnkID(int fid) const ;

  /** Determine whether a given func ID is listed as a unk parameter 
   * in this equation set */
  bool hasUnkParamID(int fid) const ;

  /** Determine whether a given func ID is listed as a fixed parameter 
   * in this equation set */
  bool hasFixedParamID(int fid) const ;

  /** get the block number for the variational function having the
   * specified unreduced funcID */
  int blockForVarID(int varID) const ;

  /** get the block number for the unknown function having the
   * specified unreduced funcID */
  int blockForUnkID(int unkID) const ;
  //@}


  /** \name Finding the functions that appear on regions */
  //@{
  /** Returns the variational functions that appear explicitly
   * on the d-th region */
  const Set<int>& varsOnRegion(int d) const ;

  /** Returns the unknown functions that appear explicitly on the
   * d-th region. */
  const Set<int>& unksOnRegion(int d) const ;

  /** Returns the variational functions that 
   * appear in BCs on the d-th region.
   * We can use this information to tag certain rows as BC rows */
  const Set<int>& bcVarsOnRegion(int d) const ;

  /** Returns the unknown functions that appear in BCs on the d-th region.
   * We can use this information to tag certain columns as BC
   * columns in the event we're doing symmetrized BC application */
  const Set<int>& bcUnksOnRegion(int d) const ;


  /** Returns the reduced variational functions that appear explicitly
   * on the d-th region */
  const Array<Set<int> >& reducedVarsOnRegion(const OrderedHandle<CellFilterStub>& r) const ;


  /** Returns the reduced unknown functions that appear explicitly on the
   * d-th region. */
  const Array<Set<int> >& reducedUnksOnRegion(const OrderedHandle<CellFilterStub>& r) const ;
  //@}

      


  /** \name Transforming between unreduced and reduced function IDs */
  //@{
  /** get the reduced ID for the variational function having the
   * specified unreduced funcID */
  int reducedVarID(int varID) const ;

  /** get the reduced ID for the unknown 
   * function having the given funcID */
  int reducedUnkID(int unkID) const ;

  /** get the reduced ID for the unk parameter
   * having the given funcID */
  int reducedUnkParamID(int unkID) const ;

  /** get the reduced ID for the fixed parameter
   * having the given funcID */
  int reducedFixedParamID(int unkID) const ;

  /** get the unreduced funcID for a variational function
   * as specified by a reduced ID and block index */
  int unreducedVarID(int block, int reducedVarID) const ;


  /** get the unreduced funcID for an unknown function
   * as specified by a reduced ID and block index */
  int unreducedUnkID(int block, int reducedUnkID) const ;

  /** get the unreduced funcID for an unknown parameter
   * as specified by a reduced ID and block index */
  int unreducedUnkParamID(int reducedUnkParamID) const ;

  /** get the unreduced funcID for a fixed parameter
   * as specified by a reduced ID and block index */
  int unreducedFixedParamID(int reducedFixedParamID) const ;

  //@}


  /** \name Information about which calculations can be done */
  //@{
  /** */
  bool isFunctionalCalculator() const {return isFunctionalCalculator_;}

  /** */
  bool isSensitivityCalculator() const {return isSensitivityProblem_;}
      
  /** Indicate whether this equation set will do the
   * given computation type */
  bool hasComputationType(ComputationType compType) const 
    {return compTypes_.contains(compType);}

  /** Return the types of computations this object can perform */
  const Set<ComputationType>& computationTypes() const 
    {return compTypes_;}
  //@}

      

  /** \name Information about which functional derivatives will be computed */
  //@{
  /** Returns the set of nonzero functional derivatives appearing
   * in the equation set at the given subregion-quadrature combination */
  const DerivSet& nonzeroFunctionalDerivs(ComputationType compType,
    const RegionQuadCombo& r) const ;

  /** Returns the set of nonzero functional derivatives appearing
   * in the boundary conditions
   *  at the given subregion-quadrature combination */
  const DerivSet& nonzeroBCFunctionalDerivs(ComputationType compType,
    const RegionQuadCombo& r) const;
  //@}



  /** */
  static RefCountPtr<ParameterList> defaultVerbParams()
    {
      static RefCountPtr<ParameterList> rtn = rcp(new ParameterList("Equation Set"));
      static int first = true;
      if (first)
      {
        rtn->set<int>("setup", 0);
        first = false;
      }
      return rtn;
    }
private:

  /**
   * Flatten a spectral expression into a list of its coefficients
   */
  Expr flattenSpectral(const Expr& input) const ;
  /**
   * Flatten a spectral expression into a list of its coefficients
   */
  Array<Expr> flattenSpectral(const Array<Expr>& input) const ;

  /** 
   * Common initialization function called by all constructors
   */
  void init(const Array<Expr>& varLinearizationPts,
    const Array<Expr>& unkLinearizationPts,
    const Expr& unkParamEvalPts, 
    const Expr& fixedParamValues,
    const Array<Expr>& fixedFieldValues);

  /** Helper that converts an array of expr to a list expression */
  static Expr toList(const Array<Expr>& e);

  /** */
  void addToVarUnkPairs(const OrderedHandle<CellFilterStub>& domain,
    const Set<int>& vars,
    const Set<int>& unks,
    const DerivSet& nonzeros, 
    bool isBC,
    int verb);

  /** The FunctionSupportResolver deals with associating functions with
   * subdomains */
  RefCountPtr<FunctionSupportResolver> fsr_;

  /** Map from cell filter to pairs of (varID, unkID) appearing
   * on those cells. This is needed to construct the sparsity pattern
   * of the matrix. */
  Map<OrderedHandle<CellFilterStub>, RefCountPtr<Set<OrderedPair<int, int> > > > varUnkPairsOnRegions_;

  /** Map from cell filter to pairs of (varID, unkID) appearing
   * on those cells. This is needed to construct the sparsity pattern
   * of the matrix. */
  Map<OrderedHandle<CellFilterStub>, RefCountPtr<Set<OrderedPair<int, int> > > > bcVarUnkPairsOnRegions_;

  /** */
  Array<RegionQuadCombo> regionQuadCombos_;

  /** */
  Array<RegionQuadCombo> bcRegionQuadCombos_;

  /** */
  Map<RegionQuadCombo, Expr> regionQuadComboExprs_;

  /** */
  Map<RegionQuadCombo, Expr> bcRegionQuadComboExprs_;

  /** List of the sets of nonzero functional derivatives at 
   * each regionQuadCombo */
  Map<ComputationType, Map<RegionQuadCombo, DerivSet> > regionQuadComboNonzeroDerivs_;

  /** List of the sets of nonzero functional derivatives at 
   * each regionQuadCombo */
  Map<ComputationType, Map<RegionQuadCombo, DerivSet> > bcRegionQuadComboNonzeroDerivs_;

  /** List of the contexts for
   * each regionQuadCombo */
  Map<ComputationType, Map<RegionQuadCombo, EvalContext> > rqcToContext_;

  /** List of the contexts for
   * each BC regionQuadCombo */
  Map<ComputationType, Map<RegionQuadCombo, EvalContext> > bcRqcToContext_;

  /** List the RQCs that should be skipped for each computation type. 
   * This is needed in cases such as when a domain has matrix terms
   * but no vector terms. */
  Map<ComputationType, Set<RegionQuadCombo> > rqcToSkip_;

  /** List the BC RQCs that should be skipped for each computation type. */
  Map<ComputationType, Set<RegionQuadCombo> > bcRqcToSkip_;


  /** The point in function space about which the equations
   * are linearized */
  Array<Expr> unkLinearizationPts_;

  /** unknown parameter evaluation points for this equation set */
  Expr unkParamEvalPts_;

  /** fixed parameter evaluation points for this equation set */
  Expr fixedParamEvalPts_;

  /** Set of the computation types supported here */
  Set<ComputationType> compTypes_;

      
  /** Flag indicating whether this equation set is nonlinear */
  bool isNonlinear_;
      
  /** Flag indicating whether this equation set is 
   * a variational problem */
  bool isVariationalProblem_;

  /** Flag indicating whether this equation set is a functional
   * calculator */
  bool isFunctionalCalculator_;
      
  /** Flag indicating whether this equation set is 
   * a sensitivity problem */
  bool isSensitivityProblem_;

};
}

#endif
