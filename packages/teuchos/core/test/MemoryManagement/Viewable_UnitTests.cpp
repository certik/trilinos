/*
// @HEADER
// ***********************************************************************
//
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ***********************************************************************
// @HEADER
*/

#include "TeuchosCore_ConfigDefs.hpp"
#include "Teuchos_UnitTestHarness.hpp"
#include "Teuchos_Viewable.hpp"

namespace {


using Teuchos::as;
using Teuchos::null;
using Teuchos::Ptr;
using Teuchos::UniquePtr;
using Teuchos::uniqueptr;
using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcpFromRef;
using Teuchos::rcpFromUndefRef;
using Teuchos::outArg;
using Teuchos::rcpWithEmbeddedObj;
using Teuchos::getEmbeddedObj;
using Teuchos::getOptionalEmbeddedObj;
using Teuchos::getOptionalNonconstEmbeddedObj;
using Teuchos::set_extra_data;
using Teuchos::get_optional_nonconst_extra_data;
using Teuchos::getConst;
using Teuchos::NullReferenceError;
using Teuchos::DanglingReferenceError;
using Teuchos::DuplicateOwningRCPError;
using Teuchos::RCP_STRONG;
using Teuchos::RCP_WEAK;
using Teuchos::RCPNodeTracer;

#ifdef HAVE_TEUCHOSCORE_CXX11

// Raw pointers example with unsafe dangling pointers
TEUCHOS_UNIT_TEST( Viewable, Dangling1 )
{
  int *ip;
  {
    int i = 1;
    i += 10;
    ip = &i;
    TEST_EQUALITY(*ip, 11);
  }
  // *ip is now dangling
}

// Viewable example with safe dangling pointers
TEUCHOS_UNIT_TEST( Viewable, Dangling2 )
{
  Ptr<int> ip;
  {
    Viewable<int> i = 1;
    *i += 10;
    ip = i.ptr();
    TEST_EQUALITY(*ip, 11);
  }
#ifdef TEUCHOS_DEBUG
  TEST_THROW( *ip, DanglingReferenceError );
#endif
}

#endif // HAVE_TEUCHOSCORE_CXX11



} // namespace
