
//@HEADER
// ************************************************************************
// 
//          Trilinos: An Object-Oriented Solver Framework
//              Copyright (2001) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//   
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//   
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
// 
// ************************************************************************
//@HEADER

#include "Epetra_DistObject.h"
#include "Epetra_Comm.h"
#include "Epetra_Import.h"
#include "Epetra_Export.h"
#include "Epetra_Distributor.h"



//=============================================================================
// Constructor

Epetra_DistObject::Epetra_DistObject(const Epetra_BlockMap& Map)
  : Epetra_Object("Epetra::DistObject"),
    Map_(Map),
    Comm_(&Map_.Comm()),
    DistributedGlobal_(Map.DistributedGlobal()),
    Exports_(0),
    Imports_(0),
    LenExports_(0),
    LenImports_(0)
{}

//=============================================================================
// Constructor (label given)

Epetra_DistObject::Epetra_DistObject(const Epetra_BlockMap& Map, const char* const Label)
  : Epetra_Object(Label),
    Map_(Map),
    Comm_(&Map_.Comm()),
    DistributedGlobal_(Map.DistributedGlobal()),
    Exports_(0),
    Imports_(0),
    LenExports_(0),
    LenImports_(0)
{}

//=============================================================================
// Copy Constructor

Epetra_DistObject::Epetra_DistObject(const Epetra_DistObject& Source)
  : Epetra_Object(Source),
    Map_(Source.Map_),
    Comm_(&Map_.Comm()),
    DistributedGlobal_(Source.DistributedGlobal_),
    Exports_(0),
    Imports_(0),
    LenExports_(0),
    LenImports_(0)
{}

//=============================================================================
Epetra_DistObject::~Epetra_DistObject(){


  if (Exports_!=0) {
		delete[] Exports_;
		Exports_ = 0;
	}
  if (Imports_!=0) {
		delete[] Imports_;
		Imports_ = 0;
	}

}

//=============================================================================
int Epetra_DistObject::Import(const Epetra_SrcDistObject& A, 
															const Epetra_Import& Importer,
															Epetra_CombineMode CombineMode) 
{

  if (!Map_.SameAs(Importer.TargetMap())) EPETRA_CHK_ERR(-2);
  if (!A.Map().SameAs(Importer.SourceMap())) EPETRA_CHK_ERR(-3);
  
  int NumSameIDs = Importer.NumSameIDs();
  int NumPermuteIDs = Importer.NumPermuteIDs();
  int NumRemoteIDs = Importer.NumRemoteIDs();
  int NumExportIDs = Importer.NumExportIDs();
  int* ExportLIDs = Importer.ExportLIDs();
  int* RemoteLIDs = Importer.RemoteLIDs();
  int* PermuteToLIDs = Importer.PermuteToLIDs();
  int* PermuteFromLIDs = Importer.PermuteFromLIDs();
  int Nsend = Importer.NumSend();
  int Nrecv = Importer.NumRecv();

  EPETRA_CHK_ERR(DoTransfer(A, CombineMode, NumSameIDs, NumPermuteIDs, NumRemoteIDs, NumExportIDs,
														PermuteToLIDs, PermuteFromLIDs, RemoteLIDs, ExportLIDs, Nsend, Nrecv, 
														LenExports_, Exports_, LenImports_, Imports_, Importer.Distributor(), 
														false));
  return(0);
}

//=============================================================================
int Epetra_DistObject::Export(const Epetra_SrcDistObject& A, 
															const Epetra_Export& Exporter,
															Epetra_CombineMode CombineMode) 
{

  if (!Map_.SameAs(Exporter.TargetMap())) EPETRA_CHK_ERR(-2);
  if (!A.Map().SameAs(Exporter.SourceMap())) EPETRA_CHK_ERR(-3);
  
  int NumSameIDs = Exporter.NumSameIDs();
  int NumPermuteIDs = Exporter.NumPermuteIDs();
  int NumRemoteIDs = Exporter.NumRemoteIDs();
  int NumExportIDs = Exporter.NumExportIDs();
  int* ExportLIDs = Exporter.ExportLIDs();
  int* RemoteLIDs = Exporter.RemoteLIDs();
  int* PermuteToLIDs = Exporter.PermuteToLIDs();
  int* PermuteFromLIDs = Exporter.PermuteFromLIDs();
  int Nsend = Exporter.NumSend();
  int Nrecv = Exporter.NumRecv();

  EPETRA_CHK_ERR(DoTransfer(A, CombineMode, NumSameIDs, NumPermuteIDs, NumRemoteIDs, NumExportIDs,
														PermuteToLIDs, PermuteFromLIDs, RemoteLIDs, ExportLIDs, Nsend, Nrecv, 
														LenExports_, Exports_,LenImports_, Imports_, Exporter.Distributor(), 
														false));
  return(0);
}

