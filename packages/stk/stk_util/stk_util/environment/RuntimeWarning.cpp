/*------------------------------------------------------------------------*/
/*                 Copyright (c) 2013, Sandia Corporation.
/*                 Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
/*                 the U.S. Governement retains certain rights in this software.
/*                 
/*                 Redistribution and use in source and binary forms, with or without
/*                 modification, are permitted provided that the following conditions are
/*                 met:
/*                 
/*                     * Redistributions of source code must retain the above copyright
/*                       notice, this list of conditions and the following disclaimer.
/*                 
/*                     * Redistributions in binary form must reproduce the above
/*                       copyright notice, this list of conditions and the following
/*                       disclaimer in the documentation and/or other materials provided
/*                       with the distribution.
/*                 
/*                     * Neither the name of Sandia Corporation nor the names of its
/*                       contributors may be used to endorse or promote products derived
/*                       from this software without specific prior written permission.
/*                 
/*                 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
/*                 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
/*                 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
/*                 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
/*                 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
/*                 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/*                 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
/*                 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
/*                 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
/*                 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
/*                 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/*                 
/*  Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive   */
/*  license for use of this work by or on behalf of the U.S. Government.  */
/*  Export of this program may require a license from the                 */
/*  United States Government.                                             */
/*------------------------------------------------------------------------*/

#include <stk_util/environment/RuntimeWarning.hpp>
#include <exception>                    // for exception
#include "stk_util/environment/RuntimeMessage.hpp"

namespace stk {

unsigned 
get_warning_count()
{
  return get_message_count(MSG_WARNING);
}


void
reset_warning_count()
{
  reset_message_count(MSG_WARNING);
}


void
set_max_warning_count(
  unsigned int	max_warnings)
{
  set_max_message_count(MSG_WARNING, max_warnings);
}


unsigned 
get_max_warning_count()
{
  return   get_max_message_count(MSG_WARNING);
}


void
report_warning(
  const char *          message,
  const MessageCode &   message_code)
{
  report_message(message, MSG_WARNING, message_code);
}


void
report_symmetric_warning(
  const char *          message,
  const MessageCode &   message_code)
{
  report_message(message, MSG_SYMMETRIC | MSG_WARNING, message_code);
}


void
report_deferred_warning(
  const char *          message,
  const char *          aggregate,
  const MessageCode &   message_code)
{
  add_deferred_message(MSG_WARNING, message_code.m_id, message_code.m_throttle.m_cutoff, message_code.m_throttle.m_group, message, aggregate);
}


RuntimeWarningAdHoc::RuntimeWarningAdHoc(
  MessageCode &   message_code)
  : m_messageCode(message_code)
{}


RuntimeWarningAdHoc::~RuntimeWarningAdHoc()
{
  try {
    report_warning(message.str().c_str(), m_messageCode);
  }
  catch (std::exception &)
  {}
}


RuntimeWarningSymmetric::RuntimeWarningSymmetric(
  MessageCode &   message_code)
  : m_messageCode(message_code)
{}


RuntimeWarningSymmetric::~RuntimeWarningSymmetric()
{
  try {
    report_symmetric_warning(message.str().c_str(), m_messageCode);
  }
  catch (std::exception &)
  {}
}


RuntimeWarningDeferred::RuntimeWarningDeferred(
  const MessageCode &   message_code)
  : m_messageCode(message_code)
{}


RuntimeWarningDeferred::~RuntimeWarningDeferred()
{
  try {
    report_deferred_warning(message.str().c_str(), aggregate.str().c_str(), m_messageCode);
  }
  catch (std::exception &)
  {}
}

} // namespace stk
