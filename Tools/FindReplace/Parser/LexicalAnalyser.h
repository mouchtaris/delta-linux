/*
 * LexicalAnalyser.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef LEXICAL_ANALYSER_H
#define LEXICAL_ANALYSER_H

#include <stdio.h>

#include <list>
#include <string>
#include <iostream>
#include <functional>



namespace frep {

	//If you add new tokes, you must add these tokes in: LexicalAnalyser::Token::GetTokenClasses
	enum TokenType {
			TokenSTART_SYMBOL		= 0,
			TokenASTERICK,
			TokenLEFT_BRACKET,
			TokenRIGHT_BRACKET,
			TokenLEFT_CURLY_BRACE,
			TokenRIGHT_CURLY_BRACE,
			TokenLESS_THAN,
			TokenGREATER_THAN,
			TokenLEFT_PARENTHESIS,
			TokenRIGHT_PARENTHESIS,
			TokenVERTICAL_BAR,
			TokenBACKSLASH,
			TokenPLUS,
			TokenNUM,
			TokenREGEX_STRING,
			TokenDELTA_ID,
			TokenANY_CHARACTER,
			TokenWHITE_SPACE,
			TokenQUOTED_STRING,
			TokenINTEGER,
			TokenEOF,
			TokenUNDEFINED_SYMBOL
		};

	//-----------------------------------------------------------------------
	
	union TokenValue {
		unsigned int intVal;
		char*		 strVal;
	};


	//-----------------------------------------------------------------------
	
	class LexicalAnalyser {
	public:
		struct Token {
			TokenType type;
			TokenValue value;
			
			struct ClearFunctor : public std::unary_function<void, Token> {
				void operator()(const Token& token) const {
					if ( (token.type == TokenREGEX_STRING) && token.value.strVal) {
						delete[] token.value.strVal;
						const_cast<Token&>(token).value.strVal =(char*) 0;
					}
				}
			};//end of ClearFunctor

			void Write(FILE *fp);
			
			void operator=(const Token& token)		{ new (this) Token(token); }
			bool operator==(const Token& token)		{ return this->type == token.type; }
			bool operator==(const TokenType& type)	{ return this->type == type; }


			static const char* GetTokenClasses(const TokenType& type);

			//constructores
			Token (const Token& token) { memcpy(this, &token, sizeof(Token)); }
			Token (void){}
			virtual ~Token(void){}
		}; //End of struct Token

		typedef std::list<Token> TokenList;

		virtual char *		GetError(void) = 0;
		virtual bool		HasError(void) = 0;
		virtual bool		Analyse (std::string & str) = 0;
		virtual TokenList*	GetTokens (void) const = 0;
		virtual void		DumpTokens (FILE *fp) = 0;
		virtual void		Clear(void) = 0;

		virtual ~LexicalAnalyser(void){}
	};//end of class LexicalAnalyser

} //namespace frep {


#endif