
/*! @HEADER */
/*
************************************************************************

                CTrilinos:  C interface to Trilinos
                Copyright (2009) Sandia Corporation

Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
license for use of this work by or on behalf of the U.S. Government.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA
Questions? Contact M. Nicole Lemaster (mnlemas@sandia.gov)

************************************************************************
*/
/*! @HEADER */


#include "CTrilinos_config.h"

#include "CEpetra_Flops_Cpp.hpp"
#include "CEpetra_CompObject_Cpp.hpp"
#include "CEpetra_CompObject.h"
#include "Epetra_CompObject.h"
#include "Teuchos_RCP.hpp"
#include "CTrilinos_enums.h"
#include "CTrilinos_utils.hpp"
#include "CTrilinos_utils_templ.hpp"
#include "CTrilinos_TableRepos.hpp"
//
// Definitions from CEpetra_CompObject.h
//


extern "C" {


CT_Epetra_CompObject_ID_t Epetra_CompObject_Degeneralize ( 
  CTrilinos_Universal_ID_t id )
{
    return CTrilinos::concreteType<CT_Epetra_CompObject_ID_t>(id);
}

CTrilinos_Universal_ID_t Epetra_CompObject_Generalize ( 
  CT_Epetra_CompObject_ID_t id )
{
    return CTrilinos::abstractType<CT_Epetra_CompObject_ID_t>(id);
}

CT_Epetra_CompObject_ID_t Epetra_CompObject_Create (  )
{
    return CTrilinos::tableRepos().store<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(new Epetra_CompObject());
}

CT_Epetra_CompObject_ID_t Epetra_CompObject_Duplicate ( 
  CT_Epetra_CompObject_ID_t SourceID )
{
    const Teuchos::RCP<const Epetra_CompObject> Source = 
        CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(SourceID);
    return CTrilinos::tableRepos().store<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(new Epetra_CompObject(*Source));
}

void Epetra_CompObject_Destroy ( CT_Epetra_CompObject_ID_t * selfID )
{
    CTrilinos::tableRepos().remove(selfID);
}

void Epetra_CompObject_SetFlopCounter ( 
  CT_Epetra_CompObject_ID_t selfID, 
  CT_Epetra_Flops_ID_t FlopCounter_inID )
{
    const Teuchos::RCP<const Epetra_Flops> FlopCounter_in = 
        CTrilinos::tableRepos().getConst<Epetra_Flops, CT_Epetra_Flops_ID_t>(
        FlopCounter_inID);
    CTrilinos::tableRepos().get<Epetra_CompObject, CT_Epetra_CompObject_ID_t>(
        selfID)->SetFlopCounter(*FlopCounter_in);
}

void Epetra_CompObject_SetFlopCounter_Matching ( 
  CT_Epetra_CompObject_ID_t selfID, 
  CT_Epetra_CompObject_ID_t CompObjectID )
{
    const Teuchos::RCP<const Epetra_CompObject> CompObject = 
        CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(CompObjectID);
    CTrilinos::tableRepos().get<Epetra_CompObject, CT_Epetra_CompObject_ID_t>(
        selfID)->SetFlopCounter(*CompObject);
}

void Epetra_CompObject_UnsetFlopCounter ( 
  CT_Epetra_CompObject_ID_t selfID )
{
    CTrilinos::tableRepos().get<Epetra_CompObject, CT_Epetra_CompObject_ID_t>(
        selfID)->UnsetFlopCounter();
}

CT_Epetra_Flops_ID_t Epetra_CompObject_GetFlopCounter ( 
  CT_Epetra_CompObject_ID_t selfID )
{
    return CTrilinos::tableRepos().store<Epetra_Flops, CT_Epetra_Flops_ID_t>(
        CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID)->GetFlopCounter());
}

void Epetra_CompObject_ResetFlops ( 
  CT_Epetra_CompObject_ID_t selfID )
{
    CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID)->ResetFlops();
}

double Epetra_CompObject_Flops ( CT_Epetra_CompObject_ID_t selfID )
{
    return CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID)->Flops();
}

void Epetra_CompObject_UpdateFlops_Int ( 
  CT_Epetra_CompObject_ID_t selfID, int Flops_in )
{
    CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_UpdateFlops_Long ( 
  CT_Epetra_CompObject_ID_t selfID, long int Flops_in )
{
    CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_UpdateFlops_Double ( 
  CT_Epetra_CompObject_ID_t selfID, double Flops_in )
{
    CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_UpdateFlops_Float ( 
  CT_Epetra_CompObject_ID_t selfID, float Flops_in )
{
    CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_Assign ( 
  CT_Epetra_CompObject_ID_t selfID, CT_Epetra_CompObject_ID_t srcID )
{
    Epetra_CompObject& self = *( CTrilinos::tableRepos().get<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(selfID) );

    const Teuchos::RCP<const Epetra_CompObject> src = 
        CTrilinos::tableRepos().getConst<Epetra_CompObject, 
        CT_Epetra_CompObject_ID_t>(srcID);
    self = *src;
}


} // extern "C"


//
// Definitions from CEpetra_CompObject_Cpp.hpp
//


/* get Epetra_CompObject from non-const table using CT_Epetra_CompObject_ID */
const Teuchos::RCP<Epetra_CompObject>
CEpetra::getCompObject( CT_Epetra_CompObject_ID_t id )
{
    return CTrilinos::tableRepos().get<Epetra_CompObject, CT_Epetra_CompObject_ID_t>(id);
}

/* get Epetra_CompObject from non-const table using CTrilinos_Universal_ID_t */
const Teuchos::RCP<Epetra_CompObject>
CEpetra::getCompObject( CTrilinos_Universal_ID_t id )
{
    return CTrilinos::tableRepos().get<Epetra_CompObject, CTrilinos_Universal_ID_t>(id);
}

/* get const Epetra_CompObject from either the const or non-const table
 * using CT_Epetra_CompObject_ID */
const Teuchos::RCP<const Epetra_CompObject>
CEpetra::getConstCompObject( CT_Epetra_CompObject_ID_t id )
{
    return CTrilinos::tableRepos().getConst<Epetra_CompObject, CT_Epetra_CompObject_ID_t>(id);
}

/* get const Epetra_CompObject from either the const or non-const table
 * using CTrilinos_Universal_ID_t */
const Teuchos::RCP<const Epetra_CompObject>
CEpetra::getConstCompObject( CTrilinos_Universal_ID_t id )
{
    return CTrilinos::tableRepos().getConst<Epetra_CompObject, CTrilinos_Universal_ID_t>(id);
}

/* store Epetra_CompObject in non-const table */
CT_Epetra_CompObject_ID_t
CEpetra::storeCompObject( Epetra_CompObject *pobj )
{
    return CTrilinos::tableRepos().store<Epetra_CompObject, CT_Epetra_CompObject_ID_t>(pobj, false);
}

/* store const Epetra_CompObject in const table */
CT_Epetra_CompObject_ID_t
CEpetra::storeConstCompObject( const Epetra_CompObject *pobj )
{
    return CTrilinos::tableRepos().store<Epetra_CompObject, CT_Epetra_CompObject_ID_t>(pobj, false);
}



