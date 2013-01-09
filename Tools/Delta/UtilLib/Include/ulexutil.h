// ulexutil.h
// Some common functions used for lexical analysis.
// ScriptFigher Project.
// A. Savidis, January 2000.
// Added c++ flex lexer functionality, January 2013.
//

#ifndef	ULEXUTIL_H
#define	ULEXUTIL_H

#include <string>
#include "utypes.h"
#include "ufunctors.h"
#include "ucallbacks.h"
#include "ucomponentdirectory.h"
#include "ulatedestructor.h"
#include "uptr.h"

UTILLIB_FUNC	const std::string	uextendescapesequences (const char* src);
UTILLIB_FUNC	const std::string	uunparseescapesequences (const char* src);
UTILLIB_FUNC	const std::string	uunparseescapesequences (const char* src, const char* from, UPTR(const char*) to);
UTILLIB_FUNC	util_ui32			ustrtohex (const char* s);
UTILLIB_FUNC	double				ustrdectodouble (const char* s);
UTILLIB_FUNC	double				ustrfracttodouble (const char* s);
UTILLIB_FUNC	const char*			usaveidstr (const char* s, std::string* buffer = (std::string*) 0);

UTILLIB_FUNC	bool				uignoreCcomments (
										char			(*input)(void), 
										void			(*unput)(char),
										util_ui32*		line,
										std::string*	error = (std::string*) 0
									);
UTILLIB_FUNC	bool				uignoreCcomments (
										const ucallbackwithclosure<char (*)(void*)>&		input,
										const ucallbackwithclosure<void (*)(char, void*)>&	unput,
										util_ui32*											line,
										std::string*										error = (std::string*) 0
									);

UTILLIB_FUNC	bool				ureadquotedstring (
										std::string&									s,
										const ucallbackwithclosure<char (*)(void*)>&	input, 
										util_ui32*										line,
										std::string*									error = (std::string*) 0
									);
UTILLIB_FUNC	bool				ureadquotedstring (
										std::string&	s,
										char			(*input)(void), 
										util_ui32*		line,
										std::string*	error = (std::string*) 0
									);

UTILLIB_FUNC	bool				uignoreCPPcomments (const ucallbackwithclosure<char (*)(void*)>& input);
UTILLIB_FUNC	bool				uignoreCPPcomments (char (*input)(void));

typedef	udestroyablewrapper<
			char*, 
			uarrdestructorfunctor<char*> 
		> udestroyablestring;

////////////////////////////////////////////////////////////////////////

template<
	typename yy_buffer_state,
	typename YYSTYPE,
	typename YYLTYPE
>
class uflexlexersuper : public ucomponentdirectoryclient {
	protected:
	char*			yytext;
	int				yyleng;
	int				yylineno;		// only maintained if you use %option yylineno
	int				yy_flex_debug;	// only has effect with -d or "%option debug"
	unsigned int	yypos;

	public:
	const char*		YYText (void) const { return yytext;	}
	int				YYLeng (void) const { return yyleng;	}
	int				lineno (void) const { return yylineno;	}

	int				debug (void) const	{ return yy_flex_debug; }
	void			set_debug (int flag){ yy_flex_debug = flag; }

	unsigned int	position (void) const { return yypos; }
	void			set_position (unsigned int value) { yypos = value; }
	
	// Pure virtual functions
	//
	virtual void				yy_switch_to_buffer (yy_buffer_state* new_buffer) = 0;
	virtual yy_buffer_state*	yy_create_buffer (std::istream* s, int size) = 0;
	virtual void				yy_delete_buffer (yy_buffer_state* b) = 0;
	virtual void				yyrestart (std::istream* s) = 0;
	virtual void				switch_streams(std::istream* new_in = 0, std::ostream* new_out = 0) = 0;

	virtual int					yylex (YYSTYPE*, YYLTYPE*) = 0;