//=============================================================================
int Epetra_DistObject::Import(const Epetra_SrcDistObject& A, 
															const Epetra_Export& Exporter,
															Epetra_CombineMode CombineMode) 
{

  if (!Map_.SameAs(Exporter.SourceMap())) EPETRA_CHK_ERR(-2);
  if (!A.Map().SameAs(Exporter.TargetMap())) EPETRA_CHK_ERR(-3);
  
  int NumSameIDs = Exporter.NumSameIDs();
  int NumPermuteIDs = Exporter.NumPermuteIDs();
  int NumRemoteIDs = Exporter.NumExportIDs();
  int NumExportIDs = Exporter.NumRemoteIDs();
  int* ExportLIDs = Exporter.RemoteLIDs();
  int* RemoteLIDs = Exporter.ExportLIDs();
  int* PermuteToLIDs = Exporter.PermuteFromLIDs();
  int* PermuteFromLIDs = Exporter.PermuteToLIDs();
  int Nsend = Exporter.NumRecv();
  int Nrecv = Exporter.NumSend();

  EPETRA_CHK_ERR(DoTransfer(A, CombineMode, NumSameIDs, NumPermuteIDs, NumRemoteIDs, NumExportIDs, 
														PermuteToLIDs, PermuteFromLIDs, RemoteLIDs, ExportLIDs, Nsend, Nrecv,
														LenImports_, Imports_, LenExports_, Exports_, Exporter.Distributor(), 
														true));
  return(0);
}

//=============================================================================
int Epetra_DistObject::Export(const Epetra_SrcDistObject& A, 
															const Epetra_Import& Importer,
															Epetra_CombineMode CombineMode) 
{

  if (!Map_.SameAs(Importer.SourceMap())) EPETRA_CHK_ERR(-2);
  if (!A.Map().SameAs(Importer.TargetMap())) EPETRA_CHK_ERR(-3);
  
  int NumSameIDs = Importer.NumSameIDs();
  int NumPermuteIDs = Importer.NumPermuteIDs();
  int NumRemoteIDs = Importer.NumExportIDs();
  int NumExportIDs = Importer.NumRemoteIDs();
  int* ExportLIDs = Importer.RemoteLIDs();
  int* RemoteLIDs = Importer.ExportLIDs();
  int* PermuteToLIDs = Importer.PermuteFromLIDs();
  int* PermuteFromLIDs = Importer.PermuteToLIDs();
  int Nsend = Importer.NumRecv();
  int Nrecv =Importer.NumSend();

  EPETRA_CHK_ERR(DoTransfer(A, CombineMode, NumSameIDs, NumPermuteIDs, NumRemoteIDs, NumExportIDs,  
														PermuteToLIDs, PermuteFromLIDs,  RemoteLIDs, ExportLIDs, Nsend, Nrecv, 
														LenImports_, Imports_, LenExports_, Exports_, 
														Importer.Distributor(), true));
  return(0);
}

//=============================================================================
int Epetra_DistObject::DoTransfer(const Epetra_SrcDistObject& A, 
																	Epetra_CombineMode CombineMode, 
																	int NumSameIDs, 
																	int NumPermuteIDs, 
																	int NumRemoteIDs, 
																	int NumExportIDs, 
																	int* PermuteToLIDs, 
																	int* PermuteFromLIDs, 
																	int* RemoteLIDs, 
																	int* ExportLIDs,
																	int Nsend, 
																	int Nrecv,
																	int& LenExports, 
																	char*& Exports,
																	int& LenImports, 
																	char*& Imports, 
																	Epetra_Distributor& Distor, 
																	bool DoReverse)
{

  EPETRA_CHK_ERR(CheckSizes(A));

  if (NumSameIDs + NumPermuteIDs > 0) {
    EPETRA_CHK_ERR(CopyAndPermute(A, NumSameIDs, NumPermuteIDs, PermuteToLIDs, PermuteFromLIDs));
  }

  if (CombineMode==Zero) 
		return(0); // All done if CombineMode only involves copying and permuting
  
  int SizeOfPacket; 
  EPETRA_CHK_ERR(PackAndPrepare(A, NumExportIDs, ExportLIDs, Nsend, Nrecv, 
																LenExports, Exports, LenImports, Imports, SizeOfPacket, Distor));

  if (DistributedGlobal_) {
    if (DoReverse) {
      // Do the exchange of remote data
      EPETRA_CHK_ERR(Distor.DoReverse(Exports, SizeOfPacket, Imports));
    }
    else {
      // Do the exchange of remote data
      EPETRA_CHK_ERR(Distor.Do(Exports, SizeOfPacket, Imports));
    }
    EPETRA_CHK_ERR(UnpackAndCombine(A, NumRemoteIDs, RemoteLIDs, Imports, SizeOfPacket, Distor, CombineMode));
  }

  return(0);
}

//=============================================================================
void Epetra_DistObject::Print(ostream& os) const {
  int MyPID = Comm().MyPID();
  int NumProc = Comm().NumProc();
  
  for (int iproc=0; iproc < NumProc; iproc++) {
    if (MyPID==iproc) {
      Comm().PrintInfo(os);
      os << "Length of Export buffer (in chars) = " << LenExports_ << endl;
      os << "Length of Import buffer (in chars) = " << LenImports_ << endl;
      os << flush;
		}
  }
  return;
}
