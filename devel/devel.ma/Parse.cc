#include "Tools.h"

#include <zypp/base/PtrTypes.h>
#include <zypp/base/Exception.h>
#include <zypp/base/LogTools.h>
#include <zypp/base/ProvideNumericId.h>

#include "zypp/ZYppFactory.h"
#include "zypp/ResPoolProxy.h"
#include <zypp/CapMatchHelper.h>

#include "zypp/ZYppCallbacks.h"
#include "zypp/NVRAD.h"
#include "zypp/ResPool.h"
#include "zypp/ResFilters.h"
#include "zypp/CapFilters.h"
#include "zypp/Package.h"
#include "zypp/Pattern.h"
#include "zypp/Language.h"
#include "zypp/PackageKeyword.h"
#include "zypp/NameKindProxy.h"
#include "zypp/pool/GetResolvablesToInsDel.h"

#include "zypp/parser/TagParser.h"
#include "zypp/parser/susetags/PackagesFileReader.h"
#include "zypp/parser/susetags/PackagesLangFileReader.h"
#include "zypp/parser/susetags/PatternFileReader.h"
#include "zypp/parser/susetags/ContentFileReader.h"
#include "zypp/parser/susetags/RepoIndex.h"
#include "zypp/parser/susetags/RepoParser.h"
#include "zypp/cache/CacheStore.h"
#include "zypp/RepoManager.h"
#include "zypp/RepoInfo.h"

using namespace std;
using namespace zypp;
using namespace zypp::functor;

using zypp::parser::TagParser;

///////////////////////////////////////////////////////////////////

static const Pathname sysRoot( "/Local/ROOT" );

///////////////////////////////////////////////////////////////////

struct Xprint
{
  bool operator()( const PoolItem & obj_r )
  {
    return true;
  }

  bool operator()( const ResObject_Ptr & obj_r )
  {
    SrcPackage_constPtr p( asKind<SrcPackage>( obj_r ) );
    if ( p )
    {
      getZYpp()->installSrcPackage( p );
      SEC << p << endl;
    }
    return true;
  }

  bool operator()( const Repository & repo_r )
  {
    USR << repo_r.resolvables() << endl;
    std::for_each( repo_r.resolvables().begin(), repo_r.resolvables().end(), Xprint() );
    return true;
  }


  template<class _C>
  bool operator()( const _C & obj_r )
  {
    return true;
  }
};

///////////////////////////////////////////////////////////////////
struct SetTransactValue
{
  SetTransactValue( ResStatus::TransactValue newVal_r, ResStatus::TransactByValue causer_r )
  : _newVal( newVal_r )
  , _causer( causer_r )
  {}

  ResStatus::TransactValue   _newVal;
  ResStatus::TransactByValue _causer;

  bool operator()( const PoolItem & pi ) const
  {
    bool ret = pi.status().setTransactValue( _newVal, _causer );
    if ( ! ret )
      ERR << _newVal <<  _causer << " " << pi << endl;
    return ret;
  }
};

struct StatusReset : public SetTransactValue
{
  StatusReset()
  : SetTransactValue( ResStatus::KEEP_STATE, ResStatus::USER )
  {}
};

struct StatusInstall : public SetTransactValue
{
  StatusInstall()
  : SetTransactValue( ResStatus::TRANSACT, ResStatus::USER )
  {}
};

inline bool g( const NameKindProxy & nkp, Arch arch = Arch() )
{
  if ( nkp.availableEmpty() )
  {
    ERR << "No Item to select: " << nkp << endl;
    return false;
    ZYPP_THROW( Exception("No Item to select") );
  }

  if ( arch != Arch() )
  {
    typeof( nkp.availableBegin() ) it =  nkp.availableBegin();
    for ( ; it != nkp.availableEnd(); ++it )
    {
      if ( (*it)->arch() == arch )
	return (*it).status().setTransact( true, ResStatus::USER );
    }
  }

  return nkp.availableBegin()->status().setTransact( true, ResStatus::USER );
}

///////////////////////////////////////////////////////////////////

bool solve( bool establish = false )
{
  if ( establish )
  {
    bool eres = false;
    {
      zypp::base::LogControl::TmpLineWriter shutUp;
      eres = getZYpp()->resolver()->establishPool();
    }
    if ( ! eres )
    {
      ERR << "establish " << eres << endl;
      return false;
    }
    MIL << "establish " << eres << endl;
  }

  bool rres = false;
  {
    zypp::base::LogControl::TmpLineWriter shutUp;
    rres = getZYpp()->resolver()->resolvePool();
  }
  if ( ! rres )
  {
    ERR << "resolve " << rres << endl;
    return false;
  }
  MIL << "resolve " << rres << endl;
  return true;
}

///////////////////////////////////////////////////////////////////

struct ConvertDbReceive : public callback::ReceiveReport<target::ScriptResolvableReport>
{
  virtual void start( const Resolvable::constPtr & script_r,
                      const Pathname & path_r,
                      Task task_r )
  {
    SEC << __FUNCTION__ << endl
    << "  " << script_r << endl
    << "  " << path_r   << endl
    << "  " << task_r   << endl;
  }

