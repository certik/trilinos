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

#include "Sundance.hpp"

using Sundance::List;


/** 
 * Solves the coupled equations
 *
 * \f[\nabla^2 u_1 = u_2 \f]
 * \f[\nabla^2 u_2 = x \f]
 * \f[u_1 = u_2 = 0\;\;\;\mathrm{on\; east\; and\; west}\f]
 * \f[\frac{\partial u_1}{\partial n} = \frac{\partial u_2}{\partial n} = 0\;\;\;\mathrm{on\; north\; and\; south}\f]
 * 
 *
 * The solution is
 * \f[u_1(x,y)=\frac{1}{360}(3 x^5 - 10 x^3 + 7 x)\f]
 * \f[u_2(x,y)= \frac{1}{6} x (x^2-1)\f]
 *
 * 
 */

int main(int argc, char** argv)
{
  
  try
		{
      Sundance::init(&argc, &argv);

      /* We will do our linear algebra using Epetra */
      VectorType<double> vecType = new EpetraVectorType();

      /* Create a mesh. It will be of type BasisSimplicialMesh, and will
       * be built using a PartitionedRectangleMesher. */
      MeshType meshType = new BasicSimplicialMeshType();
      int nx = 32;

      MeshSource mesher = new PartitionedRectangleMesher(0.0, 1.0, nx,
        0.0, 2.0, nx,
        meshType);
      Mesh mesh = mesher.getMesh();

      /* Create a cell filter that will identify the maximal cells
       * in the interior of the domain */
      CellFilter interior = new MaximalCellFilter();

      /* Create cell filters for the boundary surfaces */
      CellFilter edges = new DimensionalCellFilter(1);

      CellFilter west = edges.coordSubset(0, 0.0);
      CellFilter east = edges.coordSubset(0, 1.0);

      
      /* Create unknown and test functions, discretized using first-order
       * Lagrange interpolants */
      Expr u1 = new UnknownFunction(new Lagrange(3), "u");
      Expr u2 = new UnknownFunction(new Lagrange(2), "v");
      Expr du1 = new TestFunction(new Lagrange(3), "du");
      Expr du2 = new TestFunction(new Lagrange(2), "dv");

      /* Create differential operator and coordinate function */
      Expr dx = new Derivative(0);
      Expr x = new CoordExpr(0);
      Expr dy = new Derivative(1);
      Expr y = new CoordExpr(1);
      Expr grad = List(dx, dy);

      /* We need a quadrature rule for doing the integrations */
      QuadratureFamily quad = new GaussianQuadrature(6);

      
      /* Define the weak form */
      Expr eqn = Integral(interior, 
                          (grad*du1)*(grad*u1) + du1*u2 + (grad*du2)*(grad*u2) + x*du2, 
                          quad);
      /* Define the Dirichlet BC */
      Expr bc = EssentialBC(east + west, du1*u1 + du2*u2, quad);

      /* We can now set up the linear problem! The order of the test functions in the
      * list determines the order of these variables in the matrix. Liewise for
      * the unknown functions. */
      LinearProblem prob(mesh, eqn, bc, List(du1,du2), List(u1,u2), vecType);

      

      /* Get a solver */
      LinearSolver<double> solver = LinearSolverBuilder::createSolver("aztec-ml.xml");


      /* Solver */
      Expr soln = prob.solve(solver);

      /* Check */
      Expr x2 = x*x;
      Expr x3 = x*x2;

      Expr u1Exact = (1.0/120.0)*x2*x3 - 1.0/36.0 * x3 + 7.0/360.0 * x;
      Expr u2Exact = 1.0/6.0 * x * (x2 - 1.0);

      Expr u1Err = u1Exact - soln[0];
      Expr u2Err = u2Exact - soln[1];
      
      double u1ErrorNorm = L2Norm(mesh, interior, u1Err, quad);
      std::cerr << "u1 error norm = " << u1ErrorNorm << std::endl << std::endl;

      double u2ErrorNorm = L2Norm(mesh, interior, u2Err, quad);
      std::cerr << "u2 error norm = " << u2ErrorNorm << std::endl << std::endl;


      
      /* Write the field in VTK format */
      FieldWriter w = new VTKWriter("Coupled2D");
      w.addMesh(mesh);
      w.addField("v", new ExprFieldWrapper(soln[0]));
      w.addField("u", new ExprFieldWrapper(soln[1]));
      w.write();



      double tol = 1.0e-5;
      Sundance::passFailTest(max(u1ErrorNorm, u2ErrorNorm), tol);
    }
	catch(std::exception& e)
		{
      Sundance::handleException(e);
		}
  Sundance::finalize(); 
  return Sundance::testStatus(); 
}