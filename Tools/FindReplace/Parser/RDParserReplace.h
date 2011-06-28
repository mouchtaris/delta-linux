/*
 * RDParserReplace.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef RDPARSERREPLACE_H	
#define RDPARSERREPLACE_H


#include <list>
#include <functional>
#include "RegExpr.h"
#include "RDParser.h"


namespace frep {

	class RDParserReplace : public RDParser{
	private:

		struct ClearFunctor : public std::unary_function<void, RegExpr *> {
			void operator()(const RegExpr * reg) const{
				if(reg) { delete reg; }
			}
		};
		
		//variables
		//********************************************************
		std::list<RegExpr *> toDelete;


		//rules
		//********************************************************
		RegExpr * Regex(void);
		RegExpr * Regexs(void);
		RegExpr * String(void);
		RegExpr * Number(void);
		RegExpr * BackslashNumber(void);
		RegExpr * Epsilon(void);

	public:
		

		RegExpr * Analyse (TokenList* _tokens);
		void Clear(void);


		RDParserReplace(void);
		~RDParserReplace(void);
	};

} // namespace frep


#endif
