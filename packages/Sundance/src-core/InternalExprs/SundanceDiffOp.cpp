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

#include "SundanceDiffOp.hpp"
#include "SundanceExceptions.hpp"

#include "SundanceTestFuncElement.hpp"

#include "SundanceCoordExpr.hpp"
#include "SundanceZeroExpr.hpp"
#include "SundanceTabs.hpp"
#include "SundanceOut.hpp"

using namespace SundanceCore;
using namespace SundanceUtils;

using namespace SundanceCore;
using namespace Teuchos;
using namespace TSFExtended;

DiffOp::DiffOp(const MultiIndex& op, 
  const RefCountPtr<ScalarExpr>& arg)
  : UnaryExpr(arg), mi_(op), myCoordDeriv_(coordDeriv(op.firstOrderDirection())), requiredFunctions_(),
    ignoreFuncTerms_(false)
{}

void DiffOp::registerSpatialDerivs(const EvalContext& context, 
  const Set<MultiIndex>& miSet) const
{
  EvaluatableExpr::registerSpatialDerivs(context, miSet);
  Set<MultiIndex> s;
  for (Set<MultiIndex>::const_iterator i=miSet.begin(); i!=miSet.end(); i++)
  {
    const MultiIndex& m = *i;
    s.put(m+mi_);
  }
  evaluatableArg()->registerSpatialDerivs(context, s);
}


ostream& DiffOp::toText(ostream& os, bool /* paren */) const 
{
  string miStr = CoordExpr::coordName(mi_.firstOrderDirection(), "");
	os << "D[" << arg().toString() << ", " << miStr << "]";
	return os;
}

ostream& DiffOp::toLatex(ostream& os, bool /* paren */) const 
{
	os << "D^{" << mi_.toString() << "}" << arg().toLatex();
	return os;
}

XMLObject DiffOp::toXML() const 
{
	XMLObject rtn("DiffOp");
	rtn.addAttribute("m", mi_.toString());
  rtn.addChild(arg().toXML());

	return rtn;
}


Evaluator* DiffOp::createEvaluator(const EvaluatableExpr* expr,
  const EvalContext& context) const
{
  return new DiffOpEvaluator(dynamic_cast<const DiffOp*>(expr), context);
}



void DiffOp::requestMultiIndexAtEvalPoint(const MultiIndex& mi,
  const MultipleDeriv& u,
  const EvalContext& context) const 
{
  int verb = context.setupVerbosity();
  Tabs tab0;
  SUNDANCE_MSG3(verb, tab0 << "DiffOp::requestMultiIndexAtEvalPoint() for=" << toString());
  TEST_FOR_EXCEPT(u.size() != 1);
  const Deriv& d = *(u.begin());

  if (d.isFunctionalDeriv())
  {
    const SpatialDerivSpecifier& sds = d.opOnFunc();

    TEST_FOR_EXCEPTION(sds.isDivergence(), InternalError,
      "divergence operators not possible within DiffOp");
    TEST_FOR_EXCEPTION(sds.isNormal(), InternalError,
      "normal deriv operators not possible within DiffOp");

    const MultiIndex& newMi = sds.mi();

    const SymbolicFuncElement* sfe = d.symbFuncElem();
    TEST_FOR_EXCEPT(sfe == 0);
    const EvaluatableExpr* evalPt = sfe->evalPt();
    const ZeroExpr* z = dynamic_cast<const ZeroExpr*>(evalPt);
    if (z != 0) return;
    const DiscreteFuncElement* df 
      = dynamic_cast<const DiscreteFuncElement*>(evalPt);
    df->addMultiIndex(newMi);
    df->findW(1, context);
    df->findV(1, context);
    df->findC(1, context);
  }
}


