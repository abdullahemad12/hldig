//
// SplitMatches.h
//
// SplitMatches:  Constructed from a Configuration, see doc
// for format of config item "search_results_order".
//  Used to contain a number of ResultMatches, putting them in separate
// lists depending on the URL with method Add.
//  Iterator methods Get_First and Get_Next returns the sub-lists.
// Method Joined returns a new list with all the sub-lists
// concatenated.
//
// $Id: SplitMatches.h,v 1.1.2.1 2000/03/28 02:01:18 ghutchis Exp $
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 2000 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU Public License version 2 or later
// <http://www.gnu.org/copyleft/gpl.html>
//
#ifndef _splitmatches_h
#define _splitmatches_h

#include "Configuration.h"
#include "ResultMatch.h"
#include "List.h"

class SplitMatches
{
public:
    SplitMatches(Configuration &);
    ~SplitMatches();

    void Add(ResultMatch *, char *);
    List *JoinedLists();
    List *Get_First()
    { mySubAreas->Start_Get(); return Get_Next(); }

    List *Get_Next();

private:
    // These member functions are not supposed to be implemented.
    SplitMatches();
    SplitMatches(const SplitMatches &);
    void operator= (const SplitMatches &);

    // (Lists of) Matches for each sub-area regex.
    List *mySubAreas;

    // Matches for everything else.
    List *myDefaultList;
};

#endif /* _splitmatches_h */