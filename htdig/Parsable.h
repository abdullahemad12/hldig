//
// Parsable.h
//
// $Id: Parsable.h,v 1.4 1999/03/16 02:04:27 hp Exp $
//

#ifndef _Parsable_h_
#define _Parsable_h_

#include "htString.h"
#include "Retriever.h"

class URL;


class Parsable
{
public:
    //
    // Construction/Destruction
    //
                        Parsable();
    virtual		~Parsable();

    //
    // Main parser interface.
    //
    virtual void	parse(Retriever &retriever, URL &) = 0;

    //
    // The rest of the members are used by the Document to provide us
    // the data that we contain.
    //
    virtual void	setContents(char *data, int length);
	
protected:
    String		*contents;
    int			max_head_length;
    int			max_description_length;
    int			max_meta_description_length;
};

#endif


