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

#include "SundanceEvaluatableExpr.hpp"
#include "SundanceEvaluatorFactory.hpp"
#include "SundanceEvaluator.hpp"
#include "SundanceNullEvaluator.hpp"
#include "SundanceEvalManager.hpp"
#include "SundanceSymbolicFuncElement.hpp"
#include "SundanceExpr.hpp"
#include "SundanceTabs.hpp"
#include "SundanceOut.hpp"
#include "Teuchos_Utils.hpp"

using namespace SundanceCore;
using namespace SundanceUtils;

using namespace Teuchos;
using namespace TSFExtended;
using namespace Internal;
using namespace Internal;

TEUCHOS_TIMER(evalTimer, "Symbolic Evaluation")

EvaluatableExpr::EvaluatableExpr()
	: ScalarExpr(), 
    evaluators_(),
    sparsity_(),
    nodesHaveBeenCounted_(false),
    contextToDSSMap_(numDerivSubsetTypes(), maxFuncDiffOrder()+1),
    rIsReady_(false),
    allOrdersMap_(numDerivSubsetTypes())
{}

string EvaluatableExpr::derivType(const DerivSubsetSpecifier& dss) const
{
  switch(dss)
    {
    case RequiredNonzeros:
      return "R";
    case ConstantNonzeros:
      return "C";
    case VariableNonzeros:
      return "V";
    default:
      return "W";
    }
}

void EvaluatableExpr::registerSpatialDerivs(const EvalContext& context, 
                                            const Set<MultiIndex>& miSet) const
{
  if (activeSpatialDerivMap_.containsKey(context))
    {
      Set<MultiIndex> s = activeSpatialDerivMap_[context];
      s.merge(miSet);
      activeSpatialDerivMap_.put(context, s);
    }
  else
    {
      activeSpatialDerivMap_.put(context, miSet);
    }
}

const Set<MultiIndex>& EvaluatableExpr
::activeSpatialDerivs(const EvalContext& context) const
{
  TEST_FOR_EXCEPTION(!activeSpatialDerivMap_.containsKey(context),
                     InternalError,
                     "Unknown context " << context);
  return activeSpatialDerivMap_[context];
}

RefCountPtr<SparsitySuperset> 
EvaluatableExpr::sparsitySuperset(const EvalContext& context) const 
{
  RefCountPtr<SparsitySuperset> rtn;

  if (sparsity_.containsKey(context))
    {
      rtn = sparsity_.get(context);
    }
  else
    {
      const Set<MultipleDeriv>& R = findR(context);
      const Set<MultipleDeriv>& C = findC(context);
      const Set<MultipleDeriv>& V = findV(context);
      rtn = rcp(new SparsitySuperset(C.intersection(R), V.intersection(R)));
//      rtn = rcp(new SparsitySuperset(findC(context), findV(context)));
      sparsity_.put(context, rtn);
    }
  return rtn;
}



const RefCountPtr<Evaluator>&
EvaluatableExpr::evaluator(const EvalContext& context) const 
{
  TEST_FOR_EXCEPTION(!evaluators_.containsKey(context), RuntimeError, 
                     "Evaluator not found for context " << context);
  return evaluators_.get(context);
}

void EvaluatableExpr::evaluate(const EvalManager& mgr,
                               Array<double>& constantResults,
                               Array<RefCountPtr<EvalVector> >& vectorResults) const
{
  TimeMonitor timer(evalTimer());
  evaluator(mgr.getRegion())->eval(mgr, constantResults, vectorResults);
}

void EvaluatableExpr::setupEval(const EvalContext& context) const
{
  Tabs tabs0;
  SUNDANCE_VERB_HIGH(tabs0 << "setupEval() for " << this->toString());
  if (!evaluators_.containsKey(context))
    {
      Tabs tabs;
      SUNDANCE_VERB_HIGH(tabs << "my sparsity superset = " 
                         << *sparsitySuperset(context));
      RefCountPtr<Evaluator> eval;
      if (sparsitySuperset(context)->numDerivs()>0)
        {
          SUNDANCE_VERB_HIGH(tabs << "calling createEvalulator()");
          eval = rcp(createEvaluator(this, context));
        }
      else
        {
          SUNDANCE_VERB_HIGH(tabs << "creating null evalulator");
          eval = rcp(new NullEvaluator());
        }
      evaluators_.put(context, eval);
    }
  else
    {
      Tabs tabs;
      SUNDANCE_VERB_HIGH(tabs << "reusing existing evaluator...");
    }
}

void EvaluatableExpr::showSparsity(ostream& os, 
                                   const EvalContext& context) const
{
  Tabs tab0;
  os << tab0 << "Node: " << toString() << endl;
  sparsitySuperset(context)->print(os);
}



int EvaluatableExpr::maxOrder(const Set<MultiIndex>& m) const 
{
  int rtn = 0;
  for (Set<MultiIndex>::const_iterator i=m.begin(); i != m.end(); i++)
    {
      rtn = max(i->order(), rtn);
    }
  return rtn;
}

