/*!
  \file   LaplacianSmoother.hpp
  \brief  

  The LaplacianSmoother Class implements the Laplacian smoothing
  for a patch with one free vertex. 

  \author Thomas Leurent
  \date   2002-01-17
*/

#ifndef Mesquite_LaplacianSmoother_hpp 
#define Mesquite_LaplacianSmoother_hpp

#include "Mesquite.hpp"
#include "VertexMover.hpp"

namespace Mesquite
{

  /*! \class LaplacianSmoother
    Moves free center vertex to the average of the neighboring vertices.
   */  
  class LaplacianSmoother : public VertexMover 
  {
  public:
    LaplacianSmoother();

  protected:
    virtual void initialize(PatchData &pd, MsqError &err);
    virtual void optimize_vertex_positions(PatchData &pd,
                                         MsqError &err);
    virtual void initialize_mesh_iteration(PatchData &pd, MsqError &err);
    virtual void terminate_mesh_iteration(PatchData &pd, MsqError &err);
    virtual void cleanup();

  };

  
#undef __FUNC__
#define __FUNC__ "centroid_smooth_mesh" 
  inline void centroid_smooth_mesh(int num_incident_vtx,
                                   MsqVertex *incident_vtx,
                                   MsqVertex &free_vtx,
                                   int dimension, MsqError &err)
  {
    int i,j;
    double avg[3];

    if (num_incident_vtx==0) 
      err.set_msg("WARNING: Number of incident vertex is zero\n");

    for (j=0;j<dimension;++j) {
      avg[j] = 0.;
      for (i=0;i<num_incident_vtx;++i){
          //cout<<"INSIDE v:  i="<<i<<"   "<<incident_vtx[i];
        avg[j]+=incident_vtx[i][j];
      }
      free_vtx[j] = avg[j]/((double) num_incident_vtx);
        //cout << "centroid_smooth_mesh(): final --  avg["<<j<<"] = " << free_vtx[j] << endl;
    }

    return;
  }

  
}

#endif