	// Call yylex with new input/output sources.
	int yylex (YYSTYPE* yylval, YYLTYPE* yylloc, std::istream* new_in, std::ostream* new_out = 0)
	{
		switch_streams(new_in, new_out);
		return yylex(yylval, yylloc);
	}

	uflexlexersuper (void) : yypos(0) {}
	virtual ~uflexlexersuper () {}
};

#define	UFLEX_LEXER_SUBCLASS_DEF(_class, _protected_defs, _public_defs)				\
struct		yy_buffer_state;														\
typedef int	yy_state_type;															\
union		YYSTYPE;																\
struct		YYLTYPE;																\
class _class : public uflexlexersuper<yy_buffer_state, YYSTYPE, YYLTYPE> {			\
	protected:																		\
		virtual int LexerInput( char* buf, int max_size );							\
		virtual void LexerOutput( const char* buf, int size );						\
		virtual void LexerError( const char* msg );									\
		void yyunput( int c, char* buf_ptr );										\
		int yyinput();																\
		void yy_load_buffer_state();												\
		void yy_init_buffer( yy_buffer_state* b, std::istream* s );					\
		void yy_flush_buffer( yy_buffer_state* b );									\
		int yy_start_stack_ptr;														\
		int yy_start_stack_depth;													\
		int* yy_start_stack;														\
		void yy_push_state( int new_state );										\
		void yy_pop_state();														\
		int yy_top_state();															\
		yy_state_type yy_get_previous_state();										\
		yy_state_type yy_try_NUL_trans( yy_state_type current_state );				\
		int yy_get_next_buffer();													\
		std::istream* yyin;															\
		std::ostream* yyout;														\
		yy_buffer_state* yy_current_buffer;											\
		char yy_hold_char;															\
		int yy_n_chars;																\
		char* yy_c_buf_p;															\
		int yy_init;																\
		int yy_start;																\
		int yy_did_buffer_switch_on_eof;											\
		yy_state_type yy_last_accepting_state;										\
		char* yy_last_accepting_cpos;												\
		yy_state_type* yy_state_buf;												\
		yy_state_type* yy_state_ptr;												\
		char* yy_full_match;														\
		int* yy_full_state;															\
		int yy_full_lp;																\
		int yy_lp;																	\
		int yy_looking_for_trail_begin;												\
		int yy_more_flag;															\
		int yy_more_len;															\
		int yy_more_offset;															\
		int yy_prev_more_offset;													\
		_protected_defs																\
	public:																			\
		void yy_switch_to_buffer( yy_buffer_state* new_buffer );					\
		struct yy_buffer_state* yy_create_buffer( std::istream* s, int size );		\
		void yy_delete_buffer( yy_buffer_state* b );								\
		void yyrestart( std::istream* s );											\
		virtual void switch_streams( std::istream* new_in, std::ostream* new_out );	\
		virtual int yylex(YYSTYPE*, YYLTYPE*);										\
		_class( std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0 );			\
		virtual ~_class();															\
		_public_defs																\
};

////////////////////////////////////////////////////////////////////////

#define	UCLASSQUALIFIER_EMPTY

#define	USTRINGDESTRUCTOR_DECLARE(_qualifier, _class)					\
	class _qualifier _class : public ulatedestructionmanager<			\
							udestroyablestring,							\
							uptrdestructorfunctor<udestroyablestring*>	\
						> {												\
		private:														\
		static _class* singletonPtr;									\
		public:															\
		static _class& GetSingleton (void)								\
			{ DASSERT(singletonPtr); return *DPTR(singletonPtr); }		\
		static _class* GetSingletonPtr (void)							\
			{ DASSERT(singletonPtr); return singletonPtr; }				\
		USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS					\
	};

#define	USTRINGDESTRUCTOR_IMPL(_class)									\
	_class* _class::singletonPtr = (_class*) 0;							\
	void _class::SingletonCreate (void) { unew(singletonPtr); }			\
	void _class::SingletonDestroy (void) { udelete(singletonPtr); }

#endif	// Do not add stuff beyonf this point.