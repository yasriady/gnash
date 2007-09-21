// dlist.cpp:  Display lists, for Gnash.
// 
//   Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "dlist.h"
#include "log.h"
#include "render.h"
#include "gnash.h" 
#include "StringPredicates.h"
#include "sprite_instance.h"

#include <typeinfo>
#include <iostream>
#include <algorithm>
#include <stack>
#include <boost/bind.hpp>

namespace gnash {

class DepthEquals {
public:
	int _depth;

	DepthEquals(int depth)
		:
		_depth(depth)
	{}

	bool operator() (const DisplayItem& item) {
		if ( ! item.get() ) return false;
		return item->get_depth() == _depth;
	}
};

class DepthGreaterOrEqual {
public:
	int _depth;

	DepthGreaterOrEqual(int depth)
		:
		_depth(depth)
	{}

	bool operator() (const DisplayItem& item) {
		if ( ! item.get() ) return false;
		return item->get_depth() >= _depth;
	}
};

class NameEquals {
public:
	const std::string& _name;

	NameEquals(const std::string& name)
		:
		_name(name)
	{}

	bool operator() (const DisplayItem& item) {
		if ( ! item.get() ) return false;
		return item->get_name() == _name;
	}
};

class NameEqualsNoCase {

	StringNoCaseEqual noCaseEquals;

public:
	const std::string& _name;

	NameEqualsNoCase(const std::string& name)
		:
		_name(name)
	{}

