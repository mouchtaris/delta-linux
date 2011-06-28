/*
 * RegExpr.h
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef REGEXPR_H
#define REGEXPR_H

#include <map>
#include <string>

#include "TextSource.h"


namespace frep {

	class RegExprTreeVisitor;	//forward declaration

	//-----------------------------------------------------------------------
	class RegExpr {
	public:
		int			id;		//We use in PostOrderOptimization ever > 0
		bool		marked;	//We use in PostOrderOptimization
		RegExpr *	parent;


		virtual void Accept (RegExprTreeVisitor* ) = 0;
		virtual const std::string UF (void)	const = 0; //Unparsed Form
		virtual const std::string Lexeme (void) = 0;

		RegExpr() :	id(0), marked(false), parent((RegExpr *)0) {} 
		virtual ~RegExpr(void) {}
	};



	//-----------------------------------------------------------------------
	class WhiteSpaceExpr : public RegExpr { // Logic hard-coding.
	private:
		std::string l;

	public:
		virtual void Accept (RegExprTreeVisitor* visitor);
		
		virtual const std::string UF (void) const { return ":w"; }
		virtual const std::string Lexeme (void) { return l; }

		WhiteSpaceExpr(void) : RegExpr(), l() {}
	};


	//-----------------------------------------------------------------------
	class QuotedStringExpr : public RegExpr { // Logic hard-coding.
	private:
		std::string l;

	public:
		virtual void Accept (RegExprTreeVisitor* visitor);
		
		virtual const std::string UF (void) const { return ":q"; }
		virtual const std::string Lexeme (void) { return l; }

		QuotedStringExpr(void) : RegExpr(), l() {}
	};


	//-----------------------------------------------------------------------
	class IntegerExpr : public RegExpr { // Logic hard-coding.
	private:
		std::string l;

	public:
		virtual void Accept (RegExprTreeVisitor* visitor);
		
		virtual const std::string UF (void) const { return ":z"; }
		virtual const std::string Lexeme (void) { return l; }

		IntegerExpr(void) : RegExpr(), l() {}
	};


	//-----------------------------------------------------------------------
	class DeltaIdentExpr : public RegExpr { // a Delta identifier, logic hard-coding style
	private:
		std::string l;

	public:
		virtual void Accept (RegExprTreeVisitor* visitor);

		virtual const std::string UF (void) const { return ":i"; }
		virtual const std::string Lexeme (void) { return l; }

		DeltaIdentExpr(void) : RegExpr(), l() {}
	};



	//-----------------------------------------------------------------------
	class AnyCharExpr : public RegExpr { // ? macthes any character (except EOF).
	private:
		std::string l;

	public:
		virtual void Accept (RegExprTreeVisitor* visitor);

		virtual const std::string UF (void)	const { return "."; }
		virtual const std::string Lexeme (void) { return l; }

		AnyCharExpr(void)  : RegExpr(), l() {}
	};



	//-----------------------------------------------------------------------
	class StringExpr : public RegExpr {
	private:
		static bool (*MatchPolicy)(char, char);

	public:
		std::string text;		// to  Match against
		static bool wholeWord;
		static bool isCaseSensitive;
		
		
		static void Initialise (void);
		static bool MatchCaseSensitive (char c1, char c2);
		static bool MatchCaseInsensitive (char c1, char c2) ;

		static void SetSensitivity (bool v) {
			if (isCaseSensitive = v)
				MatchPolicy = &MatchCaseSensitive;
			else
				MatchPolicy = &MatchCaseInsensitive;
		}

		
		virtual bool Match (TextSource& s);
		virtual void Accept (RegExprTreeVisitor* visitor);
		
		virtual const std::string UF (void) const { return text; }
		virtual const std::string Lexeme (void) { return text; }

		StringExpr(void) : RegExpr() {}

		StringExpr(std::string _text) : RegExpr(), text(_text){}

	};



	//-----------------------------------------------------------------------
	class OrExpr : public RegExpr {
	private:
		std::string l;

	public:	
		RegExpr* e1;
		RegExpr* e2;

		virtual void Accept (RegExprTreeVisitor* visitor);
		
		
		virtual const std::string Lexeme (void) { return l; }
		virtual const std::string UF(void) const { 
			return "<" + e1->UF() + "|" + e2->UF() + ">";
		}

		OrExpr(void) : RegExpr(), e1((RegExpr*)0), e2((RegExpr*)0), l() {}
	};



	//-----------------------------------------------------------------------
	class ConcatExpr : public RegExpr {
	private:
		std::string l;

	public:	
		RegExpr* e1;
		RegExpr* e2;
		
		virtual void Accept (RegExprTreeVisitor* visitor);
		
		
		virtual const std::string Lexeme (void) { 
			return l.append(e1->Lexeme()).append(e2->Lexeme()); 
		}
		virtual const std::string UF(void) const { 
			return "Concat(" + e1->UF() + "," + e2->UF() + ")";
		}

		ConcatExpr(void) : RegExpr(), e1((RegExpr*)0), e2((RegExpr*)0), l() {}
	};



	//-----------------------------------------------------------------------
	class TagExpr : public RegExpr {
	private:
		unsigned tagSerial;	//the first, second, ... tag that we find 
		std::string l;

	public:	
		RegExpr* e;
		static std::map<unsigned ,std::string> allTags;	//Add all tags, key == tag num
		static void Initialise (void) {allTags.clear();}
		
		virtual void Accept (RegExprTreeVisitor* visitor);


		virtual const std::string UF (void) const { return "{" + e->UF() + "}"; }
		virtual const std::string Lexeme (void)	{ return l; }

		void		SetTagSerialNumber(unsigned num) { tagSerial = num;}
		unsigned	GetTagSerialNumber(void) const { return tagSerial; }

		TagExpr() : RegExpr(), tagSerial(0), e((RegExpr*)0) {}
		TagExpr(unsigned num) : RegExpr(), tagSerial(num), e((RegExpr*)0) {}
	};




	//-----------------------------------------------------------------------
	class ZeroOrMoreExpr : public RegExpr {
	private:
		std::string l;

	public:
		RegExpr* e;

		virtual void Accept (RegExprTreeVisitor* visitor);

		virtual const std::string UF (void)	const { return "(" + e->UF() + ")*"; }
		virtual const std::string Lexeme (void)	{ return l; }

		ZeroOrMoreExpr(void) : RegExpr(), e((RegExpr*)0), l() {}
	};

	

	//-----------------------------------------------------------------------
	class OneOrMoreExpr : public RegExpr {
	private:
		std::string l;

	public:
		RegExpr* e;

		virtual void Accept (RegExprTreeVisitor* visitor);

		virtual const std::string UF (void)	const { return "(" + e->UF() + ")+"; }
		virtual const std::string Lexeme (void)	{ return l; }

		OneOrMoreExpr(void) : RegExpr(), e((RegExpr*)0), l() {}
	};


	//-----------------------------------------------------------------------
	class EpsilonExpr : public RegExpr {
	public:
		virtual void Accept (RegExprTreeVisitor* visitor);

		virtual const std::string UF (void) const { return ""; }
		virtual const std::string Lexeme (void) { return ""; }

		EpsilonExpr(void) : RegExpr() {}
	};



	//-----------------------------------------------------------------------
	class NumberExpr: public RegExpr {
	private:
		std::string l;

	public:	
		unsigned num;

		virtual void Accept (RegExprTreeVisitor* visitor){}

		virtual const std::string UF (void) const { return "num"; }
		virtual const std::string Lexeme (void) { 
			return l.append(TagExpr::allTags[num]); 
		}

		NumberExpr(void) : RegExpr(), l(){}
	};



	//-----------------------------------------------------------------------
	class ReplaceStringExpr: public RegExpr {
	public:	
		std::string text;
		
		virtual void Accept (RegExprTreeVisitor* visitor){}

		virtual const std::string UF (void)	const { return ""; }
		virtual const std::string Lexeme (void)	{ return text; }

		ReplaceStringExpr(void) : RegExpr() {}
	};

} //namespace frep
#endif