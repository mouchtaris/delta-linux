/*
 * TextSourceCollectionProducer.h
 * Abstract Class
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef TEXTSOURCECOLLECTIONPRODUCER_H
#define TEXTSOURCECOLLECTIONPRODUCER_H

#include <list>

#include "TextSource.h"

namespace frep {

	enum LookIn{
		CURRENT_DOCUMENT	= 0,
		SELECTION			,
		ALL_OPEN_DOCUMENTS	,
		CURRENT_PROJECT		,
		ENTIRE_WORKSPACE	,
		NOWHERE				,
		ENTIRE_GRAPH		,
		LAST_VISITED_VERTEX ,
		
	};


	class TextSourceCollectionProducer {

	public:
		virtual std::list<TextSource *> & GetTextSourceList (
											const LookIn lookIn, 
											const bool findInScripts
										) = 0;
		
		virtual ~TextSourceCollectionProducer(void){}
	};
} //namespace frep
#endif //TEXTSOURCECOLLECTIONPRODUCER_H