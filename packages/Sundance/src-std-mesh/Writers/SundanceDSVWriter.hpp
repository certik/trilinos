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

#ifndef SUNDANCE_DSVWRITER_H
#define SUNDANCE_DSVWRITER_H


#include "SundanceDefs.hpp"
#include "SundanceFieldWriterBase.hpp"

namespace Sundance
{
/**
 * DSVWriter writes a 1D mesh to delimiter-separated file.
 */
class DSVWriter : public FieldWriterBase
{
public:
  /** */
  DSVWriter(const std::string& filename, int precision=6,
    const std::string& delim=" ") 
    : FieldWriterBase(filename),
      precision_(precision),
      delim_(delim) {;}
    
  /** virtual dtor */
  virtual ~DSVWriter(){;}

  /** */
  virtual void write() const ;

  /** Return a ref count pointer to self */
  virtual RCP<FieldWriterBase> getRcp() {return rcp(this);}


private:
  int precision_;
  std::string delim_;
};

/** 
 * DSVWriterFactory produces a DSV writer in contexts where a user cannot
 * do so directly.
 */
class DSVWriterFactory : public FieldWriterFactoryBase
{
public:
  /** */
  DSVWriterFactory() {}

  /** Create a writer with the specified filename */
  RCP<FieldWriterBase> createWriter(const string& name) const 
    {return rcp(new DSVWriter(name));}

  /** */
  virtual RCP<FieldWriterFactoryBase> getRcp() {return rcp(this);}
  
};
}




#endif