	bool operator() (const DisplayItem& item)
	{
		if ( ! item.get() ) return false;
		return noCaseEquals(item->get_name(), _name);
	}
};

int
DisplayList::getNextHighestDepth() const
{
	testInvariant();

	int nexthighestdepth=0;
	for (const_iterator it = _charsByDepth.begin(),
			itEnd = _charsByDepth.end();
		it != itEnd; ++it)
	{
		character* ch = it->get();
		assert(ch); // is this really needed ?

		int chdepth = ch->get_depth();
		if ( chdepth >= nexthighestdepth )
		{
			nexthighestdepth = chdepth+1;
		}
	}
	return nexthighestdepth;
}

character*
DisplayList::get_character_at_depth(int depth)
{
	testInvariant();

	//GNASH_REPORT_FUNCTION;
	//dump();

	for (iterator it = _charsByDepth.begin(),
			itEnd = _charsByDepth.end();
		it != itEnd; ++it)
	{
		character* ch = it->get();
		assert(ch); // is this really needed ?

		// found
		if ( ch->get_depth() == depth ) return ch;

		// non-existent (chars are ordered by depth)
		if ( ch->get_depth() > depth ) return NULL;
	}

	return NULL;

}


character*
DisplayList::get_character_by_name(const std::string& name)
{
	testInvariant();

	container_type::iterator it = find_if(
			_charsByDepth.begin(),
			_charsByDepth.end(),
			NameEquals(name));

	if ( it == _charsByDepth.end() ) return NULL;
	else return it->get();

}

character*
DisplayList::get_character_by_name_i(const std::string& name)
{
	testInvariant();

	container_type::iterator it = find_if(
			_charsByDepth.begin(),
			_charsByDepth.end(),
			NameEqualsNoCase(name));

	if ( it == _charsByDepth.end() ) return NULL;
	else return it->get();
}


void
DisplayList::place_character(
	character* ch, 
	int depth,
	const cxform& color_xform, 
	const matrix& mat, 
	int ratio,
	int clip_depth)
{
//	GNASH_REPORT_FUNCTION;
	//log_msg(_("dl::add(%d, '%s')"), depth, ch->get_name());

	//log_msg(_("Before adding, list is:"));
	//dump();

	assert(ch);
	assert(!ch->isUnloaded());
	ch->set_invalidated();
	ch->set_depth(depth);
	ch->set_cxform(color_xform);
	ch->set_matrix(mat);
	ch->set_ratio(ratio);
	ch->set_clip_depth(clip_depth);

	container_type::iterator it = find_if(
			_charsByDepth.begin(), _charsByDepth.end(),
			DepthGreaterOrEqual(depth));

	if ( it == _charsByDepth.end() || (*it)->get_depth() != depth )
	{
		//log_msg(_("place_character: new character at depth %d"), depth);
		
		// add the new char
		_charsByDepth.insert(it, DisplayItem(ch));
	}
	else
	{
		//log_msg(_("place_character: replacing existing character at depth %d"), depth);
		
		// remember bounds of old char
		InvalidatedRanges old_ranges;	
		(*it)->add_invalidated_bounds(old_ranges, true);	

		// make a copy (before replacing)
		boost::intrusive_ptr<character> oldCh = *it;

		// replace existing char (before calling unload!)
		*it = DisplayItem(ch);
	
		if ( oldCh->unload() )
		{
			// reinsert removed character if needed
			reinsertRemovedCharacter(oldCh);
		}
		
		// extend invalidated bounds
		ch->extend_invalidated_bounds(old_ranges); 				
	}

	// Give life to this instance
	ch->stagePlacementCallback();

	testInvariant();
}

void
DisplayList::add(character* ch, bool replace)
{
	int depth = ch->get_depth();

	container_type::iterator it = find_if(
			_charsByDepth.begin(), _charsByDepth.end(),
			DepthGreaterOrEqual(depth));
	if ( it == _charsByDepth.end() || (*it)->get_depth() != depth )
	{
		_charsByDepth.insert(it, DisplayItem(ch));
	}
	else if ( replace )
	{
		*it = DisplayItem(ch);
	}

	testInvariant();
}

void
DisplayList::addAll(std::vector<character*>& chars, bool replace)
{
	testInvariant();

	for (std::vector<character*>::iterator it=chars.begin(),
			itEnd=chars.end();
			it != itEnd; ++it)
	{
		add(*it, replace);
	}

	testInvariant();
}

void
DisplayList::replace_character(
	character* ch,
	int depth,
	const cxform* color_xform,
	const matrix* mat,
	int ratio,
	int clip_depth)
{
	testInvariant();

	//GNASH_REPORT_FUNCTION;
	assert(ch);
	assert(!ch->isUnloaded());

	ch->set_invalidated();
	ch->set_depth(depth);
	if ( color_xform ) ch->set_cxform(*color_xform);
	if ( mat ) ch->set_matrix(*mat);
	if(ratio != character::noRatioValue)
	{
		ch->set_ratio(ratio);
	}
	ch->set_clip_depth(clip_depth);

	// NOTE: currently, ::restart also cleans up all property, which include __proto__ !!
	//       For this reason I commented it out. Since no tests in the testsuite are failing
	//       I'm not sure what does this break. Udo: do you remember ? --strk;
	// ch->restart();

	container_type::iterator it = find_if(
			_charsByDepth.begin(), _charsByDepth.end(),
			DepthGreaterOrEqual(depth));

	DisplayItem di(ch);

	if ( it == _charsByDepth.end() || (*it)->get_depth() != depth )
	{

		// Error, no existing object found at depth.
//		IF_VERBOSE_DEBUG(
//			log_msg(_("dl::replace_display_object()"
//				" no obj at depth %d"), depth)
//		);

		// add the new char
		_charsByDepth.insert(it, di);

	}
	else
	{
		// Make a copy (before replacing)
		boost::intrusive_ptr<character> oldch = *it;

		InvalidatedRanges old_ranges;
	
		if (!color_xform)
		{
			// Use the cxform from the old character.
			ch->set_cxform(oldch->get_cxform());
		}

		if (!mat)
		{
			// Use the matrix from the old character.
			ch->set_matrix(oldch->get_matrix());
		}
		
		// remember bounds of old char
		oldch->add_invalidated_bounds(old_ranges, true);		

		// replace existing char (before calling unload)
		*it = di;

		// Unload old char
		if ( oldch->unload() )
		{
			// reinsert removed character if needed
			reinsertRemovedCharacter(oldch);
		}
		
		// extend invalidated bounds
		// WARNING: when a new Button character is added,
		//          the invalidated bounds computation will likely
		//          be bogus, as the actual character shown is not instantiated
		//          until ::stagePlacementCallback for buttons (I'd say this is a bug in button_character_instance)
		//          UdoG, following ? 
		//
		ch->extend_invalidated_bounds(old_ranges);				

	}

	// Give life to this instance
	ch->stagePlacementCallback();

	testInvariant();
}
	
	
// Updates the transform properties of the object at
// the specified depth.
void
DisplayList::move_display_object(
	int depth,
	const cxform* color_xform,
	const matrix* mat,
	int ratio,
	int /* clip_depth */)
{
	testInvariant();

	//GNASH_REPORT_FUNCTION;
	//IF_VERBOSE_DEBUG(log_msg(_("dl::move(%d)"), depth));

	character* ch = get_character_at_depth(depth);
	if ( ! ch )
	{
		// FIXME, should this be log_aserror?
		IF_VERBOSE_MALFORMED_SWF(
		log_swferror(_("move_display_object() -- "
			"can't find object at depth %d"),
			depth);
		);
		return;
	}

	if ( ch->isUnloaded() )
	{
		log_error("Request to move an unloaded character");
		assert(!ch->isUnloaded());
	}

	// TODO: is sign of depth related to accepting anim moves ?
	if (ch->get_accept_anim_moves() == false)
	{
		// This character is rejecting anim moves.  This happens after it
		// has been manipulated by ActionScript.
		return;
	}

	if (color_xform)
	{
		ch->set_cxform(*color_xform);
	}
	if (mat)
	{
		ch->set_matrix(*mat);
	}
	if(ratio != character::noRatioValue)
	{
		ch->set_ratio(ratio);
	}

	testInvariant();
}
	
	
// Removes the object at the specified depth.
void
DisplayList::remove_display_object(int depth)
{
	//GNASH_REPORT_FUNCTION;

	testInvariant();

	//log_msg(_("Before removing, list is:"));
	//dump();

#ifndef NDEBUG
	container_type::size_type size = _charsByDepth.size();
#endif

	// TODO: would it be legal to call remove_display_object with a depth
	//       in the "removed" zone ?
	// TODO: optimize to take by-depth order into account
	container_type::iterator it = find_if(
			_charsByDepth.begin(),
			_charsByDepth.end(),
			DepthEquals(depth));

	if ( it != _charsByDepth.end() )
	{
		// Make a copy (before erasing)
		boost::intrusive_ptr<character> oldCh = *it;

		// Erase (before callign unload)
		_charsByDepth.erase(it);

		if ( oldCh->unload() )
		{
			// reinsert removed character if needed
			// NOTE: could be optimized if we knew exactly how
			//       to handle the case in which the target depth
			//       (after the shift) is occupied already
			//
			reinsertRemovedCharacter(oldCh);
		}
	}

#ifndef NDEBUG
	assert(size >= _charsByDepth.size());
#endif

	testInvariant();

	//log_msg(_("Done removing, list is:"));
	//dump();
}

// TODO: take character by ref ?
void
DisplayList::swapDepths(character* ch1, int newdepth)
{
	testInvariant();

	if ( newdepth < character::staticDepthOffset )
	{
		IF_VERBOSE_ASCODING_ERRORS(
		log_aserror("%s.swapDepth(%d) : ignored call with target depth less then %d",
			ch1->getTarget().c_str(), newdepth, character::staticDepthOffset);
		);
		return;
	}

	int srcdepth = ch1->get_depth();

	// what if source char is at a lower depth ?
	assert(srcdepth >= character::staticDepthOffset);

	assert(srcdepth != newdepth);

	// TODO: optimize this scan by taking ch1 depth into account ?
	container_type::iterator it1 = find(_charsByDepth.begin(), _charsByDepth.end(), ch1);

	// upper bound ...
	container_type::iterator it2 = find_if(_charsByDepth.begin(), _charsByDepth.end(),
			DepthGreaterOrEqual(newdepth));

	if ( it1 == _charsByDepth.end() )
	{
		log_error("First argument to DisplayList::swapDepth() is NOT a character in the list. Call ignored.");
		return;
	}

	// Found another character at the given depth
	if ( it2 != _charsByDepth.end() && (*it2)->get_depth() == newdepth )
	{
		DisplayItem ch2 = *it2;

		ch2->set_depth(srcdepth);

		// TODO: we're not actually invalidated ourselves, rather our parent is...
		//       UdoG ? Want to verify this ?
		ch2->set_invalidated();

		// We won't accept static transforms after a depth swap.
		// See displaylist_depths_test6.swf for more info.
		ch2->transformedByScript();

		iter_swap(it1, it2);
	}

	// No character found at the given depth
	else
	{
		// Move the character to the new position
		// NOTE: insert *before* erasing, in case the list is
		//       the only referer of the ref-counted character
		_charsByDepth.insert(it2, ch1);
		_charsByDepth.erase(it1);
	}

	// don't change depth before the iter_swap case above, as
	// we'll need it to assign to the new character
	ch1->set_depth(newdepth);

	// TODO: we're not actually invalidated ourselves, rather our parent is...
	//       UdoG ? Want to verify this ?
	ch1->set_invalidated();

	// We won't accept static transforms after a depth swap.
	// See displaylist_depths_test6.swf for more info.
	ch1->transformedByScript();

	testInvariant();

}
	
void DisplayList::reset(movie_definition& movieDef, size_t tgtFrame, sprite_instance& owner)
{
	testInvariant();

	//GNASH_REPORT_FUNCTION;

	// 1. Find all "timeline depth" for the target frame, querying the
	//    Timeline object in the sprite/movie definition (see implementation details)
	std::vector<int>  save;
	movieDef.getTimelineDepths(tgtFrame, save);

//#define GNASH_DEBUG_TIMELINE 1
#undef GNASH_DEBUG_TIMELINE
#ifdef GNASH_DEBUG_TIMELINE
	cout << "Depths found to save: " << endl;
	std::ostream_iterator<int> ostrIter(cout, "," ) ;
	std::copy(save.begin(), save.end(), ostrIter);
	cout << endl;
	cout << "Current DisplayList: " << *this << endl;
#endif

	typedef std::vector<int>::iterator SeekIter;

	SeekIter startSeek = save.begin();
	SeekIter endSeek = save.end();

	std::vector<DisplayItem> toReinsert;

	iterator it = beginNonRemoved(_charsByDepth);
	for (iterator itEnd = _charsByDepth.end(); it != itEnd; )
	{
		testInvariant();

		// Make a copy here, in case we're going to replace it
		DisplayItem di = *it;

		// Character already unloaded, leave it where it is
		if ( di->isUnloaded() )
		{
			++it;
			continue;
		}

		int di_depth = di->get_depth();

		/// We won't scan chars in the dynamic depth zone
		if ( di_depth >= 0 ) break;

		/// Always remove non-timeline instances ?
		/// Seems so, at least for duplicateMovieClip
		TimelineInfo* info = di->getTimelineInfo();
		//if ( di->getTimelineInfo() == NULL )
		//if ( di->isDynamic() )
		if ( ! info )
		{
			// Call set_invalidated before changing the DisplayList
			owner.set_invalidated();

			// Replace (before calling unload)
			it = _charsByDepth.erase(it);

			if ( di->unload() )
			{
				toReinsert.push_back(di);
			}

			continue;
		}
		
		// remove all shapes and morphs, it should be safe and correct.
		// but suboptimal when considering the invalidated bound.
		// we need to do this in some corner cases. 
		if(!di->isActionScriptReferenceable())
		{
			// Call set_invalidated before changing the DisplayList
			owner.set_invalidated();

			// TODO: no unload() call needed here ? would help GC ?
			// (I guess there can't be any as_value pointing at this
			// if it's not ActionScriptReferenceable after all...)
			//
			it = _charsByDepth.erase(it);
			continue;
		}

		/// Only remove if not in the save vector, 
		SeekIter match = std::find(startSeek, endSeek, di_depth);
		if( match == save.end())
		{
			// Not to be saved, killing

			// Call set_invalidated before changing the DisplayList
			owner.set_invalidated();

			// Replace (before calling unload)
			it = _charsByDepth.erase(it);

			if ( di->unload() )
			{
				toReinsert.push_back(di);
			}

			continue;
		}

		// To be saved, don't kill

		// IFF the 'save' vector is known to be sorted
		// we can let next seek start to next seek item
		// We can't assume this here, unless we request
		// by caller, in the dox.
		//startSeek = ++match;

		++it;
	}

	std::for_each(toReinsert.begin(), toReinsert.end(),
		boost::bind(&DisplayList::reinsertRemovedCharacter, this, _1));

	testInvariant();
}

void
DisplayList::clear_except(const DisplayList& exclude, bool call_unload)
{
	//log_debug("clear_except(DislpayList, %d) called", call_unload);
	//GNASH_REPORT_FUNCTION;

	testInvariant();
	//log_debug("First invariant test worked");


	assert(&exclude != this);
	const container_type& keepchars = exclude._charsByDepth;

	std::vector<DisplayItem> toReinsert;

	const_iterator keepStart = beginNonRemoved(keepchars);
	const_iterator keepEnd = keepchars.end();

	//int called=0;
	for (iterator it = _charsByDepth.begin(),	itEnd = _charsByDepth.end(); it != itEnd; )
	{
		
		testInvariant(); // TODO: expensive
		//log_debug("Invariant test in iteration %d worked", called++);

		// make a copy of the pointer here, don't take a reference
		DisplayItem di = *it;

		bool is_affected = false;
		for (const_iterator kit = keepStart; kit != keepEnd; ++kit)
		{
			if ( *kit == di )
			{
				is_affected = true;
				break;
			}
		}

		if (is_affected == false)
		{
			it = _charsByDepth.erase(it);

			if ( call_unload )
			{
				if ( di->unload() )
				{
					toReinsert.push_back(di);
				}
			}

			continue;
		}
		it++;
	}

	testInvariant();
	//log_debug("Invariant test after cleanup worked");

	std::for_each(toReinsert.begin(), toReinsert.end(),
		boost::bind(&DisplayList::reinsertRemovedCharacter, this, _1));

	testInvariant();
	//log_debug("Invariant test after reinsertion worked");
}

void
DisplayList::clear(const DisplayList& from, bool call_unload)
{
	//GNASH_REPORT_FUNCTION;

	testInvariant();

	const container_type dropchars = from._charsByDepth;

	std::vector<DisplayItem> toReinsert;

	for (iterator it = _charsByDepth.begin(), itEnd = _charsByDepth.end(); it != itEnd; )
	{
		// make a copy
		DisplayItem di = *it;

		bool is_affected = false;
		for (const_iterator kit = dropchars.begin(), kitEnd = dropchars.end(); kit != kitEnd; ++kit)
		{
			if ( *kit == di )
			{
				is_affected = true;
				break;
			}
		}

		if (is_affected)
		{
			it = _charsByDepth.erase(it);

			if ( call_unload )
			{
				if ( di->unload() )
				{
					toReinsert.push_back(di);
				}
			}
			continue;
		}
		it++;
	}

	testInvariant();

	std::for_each(toReinsert.begin(), toReinsert.end(),
		boost::bind(&DisplayList::reinsertRemovedCharacter, this, _1));

	testInvariant();
}

bool
DisplayList::unload()
{
	//GNASH_REPORT_FUNCTION;

	testInvariant();

	// Should we start looking from beginNonRemoved ?
	// If I try, I get a failure in swfdec/gotoframe.swf
	for (iterator it = _charsByDepth.begin(), itEnd = _charsByDepth.end(); it != itEnd; )
	{
		// make a copy
		DisplayItem di = *it;

		// skip if already unloaded
		if ( di->isUnloaded() )
		{
			++it;
			continue;
		}

		if ( ! di->unload() ) // no event handler queued, we remove
		{
			it = _charsByDepth.erase(it);
		}
		else
		{
			++it;
		}
	}

	testInvariant();

	return ! _charsByDepth.empty();

}
	
// Display the referenced characters. Lower depths
// are obscured by higher depths.
void
DisplayList::display()
{
	testInvariant();

    //GNASH_REPORT_FUNCTION;
    std::stack<int> clipDepthStack;
    
    // We only advance characters which are out of the "removed" zone (or should we check isUnloaded?)
    iterator it = beginNonRemoved(_charsByDepth);
    //iterator it = _charsByDepth.begin();
    for(iterator endIt = _charsByDepth.end(); it != endIt; ++it)
    {
        character* ch = it->get();
        assert(ch);

	if ( ch->isUnloaded() ) // debugging
	{
		log_error("character at depth %d is unloaded", ch->get_depth());
		abort();
	}
	assert(! ch->isUnloaded() ); // we don't advance unloaded chars


        // Check if this charater or any of its parents is a mask.
        // Characters act as masks should always be rendered to the
        // mask buffer despite their visibility.
        character * parent = ch->get_parent();
        bool renderAsMask = ch->isMask();
        while(!renderAsMask && parent)
        {
            renderAsMask = parent->isMask();
            parent = parent->get_parent();
        }
        
        // check for non-mask hiden characters
        if( !renderAsMask && (ch->get_visible() == false))
        {
            // Avoid stale old_invalidated_rect
            ch->clear_invalidated(); 
            // Don't display non-mask hiden characters
            continue;
        }
    
        int depth = ch->get_depth();
        // Discard useless masks
        while(!clipDepthStack.empty() && (depth > clipDepthStack.top()))
        {
            clipDepthStack.pop();
            render::disable_mask();
        }

        int clipDepth = ch->get_clip_depth();
        // Push a new mask to the masks stack
        if(clipDepth != character::noClipDepthValue)
        {
            clipDepthStack.push(clipDepth);
            render::begin_submit_mask();
        }
        
        ch->display();
        
        // Notify the renderer that mask drawing has finished.
        if (ch->isMask())
        {
            render::end_submit_mask();
        }
    } //end of for

    // Discard any remaining masks
    while(!clipDepthStack.empty())
    {
        clipDepthStack.pop();
        render::disable_mask();
    }
}

/*public*/
void
DisplayList::dump() const
{
	//testInvariant();

	int num=0;
	for( const_iterator it = _charsByDepth.begin(),
			endIt = _charsByDepth.end();
		it != endIt; ++it)
	{
		const DisplayItem& dobj = *it;
		log_msg(_("Item %d at depth %d (char id %d, name %s, type %s)"),
			num, dobj->get_depth(), dobj->get_id(),
			dobj->get_name().c_str(), typeName(*dobj).c_str());
		num++;
	}
}


void 
DisplayList::add_invalidated_bounds(InvalidatedRanges& ranges, bool force)
{
    
	testInvariant();

	iterator it = beginNonRemoved(_charsByDepth);
	for( iterator endIt = _charsByDepth.end(); it != endIt; ++it)
	{
		DisplayItem& dobj = *it;
#ifndef GNASH_USE_GC
		assert(dobj->get_ref_count() > 0);
#endif // ndef GNASH_USE_GC
		dobj->add_invalidated_bounds(ranges, force);
	}
	
}


/// This method is not in the header in the hope DisplayItemDepthLess
/// will be inlined by compiler

struct DisplayItemDepthLess {
	bool operator() (const DisplayItem& d1, const DisplayItem& d2)
	{
		return d1->get_depth() < d2->get_depth();
	}
};

void
DisplayList::sort()
{
	_charsByDepth.sort(DisplayItemDepthLess());
}

std::ostream& operator<< (std::ostream& os, const DisplayList& dl)
{
	os << "By depth: ";
	for (DisplayList::const_iterator it = dl._charsByDepth.begin(),
			itEnd = dl._charsByDepth.end();
			it != itEnd;
			++it)
	{
		const DisplayItem& item = *it; 
		if ( it != dl._charsByDepth.begin() ) os << " | ";
		os << "ch id:" << item->get_id()
			<< " name:" << item->get_name()
			<< " depth:" << item->get_depth();
	}

	return os;
}

void
DisplayList::reinsertRemovedCharacter(boost::intrusive_ptr<character> ch)
{
	assert(ch->isUnloaded());

	// TODO: have this done by character::unload() instead ?
	int oldDepth = ch->get_depth();
	int newDepth = character::removedDepthOffset - oldDepth;
	ch->set_depth(newDepth);

	testInvariant();

	// TODO: optimize this by searching from the end(lowest depth).
	container_type::iterator it = find_if(
			_charsByDepth.begin(), _charsByDepth.end(),
			DepthGreaterOrEqual(newDepth));
	if ( it == _charsByDepth.end() || (*it)->get_depth() != newDepth )
	{
		// add the new char
		_charsByDepth.insert(it, DisplayItem(ch));
	}
	else
	{
		// the character should not be in the displaylist already !
		assert(it->get() != ch.get());

		log_error("DisplayList::insertCharacter: target depth (%d) is occupied, and we don't know what we're supposed to do - we'll avoid inserting the character for now", newDepth);
	}

	testInvariant();
}

/*private static*/
DisplayList::iterator
DisplayList::beginNonRemoved(container_type& c)
{
	return std::find_if(c.begin(), c.end(),
			DepthGreaterOrEqual(character::removedDepthOffset - character::staticDepthOffset));
}

/*private static*/
DisplayList::const_iterator
DisplayList::beginNonRemoved(const container_type& c)
{
	return std::find_if(c.begin(), c.end(), DepthGreaterOrEqual(character::removedDepthOffset+1));
}

void
DisplayList::removeUnloaded()
{
	testInvariant();

	// TODO: erase from begin() to beginNonRemoved()-1 ?
	iterator last = std::remove_if(_charsByDepth.begin(), _charsByDepth.end(), boost::bind(&character::isUnloaded, _1));
	_charsByDepth.erase(last, _charsByDepth.end());

	testInvariant();
}

} // namespace gnash


// Local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