const EvaluatableExpr* EvaluatableExpr::getEvalExpr(const Expr& expr)
{
  const EvaluatableExpr* rtn 
    = dynamic_cast<const EvaluatableExpr*>(expr[0].ptr().get());
  TEST_FOR_EXCEPTION(rtn==0, InternalError,
                     "cast of " << expr 
                     << " failed in EvaluatableExpr::getEvalExpr()");
  TEST_FOR_EXCEPTION(expr.size() != 1, InternalError,
                     "non-scalar expression " << expr
                     << " in EvaluatableExpr::getEvalExpr()");

  return rtn;
}


bool EvaluatableExpr::isEvaluatable(const ExprBase* expr) 
{
  return dynamic_cast<const EvaluatableExpr*>(expr) != 0;
}


int EvaluatableExpr::countNodes() const
{
  nodesHaveBeenCounted_ = true;
  return 1;
}



const Set<MultipleDeriv>& 
EvaluatableExpr::findW(int order,
                       const EvalContext& context) const
{
  return findDerivSubset(order, AllNonzeros, context);
}

const Set<MultipleDeriv>& 
EvaluatableExpr::findV(int order,
                       const EvalContext& context) const
{
  return findDerivSubset(order, VariableNonzeros, context);
}

const Set<MultipleDeriv>& 
EvaluatableExpr::findC(int order,
                       const EvalContext& context) const
{
  return findDerivSubset(order, ConstantNonzeros, context);
}

const Set<MultipleDeriv>& 
EvaluatableExpr::findR(int order,
                       const EvalContext& context) const
{
  TEST_FOR_EXCEPTION(!rIsReady_, InternalError,
                     "findR() cannot be used for initial computation of the "
                     "R subset. Calling object is " << toString());
  return findDerivSubset(order, RequiredNonzeros, context);
}


const Set<MultipleDeriv>& 
EvaluatableExpr::findDerivSubset(int order,
                                 const DerivSubsetSpecifier& dss,
                                 const EvalContext& context) const
{
  Tabs tabs;
  SUNDANCE_VERB_HIGH(tabs << "finding " << derivType(dss) 
                     << "[" << order << "] for " << toString());
  if (contextToDSSMap_[dss][order].containsKey(context))
    {
      Tabs tab1;
      SUNDANCE_VERB_HIGH(tab1 << "...reusing previously computed data");
    }
  else
    {
      switch(dss)
        {
        case AllNonzeros:
          contextToDSSMap_[dss][order].put(context, internalFindW(order, context));
          break;
        case RequiredNonzeros:
          break;
        case VariableNonzeros:
          contextToDSSMap_[dss][order].put(context, internalFindV(order, context));
          break;
        case ConstantNonzeros:
          contextToDSSMap_[dss][order].put(context, internalFindC(order, context));
          break;
        default:
          TEST_FOR_EXCEPTION(true, InternalError, "this should never happen");
        }
    }


  if (!contextToDSSMap_[dss][order].containsKey(context))
    {
      contextToDSSMap_[dss][order].put(context, Set<MultipleDeriv>());
    }
  const Set<MultipleDeriv>& rtn = contextToDSSMap_[dss][order].get(context);
  SUNDANCE_VERB_HIGH(tabs << "found " << rtn);
  return rtn;
}


Set<MultipleDeriv> 
EvaluatableExpr::setProduct(const Set<MultipleDeriv>& a,
                            const Set<MultipleDeriv>& b) const
{
  Set<MultipleDeriv> rtn;
  for (Set<MultipleDeriv>::const_iterator i=a.begin(); i!=a.end(); i++)
    {
      for (Set<MultipleDeriv>::const_iterator j=b.begin(); j!=b.end(); j++)
        {
          rtn.put(i->product(*j));
        }
    }
  return rtn;
}

Set<MultiSet<int> > EvaluatableExpr::setDivision(const Set<MultiSet<int> >& s,
                                                 int index) const 
{
  Set<MultiSet<int> > rtn;
  for (Set<MultiSet<int> >::const_iterator 
         i=s.begin(); i!=s.end(); i++)
    {
      if (i->contains(index))
        {
          MultiSet<int> e = *i;
          e.erase(e.find(index));
          rtn.put(e);
        }
    }
  return rtn;
}


Set<MultipleDeriv>  
EvaluatableExpr::setDivision(const Set<MultipleDeriv>& a,
                             const Set<MultipleDeriv>& b) const
{
  Set<MultipleDeriv> rtn;
  for (Set<MultipleDeriv>::const_iterator i=a.begin(); i!=a.end(); i++)
    {
      for (Set<MultipleDeriv>::const_iterator j=b.begin(); j!=b.end(); j++)
        {
          MultipleDeriv c = i->factorOutDeriv(*j);
          if (c.size() != 0) rtn.put(c);
          if (*i == *j) rtn.put(MultipleDeriv());
        }
    }
  return rtn;
}

