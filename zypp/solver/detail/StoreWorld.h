/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* StoreWorld.h
 *
 * Copyright (C) 2000-2002 Ximian, Inc.
 * Copyright (C) 2005 SUSE Linux Products GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef _StoreWorld_h
#define _StoreWorld_h

#include <iosfwd>
#include <string>
#include <list>
#include <map>

#include <zypp/solver/detail/StoreWorldPtr.h>
#include <zypp/solver/detail/ResItemAndDependency.h>
#include <zypp/solver/detail/PackmanPtr.h>
#include <zypp/solver/detail/World.h>
#include <zypp/solver/detail/ResItem.h>
#include <zypp/solver/detail/Match.h>
#include <zypp/Capability.h>

/////////////////////////////////////////////////////////////////////////
namespace zypp 
{ ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  namespace solver
  { /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    namespace detail
    { ///////////////////////////////////////////////////////////////////

      ///////////////////////////////////////////////////////////////////
      //
      //	CLASS NAME : StoreWorld
      
      class StoreWorld : public World {
          
      
        private:
      
          int _freeze_count;
      
          ResItemTable _resItems_by_name;
          ResItemAndDependencyTable _provides_by_name;
          ResItemAndDependencyTable _requires_by_name;
          ResItemAndDependencyTable _conflicts_by_name;
      
          Packman_Ptr _packman;
      
          ChannelList _channels;
      
        public:
      
          StoreWorld (WorldType type = STORE_WORLD);
          virtual ~StoreWorld();
      
          // ---------------------------------- I/O
      
          static std::string toString (const StoreWorld & storeworld);
      
          virtual std::ostream & dumpOn(std::ostream & str ) const;
      
          friend std::ostream& operator<<(std::ostream&, const StoreWorld & storeworld);
      
          std::string asString (void ) const;
      
          // ---------------------------------- accessors
      
          virtual ChannelList channels () const { return _channels; }
      
          // ---------------------------------- methods
      
          // Add/remove resItems
      
          bool addResItem (ResItem_constPtr resItem);
          void addResItemsFromList (const CResItemList & slist);
          void removeResItem (ResItem_constPtr resItem);
          void removeResItems (Channel_constPtr channel);
          void clear ();
      
          // Iterate over resItems
      
          virtual int foreachResItem (Channel_Ptr channel, CResItemFn fn, void *data);
          virtual int foreachResItemByName (const std::string & name, Channel_Ptr channel, CResItemFn fn, void *data);
          virtual int foreachResItemByMatch (Match_constPtr match, CResItemFn fn, void *data);
      
          // Iterate across provides or requirement
      
          virtual int foreachProvidingResItem (const Capability & dep, ResItemAndDepFn fn, void *data);
          virtual int foreachRequiringResItem (const Capability & dep, ResItemAndDepFn fn, void *data);
          virtual int foreachConflictingResItem (const Capability & dep, ResItemAndDepFn fn, void *data);
      
          // Channels
      
          void addChannel (Channel_Ptr channel);
          void removeChannel (Channel_constPtr channel);
      
          virtual bool containsChannel (Channel_constPtr channel) const;
      
          virtual Channel_Ptr getChannelByName (const char *channel_name) const;
          virtual Channel_Ptr getChannelByAlias (const char *alias) const;
          virtual Channel_Ptr getChannelById (const char *channel_id) const;
      
          virtual int foreachChannel (ChannelFn fn, void *data) const;
      
          // Single resItem queries
      
          virtual ResItem_constPtr findInstalledResItem (ResItem_constPtr resItem);
          virtual ResItem_constPtr findResItem (Channel_constPtr channel, const char *name) const;
          virtual ResItem_constPtr findResItemWithConstraint (Channel_constPtr channel, const char *name, const Capability & constraint, bool is_and) const;
          virtual Channel_Ptr guessResItemChannel (ResItem_constPtr resItem) const;
      
      };
      ///////////////////////////////////////////////////////////////////
    };// namespace detail
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
  };// namespace solver
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
};// namespace zypp
/////////////////////////////////////////////////////////////////////////

#endif // _StoreWorld_h