RefCountPtr<Array<Set<MultipleDeriv> > > 
DiffOp::internalDetermineR(const EvalContext& context,
  const Array<Set<MultipleDeriv> >& RInput) const
{
  Tabs tab0;
  int verb = context.setupVerbosity();

  SUNDANCE_MSG3(verb, tab0 << "DiffOp::internalDetermineR for=" << toString());
  SUNDANCE_MSG3(verb, tab0 << "RInput = " << RInput );

  RefCountPtr<Array<Set<MultipleDeriv> > > rtn 
    = rcp(new Array<Set<MultipleDeriv> >(RInput.size()));
  
  {
    Tabs tab1;
    for (unsigned int i=0; i<RInput.size(); i++)
    {
      Tabs tab2;
      const Set<MultipleDeriv>& Wi = findW(i, context);
      SUNDANCE_MSG5(verb,  tab2 << "W[" << i << "] = " << Wi );
      (*rtn)[i] = RInput[i].intersection(Wi);
    }

    const Set<MultipleDeriv>& W1 = evaluatableArg()->findW(1, context);
    SUNDANCE_MSG3(verb, tab1 << "arg W1 = " << W1);
    Set<MultipleDeriv> ZxXx = applyZx(W1, mi_).setUnion(Xx(mi_));
    SUNDANCE_MSG3(verb, tab1 << "Z union X = " << ZxXx);
    Array<Set<MultipleDeriv> > RArg(RInput.size()+1);
    RArg[0].put(MultipleDeriv());
    RArg[1].put(MultipleDeriv(coordDeriv(mi_.firstOrderDirection())));


    
    for (unsigned int order=0; order<RInput.size(); order++)
    {
      Tabs tab2;
      const Set<MultipleDeriv>& WArgPlus = evaluatableArg()->findW(order+1, context);
      const Set<MultipleDeriv>& WArg = evaluatableArg()->findW(order, context);
      SUNDANCE_MSG3(verb, tab2 << "order = " << order);
      SUNDANCE_MSG3(verb, tab2 << "RInput = " << RInput[order]);
      SUNDANCE_MSG3(verb, tab2 << "WArg = " << WArg);
      SUNDANCE_MSG3(verb, tab2 << "WArgPlus = " << WArgPlus);
      SUNDANCE_MSG3(verb, tab2 << "ZxXx times RInput = " 
        << setProduct(ZxXx, RInput[order]));
      SUNDANCE_MSG3(verb, tab2 << "Tx(RInput, " << -mi_ << ") = " 
        << applyTx(RInput[order], -mi_) );
      RArg[order+1].merge(setProduct(ZxXx, RInput[order]).intersection(WArgPlus));
      RArg[order].merge(applyTx(RInput[order], -mi_).intersection(WArg));
    }
    SUNDANCE_MSG3(verb, tab1 << "RArg = " << RArg);
    
    SUNDANCE_MSG3(verb, tab1 << "calling determineR() for arg "
      << evaluatableArg()->toString());
    evaluatableArg()->determineR(context, RArg);

    /* inform the evaluation points of all functions appearing in the argument
     * that we'll need their spatial derivatives in direction mi(). */
    
    SUNDANCE_MSG3(verb, tab1 << "calling findR(1) to determine required spatial derivatives ");
    const Set<MultipleDeriv>& RArg1 = evaluatableArg()->findR(1, context);
    SUNDANCE_MSG3(verb, tab1 << "RArg1 = " << RArg1);
    for (Set<MultipleDeriv>::const_iterator i=RArg1.begin(); i!=RArg1.end(); i++)
    {
      requestMultiIndexAtEvalPoint(mi(), *i, context);
    }
  }
  SUNDANCE_MSG3(verb, tab0 << "R = " << (*rtn) );
  SUNDANCE_MSG3(verb, tab0 << "done with DiffOp::internalDetermineR for "
    << toString());
  /* all done */  
  return rtn;
}


Set<MultipleDeriv> DiffOp::internalFindW(int order, const EvalContext& context) const
{
  Tabs tabs;
  int verb = context.setupVerbosity();

  SUNDANCE_MSG3(verb, tabs << "DiffOp::internalFindW(order="
    << order << ") for " << toString());

  Set<MultipleDeriv> rtn ;
  if (order <= 2)
  {
    Tabs tab1;
    const Set<MultipleDeriv>& W1 = evaluatableArg()->findW(1, context);
    const Set<MultipleDeriv>& WArg = evaluatableArg()->findW(order, context);
    const Set<MultipleDeriv>& WArgPlus = evaluatableArg()->findW(order+1, context);

    SUNDANCE_MSG5(verb, tab1 << "W1 = " << W1);
    SUNDANCE_MSG5(verb, tab1 << "WArg = " << WArg);
    SUNDANCE_MSG5(verb, tab1 << "WArgPlus = " << WArgPlus);
    Set<MultipleDeriv> Tx = applyTx(WArg, mi_);
    Set<MultipleDeriv> ZXx = applyZx(W1, mi_).setUnion(Xx(mi_));
    SUNDANCE_MSG5(verb, tab1 << "Tx(Warg) = " << Tx);
    SUNDANCE_MSG5(verb, tab1 << "ZXx(W1) = " << ZXx);
    Set<MultipleDeriv> WargPlusOslashZXx = setDivision(WArgPlus, ZXx);
    SUNDANCE_MSG5(verb, tab1 << "WArgPlus / ZXx = " 
      << WargPlusOslashZXx);
    rtn = WargPlusOslashZXx.setUnion(Tx); 
  }
  SUNDANCE_MSG3(verb, tabs << "W[" << order << "]=" << rtn);
  SUNDANCE_MSG3(verb, tabs << "done with DiffOp::internalFindW(" << order << ") for "
    << toString());

  return rtn;
}