  virtual bool progress( Notify notify_r, const std::string & text_r )
  {
    SEC << __FUNCTION__ << endl
    << "  " << notify_r << endl
    << "  " << text_r   << endl;
    return true;
  }

  virtual void problem( const std::string & description_r )
  {
    SEC << __FUNCTION__ << endl
    << "  " << description_r << endl;
  }

  virtual void finish()
  {
    SEC << __FUNCTION__ << endl;
  }

};

///////////////////////////////////////////////////////////////////

struct MediaChangeReceive : public callback::ReceiveReport<media::MediaChangeReport>
{
  virtual Action requestMedia( Repository source
                               , unsigned mediumNr
                               , Error error
                               , const std::string & description )
  {
    SEC << __FUNCTION__ << endl
    << "  " << source << endl
    << "  " << mediumNr << endl
    << "  " << error << endl
    << "  " << description << endl;
    return IGNORE;
  }
};

///////////////////////////////////////////////////////////////////

namespace container
{
  template<class _Tp>
    bool isIn( const std::set<_Tp> & cont, const typename std::set<_Tp>::value_type & val )
    { return cont.find( val ) != cont.end(); }
}

///////////////////////////////////////////////////////////////////

struct AddResolvables
{
  bool operator()( const Repository & src ) const
  {
    getZYpp()->addResolvables( src.resolvables() );
    return true;
  }
};

///////////////////////////////////////////////////////////////////


std::ostream & operator<<( std::ostream & str, const iostr::EachLine & obj )
{
  str << "(" << obj.valid() << ")[" << obj.lineNo() << "|" << obj.lineStart() << "]{" << *obj << "}";
  return str;

}

///////////////////////////////////////////////////////////////////

#define for_(IT,BEG,END) for ( typeof(BEG) IT = BEG; IT != END; ++IT )

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
bool repolst( const Repository & r )
{
  USR << (r?"Y":"N") << ": " << r << endl;
  return true;
}

using namespace zypp;

/******************************************************************
**
**      FUNCTION NAME : main
**      FUNCTION TYPE : int
*/
int main( int argc, char * argv[] )
{
  //zypp::base::LogControl::instance().logfile( "log.restrict" );
  INT << "===[START]==========================================" << endl;

  RepoManager repoManager( makeRepoManager( "/Local/ROOT" ) );
  RepoInfoList repos = repoManager.knownRepositories();
  SEC << repos << endl;

  if ( repos.empty() )
  {
    RepoInfo nrepo;
    nrepo
	.setAlias( "factorytest" )
	.setName( "Test Repo for factory." )
	.setEnabled( true )
	.setAutorefresh( false )
	.addBaseUrl( Url("ftp://dist.suse.de/install/stable-x86/") );

    repoManager.addRepository( nrepo );
    SEC << "refreshMetadat" << endl;
    repoManager.refreshMetadata( nrepo );
    SEC << "buildCache" << endl;
    repoManager.buildCache( nrepo );
    SEC << "------" << endl;
    repos = repoManager.knownRepositories();
  }

  ResPool pool( getZYpp()->pool() );
  vdumpPoolStats( USR << "Initial pool:" << endl,
		  pool.begin(),
		  pool.end() ) << endl;


  repolst( Repository::noRepository );

  for ( RepoInfoList::iterator it = repos.begin(); it != repos.end(); ++it )
  {
    RepoInfo & nrepo( *it );
    if ( ! nrepo.enabled() )
      continue;

    if ( ! repoManager.isCached( nrepo ) || 0 )
    {
      if ( repoManager.isCached( nrepo ) )
      {
	SEC << "cleanCache" << endl;
	repoManager.cleanCache( nrepo );
      }
      SEC << "refreshMetadat" << endl;
      repoManager.refreshMetadata( nrepo );
      SEC << "buildCache" << endl;
      repoManager.buildCache( nrepo );
    }

    SEC << nrepo << endl;

    Repository nrep( repoManager.createFromCache( nrepo ) );
    const zypp::ResStore & store( nrep.resolvables() );

    dumpPoolStats( SEC << "Store: " << endl,
		   store.begin(), store.end() ) << endl;
    getZYpp()->addResolvables( store );

    repolst( nrep );
  }

  USR << "pool: " << pool << endl;
  SEC << pool.knownRepositoriesSize() << endl;

  if ( 0 )
  {
    zypp::base::LogControl::TmpLineWriter shutUp;
    getZYpp()->initTarget( sysRoot );
  }
  MIL << "Added target: " << pool << endl;

  std::for_each( pool.knownRepositoriesBegin(), pool.knownRepositoriesEnd(), Xprint() );

  ///////////////////////////////////////////////////////////////////
  INT << "===[END]============================================" << endl << endl;
  zypp::base::LogControl::instance().logNothing();
  return 0;
}

