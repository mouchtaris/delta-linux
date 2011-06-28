// ASTEvaluationHelpers.h
// Helper classes for evaluation over an AST.
// ScriptFighter Project.
// A. Savidis, October 2009.
//
#ifndef	ASTEVALUATIONHELPERS_H
#define	ASTEVALUATIONHELPERS_H

#include "DDebug.h"
#include "uvector.h"
#include "ufunctors.h"
#include "ustlhelpers.h"
#include <string>

/////////////////////////////////////////////////////////

namespace AST {

template <typename Tval, typename Tvalisundef> class EvaluationStack {

	public:
	typedef utriple<Tval, util_ui32, const char*>	StackItem;
	typedef std::list<StackItem>					StackValues;

	/////////////////////////////////////////////////////////

	class TopRule {

		protected:
		StackValues&		yystack;
		Tval				yylval;
		util_ui32			totalValues;
		uvector<bool>		usage;
		bool				yylvalUsed;
		bool				preserve;
		util_ui32			line;	// For debug puproses.
		const char*			rule;	// For debug purposes.
	
		void				SetRuleSize (util_ui32 size) {
								DASSERT(!totalValues && size <= yystack.size()); 
								totalValues = size;
								usage.resize(totalValues + 1);
								for (util_ui32 i = 0; i <= totalValues; ++i)
									usage[i] = false;
							}

		//**************************

		public:
		struct RuleSize {
			util_ui32 size;
			const RuleSize& operator()(util_ui32 n) 
				{ size = n; return *this; }
			RuleSize (util_ui32 n = 0) : size(n){}		// (RuleSize(n)) equivalent to (RuleSize()(n))
		};

		Tval&			operator[](util_ui32 i) {
							DASSERT(i <= totalValues && yystack.size() >= totalValues);
							usage[i] = true;					// Used.
							if (!i)								// $$
								return yylval;
							else
								return	ulistgetbyindex(		// $1,$2,...
											yystack, 
											(util_ui32) (totalValues - i)
										).first;
						}

		void			Apply (util_ui32 l, const char* r) {
							DASSERT(!yylvalUsed);
							yylvalUsed = true;
							line = l;
							rule = r;
						}

		void			Preserve (void) 
							{ DASSERT(!preserve); preserve = true; }

		void			operator=(const RuleSize& rs)				// Setting size by assigning from RuleSize.
							{ SetRuleSize(rs.size); }

		TopRule (StackValues& values) :
			yystack(values),
			totalValues(0),
			yylvalUsed(false),
			preserve(false),
			line(0) {}

		~TopRule() {
			if (yylvalUsed)
				DASSERT(usage[0]);									// Left symbol must have been accessed.
			bool someUsed = !totalValues;
			for (util_ui32 i = 1; i <= totalValues; ++i)
				someUsed = someUsed || usage[i];
			DASSERT(someUsed);										// Some right symbols must have been accessed.

			if (!preserve)
				while (totalValues--)								// Pop rule grammar symbols (right part).
					yystack.pop_front();

			if (yylvalUsed) {
				DASSERT(!Tvalisundef()(yylval));
				yystack.push_front(StackItem(yylval, line, rule));	// Push rule value (left part).
			}
		}
	};

	/////////////////////////////////////////////////////////

	private:
	friend class TopRule;
	StackValues		values;
	util_ui32		line;
	const char*		rule;

	public:

	StackValues&		GetValues (void)				{ return values; }
	Tval&				Top (void)						{ DASSERT(!values.empty()); return values.front().first; }
	void				Push (const Tval& val)			{ values.push_front(StackItem(val, line, rule)); }
	EvaluationStack&	Set (util_ui32 l, const char* r){ line = l; rule = r; return *this; }
	void				Initialise (void)				{ values.clear();}
	void				CleanUp (void)					{ values.clear(); }
	
	EvaluationStack (void): line(0){}
	~EvaluationStack(){}
};
}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
