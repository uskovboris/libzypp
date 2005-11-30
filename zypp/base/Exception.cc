/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/base/Exception.cc
 *
*/
#include <iostream>

#include "zypp/base/Logger.h"
#include "zypp/base/String.h"
#include "zypp/base/Exception.h"

using std::endl;

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  namespace exception_detail
  { /////////////////////////////////////////////////////////////////

    std::string CodeLocation::asString() const
    {
      return str::form( "%s(%s):%u",
                        _file.c_str(),
                        _func.c_str(),
                        _line );
    }

    std::ostream & operator<<( std::ostream & str, const CodeLocation & obj )
    { return str << obj.asString(); }

    /////////////////////////////////////////////////////////////////
  } // namespace exception_detail
  ///////////////////////////////////////////////////////////////////

  Exception::Exception( const CodeLocation & where_r, const std::string & msg_r )
  : _where( where_r ), _msg( msg_r )
  {}

  Exception::~Exception() throw()
  {}

  std::string Exception::asString() const
  {
    std::string ret( _where.asString() );
    ret += ": ";
    return ret += _msg;
  }

  std::string Exception::strErrno( int errno_r )
  {
    return str::strerror( errno_r );
  }

  std::string Exception::strErrno( int errno_r, const std::string & msg_r )
  {
    std::string ret( msg_r );
    ret += ": ";
    return ret += strErrno( errno_r );
  }

  void Exception::relocate( Exception & excpt_r, const CodeLocation & where_r )
  {
    excpt_r._where = where_r;
  }

  void Exception::log( const Exception & excpt_r, const CodeLocation & where_r,
                       const char *const prefix_r )
  {
    INT << where_r << " " << prefix_r << " " << excpt_r << endl;
  }

  std::ostream & operator<<( std::ostream & str, const Exception & obj )
  { return str << obj.asString(); }

  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
