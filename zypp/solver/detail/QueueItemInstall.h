/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* QueueItemInstall.h
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

#ifndef _QueueItemInstall_h
#define _QueueItemInstall_h

#include <iosfwd>
#include <list>
#include <string>

#include <zypp/solver/detail/QueueItem.h>
#include <zypp/solver/detail/QueueItemInstallPtr.h>
#include <zypp/solver/detail/ResItem.h>
#include <zypp/solver/detail/Channel.h>
#include <zypp/CapSet.h>

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
      //	CLASS NAME : QueueItemInstall

      class QueueItemInstall : public QueueItem {


        private:
          ResItem_constPtr _resItem;
          ResItem_constPtr _upgrades;
          CapSet _deps_satisfied_by_this_install;
          CResItemList _needed_by;
          int _channel_priority;
          int _other_penalty;

          bool _explicitly_requested;

        public:

          QueueItemInstall (World_Ptr world, ResItem_constPtr resItem);
          virtual ~QueueItemInstall();

          // ---------------------------------- I/O

          static std::string toString (const QueueItemInstall & item);

          virtual std::ostream & dumpOn(std::ostream & str ) const;

          friend std::ostream& operator<<(std::ostream&, const QueueItemInstall & item);

          std::string asString (void ) const;

          // ---------------------------------- accessors

          ResItem_constPtr resItem (void) const { return _resItem; }

          ResItem_constPtr upgrades (void) const { return _upgrades; }
          void setUpgrades (ResItem_constPtr upgrades) { _upgrades = upgrades; }

          int channelPriority (void) const { return _channel_priority; }
          void setChannelPriority (int channel_priority) { _channel_priority = channel_priority; }

          int otherPenalty (void) { return _other_penalty; }
          void setOtherPenalty (int other_penalty) { _other_penalty = other_penalty; }

          void setExplicitlyRequested (void) { _explicitly_requested = true; }

          // ---------------------------------- methods

          virtual bool process (ResolverContext_Ptr context, QueueItemList & qil);
          virtual QueueItem_Ptr copy (void) const;
          virtual int cmp (QueueItem_constPtr item) const;

          virtual bool isRedundant (ResolverContext_Ptr context) const { return false; }
          virtual bool isSatisfied (ResolverContext_Ptr context) const;

          void addDependency (const Capability & dep);
          void addNeededBy (const ResItem_constPtr resItem);

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

#endif // _QueueItem_h
