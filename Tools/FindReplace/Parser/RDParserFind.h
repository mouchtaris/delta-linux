/*
 * RDParserFind.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef RDPARSERFIND_H	
#define RDPARSERFIND_H

#include <list>
#include <sstream>
#include <functional>
#include "RegExpr.h"
#include "RDParser.h"


namespace frep {

	class RDParserFind  : public RDParser{
	private:
		typedef frep::TokenType TokenType;

		struct ClearFunctor : public std::unary_function<void, RegExpr *> {
			void operator()(const RegExpr * reg) const{
				if(reg) { 
					delete reg; 
					reg = (RegExpr*)0;
				}
			}
		};

		//variables
		//********************************************************
		//bool	notMatch;
		std::string tmpErrorMessage;
		unsigned tagSerial;
		std::list<RegExpr *> toDelete;

		//rules
		//********************************************************
		RegExpr * OR(void);
		RegExpr * Tag(void);
		RegExpr * Regex(void);
		RegExpr * Regexs(void);
		RegExpr * Number(void);
		RegExpr * String(void);
		RegExpr * DeltaId(void);
		RegExpr * Epsilon(void);
		RegExpr * Integer(void);
		RegExpr * OneOrMore(void);
		RegExpr * WhiteSpace(void);
		RegExpr * ZeroOrMore(void);
		RegExpr * AnyCharacter(void);
		RegExpr * QuotedString(void);

		template <class T>
		RegExpr * CreateRegExprWithSingleChild(	TokenType leftToken, 
												TokenType rightToken, 
												TokenType lastToken, 
												bool isTag) 
		{
			assert(lookAhead == leftToken);
			T * regExpr = new T();	
														
			Match(leftToken);					
			regExpr->e = Regex();				
			Match(rightToken);					
			if (!isTag)						
				Match(lastToken);				
												
			if( !HasError() )				
				regExpr->e->parent = regExpr;	

			else{								
				delete regExpr;					
				regExpr = (T *)0;	
			}									
		
			return regExpr;
		}
		
	public:
	
		RegExpr * Analyse (TokenList* _tokens);
		void Clear(void);

		void PrintDeleteElements(void);

		RDParserFind();
		~RDParserFind(void);
	};
} // namespace frep


#endif