void EvaluatableExpr::determineR(const EvalContext& context,
                                 const Array<Set<MultipleDeriv> >& RInput) const
{
  Tabs tabs;
  RefCountPtr<Array<Set<MultipleDeriv> > > additionToR 
    =  internalDetermineR(context, RInput);
  
  for (unsigned int i=0; i<RInput.size(); i++)
    {
      if ((*additionToR)[i].size()==0U) continue;
      if (!contextToDSSMap_[RequiredNonzeros][i].containsKey(context))
        {
          contextToDSSMap_[RequiredNonzeros][i].put(context, (*additionToR)[i]); 
        }
      else
        {
          contextToDSSMap_[RequiredNonzeros][i][context].merge((*additionToR)[i]);
        }
    }
  rIsReady_ = true;

}

RefCountPtr<Array<Set<MultipleDeriv> > > EvaluatableExpr
::internalDetermineR(const EvalContext& context,
                     const Array<Set<MultipleDeriv> >& RInput) const
{
  Tabs tab0;
  RefCountPtr<Array<Set<MultipleDeriv> > > rtn 
    = rcp(new Array<Set<MultipleDeriv> >(RInput.size()));

  SUNDANCE_VERB_HIGH( tab0 << "EE::internalDetermineR() for " << toString() );
  SUNDANCE_VERB_HIGH( tab0 << "RInput = " << RInput );

  for (unsigned int i=0; i<RInput.size(); i++)
    {
      Tabs tab1;
      if (RInput[i].size()==0U) continue;
      const Set<MultipleDeriv>& Wi = findW(i, context);
      SUNDANCE_VERB_EXTREME( tab1 << "W[" << i << "] = " << Wi );
      (*rtn)[i] = RInput[i].intersection(Wi);
    }

  SUNDANCE_VERB_HIGH( tab0 << "R = " << (*rtn) );

  return rtn;
}


Array<Set<MultipleDeriv> > EvaluatableExpr
::computeInputR(const EvalContext& context,
                const Array<Set<MultiSet<int> > >& funcIDCombinations,
                const Array<Set<MultiIndex> >& spatialDerivs) const 
{
  Array<Set<MultipleDeriv> > rtn(funcIDCombinations.size());
  for (unsigned int order=0; order<funcIDCombinations.size(); order++)
    {
      const Set<MultipleDeriv>& W = findW(order, context);

      for (Set<MultipleDeriv>::const_iterator i=W.begin(); i!=W.end(); i++)
        {
          const MultipleDeriv& nonzeroDeriv = *i;
          if (nonzeroDeriv.isInRequiredSet(funcIDCombinations[order],
                                           spatialDerivs[order]))
            {
              rtn[order].put(nonzeroDeriv);
            }
        }
    }
  return rtn;
}


const Set<MultipleDeriv>& EvaluatableExpr::findW(const EvalContext& context) const
{
  return findDerivSubset(AllNonzeros, context);
}

const Set<MultipleDeriv>& EvaluatableExpr::findR(const EvalContext& context) const
{
  return findDerivSubset(RequiredNonzeros, context);
}

const Set<MultipleDeriv>& EvaluatableExpr::findC(const EvalContext& context) const
{
  return findDerivSubset(ConstantNonzeros, context);
}

const Set<MultipleDeriv>& EvaluatableExpr::findV(const EvalContext& context) const
{
  return findDerivSubset(VariableNonzeros, context);
}

const Set<MultipleDeriv>& 
EvaluatableExpr::findDerivSubset(const DerivSubsetSpecifier& dss,
                                 const EvalContext& context) const
{
  if (!allOrdersMap_[dss].containsKey(context))
    {
      Set<MultipleDeriv> tmp;
      for (int i=0; i<=3; i++)
        {
          tmp.merge(findDerivSubset(i, dss, context));
        }
      allOrdersMap_[dss].put(context, tmp);
    }

  return allOrdersMap_[dss].get(context);
}

void EvaluatableExpr::displayNonzeros(ostream& os, const EvalContext& context) const 
{
  Tabs tabs0;
  os << tabs0 << "Nonzeros of " << toString() << endl;

  const Set<MultipleDeriv>& W = findW(context);
  const Set<MultipleDeriv>& R = findR(context);
  const Set<MultipleDeriv>& C = findC(context);
  const Set<MultipleDeriv>& V = findV(context);

  TEST_FOR_EXCEPT(C.intersection(V).size() != 0);

  for (Set<MultipleDeriv>::const_iterator i=W.begin(); i != W.end(); i++)
    {
      Tabs tab1;
      string state = "Variable";
      if (C.contains(*i)) state = "Constant";
      if (!R.contains(*i)) state = "Not Required";
      os << tab1 << std::setw(25) << std::left << i->toString() << ": " << state << endl;
    }
}