Set<MultipleDeriv> DiffOp::internalFindV(int order, const EvalContext& context) const
{
  Tabs tabs;
  int verb = context.setupVerbosity();
  SUNDANCE_MSG3(verb, tabs << "DiffOp::internalFindV(" << order << ") for " 
    << toString());

  Set<MultipleDeriv> rtn;
  
  if (order <= 2)
  {
    Tabs tab1;
    const Set<MultipleDeriv>& W1 = evaluatableArg()->findW(1, context);
    const Set<MultipleDeriv>& VArg = evaluatableArg()->findV(order, context);
    const Set<MultipleDeriv>& VArgPlus 
      = evaluatableArg()->findV(order+1, context);
    const Set<MultipleDeriv>& WArg = evaluatableArg()->findW(order, context);
    const Set<MultipleDeriv>& WArgPlus 
      = evaluatableArg()->findW(order+1, context);

    SUNDANCE_MSG5(verb, tab1 << "W1=" << W1);
    SUNDANCE_MSG5(verb, tab1 << "VArg=" << VArg);
    SUNDANCE_MSG5(verb, tab1 << "VArgPlus=" << VArgPlus);
    SUNDANCE_MSG5(verb, tab1 << "WArg=" << WArg);
    SUNDANCE_MSG5(verb, tab1 << "WArgPlus=" << WArgPlus);

    Set<MultipleDeriv> Tx = applyTx(VArg, mi_);
    Set<MultipleDeriv> Zx = applyZx(W1, mi_);
    SUNDANCE_MSG5(verb, tab1 << "Tx(Varg) = " << Tx);
    SUNDANCE_MSG5(verb, tab1 << "Zx(W1) = " << Zx);
    Set<MultipleDeriv> WargPlusOslashZx = setDivision(WArgPlus, Zx);
    Set<MultipleDeriv> VargPlusOslashXx = setDivision(VArgPlus, Xx(mi_));
    SUNDANCE_MSG5(verb, tab1 << "WArgPlus / Z_alpha = " 
      << WargPlusOslashZx);
    SUNDANCE_MSG5(verb, tab1 << "VArgPlus / X_alpha = " 
      << VargPlusOslashXx);
    rtn = WargPlusOslashZx.setUnion(VargPlusOslashXx).setUnion(Tx); 
   
    SUNDANCE_MSG5(verb, tab1 << "WArgPlus/Z union VArgPlus/X union T =" << rtn);
    rtn = rtn.intersection(findR(order, context));
  }

  SUNDANCE_MSG3(verb, tabs << "V[" << order << "]=" << rtn);
  SUNDANCE_MSG3(verb, tabs << "done with DiffOp::internalFindV(" << order << ") for "
    << toString());

  return rtn;
}


Set<MultipleDeriv> DiffOp::internalFindC(int order, const EvalContext& context) const
{
  Tabs tabs;
  int verb = context.setupVerbosity();
  SUNDANCE_MSG3(verb, tabs << "DiffOp::internalFindC() for " 
    << toString());
  Set<MultipleDeriv> rtn ;

  {
    Tabs tab1;
    SUNDANCE_MSG5(verb, tab1 << "finding R");
    const Set<MultipleDeriv>& R = findR(order, context);
    SUNDANCE_MSG5(verb, tab1 << "finding V");
    const Set<MultipleDeriv>& V = findV(order, context);
    /** Call findC() to ensure that the argument has C tabulated */
    evaluatableArg()->findC(order, context);

    SUNDANCE_MSG5(verb, tab1 << "R=" << R);
    SUNDANCE_MSG5(verb, tab1 << "V=" << V);
    rtn = R.setDifference(V);
    SUNDANCE_MSG3(verb, tabs << "C[" << order << "]=" << rtn);
  }

  SUNDANCE_MSG3(verb, tabs << "C[" << order << "]=R\\V = " << rtn);
  SUNDANCE_MSG3(verb, tabs << "done with DiffOp::internalFindC for "
    << toString());
  return rtn;
}




bool DiffOp::lessThan(const ScalarExpr* other) const
{
  const DiffOp* d = dynamic_cast<const DiffOp*>(other);
  TEST_FOR_EXCEPTION(d==0, InternalError, "cast should never fail at this point");
  
  if (myCoordDeriv_ < d->myCoordDeriv_) return true;
  if (d->myCoordDeriv_ < myCoordDeriv_) return false;
  
  return ExprWithChildren::lessThan(other);
}

