/* ***************************************************************** 
    MESQUITE -- The Mesh Quality Improvement Toolkit

    Copyright 2004 Sandia Corporation and Argonne National
    Laboratory.  Under the terms of Contract DE-AC04-94AL85000 
    with Sandia Corporation, the U.S. Government retains certain 
    rights in this software.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License 
    (lgpl.txt) along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    diachin2@llnl.gov, djmelan@sandia.gov, mbrewer@sandia.gov, 
    pknupp@sandia.gov, tleurent@mcs.anl.gov, tmunson@mcs.anl.gov      
   
  ***************************************************************** */
// -*- Mode : c++; tab-width: 3; c-tab-always-indent: t; indent-tabs-mode: nil; c-basic-offset: 3 -*-

/*! \file InstructionQueue.hpp

Header file for the Mesquite::InstructionQueue class

  \author Thomas Leurent
  \date   2002-05-01
 */


#ifndef MSQ_INSTRUCTION_QUEUE_HPP
#define MSQ_INSTRUCTION_QUEUE_HPP

#include "Mesquite.hpp"
#include "Settings.hpp"

#ifdef MSQ_USE_OLD_STD_HEADERS
#  include <list.h>
#else
#  include <list>
#endif

namespace Mesquite {

  class MsqError;
  class QualityImprover;
  class QualityAssessor;
  class Mesh;
  class ParallelMesh;
  class MeshDomain;
  class Instruction;
  class MappingFunctionSet;
  class TargetWriter;

  /*! \class InstructionQueue
    \brief An InstructionQueue object gathers Mesquite Instructions and ensures
           that the instruction queue is coherent for mesh improvement and/or
           mesh quality assessment purposes.

           The user can instantiate several InstructionQueue objects to be used
           with various MeshSet objects.
           
           The most commonly used functions are:
           -# add_preconditioner(...)
           -# add_quality_assessor(...)
           -# set_master_quality_improver(...)
           -# run_instructions(...)
  */
  class InstructionQueue : public Settings
  {

  public:
    MESQUITE_EXPORT InstructionQueue();

    MESQUITE_EXPORT virtual ~InstructionQueue() {};
    
    MESQUITE_EXPORT void add_target_calculator( TargetWriter* tc, MsqError& err );
    
    MESQUITE_EXPORT void add_preconditioner(QualityImprover* instr, MsqError &err);
    MESQUITE_EXPORT void remove_preconditioner(size_t index, MsqError &err);
    MESQUITE_EXPORT void insert_preconditioner(QualityImprover* instr, size_t index, MsqError &err);
    
    MESQUITE_EXPORT void add_quality_assessor(QualityAssessor* instr, MsqError &err);
    MESQUITE_EXPORT void remove_quality_assessor(size_t index, MsqError &err);
    MESQUITE_EXPORT void insert_quality_assessor(QualityAssessor* instr, size_t index, MsqError &err);
    
    MESQUITE_EXPORT void set_master_quality_improver(QualityImprover* instr, MsqError &err);
    
    MESQUITE_EXPORT void disable_automatic_quality_assessment()
       { autoQualAssess = false; }
    MESQUITE_EXPORT void enable_automatic_quality_assessment()
       { autoQualAssess = true; }

      /**\brief Exectute the instruction queue.
       *
       * Execute all operations in the instruction queue.
       *
       *\param mesh   The mesh to run each instruction on.
       *\param domain The domain of the mesh -- may be NULL if no domain.
       */
    MESQUITE_EXPORT virtual void run_instructions( Mesh* mesh,
                                   MeshDomain* domain,
                                   MsqError &err);
    
    inline void run_instructions( Mesh* mesh, MsqError& err )
      { this->run_instructions( mesh, 0, err ); }
    
    MESQUITE_EXPORT virtual void run_instructions( ParallelMesh* mesh,
                                   MeshDomain* domain,
                                   MsqError &err);
    
    inline void run_instructions( ParallelMesh* mesh, MsqError& err )
      { this->run_instructions( mesh, 0, err ); }

    MESQUITE_EXPORT void clear();  
    
  protected:
    
  private:
    msq_std::list<Instruction*>::iterator clear_master(MsqError &err);

    msq_std::list<Instruction*> instructions;

    bool autoQualAssess;
    
    size_t nbPreConditionners;
    bool isMasterSet;
    size_t masterInstrIndex; //!< 0-based. Keeping an index instead of an iterator
                             //!< in case list is reallocated
  };


} //namespace


#endif // InstructionQueue_hpp
