
#include "RTOpPack_ROpSum.hpp"
#include "opsUnitTestsHelpers.hpp"


namespace {


TEUCHOS_UNIT_TEST_TEMPLATE_1_DECL( ROpSum, unitStride, Scalar )
{
  using Teuchos::as;
  typedef ScalarTraits<Scalar> ST;
  typedef typename ST::magnitudeType ScalarMag;
  const Scalar v = - ST::random();
  out << "n="<<n<<"\n";
  out << "v="<<v<<"\n";
  SubVectorView<Scalar> sv = newSubVectorView<Scalar>(n, v);
  RTOpPack::ROpSum<Scalar> sumOp;
  RCP<RTOpPack::ReductTarget> sum = sumOp.reduct_obj_create();
  sumOp.apply_op(
    tuple<ConstSubVectorView<Scalar> >(sv)(),
    Teuchos::null,
    sum.ptr()
    );
  TEST_FLOATING_EQUALITY( sumOp(*sum), as<Scalar>(as<Scalar>(n)*v),
    as<ScalarMag>(ST::eps() * errorTolSlack * n) );
}

TEUCHOS_UNIT_TEST_TEMPLATE_1_INSTANT_SCALAR_TYPES( ROpSum, unitStride )


TEUCHOS_UNIT_TEST_TEMPLATE_1_DECL( ROpSum, reduct, Scalar )
{
  using Teuchos::as;
  using Teuchos::dyn_cast;
  using RTOpPack::ReductTargetScalar;
  typedef ScalarTraits<Scalar> ST;
  typedef typename ST::magnitudeType ScalarMag;
  typedef ScalarTraits<ScalarMag> SMT;

  const Scalar v1 = - ST::random();
  const Scalar v2 = + ST::random();
  const Scalar v3 = + ST::random();

  RTOpPack::ROpSum<Scalar> sumOp;

  RCP<RTOpPack::ReductTarget> reduct1 = sumOp.reduct_obj_create();
  RCP<RTOpPack::ReductTarget> reduct2 = sumOp.reduct_obj_create();

  ReductTargetScalar<Scalar> &scalarReduct1 =
    dyn_cast<ReductTargetScalar<Scalar> >(*reduct1); 
  ReductTargetScalar<Scalar> &scalarReduct2 =
    dyn_cast<ReductTargetScalar<Scalar> >(*reduct2); 

  scalarReduct1.set(v1);
  scalarReduct2.set(v2);
  sumOp.reduct_reduct_objs( *reduct1, reduct2.ptr() );

  scalarReduct1.set(v3);
  sumOp.reduct_reduct_objs( *reduct1, reduct2.ptr() );

  TEST_FLOATING_EQUALITY( sumOp(*reduct2), v1+v2+v3,
    as<ScalarMag>(ST::eps() * errorTolSlack) );

}

TEUCHOS_UNIT_TEST_TEMPLATE_1_INSTANT_SCALAR_TYPES( ROpSum, reduct )


} // namespace
